//
//  debug_helper.cpp
//  RealtimeSegment
//
//  Created by ashtower on 2019/1/18.
//  Copyright © 2019年 Yourtion. All rights reserved.
//

#include "debug_helper.h"

//#import <UIKit/UIKit.h>
//#import <Uikit/UIImagePickerController.h>
#include "UIImage+cdata.h"
#include "UIImage+ImageData.h"
#include <iostream>
#include <sstream>

//#include "MTRealtimeSegmentGPUBase.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_image.h"
//dhf debug end

//#include <mtcvlite/glbase/glUtil/GLUtils.h>

#if !defined(_WIN32) && !defined(WIN32) && !defined(_WIN64) && !defined(WIN64)
#include <sys/time.h>
#endif


#if defined(TARGET_OS_IPHONE) && !defined(TARGET_OS_MAC)
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/gltypes.h>
#endif // PLATFORM_IOS

#ifdef PLATFORM_WINDOWS
#include <gl\glew.h>            // OpenGL Extension "autoloader"
#endif // PLATFORM_WINDOWS

#if !defined(_WIN32) && !defined(WIN32) && !defined(_WIN64) && !defined(WIN64)
#include <unistd.h>
#else
#include <io.h>
#define access _access
#endif


#include "UIImage+ImageData.h"
#include "UIImage+cdata.h"

const char* getFileFromDocumentbundleAbsolutePath(NSString *fileCompent) {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    return [[NSString stringWithFormat:@"%@/%@", [paths objectAtIndex:0], fileCompent] UTF8String];
}

void loadTextureToUIImage(GLuint texture, int width, int height, const char * file_name, int channel, bool is_mask)
{
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    //绑定framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(0, 0, width, height);
    unsigned char *pData = new unsigned char[width * height << 2];
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pData);
    auto pp = pData;
    if (is_mask) {
        for (int i = 0 ; i < width * height; ++i) {
            pp[3] = 255;
            pp +=4;
        }
    }

    auto d = [UIImage segment_imageWithRGBAData:pData withWidth:width withHeight:height];
//    stbi_write_png(file_name, width, height, channel, pData, width*4);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
}

void ViewUnsignedCharByUIImage(unsigned char *img, int width, int height, bool is_mask)
{
    if (!is_mask) {
        auto d = [UIImage segment_imageWithRGBAData:img withWidth:width withHeight:height];
        int dummy = 1;
    } else {
        auto buffer = std::unique_ptr<unsigned char[]> (new unsigned char [width * height <<2]);
        
        for (int i = 0; i < width; ++i) {
            int cur_dst = 4 * i * width;
            int cur_src = i*width;
            for (int j = 0; j < height; ++j) {
                buffer[cur_dst] = img[cur_src];
                buffer[cur_dst+1] = img[cur_src];
                buffer[cur_dst+2] = img[cur_src];
                buffer[cur_dst+3] = 255;
                cur_dst += 4;
                cur_src += 1;
            }
        }
        auto d = [UIImage segment_imageWithRGBAData:buffer.get() withWidth:width withHeight:height];
        int dummy = 1;
    }
    
}

template <typename T>
std::string ToString(T val)
{
    std::stringstream stream;
    if (val <100)
        stream<<0;
    if (val < 10)
        stream <<0;
    stream << val;
    return stream.str();
}

void CountTexture(GLuint texture, int width, int height, std::string token)
{
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    //绑定framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(0, 0, width, height);
    unsigned char *pData = new unsigned char[width * height << 2];
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pData);
    
    double cnt = 0;
    for (int i = 0; i < width * height * 4 ;++i) {
        cnt += pData[i];
    }
    std::cout<<token<<" "<<cnt<<"w "<<width<<"h "<< height << "\n";
    
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
}
