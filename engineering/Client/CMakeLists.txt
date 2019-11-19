cmake_minimum_required(VERSION 2.6)
set(TARGET_NAME "DemoProject")
project(${TARGET_NAME})
add_definitions(-std=c++11)
add_definitions(-DLIB_TARGET_NAME=mtbodypose)

set(DEV_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/")
set(SHARED_FILE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/shared_files")
set(3RDPARTY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty")

#### include 3rdparty cmake ####
include("${SHARED_FILE_ROOT}/cmake/libManis-config.cmake")
include("${SHARED_FILE_ROOT}/cmake/libmtcvlite-config.cmake")
include("${SHARED_FILE_ROOT}/cmake/libyuv-config.cmake")
include("${SHARED_FILE_ROOT}/cmake/libmtimage-config.cmake")
include("${SHARED_FILE_ROOT}/cmake/libopencv-demo-config.cmake")
include("${DEV_SDK_ROOT}/libmtbodypose-config.cmake")


#### include headers ####
include_directories (
    ### 3rdparty ###
    ${libManis_INCLUDE_DIRS}
    ${libopencv_INCLUDE_DIRS}
    ${libmtcvlite_INCLUDE_DIRS}
    ${libmtimage_INCLUDE_DIRS}
    ${libyuv_INCLUDE_DIRS}
    
    ### demo ###
    ${3RDPARTY_ROOT}/OpenGL/stb_image/
message(====${3RDPARTY_ROOT}/OpenGL/stb_image/)

    ### src ###
    ${libmtbodypose_INCLUDE_DIRS}
    
)

add_executable(${TARGET_NAME} main.cpp ${SOURCE_FILES})
target_link_libraries(${TARGET_NAME}
    ${libManis_LIBS}
    ${libmtbodypose_LIBS}
    ${libopencv_LIBS}
    ${libmtcvlite_LIBS}
    ${libmtimage_LIBS}
    ${libyuv_LIBS}


)

if(APPLE)
target_link_libraries(${TARGET_NAME} "-framework Accelerate")
elseif(MSVC)
else()
target_link_libraries(${TARGET_NAME} pthread z rt)
endif()
