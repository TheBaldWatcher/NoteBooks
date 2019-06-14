#!/bin/bash

set -e

current_dir=$(cd `dirname $0`; pwd)
echo $current_dir
pushd $current_dir

#设置外部第三方库目录，可以将第三方库都放在这个目录下，视个人目录而定
#TODO: change me

#由于mac输出为静态库，因此不进行符号显示和隐藏的操作画
# 项目名称
export PROJECT_NAME="MacRealtimeSegment"
# 静态库OFF, 动态库ON
export BUILD_SHARED_LIBS="OFF"
# 隐藏符号ON, 不隐藏符号OFF
export BUILD_HIDDEN_SYMBOL="ON"

# hidden symbol
if [ "$BUILD_HIDDEN_SYMBOL" != "OFF" ]; then
    COMMON_FLAGS="$COMMON_FLAGS -fvisibility=hidden -fvisibility-inlines-hidden"
fi

export BUILD_C_FLAGS="$BUILD_C_FLAGS $COMMON_FLAGS "
export BUILD_CXX_FLAGS="$BUILD_CXX_FLAGS $COMMON_FLAGS"

# 编译安装目录
OUTPUT_DIR=$current_dir/libs_without_symbol/macOS
BUILD_DIR=$current_dir/build/macOS

rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR
pushd $BUILD_DIR
export CMAKE_CXX_FLAGS="$BUILD_CXX_FLAGS "
export CMAKE_C_FLAGS="$BUILD_C_FLAGS "

cmake \
-DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS -g -O3 -fPIC -std=c++11" \
-DBUILD_SHARED_LIBS="$BUILD_SHARED_LIBS"          \
-DTARGET_OS_MAC="1"          \
$PARAM_TO_CMAKE \
-DAPPLE="APPLE"         \
$current_dir/.

make all -j8
rm -rf $OUTPUT_DIR
make install/strip DESTDIR=$OUTPUT_DIR
popd # $BUILD_DIR

mv $OUTPUT_DIR/usr/local/* $OUTPUT_DIR
rm -rf $OUTPUT_DIR/usr