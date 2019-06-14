cmake_minimum_required(VERSION 2.6)
project(mtphotosegment)
set(CMAKE_CXX_STANDARD 11)

set(DEV_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(SHARED_FILE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../shared_files")
set(3RDPARTY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty")

include (FindPkgConfig)
include (CheckCCompilerFlag)
include (CheckSymbolExists)

set(TARGET_NAME ${PROJECT_NAME})
add_definitions(-DBJ_SDK_BUILD_LIBRARY)
add_definitions(-DLIB_TARGET_NAME=${TARGET_NAME})
add_definitions(-DNEED_SUPPORT_CROP_IMAGE)  # 是否要有裁图逻辑，目前只有分割有这个需求
if (BUILD_SHARED_LIBS)  # 静态库隐藏符号，动态库则需要导出
    add_definitions(-DNEED_EXPORT_SYMBOL)
endif()
if (MT_PHONE)
    add_definitions(-DMT_PHONE)
endif()
if (INCLUDE_ASSET_CODE)
    add_definitions(-DINCLUDE_ASSET_CODE)
endif()
if (TARGET_OS_IPHONE)
    add_definitions(-DTARGET_OS_IPHONE)
endif()
if (DTARGET_OS_MAC)
    add_definitions(-DTARGET_OS_MAC)
endif()

# if (NOT APPLE)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections") 
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections") 
# endif(NOT APPLE)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # using Clang
    SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -dead_strip")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -dead_strip")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -dead_strip")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # using GCC
    SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--gc-sections")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # using Visual Studio C++
endif()

# # 请认真对待 **警告**
if(MSVC)
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /Wall")
     add_definitions(-DNOMINMAX)
# else()
#     # set(xxCMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
endif()

add_definitions(-DBJ_SDK_BUILD_LIBRARY)
if (MEITU_PHONE)
    add_definitions(-DMEITU_PHONE)
endif()

# RPATH
SET(CMAKE_SKIP_RPATH TRUE)
SET(CMAKE_SKIP_BUILD_RPATH TRUE)
SET(CMAKE_SKIP_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_DO_STRIP TRUE) # CACHE BOOL "If set to true, executables and libraries will be stripped when installing.")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)


# set(ANDROID_CPUFEATURE "${ANDROID_NDK}/sources/android/cpufeatures")

if(ANDROID)
    add_definitions(-DDYNAMIC_ES3)
endif(ANDROID)


#### include 3rdparty cmake
if (ANDROID AND NOT MEITU_PHONE)
    # include("${SHARED_FILE_ROOT}/cmake/libbase-config.cmake")
endif()
if (MSVC)
    add_definitions(-DGLEW_STATIC)
    include("${SHARED_FILE_ROOT}/cmake/libglew-config.cmake")
    include_directories (${libglew_INCLUDE_DIRS})
endif()
if (USE_MTNN)
    include("${SHARED_FILE_ROOT}/cmake/libmtnn-config.cmake")
else ()
    include("${SHARED_FILE_ROOT}/cmake/libManis-config.cmake")
endif(USE_MTNN)
include("${SHARED_FILE_ROOT}/cmake/libmtcvlite-config.cmake")
include("${SHARED_FILE_ROOT}/cmake/libyuv-config.cmake")
include("${SHARED_FILE_ROOT}/cmake/libmtimage-config.cmake")



#### include 3rdparty headers
include_directories (
    ${libmtnn_INCLUDE_DIRS}
    ${libManis_INCLUDE_DIRS}

    ${libmtcvlite_INCLUDE_DIRS}
    ${libmtimage_INCLUDE_DIRS}
    ${libyuv_INCLUDE_DIRS}

    ${DEV_SDK_ROOT}/src
    ${SHARED_FILE_ROOT}/headers
    ${SHARED_FILE_ROOT}/src

    # realtimesegment
    ${DEV_SDK_ROOT}/realtime_segment/src
    ${DEV_SDK_ROOT}/realtime_segment/src/srcManis
)
message(---||--${libmtcvlite_INCLUDE_DIRS})

# General source files
if (USE_MTNN)
    file(GLOB SOURCE_FILES
        ${DEV_SDK_ROOT}/src/*.cpp
        ${DEV_SDK_ROOT}/src/SrcMtnn/*.cpp

    )
else ()
    file(GLOB SOURCE_FILES
        ${DEV_SDK_ROOT}/src/*.cpp
        ${DEV_SDK_ROOT}/src/SrcManis/*.cpp
        ${SHARED_FILE_ROOT}/src/*.cpp

        # realtimesegment
        ${DEV_SDK_ROOT}/realtime_segment/src/*.cpp
        ${DEV_SDK_ROOT}/realtime_segment/src/srcManis/*.cpp
    )
endif()

if(ANDROID)
    if (INCLUDE_JNI_CODE)
        include_directories (
            ${SHARED_FILE_ROOT}/android_shared_files
            ${libbase_INCLUDE_DIRS}

            ${DEV_SDK_ROOT}/AndroidSdkDev/jni
            ${DEV_SDK_ROOT}/realtime_segment/AndroidSdkDev/jni
        )
        file(GLOB 
             SOURCE_FILES_JNI

             ${SHARED_FILE_ROOT}/android_shared_files/mtegl.cpp
             ${SHARED_FILE_ROOT}/android_shared_files/BitmapUtil.cpp

             ${DEV_SDK_ROOT}/realtime_segment/AndroidSdkDev/jni/*.cpp
             ${DEV_SDK_ROOT}/AndroidSdkDev/jni/*.cpp
        )
        list(APPEND
            SOURCE_FILES
            ${SOURCE_FILES_JNI}
        )

        if (INCLUDE_ASSET_CODE)
            file(GLOB 
                SOURCE_FILES_JNI
                ${SHARED_FILE_ROOT}/android_shared_files/AndroidAssetFileHelper.cpp
            )
            list(APPEND
                SOURCE_FILES
                ${SOURCE_FILES_JNI}
            )
        endif (INCLUDE_ASSET_CODE)
    endif (INCLUDE_JNI_CODE)
elseif(APPLE)
    file(GLOB 
         SOURCE_FILES_IOS 
         # plateform/iOS/*.mm
    )
    list(APPEND
         SOURCE_FILES
         ${SOURCE_FILES_IOS}
    )
elseif(MSVC)
    file(GLOB 
         SOURCE_FILES_ANDROID
         ${libglew_INCLUDE_DIRS}
    )
    list(APPEND
         SOURCE_FILES
         ${SOURCE_FILES_ANDROID}
    )
endif(ANDROID)

add_library(${TARGET_NAME} ${SOURCE_FILES})


set_target_properties(${TARGET_NAME} PROPERTIES 
    PREFIX "lib" 
    DEBUG_POSTFIX "d"
    OUTPUT_NAME "${TARGET_NAME}"
)


target_link_libraries(${TARGET_NAME}
    ${libmtnn_LIBS}
    ${libManis_LIBS}

#    ${pthreads_LIBS}
    ${libmtcvlite_LIBS}
    ${libmtimage_LIBS}
    ${libyuv_LIBS}
)

if(ANDROID)
    if (INCLUDE_JNI_CODE)
         target_link_libraries(${TARGET_NAME}
            ${libbase_LIBS}
            android
            jnigraphics
    )
    endif()
    if (BUILD_SHARED_LIBS)
        target_link_libraries(${TARGET_NAME}
            z
            log
            EGL
            GLESv2
        )
    endif(BUILD_SHARED_LIBS)
elseif(APPLE)
    target_link_libraries(${TARGET_NAME}
        ObjC
    )
elseif(MSVC)
    target_link_libraries(${TARGET_NAME}
        opengl32
	${libglew_LIBS} 
    )
endif(ANDROID)

# install library
install(TARGETS ${TARGET_NAME} DESTINATION "lib")

# install header
set(HEADER_FILES
    ${DEV_SDK_ROOT}/src/MTPhotoSegment.hpp
    ${DEV_SDK_ROOT}/realtime_segment/src/mt_realtime_segment.h
)
install(FILES ${HEADER_FILES} DESTINATION "include")

