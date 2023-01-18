#!/usr/bin/env python3

import os
import sys
import time

from . import test_group_names
from .cam import Cam, CamError
from .menu_test import MenuTest
from .log_test import LogTest

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
                 qemu_dir="",
                 rom_dir="",
                 source_dir="",
                 test_output_dir="",
                 test_names=[],
                 fail_early=False,
                 force_continue=False,
                 verbose=False
                ):
        # fail_early controls abort strategy.  We default to running all
        # specified cams, and giving a summary of results.  With fail_early,
        # we abort as soon as any cam test reports failure.  Faster, but
        # less comprehensive, intended for dev work.

        # TODO validate rom dir
        
        # TODO validate qemu dir

        self.fail_early = fail_early

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

        self.cams = []
        for c in cams:
            try:
                self.cams.append(Cam(c, rom_dir, source_dir,
                                     fail_early=self.fail_early))
            except CamError as e:
                print("FAIL: %s" % c)
                print("      %s" % e)

        # add appropriate tests to each cam
        for c in self.cams:
            for t in test_names:
                if t not in test_group_names:
                    raise TestSuiteError("Unexpected test name: %s" % t)
                if t == "menu" and c.can_emulate_gui:
                    c.tests.append(MenuTest(c, qemu_dir,
                                            self.test_output_dir,
                                            verbose=verbose,
                                            force_continue=force_continue))
                if t == "log":
                    c.tests.append(LogTest(c, qemu_dir,
                                           self.test_output_dir,
                                           verbose=verbose,
                                           force_continue=force_continue))

            if not c.tests:
                print("WARN: Cam has no valid tests to run: %s" % c.model)

    def run_tests(self):
        for c in self.cams:
            print("INFO: starting %s tests" % c.model)

            # fail if any of these are true:
            #   run_tests() reports failure
            #   the cam had no tests
            #   any test is in a failed state
            #   any test is not in a passed state
            #   any test has a fail_reason

            result = c.run_tests()
            failed_tests = [t for t in c.tests if t.passed == False]
            unpassed_tests = [t for t in c.tests if t.passed != True]
            fail_reason_tests = [t for t in c.tests if t.fail_reason]

            if not result:
                # at least one test failed
                print("FAIL: %s" % c.model)
                for t in [t for t in c.tests if not t.passed]:
                    test_name = t.__class__.__name__
                    print("      %s" % t.fail_reason)
            elif not c.tests:
                # no tests were defined, fail but don't raise
                print("FAIL: %s" % c.model)
                print("      No tests defined for this cam")
            elif failed_tests:
                # shouldn't happen, the first case should catch this
                print("ERR : badly detected failure")
                print("FAIL: %s" % c.model)
                for t in failed_tests:
                    test_name = t.__class__.__name__
                    print("      %s" % t.fail_reason)
                raise TestSuiteError("badly detected failure, failed tests")
            elif unpassed_tests:
                # shouldn't happen, the first case should catch this
                print("ERR : badly detected failure")
                print("FAIL: %s" % c.model)
                for t in unpassed_tests:
                    test_name = t.__class__.__name__
                    print("      %s" % t.fail_reason)
                    print("      passed: %s" % t.passed)
                    print("      passed type: %s" % type(t.passed))
                raise TestSuiteError("badly detected failure, unpassed test")
            elif fail_reason_tests:
                # shouldn't happen, the first case should catch this
                #
                # Can be a signal you forgot to "return self.return_failure()"
                print("ERR : badly detected failure")
                print("FAIL: %s" % c.model)
                for t in fail_reason_tests:
                    test_name = t.__class__.__name__
                    print("      fail reason: %s" % t.fail_reason)
                    print("      fail reason type: %s" % type(t.fail_reason))
                raise TestSuiteError("badly detected failure, fail reason tests")
            else:
                print("PASS: %s" % c.model)

    def __enter__(self):
        os.chdir(self.test_output_dir)
        return self

    def __exit__(self, *args):
        os.chdir(self.orig_dir)

