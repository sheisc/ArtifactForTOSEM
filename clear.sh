#!/bin/bash

#rm -rf ./llvm-7.0.0
#rm -rf ./pin-3.13

make -C src/ParallelShadowStacks/MyPinTool clean
make -C src/ParallelShadowStacks/BuddyStack clean
make -C src/timing clean

