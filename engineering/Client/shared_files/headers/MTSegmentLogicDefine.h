//
//  MTSegmentLogicDefine.h
//
//  Created by Carl on 17/6/2.
//  Copyright © 2016年 meitu. All rights reserved.
//

#ifndef _H_MT_SEGMENT_LOGIC_DEFINE_H_
#define _H_MT_SEGMENT_LOGIC_DEFINE_H_

#include "MTSegmentLogicLog.h"

//安全释放文件
#ifndef SAFE_DELETE
#define SAFE_DELETE(x) { if (x) delete (x); (x) = NULL; }	//定义安全释放函数
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) { if (x) delete [] (x); (x) = NULL; }	//定义安全释放函数
#endif

#if defined(_WIN32_) || defined(WIN32) || defined(_WIN64_) || defined(WIN64)
#   ifdef NEED_EXPORT_SYMBOL
#       define MEITU_EXPORT __declspec(dllexport)
#   else
#       define MEITU_EXPORT
#   endif
#elif defined(_ADNROID_) || defined(ANDROID) || defined(__APPLE__) || defined(__linux__)
#   ifdef NEED_EXPORT_SYMBOL
#       define MEITU_EXPORT __attribute__((visibility("default")))
#   else
#       define MEITU_EXPORT
#   endif
#else
#   define MEITU_EXPORT
#endif

//#if defined(__APPLE__)
//#define MT_ALPHA            3
//#define MT_RED              0
//#define MT_GREEN            1
//#define MT_BLUE             2
//#else
//#define MT_ALPHA            3
//#define MT_RED              0
//#define MT_GREEN            1
//#define MT_BLUE             2
//#endif
//
//#define MT_GL_RED           0
//#define MT_GL_GREEN         1
//#define MT_GL_BLUE          2
//#define MT_GL_ALPHA         3
//
//#ifndef NULL
//#define NULL 0
//#endif
//
//typedef unsigned char BYTE;
//typedef unsigned char byte;

#endif /* _H_MT_SEGMENT_LOGIC_DEFINE_H_ */
