#!/usr/bin/env python3

test_group_names = {"menu", "log"}

def locking_print(s, lock=None):
    if lock:
        lock.acquire()
    try:
        print(s)
    finally:
        if lock:
            lock.release()

from . import test_suite
from . import menu_test
from . import fail_test
from . import cam
