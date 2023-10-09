set(_ROOT_photo_detector "${3RDPARTY_ROOT}/libmtphotodetector")

if(MSVC)
    if("${CMAKE_SIZEOF_VOID_P}" EQUAL "4") 
        set(_ROOT_photo_detector "${_ROOT_photo_detector}/Win/v150/x86")
    else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8") 
        set(_ROOT_photo_detector "${_ROOT_photo_detector}/Win/v150/x64")
    endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "4") 
	
	# debug properties
	set(libphotodetector_INCLUDE_DIRS "${_ROOT_photo_detector}/include")
    add_library(libmtphotodetector SHARED IMPORTED)

    set_property(TARGET libmtphotodetector APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(libmtphotodetector PROPERTIES
         IMPORTED_IMPLIB_DEBUG "${_ROOT_photo_detector}/lib/libmtphotodetectord.lib")
    set_property(TARGET libmtphotodetector APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
    set_target_properties(libmtphotodetector PROPERTIES
         IMPORTED_IMPLIB_RELEASE "${_ROOT_photo_detector}/lib/libmtphotodetector.lib")
    set(libphotodetector_LIBS libmtphotodetector)
else()

    if(ANDROID)
        if(MEITU_PHONE OR NOT BUILD_SHARED_LIBS) 
            set(libphotodetector_INCLUDE_DIRS "${_ROOT_photo_detector}/android_static_noJni/${ANDROID_ABI}/include")
            add_library(libmtphotodetector STATIC IMPORTED)
            set_target_properties(libmtphotodetector PROPERTIES
                IMPORTED_LOCATION "${_ROOT_photo_detector}/android_static_noJni/${ANDROID_ABI}/LIB/libmtphotodetector.a"
            )
        else()
            set(libphotodetector_INCLUDE_DIRS "${_ROOT_photo_detector}/android/${ANDROID_ABI}/include")
            add_library(libmtphotodetector SHARED IMPORTED)
            set_target_properties(libmtphotodetector PROPERTIES
                IMPORTED_LOCATION "${_ROOT_photo_detector}/android/${ANDROID_ABI}/LIB/libmtphotodetector.so"
            )
        endif()
    elseif(APPLE)

        if(IOS_DEPLOYMENT_TARGET) # IOS 
            set(lib_dir "${_ROOT_photo_detector}/iOS/lib/")
            set(libphotodetector_INCLUDE_DIRS "${_ROOT_photo_detector}/iOS/include")
        else()              # macOS
            set(lib_dir "${_ROOT_photo_detector}/macOS/lib/")
            set(libphotodetector_INCLUDE_DIRS "${_ROOT_photo_detector}/macOS/include")
        endif()
         # "${CMAKE_SOURCE_DIR}/3rdparty/ARM_NEON_2_x86_SSE")

        add_library(libmtphotodetector STATIC IMPORTED)

        set_target_properties(libmtphotodetector PROPERTIES
            IMPORTED_LOCATION "${lib_dir}/libmtphotodetector.a"
        )
    else()
        if("${LINUX_DISTRIBUTION}" STREQUAL "ubuntu")
            set(libphotodetector_INCLUDE_DIRS "${_ROOT_photo_detector}/include")
            set(libphoto_detector_lib_dir "${_ROOT_photo_detector}/ubuntu/lib")
        elseif("${LINUX_DISTRIBUTION}" STREQUAL "centos")
            set(libphotodetector_INCLUDE_DIRS "${_ROOT_photo_detector}/Centos/include")
            set(libphoto_detector_lib_dir "${_ROOT_photo_detector}/Centos/lib")
        endif()

        add_library(libmtphotodetector STATIC IMPORTED)

        set_target_properties(libmtphotodetector PROPERTIES
            IMPORTED_LOCATION "${libphoto_detector_lib_dir}/libmtphotodetector.so"
        )
    endif()

    set(libphotodetector_LIBS libmtphotodetector)
    message(==== "${libphoto_detector_lib_dir}/libmtphotodetector.a")
    message(==== ${libphoto_detector_lib_dir})


endif()
