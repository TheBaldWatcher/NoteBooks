#!/bin/sh
set -e

work_dir=`pwd`
echo $work_dir
pushd $work_dir

# ANDROID_NDK env
if [ "$ANDROID_NDK" = "" ]; then
    ANDROID_NDK=$(dirname `which ndk-build`)
    echo "set ANDROID_NDK by path of ndk-buid : $ANDROID_NDK"
else
    echo "find ANDROID_NDK from system env : $ANDROID_NDK"
fi

if [ "$ANDROID_NDK" = "" ]; then
    echo "can't find ANDROID_NDK env and ndk-buid, please set before run"
    exit -1;
fi

#try to detect NDK version
NDK_REL=$(grep -i '^Pkg.Revision'  $ANDROID_NDK/source.properties | awk -F '= ' '{print $2}')
NDK_VER=$(echo $NDK_REL | awk -F '.' '{print $1}')
if [ "$NDK_VER" -lt "16" ]; then
    echo "You need the ndk-r16 or later"
    exit 1
fi

#export BUILD_TYPE=MinSizeRel
export BUILD_TYPE=Release
# 静态库OFF, 动态库ON
export BUILD_SHARED_LIBS="ON"
# 隐藏符号ON, 不隐藏符号OFF
export BUILD_HIDDEN_SYMBOL="ON"
# rtti switch
export BUILD_RTTI="ON"
# exceptions switch
export BUILD_EXCEPTIONS="ON"
# stltype: gnustl, stlport, c++
export ANDROID_STL_32BIT="gnustl_shared"
export ANDROID_STL_64BIT="c++_shared"
# Android API 版本
export ANDROID_PLATFORM="android-14"
# 强制使用gcc编译器, NDK r13已经默认使用clang编译器
export ANDROID_TOOLCHAIN_32BIT="gcc"
export ANDROID_TOOLCHAIN_64BIT="clang"
# 使用NDK r13以上版本
export CMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake"
# 编译的平台
export ALL_ARCHS="armeabi-v7a arm64-v8a" # x86 x86_64"

# set toolchain
# cmake 3.7以后支持Android交叉编译, 但变量是NDK R13前的方式，对于clang要特殊设置
if [ "$ANDROID_TOOLCHAIN" = "clang" ]; then
    CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION="clang"
fi

# 公用FLAGS
export COMMON_FLAGS="$COMMON_FLAGS "
export COMMON_FLAGS_RELEASE="-O3"

# hidden symbol
if [ "$BUILD_HIDDEN_SYMBOL" != "OFF" ]; then
    COMMON_FLAGS="$COMMON_FLAGS -fvisibility=hidden -fvisibility-inlines-hidden"
fi

# rtti
if [ "$BUILD_RTTI" != "OFF" ]; then
    COMMON_FLAGS="$COMMON_FLAGS -frtti"
else
    COMMON_FLAGS="$COMMON_FLAGS -fno-rtti"
fi

# excepitons
if [ "$BUILD_EXCEPTIONS" != "OFF" ]; then
    COMMON_FLAGS="$COMMON_FLAGS -fexceptions"
else
    COMMON_FLAGS="$COMMON_FLAGS -fno-exceptions"
fi

export BUILD_C_FLAGS="$BUILD_C_FLAGS $COMMON_FLAGS "
export BUILD_CXX_FLAGS="$BUILD_CXX_FLAGS $COMMON_FLAGS"

do_build()
{
    BUILD_ARCH=$1
    if [ "$BUILD_ARCH" == "armeabi-v7a" ]; then
        ANDROID_ABI="armeabi-v7a with NEON"
        ANDROID_STL=$ANDROID_STL_32BIT
        ANDROID_TOOLCHAIN=$ANDROID_TOOLCHAIN_32BIT
    else
        ANDROID_ABI="$BUILD_ARCH"
        ANDROID_STL=$ANDROID_STL_64BIT
        ANDROID_TOOLCHAIN=$ANDROID_TOOLCHAIN_64BIT
    fi
    # 编译安装目录
    ARCH_BUILD_DIR=$work_dir/build/android/$BUILD_ARCH
    #rm -rf $ARCH_BUILD_DIR
    mkdir -p $ARCH_BUILD_DIR
    pushd $ARCH_BUILD_DIR

    export CMAKE_CXX_FLAGS="$BUILD_CXX_FLAGS "
    export CMAKE_C_FLAGS="$BUILD_C_FLAGS "
    cmake -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION="$CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION" \
          -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE      \
          -DANDROID_NDK=$ANDROID_NDK                        \
          -DANDROID_PLATFORM="$ANDROID_PLATFORM"            \
          -DANDROID_ABI="$ANDROID_ABI"                      \
          -DCMAKE_INSTALL_PREFIX="/"                        \
          -DCMAKE_BUILD_TYPE=$BUILD_TYPE                    \
          -DANDROID_STL="$ANDROID_STL"                      \
          -DCMAKE_C_FLAGS="$CMAKE_C_FLAGS"                  \
          -DCMAKE_C_FLAGS_RELEASE="-O3"                     \
          -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS"              \
          -DCMAKE_CXX_FLAGS_RELEASE="-O3"                   \
          -DBUILD_SHARED_LIBS="$BUILD_SHARED_LIBS"          \
          -DANDROID_TOOLCHAIN="$ANDROID_TOOLCHAIN"          \
          -DINCLUDE_JNI_CODE="YES"                             \
          -DINCLUDE_ASSET_CODE="YES"                             \
          -DMEITU_PHONE="NO"                             \
          $work_dir/

    #cmake --build .
    make all -j8

    if [ "$BUILD_TYPE" == "Release" ]; then
        NOSYMBOL_DIR=${work_dir}/libs_without_symbol/android/${BUILD_ARCH}
        SYMBOL_DIR=${work_dir}/libs_with_symbol/Android/${BUILD_ARCH}/

        INSTALL_DIR=${NOSYMBOL_DIR}
        rm -rf $INSTALL_DIR
        make install/strip DESTDIR=$INSTALL_DIR

        # 备份一下带符号的库，用于日后debug
        if [ ! -d ${SYMBOL_DIR} ]; then
          mkdir -p ${SYMBOL_DIR}
          echo "${SYMBOL_DIR} created!."
        fi
        cp ${ARCH_BUILD_DIR}/*/*.so ${SYMBOL_DIR}

        # 将编译的so库放置到AS中的文件夹下
        ANDROID_STUDIO_DIR=${work_dir}/../AndroidSdkDev/SdkDevDemo/PhotoSegmentLibrary/jni_build/${BUILD_ARCH}
          if [ ! -d ${ANDROID_STUDIO_DIR} ]; then
            mkdir -p ${ANDROID_STUDIO_DIR}
            echo "${ANDROID_STUDIO_DIR} created!."
          fi
          cp ${NOSYMBOL_DIR}/*/*.so ${ANDROID_STUDIO_DIR}

    fi

    popd # $ARCH_BUILD_DIR
}

# 开始编译
for ARCH in $ALL_ARCHS
do
    do_build $ARCH 
done

popd # $work_dir
