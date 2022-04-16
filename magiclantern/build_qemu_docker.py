#!/usr/bin/env python3

import os
import sys
import argparse
import subprocess
import zipfile
import lzma
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
    Given the location of Qemu source, use Docker to build it,
    producing a zip, then add ML specific files into the zip.

    When extracted, the zip creates a fully functional qemu-eos
    install.

    NB gets called from main(), which wraps it in exception
    handling which restores directory on any exceptions, so
    we can change path in here and it's okay.
    """
    version = get_qemu_version(source_dir)
    # major version match is probably good enough,
    # deps don't change that fast.
    #
    # Filename is a bit weird because Qemu has a gitignore rule
    # that matches any file ending with ".[1-9]", and I don't know
    # what it's for so I don't want to change it.
    if version.major == 2:
        dockerfile = "dockerfile_2_5_0"
    elif version.major == 4:
        dockerfile = "dockerfile_4_2_1"
    else:
        raise QemuBuilderError("unexpected Qemu version: %d.%d.%d" % version)

    # we assume this script is co-located with other required files,
    # and use the location of the script to find them
    script_dir = os.path.split(os.path.realpath(__file__))[0]

    target_tar = os.path.join(script_dir, "docker_builder", "qemu_src_" +
                                    str(version.major) + ".tar")
    run(command=["./scripts/archive-source.sh", target_tar],
        error_message="Tarring qemu source failed: ",
        cwd=source_dir)

    run(command=["sudo", "docker", "build",
                 "-t", "qemu_build",
                 "-f", os.path.join(script_dir, "docker_builder", dockerfile),
                 os.path.join(script_dir, "docker_builder")],
        error_message="sudo docker build failed: ")

    try:
        run(command=["sudo", "docker", "rm", "qemu_build_output"],
            error_message="sudo docker rm failed: ")
    except QemuBuilderError:
        # It's fine if this fails due to the image not existing
        # on first attempt
        pass

    run(command=["sudo", "docker", "create", "--name", "qemu_build_output",
                 "qemu_build"],
        error_message="sudo docker create failed: ")

    # copy zipped arm-softmmu dir to host
    zip_file = "qemu_" + str(version.major) + ".zip"
    run(command=["sudo", "docker", "cp", "qemu_build_output:/home/ml_builder/" +
                 zip_file,
                 "."],
        error_message="sudo docker cp failed: ")

    username = os.getlogin()
    if username:
        run(command=["sudo", "chown", username, zip_file])

    with zipfile.ZipFile(zip_file, "a") as zipf:
        # add per cam scripts to zip, mangling the path name so they're in correct dir
        zip_path_prefix = "qemu-eos-build"
        cam_config_dir = os.path.join(script_dir, "cam_config")
        for (root, _, files) in os.walk(cam_config_dir):
            for f in files:
                fullpath = os.path.join(root, f)
                relpath = os.path.relpath(fullpath, cam_config_dir)
                zipf.write(fullpath, arcname=os.path.join(zip_path_prefix, relpath))

        # add ML scripts to zip
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
                    print(root, subd, f, fullpath)
                    zipf.write(fullpath, arcname=os.path.join(zip_path_prefix, fullpath))

        # add default disk images to zip
        disk_image = lzma.open(os.path.join(script_dir, "disk_images",
                                            "sd.img.xz")).read()
        zipf.writestr(os.path.join(zip_path_prefix, "disk_images",
                                   "sd.img"), disk_image)
        zipf.writestr(os.path.join(zip_path_prefix, "disk_images",
                                   "cf.img"), disk_image)



def parse_args():
    description = """
    Script to build Qemu with EOS support, using Docker.
    The Qemu binaries are created as qemu.zip.
    """

    script_dir = os.path.split(os.path.realpath(__file__))[0]
    parser = argparse.ArgumentParser(description=description)

    parser.add_argument("-s", "--qemu_source_dir",
                        default=os.path.realpath(os.path.join(script_dir,
                                                              "..", "..", "qemu-eos")),
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
