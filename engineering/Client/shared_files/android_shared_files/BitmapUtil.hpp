//
//  BitmapUtil.hpp
//  meitie_core
//
//  Created by Ryan on 16/1/4.
//  Copyright © 2016年 Ryan. All rights reserved.
//

#ifndef BitmapUtil_hpp
#define BitmapUtil_hpp
#include <stdio.h>
#include <jni.h>

typedef enum COLOR_SPACE{
    CS_RGBA = 0,
    CS_BGRA = 1,
}COLOR_SPACE;

class CBitmapUtil{
public:
    
    /**
     创建Bitmap
     
     @param env     env
     @param nWidth  width
     @param nHeight height
     @return
     */
    static jobject BitmapCreate(JNIEnv* env,int nWidth,int nHeight);
    
    
    /**
     读取bitmap尺寸

     @param env
     @param bitmap
     @param width
     @param height 
     */
    static void getBitmapSize(JNIEnv* env,jobject bitmap,int& width,int& height);
    
    /**
     锁定bitmap 获取图片像素数据，RGBA
     @param env
     @param bitmap
     @param width
     @param height
     
     @return 图片像素数据原地址
     注：外面不可以释放内存，用完调用unlock
     */
    static unsigned char* lock(JNIEnv* env,jobject bitmap,int& width,int& height);
    
    /**
     释放bitmap引用
     @param env
     @param bitmap 
     注：与lock 配套使用
     */
    static void unlock(JNIEnv* env,jobject bitmap);
    
    /**
     获取图片的像素数据
     @param env
     @param bitmap
     @param width
     @param height
     @param outputCS              颜色空间    默认RGBA
     @param needPreMultiplyAlpha  因为Android bitmap 存储的C数据已经经过预乘处理(RGB三个通道与alpha 通道做乘法)，所以如果需要得到正确的RGB值，需要将RGB数据反推回来。(非半透明图无影响)
     @return
     */
    static unsigned char* getPixels(JNIEnv* env,jobject bitmap,int& width,int& height,COLOR_SPACE outputCS = CS_RGBA,bool needPreMultiplyAlpha = false);
    // add by yz
    static unsigned char* getPixels2(JNIEnv* env,jobject bitmap,int& width,int& height,COLOR_SPACE outputCS = CS_RGBA,bool needPreMultiplyAlpha = false);
    
    /**
     设置图片的像素数据

     @param env
     @param bitmap
     @param pixels
     @param width
     @param height 
     @param inputCS            输出的颜色空间 默认 RGBA
     @param needPreMultiplyAlpha 是否已经对alpha做预乘，默认false。因为Android bitmap 存储的C数据已经经过预乘处理(RGB三个通道与alpha 通道做乘法)。输入的数据如果已经是经过预乘处理，传false,反之true
     注：做内存拷贝，外面需要释放内存
     */
    static void setPixels(JNIEnv* env,jobject bitmap,unsigned char* pixels,int width,int height,COLOR_SPACE inputCS = CS_RGBA,bool needPreMultiplyAlpha = false);
    // add by yz
    static void setPixels2(JNIEnv* env,jobject bitmap,unsigned char* pixels,int width,int height,COLOR_SPACE inputCS = CS_RGBA,bool needPreMultiplyAlpha = false);
};

#endif /* BitmapUtil_hpp */
