#!/usr/bin/env python3

import os
import argparse
import subprocess


class QemuRunnerError(Exception):
    pass


class QemuRunner:
    """
    Context manager for running Qemu, this allows automatically
    cleaning up Qemu monitor socket via "with".  Entering the
    context starts Qemu, via Popen therefore non-blocking.

    You can control Qemu within the context, either via Qemu monitor,
    or VNC api.

    Leaving the context ends Qemu and cleans up (principally the
    monitor socket).

    If you fall out of the context, Qemu will be ended by terminating
    the process.  This is not generally what you want.  If you
    do something like: 'with QemuRunner() as q', then you can q.wait()
    and it will block until the user ends Qemu.  You can attempt a
    graceful power down of the VM by sending b'system_powerdown\n'
    to the monitor socket, or force a power down via b'quit\n'.
    """
    def __init__(self, build_dir, rom_dir, cam,
                 monitor_socket_path="",
                 vnc_display="",
                 boot=False):
        # TODO:
        # handle QEMU_EOS_DEBUGMSG,
        # allow selecting drive images,
        # ensure Qemu test suite works in the same way with this vs run_canon_fw.sh,
        # handle passing other args, e.g. -d romcpy
        # improve boot / non-boot selection (currently, -M CAM,firmware=boot=0, a better way
        #   would be creating a proper qemu option group)
        self.rom_dir = rom_dir
        if monitor_socket_path:
            self.monitor_socket = monitor_socket_path
        else:
            self.monitor_socket = "qemu.monitor"

        if boot:
            model = cam + ",firmware=boot=1"
        else:
            model = cam + ",firmware=boot=0"

        self.qemu_command = [os.path.join(build_dir, "arm-softmmu", "qemu-system-arm"),
                             "-drive", "if=sd,format=raw,file=" +
                                     os.path.join(build_dir, "disk_images", "sd.img"),
                             "-drive", "if=ide,format=raw,file=" +
                                     os.path.join(build_dir, "disk_images", "cf.img"),
                             "-chardev", "socket,server,nowait,path=" + self.monitor_socket + ",id=monsock",
                             "-mon", "chardev=monsock,mode=readline",
                             "-name", cam,
                             "-M", model,
                            ]

        if vnc_display:
            self.qemu_command.extend(["-vnc", vnc_display])

    def __enter__(self):
        qemu_env = os.environ
        # FIXME remove QEMU_EOS_WORKDIR and make it a proper qemu CLI option
        qemu_env["QEMU_EOS_WORKDIR"] = self.rom_dir
        print(self.qemu_command)
        self.qemu_process = subprocess.Popen(self.qemu_command,
                                             env=qemu_env,
                                             stdin=subprocess.PIPE,
                                             stdout=subprocess.PIPE)
        return self.qemu_process

    def __exit__(self, *args):
        self.qemu_process.terminate()
        try:
            os.remove(self.monitor_socket)
        except FileNotFoundError:
            pass

