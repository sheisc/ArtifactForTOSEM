#!/bin/bash

CUR_LLVM_DIR=llvm-7.0.0
CUR_PIN_DIR=pin-3.13


if [ ! -d $CUR_LLVM_DIR ]; then
    wget http://releases.llvm.org/7.0.0/clang+llvm-7.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz
    tar -xvf clang+llvm-7.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz
    mv clang+llvm-7.0.0-x86_64-linux-gnu-ubuntu-16.04 llvm-7.0.0
    rm -f clang+llvm-7.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz	
fi



if [ ! -d $CUR_PIN_DIR ]; then
    wget https://software.intel.com/sites/landingpage/pintool/downloads/pin-3.13-98189-g60a6ef199-gcc-linux.tar.gz
    tar -xvf pin-3.13-98189-g60a6ef199-gcc-linux.tar.gz
    mv pin-3.13-98189-g60a6ef199-gcc-linux pin-3.13
    rm -f pin-3.13-98189-g60a6ef199-gcc-linux.tar.gz
fi

make -C src/ParallelShadowStacks/BuddyStack
make -C src/ParallelShadowStacks/MyPinTool
make -C src/timing

# curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
# rustup self uninstall

# rustup toolchain install 1.43
# rustup default 1.43