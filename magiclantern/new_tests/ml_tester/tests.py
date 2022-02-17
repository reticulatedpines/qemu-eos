#!/usr/bin/env python3

from abc import ABC

class TestError(Exception):
    pass


class Test(ABC):
    """
    Simple abstract base class, so all tests share run()
    """
    def __init__(self, cam, qemu_dir):
        self.cam = cam
        self.qemu_dir = qemu_dir

    def run(self):
        pass


class MenuTest(Test):
    """
    This test steps through Canon menus in Qemu, ensuring they look how we expect,
    and tries to cleanly shutdown the cam.

    ML is not active.
    """
    def run(self):
        print("MenuTest ran on %s" % self.cam.model)
        # 
