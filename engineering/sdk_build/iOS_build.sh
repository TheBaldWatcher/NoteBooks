#!/bin/sh
set -e

current_dir=$(cd `dirname $0`; pwd)
echo $current_dir

ALL_ARCHS="armv7 arm64 x86_64"

export IOS_DEPLOYMENT_TARGET=8.0
export BUILD_TYPE=Release
# 静态库OFF, 动态库ON
export BUILD_SHARED_LIBS="OFF"
# 隐藏符号ON, 不隐藏符号OFF
export BUILD_HIDDEN_SYMBOL="ON"
# 项目名称
export PROJECT_NAME="mtphotosegment"

export COMMON_FLAGS="-g"
export BUILD_C_FLAGS="$COMMON_FLAGS "
export BUILD_CXX_FLAGS="$COMMON_FLAGS -std=c++11"
# hidden symbol
if [ "$BUILD_HIDDEN_SYMBOL" != "OFF" ]; then
    BUILD_C_FLAGS="$BUILD_C_FLAGS -fvisibility=hidden -fvisibility-inlines-hidden"
    BUILD_CXX_FLAGS="$BUILD_CXX_FLAGS -fvisibility=hidden -fvisibility-inlines-hidden"
    CMAKE_MODULE_LINKER_FLAGS="-Xlinker -bitcode_verify -Xlinker -bitcode_hide_symbols"
    CMAKE_SHARED_LINKER_FLAGS="-Xlinker -bitcode_verify -Xlinker -bitcode_hide_symbols"
fi
export LIBRARY_POSTFIX="a"
if [ "$BUILD_SHARED_LIBS" != "OFF" ]; then
    LIBRARY_POSTFIX="framework"
fi

export BUILD_ROOT_DIR="$current_dir/libs_with_symbol/iOS/"
export OUT_ROOT_DIR="$current_dir/libs_with_symbol/iOS/AllArch"
export OUT_UNIVERSAL_SYM_DIR="$current_dir/libs_with_symbol/iOS"
export OUT_UNIVERSAL_DIR="$current_dir/libs_without_symbol/iOS"

do_build()
{
    IOS_ARCH=$1

    CMAKE_C_FLAGS="$BUILD_C_FLAGS "
    CMAKE_CXX_FLAGS="$BUILD_CXX_FLAGS "

    BUILD_PATH="$BUILD_ROOT_DIR/$IOS_ARCH"
    rm -rf $BUILD_PATH
    mkdir -p $BUILD_PATH
    pushd $BUILD_PATH

    cmake -DCMAKE_TOOLCHAIN_FILE=$current_dir/iOS.cmake            \
          -DIOS_DEPLOYMENT_TARGET="$IOS_DEPLOYMENT_TARGET"         \
          -DCMAKE_OSX_ARCHITECTURES="$IOS_ARCH"                    \
          -DCMAKE_INSTALL_PREFIX="/"                               \
          -DCMAKE_BUILD_TYPE="$BUILD_TYPE"                         \
          -DCMAKE_C_FLAGS="$CMAKE_C_FLAGS"                         \
          -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS"                     \
          -DBUILD_SHARED_LIBS="$BUILD_SHARED_LIBS"                 \
          -DCMAKE_MODULE_LINKER_FLAGS="$CMAKE_MODULE_LINKER_FLAGS" \
          -DCMAKE_SHARED_LINKER_FLAGS="$CMAKE_SHARED_LINKER_FLAGS" \
          -DTARGET_OS_IPHONE="1"                                   \
          $current_dir

    make -j8
    rm -rf $OUT_ROOT_DIR/$IOS_ARCH
    make -j8 install DESTDIR=$OUT_ROOT_DIR/$IOS_ARCH

    popd
}

do_lipo()
{
    if [ "$BUILD_SHARED_LIBS" != "OFF" ]; then
        rm -rf $OUT_UNIVERSAL_DIR/$PROJECT_NAME.framework
        mkdir -p $OUT_UNIVERSAL_DIR/$PROJECT_NAME.framework
        LIB_FILE=$PROJECT_NAME.framework/$PROJECT_NAME
        LIPO_FILE=$OUT_UNIVERSAL_DIR/$LIB_FILE
    else
        rm -rf $OUT_UNIVERSAL_DIR/lib
        mkdir -p $OUT_UNIVERSAL_DIR/lib
        LIB_FILE=lib$PROJECT_NAME.$LIBRARY_POSTFIX
        LIPO_FILE=$OUT_UNIVERSAL_DIR/lib/$LIB_FILE
    fi

    LIPO_FLAGS=
    LIPO_FLAGS_RELOCATION=
    for ARCH in $ALL_ARCHS
    do
        ARCH_LIB_FILE="$OUT_ROOT_DIR/$ARCH/lib/$LIB_FILE"
        echo $ARCH_LIB_FILE
        if [ -f "$ARCH_LIB_FILE" ]; then
            LIPO_FLAGS="$LIPO_FLAGS $ARCH_LIB_FILE"
        else
            echo "skip $LIB_FILE of $ARCH";
        fi
    done

    # merge static/shared library
    xcrun lipo -create $LIPO_FLAGS -output $LIPO_FILE
    xcrun lipo -info $LIPO_FILE

    if [ "$BUILD_SHARED_LIBS" != "OFF" ]; then
        # install_name
        install_name_tool -id @rpath/$LIB_FILE $LIPO_FILE

        # create dSYM file
        dsymutil $OUT_UNIVERSAL_DIR/$LIB_FILE -o $OUT_UNIVERSAL_SYM_DIR/$LIB_FILE.dSYM
        xcrun strip -x $OUT_UNIVERSAL_DIR/$LIB_FILE
        cp $OUT_UNIVERSAL_DIR/$LIB_FILE $OUT_UNIVERSAL_SYM_DIR/$PROJECT_NAME.framework
    fi
}

cp_headers()
{
    if [ "$BUILD_SHARED_LIBS" != "OFF" ]; then
        rm -rf $OUT_UNIVERSAL_DIR/$PROJECT_NAME.framework/Headers
        mkdir -p $OUT_UNIVERSAL_DIR/$PROJECT_NAME.framework/Headers
        ANY_ARCH=
        for ARCH in $ALL_ARCHS
        do
            ARCH_INC_DIR="$OUT_ROOT_DIR/$ARCH/include"
            if [ -d "$ARCH_INC_DIR" ]; then
                if [ -z "$ANY_ARCH" ]; then
                    ANY_ARCH=$ARCH
                    cp -R "$ARCH_INC_DIR/"* "$OUT_UNIVERSAL_DIR/$PROJECT_NAME.framework/Headers/"
                fi
            fi
        done
    else
        rm -rf $OUT_UNIVERSAL_DIR/include
        ANY_ARCH=
        for ARCH in $ALL_ARCHS
        do
            ARCH_INC_DIR="$OUT_ROOT_DIR/$ARCH/include"
            if [ -d "$ARCH_INC_DIR" ]; then
                if [ -z "$ANY_ARCH" ]; then
                    ANY_ARCH=$ARCH
                    cp -R "$ARCH_INC_DIR" "$OUT_UNIVERSAL_DIR/"
                fi
            fi
        done
    fi

    # 裁剪头文件，移除一些预编译条件、移除private部分
    # python3.6 ${current_dir}/header_crop.pyc ${OUT_UNIVERSAL_DIR}/include/MTPhotoSegment.hpp ${OUT_UNIVERSAL_DIR}/include/MTPhotoSegment.hpp
}

cp_info_plist()
{
    if [ "$BUILD_SHARED_LIBS" != "OFF" ]; then
        mkdir -p $OUT_UNIVERSAL_DIR/$PROJECT_NAME.framework
        ANY_ARCH=
        for ARCH in $ALL_ARCHS
        do
            ARCH_INFO_PLIST="$OUT_ROOT_DIR/$ARCH/lib/$PROJECT_NAME.framework/Info.plist"
            if [ -f "$ARCH_INFO_PLIST" ]; then
                if [ -z "$ANY_ARCH" ]; then
                    ANY_ARCH=$ARCH
                    cp "$ARCH_INFO_PLIST" "$OUT_UNIVERSAL_DIR/$PROJECT_NAME.framework/"
                fi
            fi
        done
    fi
}

for ARCH in $ALL_ARCHS
do
    do_build $ARCH
done

do_lipo
cp_headers
cp_info_plist

