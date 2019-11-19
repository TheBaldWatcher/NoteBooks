//
// Copyright (c) 2018 Meitu Inc. All rights reserved.
//

#include "BitmapHelper.h"
#include <string.h>
#include <android/bitmap.h>

unsigned char *mt_vision::LockBitmap(JNIEnv *env, jobject jbitmap) {
    unsigned char *data = NULL;
    int ret = AndroidBitmap_lockPixels(env, jbitmap, (void **) &data);
    if (ret != 0) {
        return NULL;
    }
//    AndroidBitmapInfo *info = GetBitmapInfo(env, jbitmap);
//    if (info->format != AndroidBitmapFormat::ANDROID_BITMAP_FORMAT_RGBA_8888) {
//        return NULL;
//    }
//    int size = info->width * info->height * 4;
//    unsigned char *dataCpy = new unsigned char[size];
//    memcpy(dataCpy, data, size);
//    UnLockBitmap(env, jbitmap);
    return data;
}

AndroidBitmapInfo *mt_vision::GetBitmapInfo(JNIEnv *env, jobject jbitmap) {
    AndroidBitmapInfo *bitmapInfo = new AndroidBitmapInfo();
    int getInfoRet = AndroidBitmap_getInfo(env, jbitmap, bitmapInfo);
    if (getInfoRet != ANDROID_BITMAP_RESULT_SUCCESS) {
        return NULL;
    }
    return bitmapInfo;
}

void mt_vision::UnLockBitmap(JNIEnv *env, jobject jbitmap) {
    AndroidBitmap_unlockPixels(env, jbitmap);
}
