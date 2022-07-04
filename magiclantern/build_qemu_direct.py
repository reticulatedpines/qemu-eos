#!/usr/bin/env python3

import os
import sys
import argparse
import subprocess
import zipfile
import lzma
import shutil
from collections import namedtuple


def main():
    args = parse_args()

    starting_dir = os.getcwd()
    try:
        build_qemu(args.qemu_source_dir)
    except QemuBuilderError as e:
        print("ERROR: " + str(e))
        sys.exit(-1) # finally clause will still happen
    finally:
        os.chdir(starting_dir)


class QemuBuilderError(Exception):
    pass


Version = namedtuple("Version", "major minor micro")

def get_qemu_version(source_dir):
    version_filepath = os.path.join(source_dir, "VERSION")
    if not os.path.isfile(version_filepath):
        raise QemuBuilderError("Missing VERSION file")

    with open(version_filepath, "r") as f:
        version_string = f.readline()

    version_parts = version_string.split(".")
    return Version(int(version_parts[0]),
                   int(version_parts[1]),
                   int(version_parts[2]))


def run(command=[], error_message="", cwd="."):
    """
    Wraps subprocess.run() in order to provide consistent
    exception type and allow custom error messages on failure.

    Supports changing working directory.
    """
    try:
        res = subprocess.run(command,
                             env=os.environ, check=True,
                             cwd=cwd)
    except (subprocess.CalledProcessError, FileNotFoundError) as e:
        raise QemuBuilderError(error_message + str(e))


def build_qemu(source_dir):
    """
    Given the location of Qemu source, attempt to build it directly,
    producing a zip, then add ML specific files into the zip.

    When extracted, the zip creates a fully functional qemu-eos
    install.

    NB gets called from main(), which wraps it in exception
    handling which restores directory on any exceptions, so
    we can change path in here and it's okay.
    """
    version = get_qemu_version(source_dir)

    # we assume this script is co-located with other required files,
    # and use the location of the script to find them
    script_dir = os.path.split(os.path.realpath(__file__))[0]

    build_dir = os.path.join(script_dir, "direct_builder")
    if os.path.isdir(build_dir):
        raise QemuBuilderError("'direct_builder' build dir already existed, "
                               "did the previous build fail?  Check it's "
                               "safe to clean, then retry.")
    else:
        os.mkdir(build_dir)

    tar_name = "qemu_src_" + str(version.major) + ".tar"
    target_tar = os.path.join(build_dir, tar_name)
    run(command=["./scripts/archive-source.sh", target_tar],
        error_message="Tarring qemu source failed: ",
        cwd=source_dir)

    # configure
    run(command=["tar", "-xf", tar_name],
        error_message="tar -xf failed: ",
        cwd=build_dir)
    if version.major == 2:
        config = ["./configure", "--target-list=arm-softmmu",
                  "--disable-docs", "--enable-vnc", "--enable-gtk",
                  "--python=/usr/bin/python2",
                  '--extra-cflags="-Wno-deprecated-declarations"',]
    elif version.major == 4:
        config = ["./configure", "--target-list=arm-softmmu",
                  "--enable-plugins",
                  "--disable-docs", "--enable-vnc", "--enable-gtk"]
    else:
        raise QemuBuilderError("unexpected Qemu version: %d.%d.%d" % version)
    run(command=config,
        error_message="configure failed for Qemu: ",
        cwd=build_dir)

    # make
    run(command=["make", "-j" + str(os.cpu_count())],
        error_message="make failed for Qemu: ",
        cwd=build_dir)
    if version.major == 4:
        run(command=["make", "plugins"],
            error_message="make failed for Qemu plugins: ",
            cwd=build_dir)

    zip_file = "qemu_" + str(version.major) + ".zip"
    # zip built arm-softmmu, ML utils, per cam GDB scripts, disk images.
    # Plugins if Qemu 4.
    # Mangle the paths appropriately.
    with zipfile.ZipFile(zip_file, "w") as zipf:
        zip_path_prefix = "qemu-eos-build"

        # copy plugins to arm-softmmu
        plugins = ["libmagiclantern.so"]
        arm_softmmu_dir = os.path.join(build_dir, "arm-softmmu")
        for p in plugins:
            # later versions of Qemu have a contrib dir for plugins,
            # rather than test/plugins, so this will fail and need updating
            shutil.copy(os.path.join(build_dir, "tests", "plugin", p),
                        os.path.join(arm_softmmu_dir, "plugins"))

        # add arm-softmmu
        for (root, _, files) in os.walk(arm_softmmu_dir):
            for f in files:
                fullpath = os.path.join(root, f)
                relpath = os.path.relpath(fullpath, arm_softmmu_dir)
                zipf.write(fullpath, arcname=os.path.join(zip_path_prefix,
                                                          "arm-softmmu",
                                                          relpath))

        # add per cam scripts
        cam_config_dir = os.path.join(script_dir, "cam_config")
        for (root, _, files) in os.walk(cam_config_dir):
            for f in files:
                fullpath = os.path.join(root, f)
                relpath = os.path.relpath(fullpath, cam_config_dir)
                zipf.write(fullpath, arcname=os.path.join(zip_path_prefix, relpath))

        # add ML scripts
        ml_scripts = ["run_qemu.py", "run_tests.py"]
        for s in ml_scripts:
            zipf.write(os.path.join(script_dir, s),
                       arcname=os.path.join(zip_path_prefix, s))

        # add modules required by ML scripts
        ml_module_dirs = ["ml_qemu", "ml_tests"]
        for d in ml_module_dirs:
            for (root, subd, files) in os.walk(d):
                if os.path.split(root)[1] == "__pycache__":
                    continue
                for f in files:
                    fullpath = os.path.join(root, f)
                    zipf.write(fullpath, arcname=os.path.join(zip_path_prefix, fullpath))

        # add default disk images
        disk_image = lzma.open(os.path.join(script_dir, "disk_images",
                                            "sd.img.xz")).read()
        zipf.writestr(os.path.join(zip_path_prefix, "disk_images",
                                   "sd.img"), disk_image)
        zipf.writestr(os.path.join(zip_path_prefix, "disk_images",
                                   "cf.img"), disk_image)

    # cleanup build dir
    if build_dir:
        run(command=["rm", "-r", build_dir],
            error_message="failed to remove build_dir: ")
    else:
        # make sure we don't rm -r with no name, just in case there's
        # some crazy rm out there that does something bad.
        raise QemuBuilderError("build_dir was empty?  Shouldn't happen")


def parse_args():
    description = """
    Script to build Qemu with EOS support, using whatever toolchain is in path.
    The Qemu binaries are created as qemu.zip.
    """

    script_dir = os.path.split(os.path.realpath(__file__))[0]
    parser = argparse.ArgumentParser(description=description)

    parser.add_argument("-s", "--qemu_source_dir",
                        default=os.path.realpath(os.path.join(script_dir,
                                                              "..")),
                        help="source dir for ML Qemu, default: %(default)s")

    args = parser.parse_args()

    try:
        if not os.path.isdir(args.qemu_source_dir):
            raise QemuBuilderError("Qemu source dir didn't exist.  "
                                     "You may need to clone the qemu-eos repo.")
        if not os.path.isdir(os.path.join(args.qemu_source_dir,
                                          ".git")):
            raise QemuBuilderError("Qemu source dir didn't look like a git repo.  "
                                     "It should contain the qemu-eos repo.")
    except QemuBuilderError as e:
        print("ERROR: " + str(e))
        sys.exit(-1)

    return args


if __name__ == "__main__":
    main()
