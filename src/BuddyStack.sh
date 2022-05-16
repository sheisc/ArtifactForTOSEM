ulimit -s 32768
export AFL_PATH=/home/bdf/src/ParallelShadowStacks/BuddyStack
export AFL_RUSTC=/home/bdf/.rustup/toolchains/1.43-x86_64-unknown-linux-gnu/bin/rustc

export LC_ALL=C


#export AFL_KEEP_ASSEMBLY=1
#export CC=$AFL_PATH/afl-clang
#export CXX=$AFL_PATH/afl-clang++

export LLVM_INSTALL_PATH=/home/bdf/src/llvm7.0
export PATH=$LLVM_INSTALL_PATH/bin:$PATH
export PATH=$AFL_PATH:$PATH
export LD_PRELOAD=$AFL_PATH/libbustk.so



#export LD_PRELOAD=/home/bdf/src/ParallelShadowStacks/BuddyStack/libbustk.so


