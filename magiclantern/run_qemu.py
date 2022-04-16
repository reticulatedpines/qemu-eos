#!/usr/bin/env python3

import os
import argparse
import subprocess
import sys

from ml_qemu.run import QemuRunner, QemuRunnerError

def main():
    args = parse_args()

    try:
        with QemuRunner(args.qemu_build_dir, args.rom_dir, args.model) as q:
            q.qemu_process.wait()
    except QemuRunnerError as e:
        print("ERROR: " + str(e))
        sys.exit(-1)


def parse_args():
    description = """
    Script to run Qemu with EOS support.
    """

    script_dir = os.path.split(os.path.realpath(__file__))[0]
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

    # There are two common locations we may run this file from;
    # 1) the repo location - qemu-eos/magiclantern/run_qemu.py
    # 2) the extracted build from the zip - qemu-eos-build/run_qemu.py
    #
    # Here we try to detect these cases and provide an appropriate default.
    if os.path.split(os.getcwd())[1] == "qemu-eos-build":
        build_dir_default = "."
    else:
        build_dir_default = os.path.join("..", "..", "qemu-eos-build")
    parser.add_argument("-q", "--qemu_build_dir",
                        default=os.path.realpath(os.path.join(script_dir,
                                                              build_dir_default)),
                        help="build dir for ML Qemu, default: %(default)s")

    # Similar to above, the ROM dir will be in a different (relative) place,
    # depending on if we're running direct from repo, or from extracted build.
    if os.path.split(os.getcwd())[1] == "qemu-eos-build":
        rom_dir_default = os.path.join("..", "roms")
    else:
        rom_dir_default = os.path.join("..", "..", "roms")
    parser.add_argument("-r", "--rom_dir",
                        default=os.path.realpath(os.path.join(script_dir,
                                                              rom_dir_default)),
                        help="location of roms, default: %(default)s")

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
    except QemuRunnerError as e:
        print("ERROR: " + str(e))
        sys.exit(-1)

    return args


if __name__ == "__main__":
    main()
