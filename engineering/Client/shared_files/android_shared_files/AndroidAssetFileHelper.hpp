#ifndef _H_ANDROID_ASSET_FILE_HELPER_H_
#define _H_ANDROID_ASSET_FILE_HELPER_H_


#include <tuple>
#include <memory>

#include <android/asset_manager_jni.h>

#include "MTBeijingSdkDefine.h"

std::tuple<std::shared_ptr<unsigned char >, long unsigned> ReadFileFromAsset(AAssetManager* mManeger, const char *data_file);

#endif // _H_ANDROID_ASSET_FILE_HELPER_H_
