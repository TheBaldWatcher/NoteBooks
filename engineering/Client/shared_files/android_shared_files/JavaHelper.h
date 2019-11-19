//
// Copyright (c) 2018 Meitu Inc. All rights reserved.
//

#ifndef MTFACESDK_ANDROID_JAVAHELPER_H
#define MTFACESDK_ANDROID_JAVAHELPER_H

#include <jni.h>
#include <Android/asset_manager.h>
#include <Android/asset_manager_jni.h>

namespace mt_vision {

#define MTS_OK   0 //没有错误.
#define MTS_FAIL  (-1) //未指明的错误

/**
 * 获取设备的IMIE号
 * @param env
 * @param context
 * @param DP_IMIE
 * @return
 */
int getAndroidDeviceID(JNIEnv *env, jobject context, char **DP_IMIE);

/**
 * 获取app的包名
 * @param env
 * @param context
 * @return
 */
char *getAndroidPackageName(JNIEnv *env, jobject context);

/**
 * 获取设备的apk路径
 * @param env
 * @param context
 * @return
 */
char *getAndroidAPKPath(JNIEnv *env, jobject context);

/**
 * 获取app的Data目录
 * @param env
 * @param context
 * @return
 */
char *getAndroidDataDir(JNIEnv *env, jobject context);

/**
 * 拿SDK版本
 * @param env
 * @return
 */
int getAndroidSDK_INT(JNIEnv *env);

/**
 * 获得CPU数量
 * @return
 */
int getAndroidCpuCount();

/**
 * APK是否是Debug模式
 * @param env
 * @param context
 * @return
 */
int getAndroidDebugMode(JNIEnv *env, jobject context);

/**
 * 拿apk的签名
 * @param env
 * @param context
 * @return
 */
char *getAndroidAPKKeyHash(JNIEnv *env, jobject context);

/**
 * 获取 JNI assetManager对象
 * @param env
 * @param context
 * @return
 */
AAssetManager *getAssetManager(JNIEnv *env, jobject context);

/**
 * 根据路径读取文件(支持SDcard读取和资源文件读取)
 * @param assets
 * @param fileName
 * @param dataLen
 * @return
 */
unsigned char* readFile(AAssetManager *assets, const char* fileName,unsigned long& dataLen);

}


#endif //MTFACESDK_ANDROID_JAVAHELPER_H
