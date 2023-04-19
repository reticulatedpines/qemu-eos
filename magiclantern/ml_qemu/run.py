#!/usr/bin/env python3

import os
import shutil
import argparse
import subprocess
from time import sleep
import socket
import hashlib
import tempfile

import vncdotool
from vncdotool import api

class QemuRunnerError(Exception):
    pass


def get_default_dirs(cwd):
    """
    Given the working directory of the process wanting
    to obtain default ML dirs, return a dict:
    "qemu-build-dir": "path/to/qemu-eos-build"
    "rom-dir": "path/to/roms"
    "source-dir": "path/to/magiclantern-source"
    """
    if not os.path.isdir(cwd):
        raise QemuRunnerError("cwd wasn't a dir: %s" % cwd)

    # There are two locations we are expecting to run from;
    # 1) the repo location - qemu-eos/magiclantern/run_qemu.py
    # 2) the extracted build from the zip - qemu-eos-build/run_qemu.py
    #
    # Here we try to detect these cases and provide appropriate defaults.
    if os.path.split(os.getcwd())[1] == "qemu-eos-build":
        build_dir = "."
        rom_dir = os.path.join("..", "roms")
        source_dir = os.path.join("..", "magiclantern_simplified")
    else:
        build_dir = os.path.join("..", "..", "qemu-eos-build")
        rom_dir = os.path.join("..", "..", "roms")
        source_dir = os.path.join("..", "..", "magiclantern_simplified")

    return {"qemu-build-dir": build_dir,
            "rom-dir": rom_dir,
            "source-dir": source_dir}


def get_cam_path(source_dir, cam):
    platform_path = os.path.join(source_dir, "platform")
    platform_dirs = next(os.walk(platform_path))[1]

    for d in platform_dirs:
        if d.startswith(cam):
            return os.path.join(platform_path, d)
    return ""


def get_debugmsg_addr(source_dir, cam):
    """
    Extract address for DryosDebugMsg from stubs.S file
    """
    cam_path = get_cam_path(source_dir, cam)
    if cam_path:
        stubs_path = os.path.join(cam_path, "stubs.S")
    else:
        print("WARNING: no cam path found for cam: %s" % cam)
        return ""

    # we expect lines to look something like this:
    # NSTUB(    0x395c,  DryosDebugMsg) // 0xFFA50C3C - RAM_OFFSET
    # NSTUB(0xFFAA395C,  DryosDebugMsg)
    with open(stubs_path, "r") as f:
        for line in f.readlines():
            if "DryosDebugMsg" in line \
                    and line.startswith(("ARM32_FN", "NSTUB", "THUMB_FN")):
                addr = line.split(",")[0].split("(")[1].lower().strip()
                if addr.startswith("0x"):
                    addr_val = int(addr, 16)
                    # Mask out low bit in case of raw Thumb addr, Qemu requires actual addr
                    return hex(addr_val >> 1 << 1)
                else:
                    print("WARNING: found DryosDebugMsg but couldn't parse address")
                    return ""
    print("WARNING: no DryosDebugMsg found in stubs.S, can't enable debugmsg")
    return ""


class QemuRunner:
    """
    Context manager for running Qemu, this allows automatically
    cleaning up Qemu monitor socket via "with".  Entering the
    context starts Qemu, via Popen therefore non-blocking.

    You can control Qemu within the context, either via Qemu monitor,
    or VNC api.

    Leaving the context ends Qemu and cleans up (principally the
    monitor socket).

    If you fall out of the context, Qemu will be ended by terminating
    the process.  This is not generally what you want.  If you
    do something like: 'with QemuRunner() as q', then you can do
    q.qemu_process.wait() and it will block until the user ends Qemu.
    You can attempt a graceful power down of the VM via q.shutdown(),
    or tell Qemu to force a power down via q.shutdown(force=True)
    """
    def __init__(self, build_dir, rom_dir, source_dir,
                 cam,
                 sd_file="", cf_file="",
                 stdout="", stderr="",
                 serial_out="",
                 monitor_socket_path="",
                 vnc_display="",
                 verbose=False,
                 gdb_port=0,
                 boot=False,
                 d_args=[]):
        # TODO:
        # allow selecting drive images,
        # ensure Qemu test suite works in the same way with this vs run_canon_fw.sh,
        # improve boot / non-boot selection (currently, -M CAM,firmware=boot=0, a better way
        #   would be creating a proper qemu option group)
        # check for arm-softmmu subdir and fail gracefully if missing,
        # check for disk_images subdir, fail gracefully,
        # check for model specific rom subdir, fail gracefully
        self.rom_dir = rom_dir
        # FIXME make this a class property, can't remember syntax right now
        self.screen_cap_prefix = "test_"
        self.screen_cap_counter = 0
        self.screen_cap_name = None
        if monitor_socket_path:
            self.monitor_socket_path = monitor_socket_path
        else:
            self.monitor_socket_path = "qemu.monitor"
        self.monitor_socket = None # init'd in __enter__ after Qemu comes up

        if boot:
            model = cam + ",firmware=boot=1"
        else:
            model = cam + ",firmware=boot=0"

        self.verbose = verbose

        # in __enter__, we attach files to these,
        # if None, these are temp files
        self.stdout = stdout
        self.stderr = stderr

        self.qemu_command = [os.path.join(build_dir, "arm-softmmu", "qemu-system-arm"),
                             "-drive", "if=sd,file=" + sd_file,
                             "-drive", "if=ide,file=" + cf_file,
                             "-chardev", "socket,server,nowait,path=" + self.monitor_socket_path + ",id=monsock",
                             "-mon", "chardev=monsock,mode=readline",
                             "-name", cam,
                             "-M", model,
                            ]

        self.vnc_display = vnc_display
        if vnc_display:
            self.qemu_command.extend(["-vnc", vnc_display])
        else:
            self.vnc_client = None

        # We can instruct qemu to redirect serial output to file.
        # This changes stdout since it's no longer going there, but
        # allows clean comparison of only serial output.
        self.serial_out = serial_out
        if serial_out:
            self.qemu_command.extend(["-serial",  "file:" + str(serial_out)])

        self.gdb_port = gdb_port
        if gdb_port:
            self.qemu_command.extend(["-S", "-gdb", "tcp::" + str(1234)])

        self.d_args = d_args
        if "debugmsg" in d_args:
            self.debugmsg_addr = get_debugmsg_addr(source_dir, cam)
            if self.debugmsg_addr:
                self.qemu_command.extend(["-plugin",
                                os.path.join(build_dir, "arm-softmmu", "plugins",
                                             "libmagiclantern.so,arg=debugmsg_addr="
                                             + self.debugmsg_addr)])
        if d_args:
            d_args_str = ",".join(d_args)
            self.qemu_command.extend(["-d", d_args_str])

    def __enter__(self):
        qemu_env = os.environ
        # FIXME remove QEMU_EOS_WORKDIR and make it a proper qemu CLI option
        # TODO check if this class works outside of use as a context
        # manager, I suspect it doesn't.  Fix as appropriate, or make
        # failure explicit outside that usage?
        qemu_env["QEMU_EOS_WORKDIR"] = self.rom_dir
        kwargs = {"env":qemu_env,
                  "stdin":subprocess.PIPE}
        if self.verbose:
            # don't redirect stdout stderr, just spam console
            self.stdout = None
            self.stderr = None
            pass
        else:
            # capture stdout and stderr, this makes it quiet,
            # and allows processing the output.
            if self.stdout:
                self.stdout = open(self.stdout, "wb+")
            else:
                self.stdout = tempfile.TemporaryFile()
            if self.stderr:
                self.stderr = open(self.stderr, "wb+")
            else:
                self.stderr = tempfile.TemporaryFile()
            kwargs["stdout"] = self.stdout
            kwargs["stderr"] = self.stderr
        self.qemu_process = subprocess.Popen(self.qemu_command,
                                             **kwargs)
        # TODO: bit hackish, but we give some time for Qemu
        # to start.  This prevents problems with VNC access
        # happening before Qemu is up.  There should be a more
        # graceful way.  Check status via monitor socket possibly?
        sleep(5.5)
        # connect to VNC
        if self.vnc_display:
            try:
                self.vnc_client = vncdotool.api.connect(self.vnc_display)
                self.vnc_client.timeout = 6 # for use with e.g. expectScreen(),
                                            # throws TimeoutError
            except Exception as e:
                self._cleanup()
                raise(e)

        # connect to Qemu monitor
        self.monitor_socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.monitor_socket.connect(self.monitor_socket_path)

        # check it didn't die early
        if self.qemu_process.poll():
            if self.stdout:
                print("Qemu stdout:\n")
                self.stdout.seek(0)
                print(self.stdout.read().decode())
            if self.stderr:
                print("Qemu stderr:\n")
                self.stderr.seek(0)
                print(self.stderr.read().decode())
            self._cleanup()
            raise QemuRunnerError("Qemu died unexpectedly early")

        return self

    def __exit__(self, *args):
        self._cleanup()

    def _cleanup(self):
        """
        If Qemu dies during our init, we want to cleanup
        before the context manager ends, which will not happen
        if an exception is raised during __enter__.

        If we may encounter an unhandled exception, we should
        ensure this function is called first.
        """
        self.qemu_process.terminate()
        # trigger removal of temp files, if any
        if self.stdout:
            self.stdout.close()
            self.stdout = None
        if self.stderr:
            self.stderr.close()
            self.stderr = None
        if self.vnc_client:
            self.vnc_client.timeout = None
            self.vnc_client.disconnect()
        try:
            os.remove(self.monitor_socket_path)
        except FileNotFoundError:
            pass

    def shutdown(self, force=False):
        """
        Instructs Qemu to shut down the VM, via monitor socket.
        """
        if self.monitor_socket:
            pass
        else:
            return

        # check if socket is connected
        try:
            self.monitor_socket.getpeername()
        except OSError:
            # Getting here means socket existed, but not connected.
            # This shouldn't happen and suggests the connection broke.
            print("monitor socket not connected")
            return

        if force:
            self.monitor_socket.send(b"quit\n")
        else:
            self.monitor_socket.send(b"system_powerdown\n")
        self.monitor_socket.close()
        self.monitor_socket = None
        sleep(2)

    def key_press(self, key):
        """
        Use Qemu monitor to press a key in the VM.
        """
        #print("pressing: %s" % key)
        self.monitor_socket.send(b"sendkey " + key.encode() + b"\n")

    def capture_screen(self, delay):
        """
        Capture VM screen via VNC.
        """
        sleep(delay)
        n = self.screen_cap_counter
        self.screen_cap_counter += 1
        self.screen_cap_name = self.screen_cap_prefix + str(n).zfill(2) + ".png"

        self.vnc_client.captureScreen(self.screen_cap_name)
        sleep(0.1)
        return self.screen_cap_name

