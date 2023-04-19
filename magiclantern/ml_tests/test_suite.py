#!/usr/bin/env python3

import os
import sys
import queue # only for queue.Empty
import multiprocessing
import time

from . import test_group_names
from . import locking_print
from .cam import Cam, CamError
from .menu_test import MenuTest
from .log_test import LogTest


class TestSuiteError(Exception):
    pass


def test_worker(tests, q, lock, verbose=False):
    """
    Takes a synchronised Manager list,
    and a queue of indices into the list.

    The queue is used to ensure only one worker
    will perform a given test.
    """
    while True:
        try:
            i = q.get(block=False)
        except queue.Empty:
            # This occurs, despite the queue being populated in one place,
            # before any workers are started.  Queues are more strongly async
            # than I expected?  Wait for another item to be available.
            time.sleep(0.2)
            continue

        if i is None:
            break # sentinel / poison pill for this worker, we are done

        test = tests[i]

        locking_print("INFO: %s starting %s" % (test.cam.model, test.__class__.__name__),
                      lock)

        with test as t:
            try:
                t.run(lock)
            except TimeoutError:
                locking_print("FAIL: timeout during test, %s, %s"
                              % (test.cam.model, test.__class__.__name__),
                              lock)

        locking_print("INFO: %s finished %s" % (test.cam.model, test.__class__.__name__),
                      lock)
        # update the shared list so it has the result from the run
        tests[i] = test


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
        self.verbose = verbose

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
        self.early_failed_cams = []
        for c in cams:
            try:
                self.cams.append(Cam(c, rom_dir, source_dir,
                                     fail_early=self.fail_early))
            except CamError as e:
                print("FAIL: %s" % c)
                print("      %s" % e)
                self.early_failed_cams.append(c)

        # create tests appropriate for each cam
        job_ID = 1
        self._tests = []
        self.finished_tests = [] # populated by run_tests()
        for c in self.cams:
            tests = []
            for t in test_names:
                if t not in test_group_names:
                    raise TestSuiteError("Unexpected test name: %s" % t)
                if t == "menu" and c.can_emulate_gui:
                    tests.append(MenuTest(c, qemu_dir,
                                          self.test_output_dir,
                                          job_ID=job_ID,
                                          verbose=verbose,
                                          force_continue=force_continue))
                if t == "log":
                    tests.append(LogTest(c, qemu_dir,
                                         self.test_output_dir,
                                         job_ID=job_ID,
                                         verbose=verbose,
                                         force_continue=force_continue))
                job_ID += 1

            if not tests:
                print("WARN: Cam has no valid tests to run: %s" % c.model)
            self._tests.extend(tests)

    def run_tests(self):
        """
        Using multiple worker processes, run the tests in the suite.
        """

        # We use a Manager to send copies of the Test objects,
        # in an array, to a set of worker processes.
        # These run and modify the objects, sending them back.
        #
        # A queue holds indices into the array, workers
        # use this to avoid conflicts over work items.

        mp_context = multiprocessing.get_context("fork")
        with multiprocessing.Manager() as manager:
            test_index_queue = mp_context.Queue()

            managed_tests = manager.list(self._tests)

            for i in range(len(self._tests)):
                test_index_queue.put(i)

            num_workers = os.cpu_count()
            if num_workers > 1:
                num_workers -= 1 # be friendly, keep a core free
            if num_workers > len(self._tests):
                num_workers = len(self._tests)

            for _ in range(num_workers):
                test_index_queue.put(None) # sentinel value, checking for queue.Empty exception
                                           # is supposed to be unreliable.  I suspect it's fine
                                           # here because we only queue items once, and it's
                                           # before any workers start.  Does no harm though.

            print("INFO: starting test suite, %d workers" % num_workers)
            workers = []
            lock = mp_context.Lock()
            for _ in range(num_workers):
                worker = mp_context.Process(target=test_worker,
                                            args=(managed_tests,
                                            test_index_queue,
                                            lock,
                                            self.verbose))
                workers.append(worker)
                worker.start()

            # All jobs queued, some are running.
            # Poll for worker process status.  This allows printing updates,
            # and checking if any / all workers died
            #
            # TODO some kind of timeout handling
            any_workers_alive = True
            while any_workers_alive:
                time.sleep(2)
                any_workers_alive = False
                for w in workers:
                    if w.exitcode == None:
                        any_workers_alive = True
                        break

            for w in workers:
                w.join()

            self.finished_tests = list(managed_tests)

        return self._check_results()

    def _check_results(self):
        """
        Inspect the results of a test suite run, mostly via
        self.finished_tests.

        Return False if any test failed for any cam,
        otherwise True.  May raise TestSuiteError if "impossible"
        results are encountered.
        """
        all_tests_passed = True
        if self.early_failed_cams:
            all_tests_passed = False
        for c in self.cams:
            tests = [t for t in self.finished_tests if t.cam.model == c.model]
            orig_tests = [t for t in self._tests if t.cam.model == c.model]

            failed_tests = {t for t in tests if t.passed == False}
            passed_tests = {t for t in tests if t.passed == True}
            unpassed_tests = {t for t in tests if t.passed != True}
            fail_reason_tests = {t for t in tests if t.fail_reason}

            if not tests:
                print("FAIL: %s" % c.model)
                print("      No tests defined for this cam")
                all_tests_passed = False
            elif len(tests) != len(orig_tests):
                print("ERR : unexpected number of finished tests")
                print("FAIL: %s" % c.model)
                print("      queued %d, got %d finished" % (len(orig_tests), len(tests)))
                all_tests_passed = False
                raise TestSuiteError("unexpected number of finished tests")
            elif fail_reason_tests - failed_tests:
                # fail_reason without passed == False
                # Can be a signal you forgot to "return self.return_failure()"
                print("ERR : badly detected failure")
                print("FAIL: %s" % c.model)
                for t in fail_reason_tests - failed_tests:
                    print(t)
                    print("      fail reason: %s" % t.fail_reason)
                    print("      fail reason type: %s" % type(t.fail_reason))
                all_tests_passed = False
                raise TestSuiteError("badly detected failure, fail reason tests")
            elif unpassed_tests - failed_tests:
                # test.passed is neither True nor False, shouldn't happen
                print("ERR : badly detected failure")
                print("FAIL: %s" % c.model)
                for t in (unpassed_tests - failed_tests):
                    print(t)
                    print("      %s" % t.fail_reason)
                    print("      passed: %s" % t.passed)
                    print("      passed type: %s" % type(t.passed))
                all_tests_passed = False
                raise TestSuiteError("badly detected failure, unpassed test")
            elif len(passed_tests) == len(tests):
                print("PASS: %s" % c.model)
            elif len(passed_tests) != len(tests):
                # didn't pass all tests
                all_tests_passed = False
            else:
                # because all_tests_passed starts as True, we want
                # to be sure we can't fall through if something goes wrong
                raise TestSuiteError("ERR : hit 'impossible' condition")
        return all_tests_passed

    def __enter__(self):
        os.chdir(self.test_output_dir)
        return self

    def __exit__(self, *args):
        os.chdir(self.orig_dir)

