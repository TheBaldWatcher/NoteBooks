//
// Copyright (c) 2018 Meitu Inc. All rights reserved.
//

#ifndef MTFACESDK_ANDROID_BITMAPHELPER_H
#define MTFACESDK_ANDROID_BITMAPHELPER_H

#include <jni.h>
#include <android/bitmap.h>


namespace mt_vision {

unsigned char *LockBitmap(JNIEnv *env, jobject jbitmap);

AndroidBitmapInfo *GetBitmapInfo(JNIEnv *env, jobject jbitmap);

void UnLockBitmap(JNIEnv *env, jobject jbitmap);

}


#endif //MTFACESDK_ANDROID_BITMAPHELPER_H
