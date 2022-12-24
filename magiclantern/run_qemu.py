#!/usr/bin/env python3

import os
import shutil
import argparse
import subprocess
import sys

from ml_qemu.run import QemuRunner, QemuRunnerError, get_default_dirs, get_cam_path

def main():
    args = parse_args()

    gdb_port = 0
    if args.gdb:
        gdb_port = 1234

    source_dir = args.source_dir
    cam = args.model
    cam_path = get_cam_path(source_dir, cam)

    if args.boot:
        # If we're trying to boot from card, we probably want to use
        # ML for the cam in question, so, attempt to find disk images
        # in platform dir for the cam.
        #
        # TODO have some way of overriding this
        sd_file = os.path.join(cam_path, "sd.qcow2")
        cf_file = os.path.join(cam_path, "cf.qcow2")
        if not os.path.isfile(sd_file):
            print("Couldn't find SD image file: %s" % sd_file)
            if not os.path.isfile(cf_file):
                print("Couldn't find CF image file: %s" % cf_file)
            print("You may want to run 'make disk_image' in the cam dir")
            sys.exit(-1)
    else:
        # use the default disk image, unzip it first if needed
        disk_images_path = os.path.join(".", "disk_images")
        xz_image = os.path.join(disk_images_path, "sd.qcow2.xz")
        xz_command = shutil.which("xz")
        if not os.path.isfile(xz_image):
            print("Couldn't find sd.qcow2.xz, tried: %s" % xz_image)
            sys.exit(-1)
        if not xz_command:
            print("No 'xz' in path, can't extract default disk image")
            sys.exit(-1)

        if not os.path.isfile(os.path.join(disk_images_path, "sd.qcow2")):
            # auto extract on first run
            for p in ["sd2.qcow2.xz", "cf.qcow2.xz"]:
                dst = os.path.join(disk_images_path, p)
                shutil.copy(xz_image, dst)
                subprocess.run([xz_command, "-d", dst])
            shutil.move(os.path.join(disk_images_path, "sd2.qcow2"),
                        os.path.join(disk_images_path, "sd.qcow2"))
        sd_file = os.path.join(disk_images_path, "sd.qcow2")
        cf_file = os.path.join(disk_images_path, "cf.qcow2")

    try:
        with QemuRunner(args.qemu_build_dir, args.rom_dir, source_dir,
                        cam,
                        verbose=True,
                        sd_file=sd_file, cf_file=cf_file,
                        gdb_port=gdb_port,
                        boot=args.boot, d_args=args.d_args) as q:
            q.qemu_process.wait()
            if q.qemu_process.returncode:
                print("ERROR from qemu (bad -d option?)")
    except QemuRunnerError as e:
        print("ERROR: " + str(e))
        sys.exit(-1)


def parse_args():
    description = """
    Script to run Qemu with EOS support.
    """

    parser = argparse.ArgumentParser(description=description)

    known_cams = ["1000D", "100D", "1100D", "1200D", "1300D",
                  "200D",
                  "400D", "40D", "450D",
                  "500D", "50D", "550D", "5D", "5D2", "5D3", "5D3eeko", "5D4", "5D4AE",
                  "600D", "60D", "650D", "6D", "6D2",
                  "700D", "70D", "750D", "760D", "77D", "7D", "7D2", "7D2S",
                  "800D", "80D", "850D",
                  "A1100",
                  "EOSM", "EOSM10", "EOSM2", "EOSM3", "EOSM5", "EOSRP",
                  "M50", "R"]
    parser.add_argument("model",
                        choices=known_cams,
                        help="Name of model to emulate, required")

    default_dirs = get_default_dirs(os.getcwd())

    parser.add_argument("-q", "--qemu-build-dir",
                        default=os.path.realpath(default_dirs["qemu-build-dir"]),
                        help="build dir for ML Qemu, default: %(default)s")
    parser.add_argument("-r", "--rom-dir",
                        default=os.path.realpath(default_dirs["rom-dir"]),
                        help="location of roms, default: %(default)s")
    parser.add_argument("-s", "--source-dir",
                        default=os.path.realpath(default_dirs["source-dir"]),
                        help="location of Magic Lantern repo, used to find stubs etc for emulation.  Default: %(default)s")
    parser.add_argument("--boot",
                        default=False,
                        help="attempt to run autoexec.bin from card (set cam bootflag), default: %(default)s",
                        action="store_true")
    parser.add_argument("--gdb",
                        default=False,
                        action="store_true",
                        help="start Qemu suspended, until gdb connects on port 1234")
    parser.add_argument("-d", "--d-args",
                        nargs="*",
                        default=[],
                        help="space separated list of qemu '-d' arguments.  See help for qemu for complete list")
    args = parser.parse_args()

    try:
        if not os.path.isdir(args.qemu_build_dir):
            raise QemuRunnerError("Qemu build dir didn't exist.  "
                                  "You may need to clone the qemu-eos repo.")
        if not os.path.isdir(os.path.join(args.qemu_build_dir, "arm-softmmu")):
            raise QemuRunnerError("Qemu build dir didn't contain 'arm-softmmu', "
                                  "did the build work?")
        if not os.path.isdir(args.rom_dir):
            raise QemuRunnerError("Rom dir didn't exist.")
        if not os.path.isdir(args.source_dir):
            raise QemuRunnerError("ML source dir didn't exist.")
    except QemuRunnerError as e:
        print("ERROR: " + str(e))
        sys.exit(-1)

    return args


if __name__ == "__main__":
    main()
