#!/usr/bin/env python3

import os
import sys
import argparse

import ml_tests


def main():
    args = parse_args()

    suite = ml_tests.test_suite.TestSuite(args.cams,
                                          args.rom_dir,
                                          args.tests,
                                          args.fail_early)

    # A test suite holds cams, each cam holds the
    # tests that are valid for itself.  The suite object
    # aggregates results from those tests.  Creating the
    # suite doesn't start the tests, it configures them,
    # checks validity etc.

    for c in suite.cams:
        print(c)

    try:
        suite.run_tests()
    except ml_tests.tests.TestError:
        # some test failed
        raise
    except ml_tests.test_suite.TestSuiteError:
        # something was wrong in the overall test setup itself
        raise

    # Suite has finished (possibly with handled failures from above?).
    # Do something with the results.


def parse_args():
    description = """
    Runs a suite of Qemu tests against EOS roms, using Magic Lantern
    fork / patches for Qemu (qemu-eos).
    """

    parser = argparse.ArgumentParser(description=description)

    supported_cams = ""
    for c in sorted(list(ml_tests.cam.Cam.supported_cams)):
        supported_cams += c + " "
    parser.add_argument("-c", "--cams",
                        nargs="+",
                        help="Which cams to test, e.g. 50D 200D.  Defaults to all. "
                             "  Supported cams: %s" % supported_cams)

    default_rom_dir = os.path.realpath(os.path.join("..", "..", "roms"))
    parser.add_argument("-r", "--rom-dir",
                        default=default_rom_dir,
                        help="Location of dir holding rom subdirs, default: %(default)s")

    default_qemu_dir = os.path.realpath(os.path.join(".."))
    parser.add_argument("-q", "--qemu-dir",
                        default=default_qemu_dir,
                        help="Location of dir holding qemu-eos install, default: %(default)s")

    test_group_names = ""
    for n in ml_tests.test_group_names:
        test_group_names += n + " "
    parser.add_argument("-t", "--tests",
                        nargs="+",
                        help="Which test groups to run, defaults to all, "
                             "available groups: %s" % test_group_names)

    parser.add_argument("--no-fail-early",
                        action="store_true",
                        default=False,
                        help="Override the default of failing all testing "
                             "at the first test that fails")

    args = parser.parse_args()

    if args.no_fail_early:
        args.fail_early = False
    else:
        args.fail_early = True

    if not os.path.isdir(args.rom_dir):
        print("FAIL: rom_dir didn't exist / couldn't be "
              "accessed: %s" % args.rom_dir)
        sys.exit(-1)

    if not os.path.isdir(args.qemu_dir):
        print("FAIL: qemu_dir didn't exist / couldn't be "
              "accessed: %s" % args.qemu_dir)
        sys.exit(-1)

    if not args.tests:
        args.tests = ml_tests.test_group_names
    else:
        args.tests = set(args.tests)
        unknown_test_groups = args.tests - ml_tests.test_group_names
        if unknown_test_groups:
            print("FAIL: unknown test group name(s): %s" % unknown_test_groups)
            sys.exit(-1)

    if not args.cams:
        args.cams = list(ml_tests.cam.Cam.supported_cams)

    return args


if __name__ == "__main__":
    main()
