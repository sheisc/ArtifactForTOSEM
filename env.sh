export LC_ALL=C

export BUSTKPATH=$(cd $(dirname $0);pwd)

# LLVM environments
export LLVM_INSTALL_PATH=$BUSTKPATH/llvm-7.0.0
export PATH=$LLVM_INSTALL_PATH/bin:$PATH
export LLVM_COMPILER=clang
export LLVM_DIR=$LLVM_INSTALL_PATH
export LD_LIBRARY_PATH=$LLVM_INSTALL_PATH/lib/


#export LD_PRELOAD=$AFL_PATH/libbustk.so

# Intel Pin environments
export PIN_ROOT=$BUSTKPATH/pin-3.13
export PATH=${PIN_ROOT}:${PATH}
export PINTOOLS=${PIN_ROOT}/source/tools

