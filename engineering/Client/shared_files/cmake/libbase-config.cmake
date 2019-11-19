#
# libbase_INCLUDE_DIRS
# libbase_LIBS
# libbase_DYLIBS
#

set(_ROOT_base "${3RDPARTY_ROOT}/androidcore-basesharedlibs")

if(MSVC)

else()

    if(ANDROID)
        set(libbase_LIBS_ROOT "${_ROOT_base}/baselib/${ANDROID_ABI}")

        add_library(libstatic-mtbase STATIC IMPORTED)
        set_target_properties(libstatic-mtbase PROPERTIES
            IMPORTED_LOCATION "${libbase_LIBS_ROOT}/libstatic-mtbase.a"
        )

        add_library(libandroid-image SHARED IMPORTED)
        set_target_properties(libandroid-image PROPERTIES
            IMPORTED_LOCATION "${libbase_LIBS_ROOT}/libandroid-image.so"
        )

        add_library(libmttypes SHARED IMPORTED)
        set_target_properties(libmttypes PROPERTIES
            IMPORTED_LOCATION "${libbase_LIBS_ROOT}/libmttypes.so"
        )

        set(libbase_INCLUDE_DIRS "${_ROOT_base}/include" "${_ROOT_base}")
        set(libbase_LIBS libmttypes)
        
    elseif(APPLE)

    else()

    endif()

endif()
