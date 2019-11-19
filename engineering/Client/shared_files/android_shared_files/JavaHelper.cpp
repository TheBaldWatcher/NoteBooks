//
// Copyright (c) 2018 Meitu Inc. All rights reserved.
//

#include "JavaHelper.h"
#include <sys/system_properties.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

using namespace mt_vision;

int mt_vision::getAndroidDeviceID(JNIEnv *env, jobject context, char **DP_IMIE) {
    if (context == 0) {
        return MTS_FAIL;
    }
    //Context类
    jclass cls_context = env->FindClass("android/content/Context");
    if (cls_context == 0) {
        return MTS_FAIL;
    }

    //Object getSystemService(String name)方法
    jmethodID getSystemService = env->GetMethodID(cls_context, "getSystemService",
                                                  "(Ljava/lang/String;)Ljava/lang/Object;");
    if (getSystemService == 0) {
        return MTS_FAIL;
    }

    //TELEPHONY_SERVICE参数
    jfieldID TELEPHONY_SERVICE = env->GetStaticFieldID(cls_context, "TELEPHONY_SERVICE",
                                                       "Ljava/lang/String;");
    if (TELEPHONY_SERVICE == 0) {

        return MTS_FAIL;

    }
    jobject str = env->GetStaticObjectField(cls_context, TELEPHONY_SERVICE);

    //telephonymanager对象
    jobject telephonymanager = env->CallObjectMethod(context, getSystemService, str);
    if (telephonymanager == 0) {

        return MTS_FAIL;
    }

    //TelephonyManager类
    jclass cls_tm = env->FindClass("android/telephony/TelephonyManager");

    if (cls_tm == 0) {
        return MTS_FAIL;
    }

    //String getDeviceId()方法
    jmethodID getDeviceId = env->GetMethodID(cls_tm, "getDeviceId", "()Ljava/lang/String;");
    if (getDeviceId == 0) {
        return MTS_FAIL;
    }

    jstring deviceid = (jstring) env->CallObjectMethod(telephonymanager, getDeviceId);
    const char *pRtnStr = env->GetStringUTFChars(deviceid, 0);
    *DP_IMIE = new char[strlen(pRtnStr) + 1];
    strcpy(*DP_IMIE, pRtnStr);
    env->ReleaseStringUTFChars(deviceid, pRtnStr);
    return MTS_OK;
}

char *mt_vision::getAndroidPackageName(JNIEnv *env, jobject context) {
    if (context == 0) {
        return NULL;
    }
    //Context类
    jclass cls_context = env->FindClass("android/content/Context");
    if (cls_context == 0) {
        return NULL;
    }

    //String getPackageName()方法
    jmethodID getPackageName = env->GetMethodID(cls_context, "getPackageName",
                                                "()Ljava/lang/String;");
    if (getPackageName == 0) {
        return NULL;
    }

    //获取包名
    jstring strPackageName = (jstring) env->CallObjectMethod(context, getPackageName);
    if (strPackageName == 0) {
        return NULL;
    }

    const char *pRtnStr = env->GetStringUTFChars(strPackageName, 0);
    char *pTemp = new char[strlen(pRtnStr) + 1];
    strcpy(pTemp, pRtnStr);
    pTemp[strlen(pRtnStr)] = '\0';

    env->ReleaseStringUTFChars(strPackageName, pRtnStr);

    return pTemp;
}

char *mt_vision::getAndroidAPKPath(JNIEnv *env, jobject context) {
    if (context == 0) {
        return NULL;
    }
    //Context类
    jclass cls_context = env->FindClass("android/content/Context");
    if (cls_context == 0) {
        return NULL;
    }

    //getPackageName方法
    jmethodID getPackageName = env->GetMethodID(cls_context, "getPackageName",
                                                "()Ljava/lang/String;");
    if (getPackageName == 0) {
        return NULL;
    }

    //getPackageManager方法
    jmethodID getPackageManager = env->GetMethodID(cls_context, "getPackageManager",
                                                   "()Landroid/content/pm/PackageManager;");
    if (getPackageManager == 0) {
        return NULL;
    }

    //获取包名
    jobject strPackageName = env->CallObjectMethod(context, getPackageName);
    if (strPackageName == 0) {
        return NULL;
    }

    //获取Packagemanager对象
    jobject objPackageManager = env->CallObjectMethod(context, getPackageManager);
    if (objPackageManager == 0) {
        return NULL;
    }

    //PackageManager类
    jclass cls_PackageManager = env->FindClass("android/content/pm/PackageManager");
    if (cls_PackageManager == 0) {
        return NULL;
    }

    //getApplicationInfo方法
    jmethodID getApplicationInfo = env->GetMethodID(cls_PackageManager, "getApplicationInfo",
                                                    "(Ljava/lang/String;I)Landroid/content/pm/ApplicationInfo;");
    if (getApplicationInfo == 0) {
        return NULL;
    }

    //ApplicationInfo对象
    jobject objApplicationInfo = env->CallObjectMethod(objPackageManager, getApplicationInfo,
                                                       strPackageName, 0);
    if (objApplicationInfo == 0) {
        return NULL;
    }

    //ApplicationInfo类
    jclass cls_ApplicationInfo = env->FindClass("android/content/pm/ApplicationInfo");
    if (cls_ApplicationInfo == 0) {
        return NULL;
    }

    //sourceDir属性
    jfieldID sourceDir = env->GetFieldID(cls_ApplicationInfo, "sourceDir",
                                         "Ljava/lang/String;");
    if (sourceDir == 0) {
        return NULL;
    }

    jstring strSourceDir = (jstring) env->GetObjectField(objApplicationInfo, sourceDir);
    if (strSourceDir == 0) {
        return NULL;
    }

    //获取字符串
    const char *pRtnStr = env->GetStringUTFChars(strSourceDir, 0);
    char *pTemp = new char[strlen(pRtnStr) + 1];
    strcpy(pTemp, pRtnStr);
    pTemp[strlen(pRtnStr)] = '\0';
    env->ReleaseStringUTFChars(strSourceDir, pRtnStr);
    return pTemp;
}

char *mt_vision::getAndroidDataDir(JNIEnv *env, jobject context) {
    if (context == 0) {
        return NULL;
    }
    //Context类
    jclass cls_context = env->FindClass("android/content/Context");
    if (cls_context == 0) {
        return NULL;
    }

    //getPackageName方法
    jmethodID getPackageName = env->GetMethodID(cls_context, "getPackageName",
                                                "()Ljava/lang/String;");
    if (getPackageName == 0) {
        return NULL;
    }

    //getPackageManager方法
    jmethodID getPackageManager = env->GetMethodID(cls_context, "getPackageManager",
                                                   "()Landroid/content/pm/PackageManager;");
    if (getPackageManager == 0) {
        return NULL;
    }

    //获取包名
    jobject strPackageName = env->CallObjectMethod(context, getPackageName);
    if (strPackageName == 0) {
        return NULL;
    }

    //获取Packagemanager对象
    jobject objPackageManager = env->CallObjectMethod(context, getPackageManager);
    if (objPackageManager == 0) {
        return NULL;
    }

    //PackageManager类
    jclass cls_PackageManager = env->FindClass("android/content/pm/PackageManager");
    if (cls_PackageManager == 0) {
        return NULL;
    }

    //getApplicationInfo方法
    jmethodID getApplicationInfo = env->GetMethodID(cls_PackageManager, "getApplicationInfo",
                                                    "(Ljava/lang/String;I)Landroid/content/pm/ApplicationInfo;");
    if (getApplicationInfo == 0) {
        return NULL;
    }

    //ApplicationInfo对象
    jobject objApplicationInfo = env->CallObjectMethod(objPackageManager, getApplicationInfo,
                                                       strPackageName, 0);
    if (objApplicationInfo == 0) {
        return NULL;
    }

    //ApplicationInfo类
    jclass cls_ApplicationInfo = env->FindClass("android/content/pm/ApplicationInfo");
    if (cls_ApplicationInfo == 0) {
        return NULL;
    }

    //sourceDir属性
    jfieldID sourceDir = env->GetFieldID(cls_ApplicationInfo, "dataDir", "Ljava/lang/String;");
    if (sourceDir == 0) {
        return NULL;
    }

    jstring strSourceDir = (jstring) env->GetObjectField(objApplicationInfo, sourceDir);
    if (strSourceDir == 0) {
        return NULL;
    }

    //获取字符串
    const char *pRtnStr = env->GetStringUTFChars(strSourceDir, 0);
    char *pTemp = new char[strlen(pRtnStr) + 1];
    strcpy(pTemp, pRtnStr);
    pTemp[strlen(pRtnStr)] = '\0';
    env->ReleaseStringUTFChars(strSourceDir, pRtnStr);
    return pTemp;
}

int mt_vision::getAndroidSDK_INT(JNIEnv *env) {
    jclass cls_VERSION = env->FindClass("android/os/Build$VERSION");
    if (cls_VERSION == 0) {
        return 0;
    }
    jfieldID SDK_INT = env->GetStaticFieldID(cls_VERSION, "SDK_INT", "I");

    if (SDK_INT == 0) {
        return 0;
    }

    jint result = env->GetStaticIntField(cls_VERSION, SDK_INT);

    return (int) result;
}

int mt_vision::getAndroidCpuCount() {
    return sysconf(_SC_NPROCESSORS_ONLN);
}

int mt_vision::getAndroidDebugMode(JNIEnv *env, jobject context) {
    char *pakageName = getAndroidPackageName(env, context);
    if (pakageName == NULL) {
        return 0;
    }
    int strLen = strlen(pakageName);
    //得到的包名是com.xx.xxx形式,应该转成com/xx/xxx的字符串中,把"."替换成"/"
    for (int i = 0; i < strLen; ++i) {
        if (pakageName[i] == '.') {
            pakageName[i] = '/';
        }
    }

    char *BuildConfig = new char[strLen + 13];
    memset(BuildConfig, 0, strLen + 13);
    sprintf(BuildConfig, "%s/BuildConfig", pakageName);
    delete[] pakageName;

    jclass jcl_BuildConfig = env->FindClass(BuildConfig);
    delete[] BuildConfig;

    if (jcl_BuildConfig == 0) {
        return 0;
    }
    jfieldID DEBUG = env->GetStaticFieldID(jcl_BuildConfig, "DEBUG", "Z");
    if (DEBUG == 0) {
        return 0;
    }
    jboolean mode = env->GetStaticBooleanField(jcl_BuildConfig, DEBUG);

    return mode;
}

char *mt_vision::getAndroidAPKKeyHash(JNIEnv *env, jobject context) {
    if (context == 0) {
        return NULL;
    }
    //Context类
    jclass cls_context = env->FindClass("android/content/Context");
    if (cls_context == 0) {
        return NULL;
    }
    //getPackageName方法
    jmethodID getPackageName = env->GetMethodID(cls_context, "getPackageName",
                                                "()Ljava/lang/String;");
    if (getPackageName == 0) {
        return NULL;
    }

    //getPackageManager方法
    jmethodID getPackageManager = env->GetMethodID(cls_context, "getPackageManager",
                                                   "()Landroid/content/pm/PackageManager;");
    if (getPackageManager == 0) {
        return NULL;
    }

    //获取包名
    jstring strPackageName = (jstring) env->CallObjectMethod(context, getPackageName);
    if (strPackageName == 0) {
        return NULL;
    }

    //获取Packagemanager对象
    jobject objPackageManager = env->CallObjectMethod(context, getPackageManager);
    if (objPackageManager == 0) {
        return NULL;
    }
    //PackageManager类
    jclass cls_PackageManager = env->FindClass("android/content/pm/PackageManager");
    if (cls_PackageManager == 0) {
        return NULL;
    }

    //拿GET_SIGNATURES 键值
    jfieldID GET_SIGNATURES = env->GetStaticFieldID(cls_PackageManager, "GET_SIGNATURES", "I");


    jint VALUE_GET_SIGNATURES = env->GetStaticIntField(cls_PackageManager, GET_SIGNATURES);

    //getApplicationInfo方法
    jmethodID getPackageInfo = env->GetMethodID(cls_PackageManager, "getPackageInfo",
                                                "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");

    if (getPackageInfo == 0) {
        return NULL;
    }

    //PackageInfo对象
    jobject objPackageInfo = env->CallObjectMethod(objPackageManager, getPackageInfo,
                                                   strPackageName, VALUE_GET_SIGNATURES);


    if (objPackageInfo == 0) {
        return NULL;
    }

    //PackageInfo类
    jclass cls_PackageInfo = env->FindClass("android/content/pm/PackageInfo");

    if (cls_PackageInfo == 0) {
        return NULL;
    }

    //拿到Signatures值
    jfieldID ID_signatures = env->GetFieldID(cls_PackageInfo, "signatures",
                                             "[Landroid/content/pm/Signature;");
    if (ID_signatures == 0) {
        return NULL;
    }

    jobjectArray signatures = (jobjectArray) env->GetObjectField(objPackageInfo, ID_signatures);
    if (signatures == 0) {
        return NULL;
    }

    //获取MessageDigest类
    jclass cls_MessageDigest = env->FindClass("java/security/MessageDigest");
    if (cls_MessageDigest == 0) {
        return NULL;
    }
    //实例化方法
    jmethodID getInstance = env->GetStaticMethodID(cls_MessageDigest, "getInstance",
                                                   "(Ljava/lang/String;)Ljava/security/MessageDigest;");
    if (getInstance == 0) {
        return NULL;
    }
    //update方法
    jmethodID update = env->GetMethodID(cls_MessageDigest, "update", "([B)V");
    if (update == 0) {
        return NULL;
    }

    //digest方法
    jmethodID digest = env->GetMethodID(cls_MessageDigest, "digest", "()[B");
    if (digest == 0) {
        return NULL;
    }
    //Signature类
    jclass cls_Signature = env->FindClass("android/content/pm/Signature");
    if (cls_Signature == 0) {
        return NULL;
    }

    //toByteArray方法
    jmethodID toByteArray = env->GetMethodID(cls_Signature, "toByteArray", "()[B");
    if (toByteArray == 0) {
        return NULL;
    }
    //Base64类
    jclass cls_Base64 = env->FindClass("android/util/Base64");
    if (cls_Base64 == 0) {
        return NULL;
    }

    //encodeToString 方法
    jmethodID encodeToString = env->GetStaticMethodID(cls_Base64, "encodeToString",
                                                      "([BI)Ljava/lang/String;");
    if (encodeToString == 0) {
        return NULL;
    }
    jobject sig = env->GetObjectArrayElement(signatures, 0);
    if (sig == 0) {
        return NULL;
    }

    jstring SHA = env->NewStringUTF("SHA");
    jobject md = env->CallStaticObjectMethod(cls_MessageDigest, getInstance, SHA);
    jbyteArray sigBytes = (jbyteArray) env->CallObjectMethod(sig, toByteArray);
    if (sigBytes == 0) {
        return NULL;
    }

    env->CallVoidMethod(md, update, sigBytes);
    jbyteArray digestBytes = (jbyteArray) env->CallObjectMethod(md, digest);
    if (digestBytes == 0) {
        return NULL;
    }

    jstring KeyHash = (jstring) env->CallStaticObjectMethod(cls_Base64, encodeToString,
                                                            digestBytes, 0);
    if (KeyHash == 0) {
        return NULL;
    }
    const char *pRtnStr = env->GetStringUTFChars(KeyHash, JNI_FALSE);
    char *pTemp = new char[strlen(pRtnStr) + 1];
    strcpy(pTemp, pRtnStr);
    pTemp[strlen(pRtnStr)] = '\0';
    env->ReleaseStringUTFChars(KeyHash, pRtnStr);
    return pTemp;
}


AAssetManager *mt_vision::getAssetManager(JNIEnv *env, jobject context) {
//Context类
    jclass cls_context = env->GetObjectClass(context);
    if (cls_context == 0) {
        return NULL;
    }
    //getPackageName方法
    jmethodID getAssets = env->GetMethodID(cls_context, "getAssets",
                                           "()Landroid/content/res/AssetManager;");
    if (getAssets == 0) {
        return NULL;
    }
    jobject assetManager = env->CallObjectMethod(context, getAssets);
    AAssetManager *assets = AAssetManager_fromJava(env, assetManager);
    return assets;
}

unsigned char *mt_vision::readFile(AAssetManager *assets, const char *fileName, unsigned long &dataLen) {
    unsigned char *binBytes = nullptr;
    //优先判断SDCard
    if (access(fileName, 0) == 0) {
        FILE *file = fopen(fileName, "rb");
        if (file) {
            // read data len
            fseek(file, 0L, SEEK_END);
            dataLen = ftell(file);
            fseek(file, 0L, SEEK_SET);

            // read data
            binBytes = new unsigned char[dataLen];
            fread(binBytes, 1, dataLen, file);
        }
    } else if (assets && access(fileName, 0) != 0) {
        AAsset *file = AAssetManager_open(assets, fileName, AASSET_MODE_UNKNOWN);
        if (file) {
            dataLen = AAsset_getLength(file);
            if (dataLen > 0) {
                binBytes = new unsigned char[dataLen];
                AAsset_read(file, binBytes, dataLen);

            }
            AAsset_close(file);
        }
    }
    return binBytes;
}