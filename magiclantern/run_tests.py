#!/usr/bin/env python3

import os
import sys
import argparse

import ml_tests
from ml_qemu.run import get_default_dirs

def main():
    args = parse_args()

    suite = ml_tests.test_suite.TestSuite(cams=args.cams,
                                          qemu_dir=args.qemu_build_dir,
                                          rom_dir=args.rom_dir,
                                          source_dir=args.source_dir,
                                          test_output_dir=args.test_output_dir,
                                          test_names=args.tests,
                                          fail_early=args.fail_early,
                                          force_continue=args.force_continue,
                                          verbose=args.verbose
                                          )

    # A test suite holds cams, each cam holds the
    # tests that are valid for itself.  The suite object
    # aggregates results from those tests.  Creating the
    # suite doesn't start the tests, it configures them,
    # checks validity etc.

    if args.verbose:
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

    default_dirs = get_default_dirs(os.getcwd())
    default_qemu_dir = os.path.realpath(default_dirs["qemu-build-dir"])
    parser.add_argument("-q", "--qemu-build-dir",
                        default=default_qemu_dir,
                        help="Location of dir holding qemu-eos install, default: %(default)s")

    default_rom_dir = os.path.realpath(default_dirs["rom-dir"])
    parser.add_argument("-r", "--rom-dir",
                        default=default_rom_dir,
                        help="Location of dir holding rom subdirs, default: %(default)s")

    default_source_dir = os.path.realpath(default_dirs["source-dir"])
    parser.add_argument("-s", "--source-dir",
                        default=default_source_dir,
                        help="location of Magic Lantern repo, used to find stubs etc for emulation.  Default: %(default)s")

    default_test_output_dir = os.path.realpath(os.path.join(".", "test_output"))
    parser.add_argument("-t", "--test-output-dir",
                        default=default_test_output_dir,
                        help="Location of dir to output test results, default: %(default)s")

    test_group_names = ""
    for n in ml_tests.test_group_names:
        test_group_names += n + " "
    parser.add_argument("--tests",
                        nargs="+",
                        help="Which test groups to run, defaults to all, "
                             "available groups: %s" % test_group_names)

    parser.add_argument("--fail-early",
                        action="store_true",
                        default=False,
                        help="Instead of running all tests and returning the summary, "
                             "exit immediately when any test fails. "
                             "Intended for dev work.")

    parser.add_argument("--force-continue",
                        action="store_true",
                        default=False,
                        help="For developers only.  Make tests do as much work "
                             "as possible.  Also forces all tests to fail.")

    parser.add_argument("--verbose",
                        action="store_true",
                        default=False,
                        help="Be spammier.  This includes Qemu output.")

    args = parser.parse_args()

    if not os.path.isdir(args.qemu_build_dir):
        print("FAIL: qemu_build_dir didn't exist / couldn't be "
              "accessed: %s" % args.qemu_build_dir)
        sys.exit(-1)

    if not os.path.isdir(args.rom_dir):
        print("FAIL: rom_dir didn't exist / couldn't be "
              "accessed: %s" % args.rom_dir)
        sys.exit(-1)

    if not os.path.isdir(args.source_dir):
        print("FAIL: source_dir didn't exist / couldn't be "
              "accessed: %s" % args.source_dir)
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
