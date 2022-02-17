#!/usr/bin/env python3

import os


class CamError(Exception):
    pass


class Cam(object):
    eos_cams = {"5D", "5D2", "5D3", "5D4", "6D", "6D2", "7D", "7D2",
                "40D", "50D", "60D", "70D", "77D", "80D",
                "400D", "450D", "500D", "550D", "600D", "650D", "700D", "750D", "760D", "800D",
                "100D", "200D", "1000D", "1100D", "1200D", "1300D", "EOSM", "EOSM2"}
    powershot_cams = {"EOSM3", "EOSM10", "EOSM5", "A1100"}
    gui_cams = {"5D2", "5D3", "6D", "40D", "50D", "60D", "70D",
                "450D", "500D", "550D", "600D", "650D", "700D",
                "100D", "1000D", "1100D", "1200D", "1300D", "EOSM", "EOSM2"}
    sd_cams = {"5D3", "5D4", "6D", "6D2", "60D", "70D", "77D", "80D",
               "450D", "500D", "550D", "600D", "650D", "700D", "750D", "760D", "800D",
               "100D", "200D", "1000D", "1100D", "1200D", "1300D", "EOSM", "EOSM2"}
    cf_cams = {"5D", "5D2", "5D3", "5D4", "7D", "7D2", "40D", "50D", "400D"}
    supported_cams = eos_cams.union(powershot_cams, gui_cams,
                                    sd_cams, cf_cams)

    def __init__(self, cam="", rom_dir=""):
        if not cam:
            raise CamError("No cam model given")

        if cam not in self.supported_cams:
            raise CamError("Model not in supported cams: %s" % cam)

        if not rom_dir:
            raise CamError("No rom dir given")

        rom_subdir = os.path.join(rom_dir, cam)
        if not os.path.isdir(rom_subdir):
            raise CamError("Rom subdir didn't exist: %s" % rom_subdir)

        # set camera traits
        self.model = cam
        self.can_emulate_gui = True if cam in self.gui_cams else False
        self.has_sd = True if cam in self.sd_cams else False
        self.has_cf = True if cam in self.cf_cams else False
        self.is_eos = True if cam in self.eos_cams else False
        self.is_powershot = True if cam in self.powershot_cams else False
        if self.is_powershot and self.is_eos:
            raise CamError("Cam shouldn't be Powershot and EOS! : %s" % cam)

        self.tests = []

    def run_tests(self):
        for t in self.tests:
            t.run()

    def __repr__(self):
        s = "Model: %s\n" % self.model

        pre = "\tCan" if self.can_emulate_gui else "\tCannot"
        s += pre + " emulate to GUI\n"

        if self.has_sd:
            s += "\tHas SD card\n"
        if self.has_cf:
            s += "\tHas CF card\n"

        return s
