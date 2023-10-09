@echo off
echo "begin build"

for /f "usebackq tokens=1* delims=: " %%i in (`vswhere -latest -requires Microsoft.Component.MSBuild`) do (
  if /i "%%i"=="installationPath" set InstallDir=%%j
)

set current_dir=%~dp0%

set BUILD_TYPE="Release"
set CMAKE_C_FLAGS="-DHAVE_STRUCT_TIMESPEC"
set CMAKE_CXX_FLAGS="-DHAVE_STRUCT_TIMESPEC"
set BUILD_SHARED_LIBS="OFF"

#生成不带符号库
set EXPORT_ALL_SYMBOLS="OFF"
set PROJECT_NAME="PhotoSegment"

set INSTALL_DIR=%current_dir%libs_without_symbol\Win\v150\x64
set BUILD_DIR=%current_dir%build\Win\v150\x64Project

rmdir /Q /S %BUILD_DIR%
mkdir %BUILD_DIR%
cd %BUILD_DIR%
cmake -G "Visual Studio 15 2017 Win64"                        ^
      -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%"                  ^
      -DCMAKE_BUILD_TYPE="%BUILD_TYPE%"                       ^
      -DCMAKE_C_FLAGS="%CMAKE_C_FLAGS%"                       ^
      -DCMAKE_CXX_FLAGS="%CMAKE_CXX_FLAGS%"                   ^
      -DBUILD_SHARED_LIBS="%BUILD_SHARED_LIBS%"               ^
	  -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS="%EXPORT_ALL_SYMBOLS%" ^
      %current_dir%

::cmake --build . --target install
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" %PROJECT_NAME%.sln /p:Configuration=Debug
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" INSTALL.vcxproj /p:Configuration=Debug
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" %PROJECT_NAME%.sln /p:Configuration=Release
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" INSTALL.vcxproj /p:Configuration=Release

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

set INSTALL_DIR=%current_dir%libs_without_symbol\Win\v150\x86
set BUILD_DIR=%current_dir%build\Win\v150\Win32Project

rmdir /Q /S %BUILD_DIR%
mkdir %BUILD_DIR%
cd %BUILD_DIR%
cmake -G "Visual Studio 15 2017"                              ^
      -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%"                  ^
      -DCMAKE_BUILD_TYPE="%BUILD_TYPE%"                       ^
      -DCMAKE_C_FLAGS="%CMAKE_C_FLAGS%"                       ^
      -DCMAKE_CXX_FLAGS="%CMAKE_CXX_FLAGS%"                   ^
      -DBUILD_SHARED_LIBS="%BUILD_SHARED_LIBS%"               ^
	  -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS="%EXPORT_ALL_SYMBOLS%" ^
      %current_dir%

::cmake --build . --target install
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" %PROJECT_NAME%.sln /p:Configuration=Debug
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" INSTALL.vcxproj /p:Configuration=Debug
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" %PROJECT_NAME%.sln /p:Configuration=Release
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" INSTALL.vcxproj /p:Configuration=Release


REM :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
REM :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
REM :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
REM #生成带符号库
REM set EXPORT_ALL_SYMBOLS="ON"
REM set PROJECT_NAME="PhotoSegment"

REM set INSTALL_DIR=%current_dir%libs_with_symbol\Win\v150\x64
REM set BUILD_DIR=%current_dir%build\Win\v150\x64Project

REM rmdir /Q /S %BUILD_DIR%
REM mkdir %BUILD_DIR%
REM cd %BUILD_DIR%
REM cmake -G "Visual Studio 15 2017 Win64"                        ^
      REM -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%"                  ^
      REM -DCMAKE_BUILD_TYPE="%BUILD_TYPE%"                       ^
      REM -DCMAKE_C_FLAGS="%CMAKE_C_FLAGS%"                       ^
      REM -DCMAKE_CXX_FLAGS="%CMAKE_CXX_FLAGS%"                   ^
      REM -DBUILD_SHARED_LIBS="%BUILD_SHARED_LIBS%"               ^
	  REM -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS="%EXPORT_ALL_SYMBOLS%" ^
      REM %current_dir%

REM ::cmake --build . --target install
REM REM "%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" %PROJECT_NAME%.sln /p:Configuration=Debug
REM REM "%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" INSTALL.vcxproj /p:Configuration=Debug
REM "%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" %PROJECT_NAME%.sln /p:Configuration=Release
REM "%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" INSTALL.vcxproj /p:Configuration=Release

REM :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
REM :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
REM :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

REM set INSTALL_DIR=%current_dir%libs_with_symbol\Win\v150\Win32
REM set BUILD_DIR=%current_dir%build\Win\v150\Win32Project

REM rmdir /Q /S %BUILD_DIR%
REM mkdir %BUILD_DIR%
REM cd %BUILD_DIR%
REM cmake -G "Visual Studio 15 2017"                              ^
      REM -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%"                  ^
      REM -DCMAKE_BUILD_TYPE="%BUILD_TYPE%"                       ^
      REM -DCMAKE_C_FLAGS="%CMAKE_C_FLAGS%"                       ^
      REM -DCMAKE_CXX_FLAGS="%CMAKE_CXX_FLAGS%"                   ^
      REM -DBUILD_SHARED_LIBS="%BUILD_SHARED_LIBS%"               ^
	  REM -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS="%EXPORT_ALL_SYMBOLS%" ^
      REM %current_dir%

REM ::cmake --build . --target install
REM REM "%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" %PROJECT_NAME%.sln /p:Configuration=Debug
REM REM "%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" INSTALL.vcxproj /p:Configuration=Debug
REM "%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" %PROJECT_NAME%.sln /p:Configuration=Release
REM "%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" INSTALL.vcxproj /p:Configuration=Release
pause