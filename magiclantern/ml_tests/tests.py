#!/usr/bin/env python3

from abc import ABC
import os
from time import sleep

import vncdotool
from vncdotool import api

import socket

from ml_qemu.run import QemuRunner

class TestError(Exception):
    pass


class Test(ABC):
    """
    All tests share some methods and properties,
    giving a common interface.

    run() does the actual work.
    """
    def __init__(self, cam, qemu_dir, job_ID=0):
        self.cam = cam
        self.qemu_dir = qemu_dir
        self.job_ID = job_ID
        self.gdb_port = 1234 + job_ID
        self.vnc_port = 12345 + job_ID
        self.vnc_display = ":" + str(self.vnc_port)
        self.qemu_monitor_path = os.path.join(".", "qemu.monitor" + str(job_ID))
        self.vnc_client = vncdotool.api.connect(self.vnc_display)

    def run(self):
        pass


class MenuTest(Test):
    """
    This test steps through Canon menus in Qemu,
    ensuring they look how we expect,
    and tries to cleanly shutdown the cam.

    ML is not active.
    """
    # Canon saves state of all menus into ROM,
    # including cursor position etc.  Thus it is simplest if we
    # treat each rom as needing a unique sequence for this test.
    # Here we only want to check if we can reach the expected menus.
    #
    # If you have a new ROM, you'll need to determine a new sequence
    # (if you're lucky, an existing rom will be close and can be
    # adapted).
    qemu_key_sequences = {"424545a5cfe10b1a5d8cefffe9fe5297":
                            ["m", "l"]}
    def run(self):
        print("MenuTest starting on %s %s" % 
              (self.cam.model, self.cam.code_rom_md5))

        try:
            key_sequence = self.qemu_key_sequences[self.cam.code_rom_md5]
        except KeyError:
            raise TestError("Unknown rom with MD5 sum: %s" % 
                            self.cam.code_rom_md5)

        with QemuRunner(self.qemu_dir, self.cam.rom_dir, self.cam.model,
                        monitor_socket_path=self.qemu_monitor_path,
                        vnc_display=self.vnc_display) as q:
            sleep(0.5) # give time for qemu to start

            for k in key_sequence:
                print(k)
            #self.vnc_client.keyPress()
            self.vnc_client.captureScreen("test.png")
            sleep(3)

            # attempt clean shutdown via Qemu monitor socket
            s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
            s.connect(self.qemu_monitor_path)
            s.send(b"system_powerdown\n");
            sleep(2)


