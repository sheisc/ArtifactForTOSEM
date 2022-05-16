ulimit -s 32768


export BUSTKPATH=$(cd $(dirname $0);pwd)

. $BUSTKPATH/env.sh

export AFL_PATH=$BUSTKPATH/src/ParallelShadowStacks/BuddyStack
#export AFL_RUSTC=/home/bdf/.rustup/toolchains/1.43-x86_64-unknown-linux-gnu/bin/rustc
export AFL_RUSTC=`which rustc`
export PATH=$AFL_PATH:$PATH
export LD_PRELOAD=$AFL_PATH/libbustk.so


