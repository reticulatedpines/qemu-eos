#!/usr/bin/env python3

from abc import ABC
import os
import hashlib
from time import sleep

from ml_qemu.run import QemuRunner, get_cam_path

class TestError(Exception):
    pass


class TestFailError(Exception):
    """
    Raised when a test fails, for signalling to
    the enclosing TestSuite
    """
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

        # get default disk image paths, individual tests may override these
        cam_path = get_cam_path(cam.source_dir, cam.model)
        self.sd_file = os.path.join(cam_path, "sd.qcow2")
        self.cf_file = os.path.join(cam_path, "cf.qcow2")

        self.qemu_dir = qemu_dir
        self.job_ID = job_ID
        self.gdb_port = 1234 + job_ID
        self.vnc_port = 12345 + job_ID
        self.vnc_display = ":" + str(self.vnc_port)
        self.qemu_monitor_path = os.path.join(".", "qemu.monitor" + str(job_ID))
        self.output_top_dir = test_dir
        self.orig_dir = os.getcwd()
        self.expected_output_dir = os.path.join(self.orig_dir,
                                                "expected_test_output",
                                                self.cam.model,
                                                self.__class__.__name__)

    def run(self):
        pass

    def __enter__(self):
        path_parts = [self.output_top_dir,
                      self.cam.model,
                      self.__class__.__name__]
        self.output_dir = os.path.join(*path_parts)
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
        with QemuRunner(self.qemu_dir, self.cam.rom_dir, self.cam.source_dir,
                        self.cam.model,
                        sd_file=self.sd_file, cf_file=self.cf_file,
                        monitor_socket_path=self.qemu_monitor_path,
                        vnc_display=self.vnc_display) as q:
            q.screen_cap_prefix = "menu_test_"
            for k in key_sequence:
                capture_filename = q.key_press(k)
                # TODO check screen matches expected, raise if not
                capture_filepath = os.path.join(self.output_dir, capture_filename)
                with open(capture_filepath, "rb") as f:
                    test_hash = hashlib.md5(f.read()).hexdigest()
                with open(os.path.join(self.expected_output_dir, capture_filename), "rb") as f:
                    expected_hash = hashlib.md5(f.read()).hexdigest()
                if test_hash != expected_hash:
                    # attempt clean shutdown via Qemu monitor socket
                    q.shutdown()
                    raise TestFailError("FAIL: mismatched hash for file '%s', expected %s, got %s"
                                        % (capture_filename, expected_hash, test_hash))

            # attempt clean shutdown via Qemu monitor socket
            q.shutdown()
        print(f"PASS: {self.__class__.__name__}, {self.cam.model}")


