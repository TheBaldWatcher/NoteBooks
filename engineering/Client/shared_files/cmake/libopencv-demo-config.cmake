#
# libopencv_INCLUDE_DIRS
# libopencv_LIBS
# libopencv_DYLIBS

set(_ROOT "${3RDPARTY_ROOT}/libOpencv4Demo")
set(_NAME libopencv)

if(MSVC)
    set(libopencv_ROOT "${_ROOT}/Win")
    # windows' caffe depends on pthreads
    if("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
        if(MSVC12) 
            set(libopencv_ROOT "${libopencv_ROOT}/v120/Win32")
        else()
            set(libopencv_ROOT "${libopencv_ROOT}/v150/Win32")
        endif()
    else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8") 
        if(MSVC12)
            set(libopencv_ROOT "${libopencv_ROOT}/v120/x64")
        else()
            set(libopencv_ROOT "${libopencv_ROOT}/v150/x64")
        endif()
    endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "4") 
    
    # debug properties
    add_library(opencv SHARED IMPORTED)

    set_property(TARGET opencv APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(opencv PROPERTIES
        IMPORTED_IMPLIB_DEBUG "${libopencv_ROOT}/lib/opencvd.lib"
    )
    set_property(TARGET opencv APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
    set_target_properties(opencv PROPERTIES
        IMPORTED_IMPLIB_RELEASE "${libopencv_ROOT}/lib/opencv.lib"
    )
    set(libopencv_INCLUDE_DIRS "${libopencv_ROOT}/include")
    set(libopencv_LIBS opencv)
    
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
            set(libopencv_ROOT "${_ROOT}/macOS/")
        endif()
        # debug properties
        set(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG "-Wl,-rpath,")
        add_library(libopencv SHARED IMPORTED)
        set_target_properties(libopencv PROPERTIES
            IMPORTED_LOCATION "${libopencv_ROOT}/lib/libopencv_world.dylib"
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
        set(libopencv_INCLUDE_DIRS "${libopencv_ROOT}/include" )
    else()
        set(libopencv_INCLUDE_DIRS "${libopencv_ROOT}/include" )
    endif()

    if(ANDROID)
        set(libopencv_LIBS libopencv_imgproc libopencv_core)
    endif()

endif()
file(GLOB libopencv_DYLIBS "${libopencv_ROOT}/lib/*.dll" "${libopencv_ROOT}/lib/*.so" "${libopencv_ROOT}/lib/*.dylib")
