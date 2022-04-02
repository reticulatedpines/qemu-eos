#!/usr/bin/env python3

from abc import ABC
import os
from time import sleep

from ml_qemu.run import QemuRunner

class TestError(Exception):
    pass


class Test(ABC):
    """
    All tests share some methods and properties,
    giving a common interface.

    run() does the actual work.

    Is a context manager, which handles changing dir
    and restoring directory state when the test ends.
    Should be used via "with".
    """
    def __init__(self, cam, qemu_dir, test_dir, job_ID=0):
        self.cam = cam
        self.qemu_dir = qemu_dir
        self.job_ID = job_ID
        self.gdb_port = 1234 + job_ID
        self.vnc_port = 12345 + job_ID
        self.vnc_display = ":" + str(self.vnc_port)
        self.qemu_monitor_path = os.path.join(".", "qemu.monitor" + str(job_ID))
        self.output_top_dir = test_dir
        self.orig_dir = os.getcwd()

    def run(self):
        pass

    def __enter__(self):
        path_parts = [self.output_top_dir,
                      self.cam.model,
                      self.__class__.__name__]
        self.output_dir = os.path.join(*path_parts)
        print(self.output_dir)
        os.makedirs(self.output_dir)
        os.chdir(self.output_dir)
        return self

    def __exit__(self, *args):
        os.chdir(self.orig_dir)


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
    qemu_key_sequences = {
                "424545a5cfe10b1a5d8cefffe9fe5297": # 50D ROM1
                ["m", "l", "l", "m", "right", "right", "right", "right",
                 "right", "right", "right", "right", "right", # cycle through all menus
                 "up", "up", "space", "down", "space", # check sub-menus work, turn beep off
                 "right", "up", "up", "space", "pgdn", "space", # check wheel controls using Expo Comp sub-menu
                ]
                }
    def run(self):
        print("MenuTest starting on %s %s" % 
              (self.cam.model, self.cam.code_rom_md5))

        try:
            key_sequence = self.qemu_key_sequences[self.cam.code_rom_md5]
        except KeyError:
            raise TestError("Unknown rom with MD5 sum: %s" % 
                            self.cam.code_rom_md5)

        # invoke qemu and control it to run the test
        with QemuRunner(self.qemu_dir, self.cam.rom_dir, self.cam.model,
                        monitor_socket_path=self.qemu_monitor_path,
                        vnc_display=self.vnc_display) as q:
            q.screen_cap_prefix = "menu_test_"
            for k in key_sequence:
                q.key_press(k)

            # attempt clean shutdown via Qemu monitor socket
            q.shutdown()


