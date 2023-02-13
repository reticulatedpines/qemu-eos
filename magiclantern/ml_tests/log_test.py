#!/usr/bin/env python3

import os
import hashlib
from time import sleep

from . import test
from . import locking_print
from ml_qemu.run import QemuRunner


class LogTest(test.Test):
    """
    This test checks stdout and stderr from Qemu,
    checking for some important strings, defined per cam,
    that indicate good emulation.

    This isn't a comparison against an exact log,
    but the strings must be found in order.

    ML is not active.
    """

    # Hashes of cam roms, and a set of lines we expect for that cam
    qemu_expected_lines = {
        "e6a90e8497c2c1187e0322010a42b9b5": # 5D3 ROM1
        [
            # early strings, emul is badly broken if these are missing
            "K285 READY",
            "K285 ICU Firmware Version 1.2.3 ( 9.0.1 )",

            # late strings; we emulated quite far
            "[STARTUP] startupInitializeComplete",
        ],

        "424545a5cfe10b1a5d8cefffe9fe5297": # 50D ROM1
        [
            "K261 READY",
            "K261 ICU Firmware Version 1.0.9 ( 3.4.5 )",

            "[STARTUP] startupInitializeComplete",
        ],

        "d266ce304585952fb3a05a9f6c304f2f": # 60D ROM1
        [
            "K287 READY",
            "K287 ICU Firmware Version 1.1.1 ( 3.3.7 )",

            "[STARTUP] startupInitializeComplete",
        ],

        "e06a0e3919ac4d4ef609a864e937a5d3": # 100D ROM1
        [
            "K346 READY",
            "K346 ICU Firmware Version 1.0.1 ( 4.4.0 )",

            "[STARTUP] startupInitializeComplete",
        ],

        "f6c20df071b3514fa65f35dc5d71b484": # 700D ROM1
        [
            "K326 READY",
            "K326 ICU Firmware Version 1.1.5 ( 3.0.2 )",

            "[STARTUP] startupInitializeComplete",
        ],

        "0a9fce1e4ef6d2ac2c3bc63eb96d3c34": # 500D ROM1
        [
            "K252 READY",
            "K252 ICU Firmware Version 1.1.1 ( 3.6.4 )",

            "[STARTUP] startupInitializeComplete",
        ]
    }

    def run(self, lock):
        self.lock = lock
        if self.verbose:
            locking_print("LogTest starting on %s %s" %
                  (self.cam.model, self.cam.code_rom_md5),
                  lock)

        if self.cam.model not in self.known_cams:
            return self.return_failure("No tests known for cam: %s"
                                       % self.cam.model)

        if self.cam.code_rom_md5 not in self.known_cams[self.cam.model]:
            return self.return_failure("Unknown rom for cam, MD5 sum: %s"
                                       % self.cam.code_rom_md5)

        if self.cam.code_rom_md5 not in self.qemu_expected_lines:
            return self.return_failure("Unknown rom for LogTest, MD5 sum: %s"
                                       % self.cam.code_rom_md5)

        expected_lines = self.qemu_expected_lines[self.cam.code_rom_md5]

        # Invoke qemu and control it to run the test.  Slightly ugly,
        # verbose is False so we can capture rather than display qemu output.
        with QemuRunner(self.qemu_dir, self.cam.rom_dir, self.cam.source_dir,
                        self.cam.model,
                        unreliable_screencaps=self.cam.unreliable_screencaps,
                        sd_file=self.sd_file, cf_file=self.cf_file,
                        stdout=os.path.join(self.output_dir, "qemu.stdout"),
                        stderr=os.path.join(self.output_dir, "qemu.stderr"),
                        monitor_socket_path=self.qemu_monitor_path,
                        vnc_display=self.vnc_display,
                        verbose=False) as self.qemu_runner:
            q = self.qemu_runner

            if q.stdout and q.stderr:
                pass
            else:
                return self.return_failure("Missing Qemu stdout and/or stderr capture file")

            # Allow some time for emulation, then shutdown and process output
            sleep(10)
            q.shutdown()

            # Qemu doesn't use stdout much (at all?)
            q.stdout.seek(0)
            stdout_text = q.stdout.read().decode()
            if stdout_text:
                return self.return_failure("Stdout was non-empty.  Content: \n\n" + stdout_text)

            # do a single pass through the logged output, checking for our strings
            q.stderr.seek(0)
            expected_lines_len = len(expected_lines)
            i = 0
            for line in q.stderr.readlines():
                if expected_lines[i] in line.decode():
                    i += 1
                    if i >= expected_lines_len:
                        break
            if i >= expected_lines_len:
                return self.return_success()
                

        #locking_print(f"PASS: {self.__class__.__name__}, {self.cam.model}", lock)
        return self.return_failure("Not all expected lines found in Qemu output.  "
                                   "First missing line:\n%s\n" % expected_lines[i])

