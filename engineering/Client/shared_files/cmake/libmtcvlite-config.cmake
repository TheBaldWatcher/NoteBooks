#libmtcvlite_INCLUDE_DIRS
#libmtcvlite_LIBS

set(_ROOT_libmtcvlite "${3RDPARTY_ROOT}/libmtcvlite")

if(MSVC)

    if("${CMAKE_SIZEOF_VOID_P}" EQUAL "4") 
         set(_ROOT_libmtcvlite "${_ROOT_libmtcvlite}/Win/v150/Win32")
    else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8") 
         set(_ROOT_libmtcvlite "${_ROOT_libmtcvlite}/Win/v150/x64")
    endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "4") 
    # debug properties
	set(libmtcvlite_INCLUDE_DIRS "${_ROOT_libmtcvlite}/include" "${_ROOT_libmtcvlite}/include/mtcvlite")
    add_library(libmtcvlite STATIC IMPORTED)

    set_property(TARGET libmtcvlite APPEND PROPERTY IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG DEBUG)
    set_target_properties(libmtcvlite PROPERTIES
         IMPORTED_LOCATION_DEBUG "${_ROOT_libmtcvlite}/lib/libmtcvlited.lib")
    set_property(TARGET libmtcvlite APPEND PROPERTY IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE RELEASE)
    set_target_properties(libmtcvlite PROPERTIES
         IMPORTED_LOCATION_RELEASE "${_ROOT_libmtcvlite}/lib/libmtcvlite.lib")
    set(libmtcvlite_LIBS libmtcvlite)

else()

    if(ANDROID)
        if (MEITU_PHONE)
            set(libmtcvlite_ROOT "${_ROOT_libmtcvlite}/android/${ANDROID_ABI}/")
            set(libmtcvlite_INCLUDE_DIRS "${libmtcvlite_ROOT}/include" "${libmtcvlite_ROOT}/include/mtcvlite" "${libmtcvlite_ROOT}/include/mtnetlite")

            add_library(libmtcvlite STATIC IMPORTED)

            set_target_properties(libmtcvlite PROPERTIES
                IMPORTED_LOCATION "${libmtcvlite_ROOT}/lib/libmtcvlite.a"
            )
        else ()
            set(libmtcvlite_ROOT "${_ROOT_libmtcvlite}/android/${ANDROID_ABI}/")
            set(libmtcvlite_INCLUDE_DIRS "${libmtcvlite_ROOT}/include" "${libmtcvlite_ROOT}/include/mtcvlite" "${libmtcvlite_ROOT}/include/mtnetlite")

            add_library(libmtcvlite SHARED IMPORTED)

            set_target_properties(libmtcvlite PROPERTIES
                IMPORTED_LOCATION "${libmtcvlite_ROOT}/lib/libmtcvlite.so"
            )
        endif()
    elseif(APPLE)

        if(IOS_DEPLOYMENT_TARGET) # IOS 
            set(libmtcvlite_ROOT "${_ROOT_libmtcvlite}/iOS/")
            set(libmtcvlite_INCLUDE_DIRS "${_ROOT_libmtcvlite}/iOS/include" "${_ROOT_libmtcvlite}/iOS/include/mtcvlite")
        else()              # macOS
            set(libmtcvlite_ROOT "${_ROOT_libmtcvlite}/macOS/lib/")
            set(libmtcvlite_INCLUDE_DIRS "${_ROOT_libmtcvlite}/macOS/include" "${_ROOT_libmtcvlite}/macOS/include/mtcvlite")
        endif()
        add_library(libmtcvlite STATIC IMPORTED)
        set_target_properties(libmtcvlite PROPERTIES
            IMPORTED_LOCATION "${libmtcvlite_ROOT}/libmtcvlite.a"
        )
    else()

        if("${LINUX_DISTRIBUTION}" STREQUAL "ubuntu")
            set(libmtcvlite_ROOT "${_ROOT_libmtcvlite}/ubuntu/lib")
            set(libmtcvlite_INCLUDE_DIRS "${_ROOT_libmtcvlite}/ubuntu/include")
        elseif("${LINUX_DISTRIBUTION}" STREQUAL "centos")
            set(libmtcvlite_ROOT "${_ROOT_libmtcvlite}/centos/")
            set(libmtcvlite_INCLUDE_DIRS "${_ROOT_libmtcvlite}/centos/include" "${_ROOT_libmtcvlite}/centos/include/mtcvlite")
        endif()
        add_library(libmtcvlite STATIC IMPORTED)
        set_target_properties(libmtcvlite PROPERTIES
            IMPORTED_LOCATION "${libmtcvlite_ROOT}/lib/libmtcvlite.a"
        )
        
    endif()

    # debug properties
    
    set(libmtcvlite_LIBS libmtcvlite)

endif()
