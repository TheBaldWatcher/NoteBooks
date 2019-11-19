//
//  BitmapUtil.cpp
//  meitie_core
//
//  Created by Ryan on 16/1/4.
//  Copyright © 2016年 Ryan. All rights reserved.
//

#include "BitmapUtil.hpp"
#include <android/bitmap.h>
#include <stdlib.h>
#include <string.h>
#include "MTSegmentLogicDefine.h"

unsigned char* CBitmapUtil::lock(JNIEnv *env, jobject bitmap, int &width, int &height)
{
    AndroidBitmapInfo info;
    void* pixels = NULL;
    int ret;
    if (bitmap)
    {
        if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
            return NULL;
        }
        
        if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0)
        {
            return NULL;
        }
        
        width = info.width;
        height = info.height;

    }
    
    return (unsigned char*)pixels;
}

void CBitmapUtil::unlock(JNIEnv *env, jobject bitmap)
{
    if (bitmap) {
        AndroidBitmap_unlockPixels(env, bitmap);
    }
}


unsigned char* CBitmapUtil::getPixels(JNIEnv* env,jobject bitmap,int& width,int& height,COLOR_SPACE outputCS,bool needPreMultiplyAlpha)
{
    unsigned char* pixels = NULL;
    unsigned char* orgPixels = lock(env, bitmap, width, height);
    if (orgPixels&&width>0&&height>0) {
        pixels = new unsigned char[width*height*4];
        memcpy(pixels, orgPixels, width*height*4);
        
        int pixelCount = width*height;
        unsigned char* dstCursor = pixels;
        if (needPreMultiplyAlpha) {
            float alphaArray[256] = {0};
            for (int i = 0; i< 256; ++i) {
                alphaArray[i] = i/255.0f;
            }
            
            if (outputCS==CS_BGRA) {
                for (int i = 0; i< pixelCount; ++i) {
                    float alphaRat = alphaArray[dstCursor[3]];
                    unsigned char tempByte = dstCursor[0]*alphaRat;
                    dstCursor[0]=dstCursor[2]*alphaRat;
                    dstCursor[1]*=alphaRat;
                    dstCursor[2]=tempByte;
                    dstCursor+=4;
                }
            }else{
                for (int i = 0; i< pixelCount; ++i) {
                    float alphaRat = alphaArray[dstCursor[3]];
                    dstCursor[0]*=alphaRat;
                    dstCursor[1]*=alphaRat;
                    dstCursor[2]*=alphaRat;
                    dstCursor+=4;
                }
            }
            
        }else if(outputCS==CS_BGRA){
            for (int i = 0; i<pixelCount; ++i) {
                dstCursor[0]= dstCursor[0]^dstCursor[2];
                dstCursor[2]= dstCursor[2]^dstCursor[0];
                dstCursor[0]= dstCursor[0]^dstCursor[2];
                dstCursor+=4;
            }
            
        }
    }
    
    unlock(env, bitmap);
    
    return pixels;
}

void CBitmapUtil::setPixels(JNIEnv* env,jobject bitmap,unsigned char* pixels,int width,int height,COLOR_SPACE inputCS, bool needPreMultiplyAlpha)
{
    int srcWidth = 0,srcHeight = 0;
    unsigned char* orgPixels = lock(env, bitmap, srcWidth, srcHeight);
    if (orgPixels&&srcWidth == width&&srcHeight == height) {
        int pixelCount = width*height;
        unsigned char* dstCursor = orgPixels;
        unsigned char* srcCursor = pixels;
        if (needPreMultiplyAlpha) {
            float alphaArray[256] = {0};
            for (int i = 0; i< 256; ++i) {
                alphaArray[i] = i/255.0f;
            }
            if (inputCS == CS_BGRA) {
                for (int i = 0; i < pixelCount; ++i) {
                    float alphaRat = alphaArray[dstCursor[3]];
                    dstCursor[0] = srcCursor[2]/alphaRat;
                    dstCursor[1] = srcCursor[1]/alphaRat;
                    dstCursor[2] = srcCursor[0]/alphaRat;
                    dstCursor[3] = srcCursor[3];
                    dstCursor+=4;
                    srcCursor+=4;
                }
            }else {
                for (int i = 0; i < pixelCount; ++i) {
                    float alphaRat = alphaArray[dstCursor[3]];
                    dstCursor[0] = srcCursor[0]/alphaRat;
                    dstCursor[1] = srcCursor[1]/alphaRat;
                    dstCursor[2] = srcCursor[2]/alphaRat;
                    dstCursor[3] = srcCursor[3];
                    dstCursor+=4;
                    srcCursor+=4;
                }
            }
        }else {
            memcpy(orgPixels, pixels, width*height*4);
            if (inputCS == CS_BGRA){
                for (int i = 0; i<pixelCount; ++i) {
                    dstCursor[0]= dstCursor[0]^dstCursor[2];
                    dstCursor[2]= dstCursor[2]^dstCursor[0];
                    dstCursor[0]= dstCursor[0]^dstCursor[2];
                    dstCursor+=4;
                }
            }
        }
    }
    
    unlock(env, bitmap);
    
}

// add by yz
unsigned char* CBitmapUtil::getPixels2(JNIEnv* env,jobject bitmap,int& width,int& height,COLOR_SPACE outputCS,bool needPreMultiplyAlpha)
{
    unsigned char* pixels = NULL;
    unsigned char* orgPixels = lock(env, bitmap, width, height);
    if (orgPixels&&width>0&&height>0) {
        pixels = new unsigned char[width*height*4];
        memcpy(pixels, orgPixels, width*height*4);
        
        int pixelCount = width*height;
        unsigned char* dstCursor = pixels;
        if (needPreMultiplyAlpha) {
            float alphaArray[256] = {0};
            for (int i = 0; i< 256; ++i) {
                alphaArray[i] = i/255.0f;
            }
            
            if (outputCS==CS_BGRA) {
                for (int i = 0; i< pixelCount; ++i) {
                    float alphaRat = alphaArray[dstCursor[3]];
                    unsigned char tempByte = dstCursor[0]/alphaRat;
                    dstCursor[0]=dstCursor[2]/alphaRat;
                    dstCursor[1]/=alphaRat;
                    dstCursor[2]=tempByte;
                    dstCursor+=4;
                }
            }else{
                for (int i = 0; i< pixelCount; ++i) {
                    float alphaRat = alphaArray[dstCursor[3]];
                    dstCursor[0]/=alphaRat;
                    dstCursor[1]/=alphaRat;
                    dstCursor[2]/=alphaRat;
                    dstCursor+=4;
                }
            }
            
        }else if(outputCS==CS_BGRA){
            for (int i = 0; i<pixelCount; ++i) {
                dstCursor[0]= dstCursor[0]^dstCursor[2];
                dstCursor[2]= dstCursor[2]^dstCursor[0];
                dstCursor[0]= dstCursor[0]^dstCursor[2];
                dstCursor+=4;
            }
            
        }
    }
    
    unlock(env, bitmap);
    
    return pixels;
}

// add by yz
void CBitmapUtil::setPixels2(JNIEnv* env,jobject bitmap,unsigned char* pixels,int width,int height,COLOR_SPACE inputCS, bool needPreMultiplyAlpha)
{
    int srcWidth = 0,srcHeight = 0;
    unsigned char* orgPixels = lock(env, bitmap, srcWidth, srcHeight);
    if (orgPixels&&srcWidth == width&&srcHeight == height) {
        int pixelCount = width*height;
        unsigned char* dstCursor = orgPixels;
        unsigned char* srcCursor = pixels;
        if (needPreMultiplyAlpha) {
            float alphaArray[256] = {0};
            for (int i = 0; i< 256; ++i) {
                alphaArray[i] = i/255.0f;
            }
            if (inputCS == CS_BGRA) {
                for (int i = 0; i < pixelCount; ++i) {
                    float alphaRat = alphaArray[srcCursor[3]];
                    dstCursor[0] = srcCursor[2]*alphaRat;
                    dstCursor[1] = srcCursor[1]*alphaRat;
                    dstCursor[2] = srcCursor[0]*alphaRat;
                    dstCursor[3] = srcCursor[3];
                    dstCursor+=4;
                    srcCursor+=4;
                }
            }else {
                for (int i = 0; i < pixelCount; ++i) {
                    float alphaRat = alphaArray[srcCursor[3]];
                    dstCursor[0] = srcCursor[0]*alphaRat;
                    dstCursor[1] = srcCursor[1]*alphaRat;
                    dstCursor[2] = srcCursor[2]*alphaRat;
                    dstCursor[3] = srcCursor[3];
                    dstCursor+=4;
                    srcCursor+=4;
                }
            }
        }else {
            memcpy(orgPixels, pixels, width*height*4);
            if (inputCS == CS_BGRA){
                for (int i = 0; i<pixelCount; ++i) {
                    dstCursor[0]= dstCursor[0]^dstCursor[2];
                    dstCursor[2]= dstCursor[2]^dstCursor[0];
                    dstCursor[0]= dstCursor[0]^dstCursor[2];
                    dstCursor+=4;
                }
            }
        }
    }
    
    unlock(env, bitmap);
    
}


//创建Bitmap
jobject CBitmapUtil::BitmapCreate(JNIEnv* env,int nWidth,int nHeight)
{
    // Creaing Bitmap Config Class
    jclass bmpCfgCls = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID bmpClsValueOfMid = env->GetStaticMethodID(bmpCfgCls, "valueOf", "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
    jobject jBmpCfg = env->CallStaticObjectMethod(bmpCfgCls, bmpClsValueOfMid, env->NewStringUTF("ARGB_8888"));
    
    // Creating a Bitmap Class
    jclass bmpCls = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapMid = env->GetStaticMethodID(bmpCls, "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jobject jBmpObj = env->CallStaticObjectMethod(bmpCls, createBitmapMid, nWidth, nHeight, jBmpCfg);
 
    return jBmpObj;
}


void CBitmapUtil::getBitmapSize(JNIEnv* env,jobject bitmap,int& width,int& height)
{
    width = 0;
    height = 0;
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, bitmap, &info)>= 0)
    {
        width = info.width;
        height = info.height;
    }
    
}
