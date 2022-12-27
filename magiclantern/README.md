
## Installation instructions

These instructions have been tested on Linux.  

Qemu is somewhat unusual in that it expects to be built from outside of the source dir.  This is clean, keeping the binaries separate from the code, with no risk of checking them in accidentally.

Clone both the ML and qemu-eos repos, from within the same directory:

```
git clone https://github.com/reticulatedpines/magiclantern_simplified
git clone https://github.com/reticulatedpines/qemu-eos
cd qemu-eos; git switch qemu-eos-v4.2.1; cd ..
```
Create a new directory, qemu-eos-build, also at the same level, and build qemu-eos from inside it:

```
mkdir qemu-eos-build
cd qemu-eos-build
../qemu-eos/configure --target-list=arm-softmmu --enable-plugins --disable-docs --enable-vnc --enable-gtk --disable-vte
make -j16
make plugins
cp tests/plugin/libmagiclantern.so arm-softmmu/plugins/
```
You can now run qemu, the main binary is `arm-softmmu/qemu-system-arm` - but you don't need to run this directly, and may want to have easy access to GDB scripts, depending on which model cam you're interested in, so:

```
cd ../qemu-eos/magiclantern
./run_qemu.py --help
```

If you make any changes to source in qemu-eos, from qemu-eos-build:

```
make -j16 && make plugins && cp tests/plugin/libmagiclantern.so arm-softmmu/plugins/
```


## Inventory

build_qemu_direct.py, build_qemu_docker.py:  
Either can be used to create a zip containing a standalone Qemu installation.  This is no use for dev work, but does allow running emulated cams.  It is intended for curious end-users, and for running the testing suite against a known clean build.

run_qemu.py:  
Runs Qemu with a valid set of parameters to emulate a cam.  Handles various error conditions, cleaning up temp files, etc.

run_tests.py:  
Runs the Qemu based regression test suite.  Currently, this is far from complete, only a proof-of-concept to replace the old, unworkable testing system.
