# glew 引用规则
# 包含以下可用变量
# libglew_INCLUDE_DIRS      头文件目录
# libglew_LIBS       		LinkTarget
# libglew_LIBS              LinkTarget
# libglew_DYLIBS            动态库
set(_ROOT "${3RDPARTY_ROOT}/libglew")

if(MSVC)
    # windows
	set(libglew_ROOT "${_ROOT}/Win")
	if("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
		set(libglew_ROOT "${libglew_ROOT}/Win32")
	else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		set(libglew_ROOT "${libglew_ROOT}/x64")
	endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")

	if (GLEW_STATIC)
		add_definitions(-DGLEW_STATIC)
		add_library(libglew STATIC IMPORTED)
		set_target_properties(libglew PROPERTIES IMPORTED_LOCATION "${libglew_ROOT}/lib/glew32s.lib")
	else (GLEW_STATIC)
		add_library(libglew SHARED IMPORTED)
		set_target_properties(libglew PROPERTIES IMPORTED_IMPLIB "${libglew_ROOT}/lib/glew32.lib")
		set(libglew_DYLIBS "${libglew_ROOT}/lib/glew32.dll")
	endif (GLEW_STATIC)
	
	set(libglew_INCLUDE_DIRS "${libglew_ROOT}/include")
    set(libglew_LIBS libglew)
endif()