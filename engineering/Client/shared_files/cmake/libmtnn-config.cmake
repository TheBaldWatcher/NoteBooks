#
# libmtnn_INCLUDE_DIRS
# libmtnn_LIBS
# libmtnn_DYLIBS

set(_ROOT "${3RDPARTY_ROOT}/libmtnn")
set(_NAME libmtnn)

if(MSVC)
    set(libmtnn_ROOT "${_ROOT}/Win")
    # windows' caffe depends on pthreads
    if("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
        if(MSVC12) 
            set(libmtnn_ROOT "${libmtnn_ROOT}/v120/Win32")
        else()
            set(libmtnn_ROOT "${libmtnn_ROOT}/v150/Win32")
        endif()
    else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8") 
        if(MSVC12)
            set(libmtnn_ROOT "${libmtnn_ROOT}/v120/x64")
        else()
            set(libmtnn_ROOT "${libmtnn_ROOT}/v150/x64")
        endif()
    endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "4") 
    
    # debug properties
    add_library(mtnn SHARED IMPORTED)

    set_property(TARGET mtnn APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(mtnn PROPERTIES
        IMPORTED_IMPLIB_DEBUG "${libmtnn_ROOT}/lib/mtnnd.lib"
    )
    set_property(TARGET mtnn APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
    set_target_properties(mtnn PROPERTIES
        IMPORTED_IMPLIB_RELEASE "${libmtnn_ROOT}/lib/mtnn.lib"
    )
    set(libmtnn_INCLUDE_DIRS "${libmtnn_ROOT}/include")
    set(libmtnn_LIBS mtnn)
    
else()
    if(ANDROID)
        set(libmtnn_ROOT "${_ROOT}/mtphone/${ANDROID_ABI}/")
        # debug properties
        add_library(libmtnn SHARED IMPORTED)
        set_target_properties(libmtnn PROPERTIES
        IMPORTED_LOCATION "${libmtnn_ROOT}/lib/libmtnn.so"
        )
    elseif(APPLE)
        if(IOS_DEPLOYMENT_TARGET) # IOS 
            set(libmtnn_ROOT "${_ROOT}/iOS-lite/universal")
        else()              # macOS
            set(libmtnn_ROOT "${_ROOT}/macOS/x64")
        endif()
        # debug properties
        set(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG "-Wl,-rpath,")
        add_library(libmtnn SHARED IMPORTED)
        set_target_properties(libmtnn PROPERTIES
            IMPORTED_LOCATION "${libmtnn_ROOT}/mtnn.framework/mtnn"
        )
    else()
        if("${LINUX_DISTRIBUTION}" STREQUAL "ubuntu")
            set(libmtnn_ROOT "${_ROOT}/ubuntu/x64")
        elseif("${LINUX_DISTRIBUTION}" STREQUAL "centos")
            set(libmtnn_ROOT "${_ROOT}/centos/x64")
        endif()
        # debug properties
        add_library(libmtnn SHARED IMPORTED)
        
        set_target_properties(libmtnn PROPERTIES
            IMPORTED_LOCATION "${libmtnn_ROOT}/lib/libmtnn.so"
        )
    endif()
    if(APPLE)
        set(libmtnn_INCLUDE_DIRS "${libmtnn_ROOT}/mtnn.framework" )
    else()
        set(libmtnn_INCLUDE_DIRS "${libmtnn_ROOT}/include" )
    endif()
    set(libmtnn_LIBS libmtnn)

endif()
file(GLOB libmtnn_DYLIBS "${libmtnn_ROOT}/lib/*.dll" "${libmtnn_ROOT}/lib/*.so" "${libmtnn_ROOT}/lib/*.dylib")