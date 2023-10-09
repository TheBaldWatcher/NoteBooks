#
# libopencv_INCLUDE_DIRS
# libopencv_LIBS
# libopencv_DYLIBS

set(_ROOT "${3RDPARTY_ROOT}/libopencv")
set(_NAME libopencv)

if(MSVC)
    set(libopencv_ROOT "${_ROOT}/win")
    if("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
		set(libopencv_ROOT "${libopencv_ROOT}/x86")
    else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8") 
		set(libopencv_ROOT "${libopencv_ROOT}/x64")
    endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")

    set(libopencv_INCLUDE_DIRS "${libopencv_ROOT}/include" "${libopencv_ROOT}/include/opencv")
    
	foreach(opencv_module opencv_video345 opencv_calib3d345 libpng libjpeg-turbo opencv_imgcodecs345 opencv_features2d345 opencv_imgproc345 opencv_core345 zlib)
        add_library(${opencv_module} STATIC IMPORTED)
        set_target_properties(${opencv_module} PROPERTIES
            IMPORTED_CONFIGURATIONS "Debug;Release"
            IMPORTED_LOCATION_DEBUG "${libopencv_ROOT}/lib/${opencv_module}d.lib"
            IMPORTED_LOCATION_RELEASE "${libopencv_ROOT}/lib/${opencv_module}.lib"
        )
	endforeach(opencv_module)
    set(libopencv_LIBS opencv_video345 opencv_calib3d345 libpng libjpeg-turbo opencv_imgcodecs345 opencv_features2d345 opencv_imgproc345 opencv_core345 zlib)
else()
    if(ANDROID)
        set(libopencv_ROOT "${_ROOT}/android/${ANDROID_ABI}/")

        # debug properties
        add_library(libopencv_core STATIC IMPORTED)
        set_target_properties(libopencv_core PROPERTIES
            IMPORTED_LOCATION "${libopencv_ROOT}/lib/libopencv_core.a"
        )

        add_library(libopencv_imgproc STATIC IMPORTED)
        set_target_properties(libopencv_imgproc PROPERTIES
            IMPORTED_LOCATION "${libopencv_ROOT}/lib/libopencv_imgproc.a"
        )
    elseif(APPLE)
        if(IOS_DEPLOYMENT_TARGET) # IOS 
            set(libopencv_ROOT "${_ROOT}/iOS")
        else()              # macOS
            set(libopencv_ROOT "${_ROOT}/macos/")
        endif()
        # debug properties
        set(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG "-Wl,-rpath,")
        add_library(libopencv SHARED IMPORTED)
        set_target_properties(libopencv PROPERTIES
            IMPORTED_LOCATION "${libopencv_ROOT}/opencv2.framework/opencv2"
        )
        set(libopencv_LIBS libopencv)
    else()
        if("${LINUX_DISTRIBUTION}" STREQUAL "ubuntu")
            set(libopencv_ROOT "${_ROOT}/ubuntu/")
        elseif("${LINUX_DISTRIBUTION}" STREQUAL "centos")
            set(libopencv_ROOT "${_ROOT}/centos/")
        endif()
        add_library(libopencv_core SHARED IMPORTED)
        set_target_properties(libopencv_core PROPERTIES
            IMPORTED_LOCATION "${libopencv_ROOT}/lib/libopencv_core.a")
        add_library(libopencv_imgproc SHARED IMPORTED)
        set_target_properties(libopencv_imgproc PROPERTIES
            IMPORTED_LOCATION "${libopencv_ROOT}/lib/libopencv_imgproc.a")
	set(libopencv_INCLUDE_DIRS "${libopencv_ROOT}/include" )
        set(libopencv_LIBS libopencv_imgproc libopencv_core)
    endif()
    if(APPLE)
        set(libopencv_INCLUDE_DIRS "${libopencv_ROOT}/opencv2.framework" )
    else()
        set(libopencv_INCLUDE_DIRS "${libopencv_ROOT}/include" )
    endif()

    if(ANDROID)
        set(libopencv_LIBS libopencv_imgproc libopencv_core)
    endif()

endif()
file(GLOB libopencv_DYLIBS "${libopencv_ROOT}/lib/*.dll" "${libopencv_ROOT}/lib/*.so" "${libopencv_ROOT}/lib/*.dylib")
