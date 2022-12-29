#!/usr/bin/env python3

from . import test

class FailTest(test.Test):
    """
    This test always fails.  Useful for testing the test system itself.
    """

    def run(self):
        if self.verbose:
            print("FailTest starting on %s %s" %
                  (self.cam.model, self.cam.code_rom_md5))

        return self.return_failure("FailTest always fails")

