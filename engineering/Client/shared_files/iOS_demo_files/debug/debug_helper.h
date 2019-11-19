//
//  debug_helper.hpp
//  RealtimeSegment
//
//  Created by ashtower on 2019/1/18.
//  Copyright © 2019年 Yourtion. All rights reserved.
//

#ifndef debug_helper_h
#define debug_helper_h

#include <stdio.h>

#import <UIKit/UIKit.h>

#include <memory>

const char* getFileFromDocumentbundleAbsolutePath(NSString *fileCompent);

void loadTextureToUIImage(GLuint texture, int width, int height, const char * file_name, int channel, bool is_mask);

void ViewUnsignedCharByUIImage(unsigned char *img, int width, int height, bool is_mask);

void CountTexture(GLuint texture, int width, int height);

void  ViewTextureUIImage(GLuint texture, int width, int height, bool is_mask);

template <typename T>
std::string ToString(T val);


#endif /* debug_helper_hpp */
