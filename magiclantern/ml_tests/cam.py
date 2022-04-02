#!/usr/bin/env python3

import os
import hashlib


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
        
        # find rom(s)
        # TODO handle cams that only have 1 rom, also SFDATA
        rom0_path = os.path.join(rom_subdir, "ROM0.BIN")
        if not os.path.isfile(rom0_path):
            raise CamError("Couldn't find ROM0: %s" % rom0_path)
        self.rom0_path = rom0_path
        with open(rom0_path, "rb") as f:
            self.rom0_md5 = hashlib.md5(f.read()).hexdigest()

        rom1_path = os.path.join(rom_subdir, "ROM1.BIN")
        if not os.path.isfile(rom1_path):
            raise CamError("Couldn't find ROM1: %s" % rom1_path)
        self.rom1_path = rom1_path
        with open(rom1_path, "rb") as f:
            self.rom1_md5 = hashlib.md5(f.read()).hexdigest()

        # determine Digic version
        if cam in {"50D", "7D", "60D", "500D", "550D", "600D",
                   "1100D", "1200D", "1300D", "2000D", "4000D",
                   "5D2"}:
            self.digic_version = 4
        elif cam in {"5D3", "6D", "70D", "650D",
                     "700D", "100D", "EOSM", "EOSM2"}:
            self.digic_version = 5
        elif cam in {"5D4", "7D2", "80D", "750D", "760D"}:
            self.digic_version = 6
        elif cam in {"6D2", "77D", "800D", "200D"}:
            self.digic_version = 7
        elif cam in {"90D", "850D", "250D", "R", "RP",
                     "M50"}:
            self.digic_version = 8
        else:
            raise CamError("Fix laziness: mapping cam name to Digic version")

        # choose which is code rom
        if self.digic_version in [4, 5]:
            self.code_rom_md5 = self.rom1_md5
        elif self.digic_version in [7, 8, 10]:
            self.code_rom_md5 = self.rom0_md5
        else:
            raise CamError("Fix laziness: which rom is code rom for D6?")

        # set camera traits
        self.model = cam
        self.rom_dir = os.path.realpath(rom_dir)
        self.can_emulate_gui = True if cam in self.gui_cams else False
        self.has_sd = True if cam in self.sd_cams else False
        self.has_cf = True if cam in self.cf_cams else False
        self.is_eos = True if cam in self.eos_cams else False
        self.is_powershot = True if cam in self.powershot_cams else False
        if self.is_powershot and self.is_eos:
            raise CamError("Cam shouldn't be Powershot and EOS! : %s" % cam)

        self.tests = []

    def run_tests(self):
        for test in self.tests:
            with test as t:
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
