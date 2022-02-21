#!/usr/bin/env python3

from abc import ABC

import vncdotool
from vncdotool import api
from .. import ml_qemu

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
        self.qemu_monitor_path = "qemu.monitor" + str(job_ID)
        self.vnc_client = vncdotool.api.connect("localhost::" + 
                                                str(self.vnc_port))

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
#        self.start_qemu()
        for k in key_sequence:
            print(k)


