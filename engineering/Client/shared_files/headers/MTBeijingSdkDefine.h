//
//  Header.h
//  SdkDev
//
//  Created by ashtower on 2018/7/27.
//  Copyright © 2018年 meitu. All rights reserved.
//

#ifndef _H_MT_BEIJING_SDK_DEFINE_H_
#define _H_MT_BEIJING_SDK_DEFINE_H_
#include <iostream>
#include <chrono>
#include "MTSegmentLogicDefine.h"
#include "MTSegmentLogicLog.h"

#if defined(__GNUC__) || defined(__clang__)
#   define _GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#   if _GCC_VERSION <  40500
#       define _BEIJING_SDK_DEPRECATED(explanation)   __attribute__((deprecated))
#   else
#       define _BEIJING_SDK_DEPRECATED(explanation)   __attribute__((deprecated(explanation)))
#   endif
#else
#   if defined(_MSC_VER)|| defined (__INTEL_COMPILER)
#       define _BEIJING_SDK_DEPRECATED(explanation) __declspec(deprecated(explanation))
#   else
#       define _BEIJING_SDK_DEPRECATED(explanation)
#   endif
#endif

#if defined(_WIN32) || defined(_WIN32_) || defined(WIN32) || defined(_WIN64_) || defined(WIN64) || defined(_WIN64)
    #define PLATFORM_WINDOWS 1
    #if _MSC_VER < 1600 // before Visual Studio 2010 have not <stdint.h>
        #define STDINT_MISSING   1
    #endif
#elif defined(ANDROID) || defined(_ANDROID_)
    #define PLATFORM_ANDROID 1
    #if !defined(__clang__) && (GCC_VERSION < 40500) // test if GCC > 4.5.0 https://gcc.gnu.org/c99status.html
        #define STDINT_MISSING   1
    #endif
#elif defined(_APPLE_)
    #define PLATFORM_IOS     1
    // macro define of TARGET_OS_IPHONE, TARGET_OS_SIMULATOR, TARGET_CPU_ARM, TARGET_CPU_ARM64 etc
    #include <TargetConditionals.h>
#elif defined(__linux__) || defined(__unix__) || defined(linux) || defined(__linux)
    #define PLATFORM_LINUX   1
#else
    #define PLATFORM_UNKNOWN 1
    #define STDINT_MISSING   1
#endif

#define net_create(nettype, net_obj)                    \
if (!mtnn::MTNet::IsSupportNet(mtnn::nettype)) {         \
    LOGE_SDK("Net Type "#nettype" is not supported!");      \
}                                                       \
net_obj.reset(mtnn::MTNet::CreateMTNet(mtnn::nettype));

// 被Xcode在namespace里的自动缩进弄得头大，定义一个宏来绕过
#define NAMESPACE_BYPASS_INDENT_OPEN namespace LIB_TARGET_NAME {
#define NAMESPACE_BYPASS_INDENT_CLOSE }

#define CreateSharedMtimage(raw_ptr)                   \
std::shared_ptr<mt_image_t> (raw_ptr, [](mt_image_t *p){mt_image_utils_destroy(p);});

// by dhf 测速
//#ifdef TIME_TEST_LOG
#if 0
struct TimeTestHelper {
    std::string name;

    std::chrono::microseconds duration{0};
    decltype(std::chrono::system_clock::now()) duration_start;
    size_t duration_cnt = 0;
    
    TimeTestHelper (const char *n) : name{n}, duration_start{std::chrono::system_clock::now()}{};
    
    ~TimeTestHelper(){
//        std::cerr<<"duration:\t"<<name<<"\t"<<duration.count()/duration_cnt<<" μs"<<" ("<<duration.count()/duration_cnt/1000.0f<<" ms)"<<std::endl;
    }
    void Record() {
        duration_start = std::chrono::system_clock::now();
    }
    void Update() {
        ++duration_cnt;
        
        auto duration_end = std::chrono::system_clock::now();
        duration += std::chrono::duration_cast<decltype(duration)>(duration_end - duration_start);
        LOGI_SDK("duration:\t %s\t %llu μs (%f ms)",name.c_str(),duration.count()/duration_cnt, duration.count()/duration_cnt/1000.0f);
//        std::cerr<<"duration:\t"<<name<<"\t"<<duration.count()/duration_cnt<<" μs"<<" ("<<duration.count()/duration_cnt/1000.0f<<" ms)"<<std::endl;
    }
};

#define TIME_TEST_RECORD(duration) \
static TimeTestHelper duration##__helper{#duration}; \
duration##__helper.Record();

#define TIME_TEST_UPDATE(duration) \
duration##__helper.Update();

#else

#define TIME_TEST_RECORD(duration) 
#define TIME_TEST_UPDATE(duration)

#endif

// plaform detection
#if defined(_WIN32) || defined(_WIN32_) || defined(WIN32) || defined(_WIN64_) || defined(WIN64) || defined(_WIN64)
#   define PLATFORM_WINDOWS 1
#   if _MSC_VER < 1600 // before Visual Studio 2010 have not <stdint.h>
#   define STDINT_MISSING   1
#   endif
#elif defined(ANDROID) || defined(_ANDROID_)
#   define PLATFORM_ANDROID 1
#   if !defined(__clang__) && (GCC_VERSION < 40500) // test if GCC > 4.5.0 https://gcc.gnu.org/c99status.html
#   define STDINT_MISSING   1
#   endif
#elif defined(_APPLE_) || defined(APPLE) || defined(__APPLE__)
#   define PLATFORM_IOS   1
#   include <TargetConditionals.h>
#elif defined(__linux__) || defined(__unix__) || defined(linux) || defined(__linux)
#   define PLATFORM_LINUX   1
#else
#   define PLATFORM_UNKNOWN 1
#   define STDINT_MISSING   1
#endif




#endif /* _H_MT_BEIJING_SDK_DEFINE_H_ */
