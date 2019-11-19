# libmtimage_INCLUDE_DIRS
# libmtimage_LIBS

set(_ROOT_libmtimage "${3RDPARTY_ROOT}/libmtimage")

if(MSVC)
	set(libmtimage_INCLUDE_DIRS "${_ROOT_libmtimage}/lib/Windows/include")
	if("${CMAKE_SIZEOF_VOID_P}" EQUAL "4") 
         set(_ROOT_libmtimage "${_ROOT_libmtimage}/lib/Windows/v150/Win32")
    else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8") 
         set(_ROOT_libmtimage "${_ROOT_libmtimage}/lib/Windows/v150/x64")
    endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "4") 
    # debug properties
    add_library(libmtimage SHARED IMPORTED)

     set_property(TARGET libmtimage APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
     set_target_properties(libmtimage PROPERTIES
         IMPORTED_IMPLIB_DEBUG "${_ROOT_libmtimage}/lib/mtimaged.lib"
     )
     set_property(TARGET libmtimage APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
     set_target_properties(libmtimage PROPERTIES
         IMPORTED_IMPLIB_RELEASE "${_ROOT_libmtimage}/lib/mtimage.lib"
     )
     set(libmtimage_LIBS libmtimage)
else()
    if(ANDROID)
        if (MEITU_PHONE)
             set(libmtimage_INCLUDE_DIRS "${_ROOT_libmtimage}/include")
            set(libmtimage_ROOT "${_ROOT_libmtimage}/lib/android/${ANDROID_ABI}/lib")
            add_library(libmtimage STATIC IMPORTED)
            set_target_properties(libmtimage PROPERTIES
                IMPORTED_LOCATION "${libmtimage_ROOT}/libmtimage.a"
            )
        else()
            set(libmtimage_INCLUDE_DIRS "${_ROOT_libmtimage}/include")
            set(libmtimage_ROOT "${_ROOT_libmtimage}/lib/android/${ANDROID_ABI}/lib")
            add_library(libmtimage SHARED IMPORTED)
            set_target_properties(libmtimage PROPERTIES
                IMPORTED_LOCATION "${libmtimage_ROOT}/libmtimage.so"
            )
        endif()
    elseif(APPLE)
        if(IOS_DEPLOYMENT_TARGET) # IOS 
            set(libmtimage_ROOT "${_ROOT_libmtimage}/lib/iOS/lib/")
        else()              # macOS
            set(libmtimage_ROOT "${_ROOT_libmtimage}/lib/macOS/lib/")
        endif()
        set(libmtimage_INCLUDE_DIRS "${_ROOT_libmtimage}/include")
        add_library(libmtimage STATIC IMPORTED)
        set_target_properties(libmtimage PROPERTIES
            IMPORTED_LOCATION "${libmtimage_ROOT}/libmtimage.a"
        )
         # "${CMAKE_SOURCE_DIR}/3rdparty/ARM_NEON_2_x86_SSE")
    else()  
        if("${LINUX_DISTRIBUTION}" STREQUAL "ubuntu")
            set(libmtiamge_ROOT "${_ROOT_libmtiamge}/lib/ubuntu/lib")
        elseif("${LINUX_DISTRIBUTION}" STREQUAL "centos")
            set(libmtimage_ROOT "${_ROOT_libmtimage}/lib/centOS/lib")
        endif()
        set(libmtimage_INCLUDE_DIRS "${_ROOT_libmtimage}/include")
        add_library(libmtimage STATIC IMPORTED)
        set_target_properties(libmtimage PROPERTIES IMPORTED_LOCATION "${libmtimage_ROOT}/libmtimage.a")
    endif()
    set(libmtimage_LIBS libmtimage)
endif()
