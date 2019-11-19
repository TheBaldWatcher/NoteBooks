#!/usr/bin/env sh
set -e

echo $(pwd)
# sdk公共代码
if [ -d shared_files ]; then
	rm -rf shared_files
fi

git clone git@techgit.meitu.com:MTLabBeijingSdkDevelop/shared_files.git
cd shared_files
./setup_dependency.sh mtcvlite:0.2.2.5 yuv:1.7.2.6 Manis:2.3.0.5 mtimage:1.0.2.4 opengl:0.0.1.0 opencv4demo:3.4.5.0 opengl:0.0.1.0
