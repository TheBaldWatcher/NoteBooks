set(LIB_NAME "libmtbodypose")
# ${LIB_NAME}_INCLUDE_DIRS
# ${LIB_NAME}_LIBS
# message(STATUS "===========")
set(_ROOT_${LIB_NAME} "${DEV_SDK_ROOT}/../../")

if(MSVC)
	if("${CMAKE_SIZEOF_VOID_P}" EQUAL "4") 
         set(_ROOT_${LIB_NAME} "${_ROOT_${LIB_NAME}}/Win/x86")
    else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8") 
         set(_ROOT_${LIB_NAME} "${_ROOT_${LIB_NAME}}/Win/x64")
    endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "4") 

	set(${LIB_NAME}_INCLUDE_DIRS "${_ROOT_${LIB_NAME}}/include")
    add_library(${LIB_NAME} SHARED IMPORTED)

     set_property(TARGET ${LIB_NAME} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
     set_target_properties(${LIB_NAME} PROPERTIES
         IMPORTED_IMPLIB_DEBUG "${_ROOT_${LIB_NAME}}/lib/${LIB_NAME}.lib"
     )
     set_property(TARGET ${LIB_NAME} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
     set_target_properties(${LIB_NAME} PROPERTIES
         IMPORTED_IMPLIB_RELEASE "${_ROOT_${LIB_NAME}}/lib/${LIB_NAME}.lib"
     )
     set(${LIB_NAME}_LIBS ${LIB_NAME})
else()
    if(ANDROID)
        if (MEITU_PHONE)
            set(${LIB_NAME}_INCLUDE_DIRS "${_ROOT_${LIB_NAME}}/include")
            set(${LIB_NAME}_ROOT "${_ROOT_${LIB_NAME}}/lib/android/${ANDROID_ABI}/lib")
            add_library(${LIB_NAME} STATIC IMPORTED)
            set_target_properties(${LIB_NAME} PROPERTIES
                IMPORTED_LOCATION "${${LIB_NAME}_ROOT}/${LIB_NAME}.a"
            )
        else()
            set(${LIB_NAME}_INCLUDE_DIRS "${_ROOT_${LIB_NAME}}/include")
            set(${LIB_NAME}_ROOT "${_ROOT_${LIB_NAME}}/lib/android/${ANDROID_ABI}/lib")
            add_library(${LIB_NAME} SHARED IMPORTED)
            set_target_properties(${LIB_NAME} PROPERTIES
                IMPORTED_LOCATION "${${LIB_NAME}_ROOT}/${LIB_NAME}.so"
            )
        endif()
    elseif(APPLE)
        if(IOS_DEPLOYMENT_TARGET) # IOS 
            set(${LIB_NAME}_ROOT "${_ROOT_${LIB_NAME}}/iOS/lib/")
        else()              # macOS
            set(_ROOT_${LIB_NAME} "${_ROOT_${LIB_NAME}}/macOS/")
        endif()
        set(${LIB_NAME}_INCLUDE_DIRS "${_ROOT_${LIB_NAME}}/include")
        add_library(${LIB_NAME} STATIC IMPORTED)
        set_target_properties(${LIB_NAME} PROPERTIES
            IMPORTED_LOCATION "${_ROOT_${LIB_NAME}}/lib/${LIB_NAME}.a"
        )
    else()  
        if("${LINUX_DISTRIBUTION}" STREQUAL "ubuntu")
            set(${LIB_NAME}_ROOT "${_ROOT_${LIB_NAME}}/lib/ubuntu/lib")
        elseif("${LINUX_DISTRIBUTION}" STREQUAL "centos")
            set(_ROOT_${LIB_NAME} "${_ROOT_${LIB_NAME}}/centos")
        endif()
        set(${LIB_NAME}_INCLUDE_DIRS "${_ROOT_${LIB_NAME}}/include")
        add_library(${LIB_NAME} SHARED IMPORTED)
        set_target_properties(${LIB_NAME} PROPERTIES IMPORTED_LOCATION "${_ROOT_${LIB_NAME}}/lib/${LIB_NAME}.so")
    endif()
    set(${LIB_NAME}_LIBS ${LIB_NAME})
endif()
