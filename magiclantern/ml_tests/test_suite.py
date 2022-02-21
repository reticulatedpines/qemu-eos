#!/usr/bin/env python3

import os

from .cam import Cam
from . import tests

class TestSuiteError(Exception):
    pass


class TestSuite(object):
    """
    """
    def __init__(self, 
                 cams=[],
                 rom_dir="",
                 qemu_dir="",
                 test_names=[],
                 fail_early=True
                ):
        # fail_early controls abort strategy.  We default to any failing
        # test aborting the job.
        #
        # Setting False allows running a complete suite even when tests fail
        # (perhaps you know a change should break things and want to see
        #  how bad the problem is)
        #
        # We do not catch exceptions from points earlier than running tests.
        # If your config is so bad that tests can't even be created,
        # this cannot be overridden.

        # TODO validate rom dir
        
        # TODO validate qemu dir

        self.cams = [Cam(c, rom_dir) for c in cams]

        # add appropriate tests to each cam
        for c in self.cams:
            if c.can_emulate_gui:
                c.tests.append(tests.MenuTest(c, qemu_dir))
            if not c.tests:
                raise TestSuiteError("Cam has no valid tests to run: %s" % c.model)

    def run_tests(self):
        for c in self.cams:
            c.run_tests()
