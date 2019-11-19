#!/usr/bin/env sh
set -e
if [ "$#" -ne 2 ]; then
    echo "Usage: "
    echo "UpdateBuildGradleVerion ModulePrefix new_verion"
    exit 1
fi

ScriptDir=$(cd `dirname $0`; pwd)
ModulePrefix=$1
VersionNum=$2


AndroidSdkDevDir="${ScriptDir}/../AndroidSdkDev/SdkDevDemo"

for file in ${AndroidSdkDevDir}/*
do
    echo "check dir ${file}"
    if [[ -d ${file} ]]; then
        flag=`echo ${file##*/} | sed -E "s#.*${ModulePrefix}(Library|Model).*#\1#g"`
        if [[ ${flag} == "Library" || ${flag} == "Model" ]]; then
            sed -Ei "" "s#( ?version ?= ?')(.*)(\'$)#\1${VersionNum}\3#g" ${file}/build.gradle
            cd ${file}
            echo "${file} matches, will upload to maven. "
            echo `pwd`
            ./gradlew uploadArchives
            cd ${ScriptDir}
        fi
    fi
done