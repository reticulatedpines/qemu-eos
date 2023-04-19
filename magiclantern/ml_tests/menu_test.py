#!/usr/bin/env python3

import os
import hashlib

from . import test
from . import locking_print
from ml_qemu.run import QemuRunner


class MenuTest(test.Test):
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
                "e6a90e8497c2c1187e0322010a42b9b5": # 5D3 ROM1
                ["m", "l", "l", "m",
                 "left", "left", "left", "left", "left", "left", "left", "left", "left",
                 "left", "left", "left", "left", "left", "left", "left", "left", "left",
                 "left", "left", "left", # cycle through all menus
                 "up", "up", "spc", "down", "spc", # sub-menu test, Tv/Av dial direction
                 "right", "right", "right", "up", "spc", "pgdn", "spc", # ISO speed increment, wheel test
                ],
                "424545a5cfe10b1a5d8cefffe9fe5297": # 50D ROM1
                ["m", "l", "l", "m", "right", "right", "right", "right",
                 "right", "right", "right", "right", "right", # cycle through all menus
                 "up", "up", "spc", "down", "spc", # check sub-menus work, turn beep off
                 "right", "up", "up", "spc", "pgdn", "spc", # check wheel controls using Expo Comp sub-menu
                ],
                "d266ce304585952fb3a05a9f6c304f2f": # 60D ROM1
                ["m", "l", "l", "m", "left", "left", "left", "left",
                 "left", "left", "left", "left", "left", "left", "left", # cycle through all menus
                 "up", "up", "spc", "down", "spc", # check sub-menus work; change auto rotation
                 "left", "up", "up", "spc", "pgup", "spc", # check wheel controls on Play options
                ],
                "e06a0e3919ac4d4ef609a864e937a5d3": # 100D ROM1
                ["m", "l", "l", "m", # LV looks weird on this cam and takes a long time to activate
                 "right", "right", "right", "right",
                 "right", "right", "right", "right", "right", "right",
                 "right", # cycle through all menus
                 "up", "up", "up", "spc", "down", "spc", # check sub-menus; LCD auto off
                ],
                "f6c20df071b3514fa65f35dc5d71b484": # 700D ROM1
                ["f1", "m", "right", "right", "right", "right", "right",
                 "right", "right", "right", "right", "right", "right",
                 "right", # cycle through all menus.  This rom has no lens attached and LV usage seems to lock the cam.
                 "spc", "right", "spc", # check sub-menus, change movie res
                 # no wheel controls on this cam?  PgUp / PgDown are unmapped.
                 "left", "spc", "down", "down", "up", "spc", # test up/down in grid display sub-menu
                ],
                "0a9fce1e4ef6d2ac2c3bc63eb96d3c34": # 500D ROM1
                ["f1", "m", "l", "l", "m", # inital info screen, menu and LV
                 "left", "left", "left", "left", "left", "left", "left", "left", # cycle through menus
                 "right", "spc", "right", "spc", # sub-menu test, change screen brightness
                 "right", "spc", "up", "up", "down", "spc", # up/down test.  Unsure on sub-menu, it's Polish lang
                ],
                }

    def run(self, lock):
        self.lock = lock
        if self.verbose:
            locking_print("MenuTest starting on %s %s" %
                  (self.cam.model, self.cam.code_rom_md5),
                  lock)

        if self.cam.model not in self.known_cams:
            return self.return_failure("No tests known for cam: %s"
                                       % self.cam.model)

        if self.cam.code_rom_md5 not in self.known_cams[self.cam.model]:
            return self.return_failure("Unknown rom for cam, MD5 sum: %s"
                                       % self.cam.code_rom_md5)

        if self.cam.code_rom_md5 not in self.qemu_key_sequences:
            return self.return_failure("Unknown rom for MenuTest, MD5 sum: %s"
                                       % self.cam.code_rom_md5)

        key_sequence = self.qemu_key_sequences[self.cam.code_rom_md5]


        # invoke qemu and control it to run the test
        with QemuRunner(self.qemu_dir, self.cam.rom_dir, self.cam.source_dir,
                        self.cam.model,
                        sd_file=self.sd_file, cf_file=self.cf_file,
                        stdout=os.path.join(self.output_dir, "qemu.stdout"),
                        stderr=os.path.join(self.output_dir, "qemu.stderr"),
                        monitor_socket_path=self.qemu_monitor_path,
                        display=None,
                        verbose=self.verbose) as self.qemu_runner:
            q = self.qemu_runner

            q.screen_cap_prefix = "menu_test_"
            for k in key_sequence:
                q.key_press(k)
                try:
                    if q.expect_screen(self.expected_output_dir, timeout=10):
                        # saves last screen seen as q.screen_cap_name,
                        # returns False if this didn't match expected
                        pass
                    elif self.force_continue:
                        pass
                    else:
                        return self.return_failure("Qemu screen never matched against "
                                                   "expected result file")
                except FileNotFoundError:
                    if self.force_continue:
                        pass
                    else:
                        return self.return_failure("Missing expected output file: %s"
                                                   % expected_output_path)

            # attempt clean shutdown via Qemu monitor socket
            q.shutdown()
        #locking_print(f"PASS: {self.__class__.__name__}, {self.cam.model}", lock)
        return self.return_success()

