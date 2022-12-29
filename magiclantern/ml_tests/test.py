#!/usr/bin/env python3

import abc
import os
import hashlib
from time import sleep

from ml_qemu.run import QemuRunner, get_cam_path


class Test(abc.ABC):
    """
    All tests share some methods and properties,
    giving a common interface.

    run() does the actual work.

    Is a context manager, which handles changing dir
    and restoring directory state when the test ends.
    Should be used via "with".
    """

    # Map cams for which we have tests, to code ROM MD5s.
    # It's a list of ROMs so we can support different ROM
    # dumps if required.
    known_cams = {
                  "5D3": ["e6a90e8497c2c1187e0322010a42b9b5"],
                  "50D": ["424545a5cfe10b1a5d8cefffe9fe5297"],
                  "60D": ["d266ce304585952fb3a05a9f6c304f2f"],
                  "100D": ["e06a0e3919ac4d4ef609a864e937a5d3"],
                  "500D": ["0a9fce1e4ef6d2ac2c3bc63eb96d3c34"],
                  "700D": ["f6c20df071b3514fa65f35dc5d71b484"],
                 }

    def __init__(self, cam, qemu_dir, test_dir, job_ID=0,
                 verbose=False, force_continue=False):
        """
        The force_continue flag makes tests try and do as much as possible
        even if they encounter a failure state.  This is for debugging /
        development only (e.g. for MenuTests it allows capturing the
        intermediate screenshots, useful for generating expected output
        files on a new cam).

        It also sets the final test state to failed, so you can't accidentally
        break things by having all tests erroneously pass.
        """
        self.cam = cam

        # get default disk image paths, individual tests may override these
        cam_path = get_cam_path(cam.source_dir, cam.model)
        self.sd_file = os.path.join(cam_path, "sd.qcow2")
        self.cf_file = os.path.join(cam_path, "cf.qcow2")

        self.qemu_dir = qemu_dir
        self.qemu_runner = None
        self.verbose = verbose
        self.force_continue = force_continue
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
        self.passed = False
        self.fail_reason = None

    @abc.abstractmethod
    def run(self):
        pass

    def return_failure(self, reason):
        self.passed = False
        self.fail_reason = self.__class__.__name__ + ": " + reason
        if self.qemu_runner:
            self.qemu_runner.shutdown()
        return False

    def return_success(self):
        if self.force_continue:
            # always fail if we're in debug mode
            self.passed = False
            return False
        else:
            self.passed = True
            return True

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

