#!/bin/bash

set -e

current_dir=$(cd `dirname $0`; pwd)
echo $current_dir
pushd $current_dir

# 静态库OFF, 动态库ON
export BUILD_SHARED_LIBS="ON"
# 隐藏符号ON, 不隐藏符号OFF
export BUILD_HIDDEN_SYMBOL="ON"
# 编译安装目录
#OUTPUT_DIR="$current_dir/libs_without_symbol/macOS"
BUILD_DIR="$current_dir/build/Centos"
# 公用FLAGS
# export COMMON_FLAGS="$COMMON_FLAGS -fPIC "
export COMMON_FLAGS="$COMMON_FLAGS  -g -O2 -fPIC -Wl,-Bsymbolic"
export BUILD_C_FLAGS="$BUILD_C_FLAGS $COMMON_FLAGS "
export BUILD_CXX_FLAGS="$BUILD_CXX_FLAGS $COMMON_FLAGS -std=c++11 "
# hidden symbol
if [ "$BUILD_HIDDEN_SYMBOL" != "OFF" ]; then
BUILD_C_FLAGS="$BUILD_C_FLAGS -fvisibility=hidden -fvisibility-inlines-hidden"
BUILD_CXX_FLAGS="$BUILD_CXX_FLAGS -fvisibility=hidden -fvisibility-inlines-hidden"
fi
rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR
pushd $BUILD_DIR

# 编译安装目录
BUILD_DIR="$current_dir/build/Centos"
rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR
pushd $BUILD_DIR
export CMAKE_CXX_FLAGS="$BUILD_CXX_FLAGS "
export CMAKE_C_FLAGS="$BUILD_C_FLAGS "

cmake -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS -g -O2 -fPIC -Wl,-Bsymbolic -std=c++11" \
      -DCMAKE_CXX_COMPILER_ID="GNU"                                                     \
	-DLINUX_DISTRIBUTION="centos"\
	-DCMAKE_C_FLAGS="$CMAKE_C_FLAGS"                  \
	-DCMAKE_C_FLAGS_RELEASE="-O3"                     \
	-DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS"              \
	-DCMAKE_CXX_FLAGS_RELEASE="-O3"                   \
	-DBUILD_SHARED_LIBS="$BUILD_SHARED_LIBS"                 \
      $current_dir/



make all -j8
NOSYMBOL_DIR=${current_dir}/libs_without_symbol/Centos/
SYMBOL_DIR=${current_dir}/libs_with_symbol/Centos/

INSTALL_DIR=${NOSYMBOL_DIR}
rm -rf $INSTALL_DIR
make install/strip DESTDIR=$INSTALL_DIR

if [ ! -d ${SYMBOL_DIR} ]; then
mkdir -p ${SYMBOL_DIR}
echo "${SYMBOL_DIR} created!."
fi
cp ${BUILD_DIR}/*/*.so ${SYMBOL_DIR}
popd # $BUILD_DIR

