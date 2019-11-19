#
# libyuv_INCLUDE_DIRS
# libyuv_LIBS



set(_ROOT_libyuv "${3RDPARTY_ROOT}/libyuv")

if(MSVC)
    set(libyuv_INCLUDE_DIRS "${_ROOT_libyuv}/include")

    if("${CMAKE_SIZEOF_VOID_P}" EQUAL "4") 
        set(_ROOT_libyuv "${_ROOT_libyuv}/Win/lib/x86")
    else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8") 
        set(_ROOT_libyuv "${_ROOT_libyuv}/Win/lib/x64")
    endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "4") 
    # debug properties
    add_library(libyuv SHARED IMPORTED)

    set_property(TARGET libyuv APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(libyuv PROPERTIES
         IMPORTED_IMPLIB_DEBUG "${_ROOT_libyuv}/Debug/yuv.lib")
    set_property(TARGET libyuv APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
    set_target_properties(libyuv PROPERTIES
         IMPORTED_IMPLIB_RELEASE "${_ROOT_libyuv}/Release/yuv.lib")
    set(libyuv_LIBS libyuv)
else()

    if(ANDROID)
        if(MEITU_PHONE) 
            set(libyuv_ROOT "${_ROOT_libyuv}/Android/${ANDROID_ABI}/")
            set(libyuv_INCLUDE_DIRS "${_ROOT_libyuv}/include")
            add_library(libyuv STATIC IMPORTED)
            set_target_properties(libyuv PROPERTIES
                IMPORTED_LOCATION "${libyuv_ROOT}/libyuv.a"
            )
        else()
            set(libyuv_ROOT "${_ROOT_libyuv}/Android/${ANDROID_ABI}/")
            set(libyuv_INCLUDE_DIRS "${_ROOT_libyuv}/include")
            add_library(libyuv SHARED IMPORTED)
            set_target_properties(libyuv PROPERTIES
                IMPORTED_LOCATION "${libyuv_ROOT}/libyuv.so"
            )
        endif(MEITU_PHONE)
    elseif(APPLE)

        if(IOS_DEPLOYMENT_TARGET) # IOS 
            set(libyuv_ROOT "${_ROOT_libyuv}/iOS/lib/")
        else()              # macOS
            set(libyuv_ROOT "${_ROOT_libyuv}/macOS/lib/")
        endif()
        set(libyuv_INCLUDE_DIRS "${_ROOT_libyuv}/include")
         # "${CMAKE_SOURCE_DIR}/3rdparty/ARM_NEON_2_x86_SSE")

        add_library(libyuv STATIC IMPORTED)

        set_target_properties(libyuv PROPERTIES
            IMPORTED_LOCATION "${libyuv_ROOT}/libyuv.a"
        )
    else()
        if("${LINUX_DISTRIBUTION}" STREQUAL "ubuntu")
            set(libyuv_ROOT "${_ROOT_libyuv}/ubuntu/lib")
        elseif("${LINUX_DISTRIBUTION}" STREQUAL "centos")
            set(libyuv_ROOT "${_ROOT_libyuv}/CentOS/lib")
        endif()
        set(libyuv_INCLUDE_DIRS "${_ROOT_libyuv}/include")

        add_library(libyuv STATIC IMPORTED)

        set_target_properties(libyuv PROPERTIES
            IMPORTED_LOCATION "${libyuv_ROOT}/libyuv.a"
        )
    endif()

    set(libyuv_LIBS libyuv)

endif()
