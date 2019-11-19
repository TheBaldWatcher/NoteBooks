#
# libManis_INCLUDE_DIRS
# libManis_LIBS
# libManis_DYLIBS
# VMProtectSDK_LIBS

set(_ROOT "${3RDPARTY_ROOT}/libManis")
set(_NAME libManis)

if(MSVC) #do not support dll for now 
    set(libManis_ROOT "${_ROOT}/Win")
    # windows' caffe depends on pthreads
    if("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
		set(libManis_ROOT "${libManis_ROOT}/x86")
    else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8") 
		set(libManis_ROOT "${libManis_ROOT}/x64")
    endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "4") 
    
    # debug properties
    set(libManis_INCLUDE_DIRS "${libManis_ROOT}/include")
    add_library(libManis SHARED IMPORTED)

     set_property(TARGET libManis APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
     set_target_properties(libManis PROPERTIES
         IMPORTED_IMPLIB_DEBUG "${libManis_ROOT}/lib/Manisd.lib"
     )
     set_property(TARGET libManis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
     set_target_properties(libManis PROPERTIES
         IMPORTED_IMPLIB_RELEASE "${libManis_ROOT}/lib/Manis.lib"
     )
     set(libManis_LIBS libManis)
    
else()
    if(ANDROID)
        if(MEITU_PHONE) 
            set(libManis_ROOT "${_ROOT}/../libManisStatic/android/${ANDROID_ABI}/")
            # debug properties
            add_library(libManis STATIC IMPORTED)
            set_target_properties(libManis PROPERTIES
                IMPORTED_LOCATION "${libManis_ROOT}/lib/libManis.a"
            )
            
        else()
            set(libManis_ROOT "${_ROOT}/android/${ANDROID_ABI}/")
            # debug properties
            add_library(libManis SHARED IMPORTED)
            set_target_properties(libManis PROPERTIES
                IMPORTED_LOCATION "${libManis_ROOT}/lib/libManis.so"
            )
        endif()
    elseif(APPLE)
        if(IOS_DEPLOYMENT_TARGET) # IOS 
            set(libManis_ROOT "${_ROOT}/iOS/universal")
        else()              # macOS
            set(libManis_ROOT "${_ROOT}/macOS")
        endif()
        # debug properties
        set(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG "-Wl,-rpath,")
        add_library(libManis SHARED IMPORTED)
        set_target_properties(libManis PROPERTIES
            IMPORTED_LOCATION "${libManis_ROOT}/Manis.framework/Manis"
        )
    else()
        if("${LINUX_DISTRIBUTION}" STREQUAL "ubuntu")
            set(libManis_ROOT "${_ROOT}/ubuntu/")
        elseif("${LINUX_DISTRIBUTION}" STREQUAL "centos")
            set(libManis_ROOT "${_ROOT}/centos/")
        endif()
        # debug properties
        add_library(libManis SHARED IMPORTED)
        
        set_target_properties(libManis PROPERTIES
            IMPORTED_LOCATION "${libManis_ROOT}/lib/libManis.so"
        )
    endif()
    if(APPLE)
        set(libManis_INCLUDE_DIRS "${libManis_ROOT}/Manis.framework" )
    else()
        set(libManis_INCLUDE_DIRS "${libManis_ROOT}/include" )
    endif()
    set(libManis_LIBS libManis)

endif()
file(GLOB libManis_DYLIBS "${libManis_ROOT}/lib/*.dll" "${libManis_ROOT}/lib/*.so" "${libManis_ROOT}/lib/*.dylib")
