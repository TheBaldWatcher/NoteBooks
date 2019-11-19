@echo off
echo setup_dependency: shared_files path : %~dp0
cd ../..

if not exist "3rdparty" (
	mkdir 3rdparty
)
	cd 3rdparty
rem loop get bat para
:LOOP
	set index=%1
	echo %index%
	if "%index%" == "" goto END
	echo setup_dependency: %index%	
		rem loop get lib name and version 
		:loop1
		for /f "tokens=1* delims=:" %%a in ("%index%") do (
			rem echo %%a
			rem echo %%b
			call:spfLibNameAndVersion %%a %%b
		) 
		if %t%! == ! goto end1
		shift
		goto :loop1
		:end1
	shift
	goto LOOP	
:END
exit /b 0
GOTO :EOF

::spfLibNameAndVersion
:spfLibNameAndVersion
    ::分支名
	set lib_name=%1
	set lib_version=%2
	rem echo %lib_name%
	rem echo %lib_version%
	if "%lib_name%"=="mtnn" ( 
		call:spfUpdate3rdparty libmtnn git@techgit.meitu.com:MTlabBinarys/libmtnn.git %lib_version%
	) else if "%lib_name%"=="Manis" ( 
		call:spfUpdate3rdparty libManis git@techgit.meitu.com:MTlabBinarys/libManis.git %lib_version%
	) else if "%lib_name%"=="mtcvlite" ( 
		call:spfUpdate3rdparty libmtcvlite git@techgit.meitu.com:MTlabBinarys/libmtcvlite.git %lib_version%
	) else if "%lib_name%"=="mtimage" ( 
		call:spfUpdate3rdparty libmtimage git@techgit.meitu.com:MTlabBinarys/libmtimage.git %lib_version%
	) else if "%lib_name%"=="MTLabFace" ( 
		call:spfUpdate3rdparty libMTLabFace git@techgit.meitu.com:MTlabBinarys/libMTLabFace.git %lib_version%
	) else if "%lib_name%"=="opencv" ( 
		call:spfUpdate3rdparty libopencv git@techgit.meitu.com:MTlabBinarys/3rdpartyBinary/libopencv.git %lib_version%
	) else if "%lib_name%"=="yuv" ( 
		call:spfUpdate3rdparty libyuv git@techgit.meitu.com:3rdpartyBinary/libyuv.git %lib_version%
	) else if "%lib_name%"=="photo_detector" ( 
		call:spfUpdate3rdparty libmtphotodetector git@techgit.meitu.com:MTlabBinarys/libmtphotodetector.git %lib_version%
	) else if "%lib_name%"=="glew" ( 
		call:spfUpdate3rdparty libglew git@techgit.meitu.com:MTlabBinarys/3rdpartyBinary/libglew.git %lib_version%
	)
	exit /b 0
GOTO :EOF

::spfUpdate3rdparty
:spfUpdate3rdparty
	set spt3rdpartyName=%1
    set sptGitDir=%2
    set sptGitVerion=%3
	echo %sptGitVerion%
	if not exist %spt3rdpartyName% (  
		if "%sptGitVerion%" NEQ "null" (
			echo setup_dependency: Start pull %spt3rdpartyName% with verion %sptGitVerion%.
			git clone -b %sptGitVerion% %sptGitDir%
		) else (
			echo setup_dependency: Start pull %spt3rdpartyName% with lastest commit.
			git clone %sptGitDir%
		)
	) else (
		if "%sptGitVerion%" NEQ "null" (
			echo setup_dependency: %spt3rdpartyName% Already exits, checkout verion %sptGitVerion%.
			cd %spt3rdpartyName%
			git checkout .
			git clean -df
			git fetch
			git checkout %sptGitVerion%
			cd ..
		) else (
			echo setup_dependency: %spt3rdpartyName% Already exits, Start pull lastest commit.
			cd %spt3rdpartyName%
			git checkout .
			git clean -df
			git pull 
			cd ..
		)
	)
	::pause
	exit /b 0
GOTO :EOF




