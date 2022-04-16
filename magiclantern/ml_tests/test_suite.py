#!/usr/bin/env python3

import os
import time

from . import test_group_names
from .cam import Cam
from . import tests

class TestSuiteError(Exception):
    pass


class TestSuite(object):
    """
    A TestSuite contains a set of Test objects, run_tests()
    iterates through running all tests.

    Is a context manager, and should be used via "with";
    this is used to put test results in a directory hierarchy
    and ensure changing dir is correctly undone should exceptions
    etc occur.
    """
    def __init__(self, 
                 cams=[],
                 rom_dir="",
                 qemu_dir="",
                 test_output_dir="",
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

        self.orig_dir = os.getcwd()
        if not test_output_dir:
            test_output_top_dir = os.path.join(os.getcwd(), "test_output")
        else:
            test_output_top_dir = test_output_dir

        if os.path.exists(test_output_top_dir):
            if not os.path.isdir(test_output_top_dir):
                raise TestSuiteError("test_output_dir path existed, "
                                     "but was not a directory, cannot use")
        else:
            os.mkdir(test_output_top_dir)

        datetime = time.localtime()
        date = str(datetime.tm_year) + "-" \
               + str(datetime.tm_mon).zfill(2) + "-" \
               + str(datetime.tm_mday).zfill(2)
        datetime_str = date + "_" \
                       + str(datetime.tm_hour).zfill(2) + ":" \
                       + str(datetime.tm_min).zfill(2) + ":" \
                       + str(datetime.tm_sec).zfill(2)

        test_output_sub_dir = os.path.join(test_output_top_dir, datetime_str)
        if os.path.exists(test_output_sub_dir):
            if not os.path.isdir(test_output_sub_dir):
                raise TestSuiteError("test_output_sub_dir path existed, "
                                     "but was not a directory, cannot use")
        else:
            os.mkdir(test_output_sub_dir)
        self.test_output_dir = test_output_sub_dir

        self.cams = [Cam(c, rom_dir) for c in cams]
        # add appropriate tests to each cam
        for c in self.cams:
            for t in test_names:
                if t not in test_group_names:
                    raise TestSuiteError("Unexpected test name: %s" % t)
                if t == "menu" and c.can_emulate_gui:
                    c.tests.append(tests.MenuTest(c, qemu_dir, self.test_output_dir))

            if not c.tests:
                print ("WARNING: Cam has no valid tests to run: %s" % c.model)

        # TODO we want to check if each cam ran and passed
        # all the tests it should (from the requested set)

    def run_tests(self):
        for c in self.cams:
            c.run_tests()

    def __enter__(self):
        os.chdir(self.test_output_dir)
        return self

    def __exit__(self, *args):
        os.chdir(self.orig_dir)

