#!/usr/bin/env python3

import os
import argparse
import subprocess


def main():
    args = parse_args()

    try:
        run_qemu(args.qemu_build_dir, args.rom_dir, args.model)
    except QemuRunnerError as e:
        print("ERROR: " + str(e))
        exit(-1)


class QemuRunnerError(Exception):
    pass


def run_qemu(build_dir, rom_dir, cam):
    # TODO:
    # handle QEMU_EOS_DEBUGMSG,
    # allow selecting drive images,
    # supporting appending QEMU_JOB_ID to monitor socket
    #   (needed for qemu testing I think so not urgent),
    # ensure Qemu test suite works in the same way with this vs run_canon_fw.sh,
    # handle passing other args, e.g. -d romcpy
    # support selecting non-boot (currently, -M CAM,firmware=boot=0, a better way
    #   would be creating a proper qemu option group)
    # add disk image xz to qemu-eos repo and get build system (or something)
    #   to unzip these into qemu-eos-build/disk_images/
    monitor_socket = "qemu.monitor"
    qemu_command = [os.path.join(build_dir, "arm-softmmu", "qemu-system-arm"),
                    "-drive", "if=sd,format=raw,file=" +
                            os.path.join(build_dir, "disk_images", "sd.img"),
                    "-drive", "if=ide,format=raw,file=" +
                            os.path.join(build_dir, "disk_images", "cf.img"),
                    "-chardev", "socket,server,nowait,path=" + monitor_socket + ",id=monsock",
                    "-mon", "chardev=monsock,mode=readline",
                    "-name", cam,
                    "-M", cam,
                    ]
    qemu_env = os.environ
    # FIXME remove QEMU_EOS_WORKDIR and make it a proper qemu CLI option
    qemu_env["QEMU_EOS_WORKDIR"] = rom_dir
    try:
        res = subprocess.run(qemu_command,
                             env=qemu_env,
                             check=True,
                             stdin=subprocess.PIPE,
                             stdout=subprocess.PIPE)
    except subprocess.CalledProcessError:
        os.remove(monitor_socket)
        raise


def parse_args():
    description = """
    Script to run Qemu with EOS support.
    """

    script_dir = os.path.split(os.path.realpath(__file__))[0]
    parser = argparse.ArgumentParser(description=description)

    known_cams = {"1000D", "100D", "1100D", "1200D", "1300D",
                  "200D",
                  "400D", "40D", "450D",
                  "500D", "50D", "550D", "5D", "5D2", "5D3", "5D3eeko", "5D4", "5D4AE",
                  "600D", "60D", "650D", "6D", "6D2",
                  "700D", "70D", "750D", "760D", "77D", "7D", "7D2", "7D2S",
                  "800D", "80D", "850D",
                  "A1100",
                  "EOSM", "EOSM10", "EOSM2", "EOSM3", "EOSM5", "EOSRP",
                  "M50", "R"}
    parser.add_argument("model",
                        choices=known_cams,
                        help="Name of model to emulate, required")

    parser.add_argument("-b", "--qemu_build_dir",
                        default=os.path.realpath(script_dir),
                        help="build dir for ML Qemu, default: %(default)s")

    parser.add_argument("-r", "--rom_dir",
                        default=os.path.realpath(os.path.join(script_dir,
                                                              "..", "roms")),
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
        exit(-1)

    return args


if __name__ == "__main__":
    main()
