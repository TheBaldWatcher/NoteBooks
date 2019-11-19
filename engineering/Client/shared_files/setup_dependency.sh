#!/bin/bash
###################   function   ################

#spfFileCheckAndUpdate
spfFileCheckAndUpdate()
{
    spt3rdpartyName=$1
    sptGitDir=$2
    sptGitVerion=$3
    echo $(pwd)
    if [ ! -d $spt3rdpartyName ]; then
        echo "setup_dependency: Start pull $spt3rdpartyName with verion ${sptGitVerion}.";
        if [[ "$#" -eq 2 ]]; then
            git clone $sptGitDir
        else
            git clone -b $sptGitVerion $sptGitDir
        fi
    else
        if [[ "$#" -eq 2 ]]; then
            echo "setup_dependency:$spt3rdpartyName Already exits, checkout latest version.";
            spfUpdate3rdpartyToLatestTag $spt3rdpartyName 
        else
            echo "setup_dependency:$spt3rdpartyName Already exits, checkout verion ${sptGitVerion}.";
            cd $spt3rdpartyName
            git fetch
            git checkout $sptGitVerion
            cd ..
        fi
    fi
}

# Update 3rdparty To Latest Tag
spfUpdate3rdpartyToLatestTag()
{
	spt3rdpartyName=$1
    cd $spt3rdpartyName
    git fetch --tags
    LatestTag=$(git describe --tags `git rev-list --tags --max-count=1`)
    if [[ $LatestTag = "" ]]; then
    	echo "spfUpdate3rdpartyToLatestTag: $spt3rdpartyName LatestTag is empty ! start git pull"
    	git pull 
    else
        git checkout $LatestTag
        echo "spfUpdate3rdpartyToLatestTag: $spt3rdpartyName tag: $LatestTag"
    fi
    cd ..
}

###################   main   ################
echo $(pwd)
cd ../..
if [ ! -d 3rdparty ]; then
	mkdir 3rdparty
fi
	cd 3rdparty

if [[ "$#" -ne 0 ]]; then
    for lib in $*
    do
        name_version=(${lib//:/ })
        lib_name=${name_version[0]}
        lib_version=${name_version[1]}

        if   [[ $lib_name == "mtnn" ]]; then
            spfFileCheckAndUpdate libmtnn git@techgit.meitu.com:MTlabBinarys/libmtnn.git $lib_version
        elif [[ $lib_name == "mtcvlite" ]]; then
            spfFileCheckAndUpdate libmtcvlite git@techgit.meitu.com:MTlabBinarys/libmtcvlite.git $lib_version
        elif [[ $lib_name == "mtimage" ]]; then
            spfFileCheckAndUpdate libmtimage git@techgit.meitu.com:MTlabBinarys/libmtimage.git $lib_version
        elif [[ $lib_name == "Manis" ]]; then
            spfFileCheckAndUpdate libManis git@techgit.meitu.com:MTlabBinarys/libManis.git $lib_version
        elif [[ $lib_name == "ManisStatic" ]]; then
            spfFileCheckAndUpdate libManisStatic git@mlabgit.meitu.com:DLA/manis/libManisStatic.git $lib_version
        elif [[ $lib_name == "MTLabFace" ]]; then
            spfFileCheckAndUpdate libMTLabFace git@techgit.meitu.com:MTlabBinarys/libMTLabFace.git $lib_version
        elif [[ $lib_name == "opencv" ]]; then
            spfFileCheckAndUpdate libopencv git@techgit.meitu.com:MTlabBinarys/3rdpartyBinary/libopencv.git $lib_version
        elif [[ $lib_name == "yuv" ]]; then
            spfFileCheckAndUpdate libyuv git@techgit.meitu.com:3rdpartyBinary/libyuv.git $lib_version
        elif [[ $lib_name == "opengl" ]]; then
            spfFileCheckAndUpdate OpenGL git@techgit.meitu.com:yx4/OpenGL.git $lib_version
        elif [[ $lib_name == "opencv4demo" ]]; then
            spfFileCheckAndUpdate libOpencv4Demo git@techgit.meitu.com:yx4/libOpencv4Demo.git $lib_version
        elif [[ $lib_name == "photo_detector" ]]; then
            spfFileCheckAndUpdate libmtphotodetector git@techgit.meitu.com:MTlabBinarys/libmtphotodetector $lib_version
        fi
    done
fi
