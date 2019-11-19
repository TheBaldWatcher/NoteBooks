#ifndef _H_MT_SEGMENT_LOGIC_LOG_H_
#define _H_MT_SEGMENT_LOGIC_LOG_H_

#define DEBUG_LOG_SDK
#ifdef DEBUG_LOG_SDK
#   ifdef LOGI_SDK
#       undef LOGI_SDK
#   endif
#   ifdef LOGE_SDK
#       undef LOGE_SDK
#   endif
#   ifdef LOGD_SDK
#       undef LOGD_SDK
#   endif
#   define LIB_TARGET_NAME_STR_HELPER(name) #name
#   define LIB_TARGET_NAME_STR(tag) LIB_TARGET_NAME_STR_HELPER(tag)
////////////////////////Android platform//////////////////////////////////////////////
#   if defined(ANDROID) || defined(__ANDROID__)
#       define LIB_LOG_TAG LIB_TARGET_NAME_STR(LIB_TARGET_NAME)
#       include <android/log.h>
#       define LOGI_SDK(...) __android_log_print(ANDROID_LOG_INFO, LIB_LOG_TAG, __VA_ARGS__);
#       define LOGD_SDK(...) __android_log_print(ANDROID_LOG_DEBUG, LIB_LOG_TAG, __VA_ARGS__);
#       define LOGE_SDK(...)  __android_log_print(ANDROID_LOG_ERROR, LIB_LOG_TAG, __VA_ARGS__);
#   elif defined(__linux__)
///////////////////////////////////Linux///////////////////////////////////////
#       include <stdio.h>
#       include <ctime>
#       define LOGI_SDK(...) fprintf(stderr,"%08d [INFO]: ",(int)clock());fprintf(stderr,__VA_ARGS__);fprintf(stderr,"\r\n");
#       define LOGD_SDK(...) fprintf(stderr,"%08d [DEBUG]: ",(int)clock());fprintf(stderr,__VA_ARGS__);fprintf(stderr,"\r\n");
#       define LOGE_SDK(...) fprintf(stderr,"%08d [ERROR]: ",(int)clock());fprintf(stderr,__VA_ARGS__);fprintf(stderr,"\r\n");
///////////////////////////////////apple///////////////////////////////////////
#   elif defined(__APPLE__)
#       define LIB_LOG_TAG(tag) LIB_TARGET_NAME_STR_HELPER(tag)
#       define LOGI_SDK(format, ...) printf(LIB_LOG_TAG(": info "));printf (format, ##__VA_ARGS__);printf ("\n")
#       define LOGD_SDK(format, ...) printf(LIB_LOG_TAG(": debug "));printf (format, ##__VA_ARGS__);printf ("\n")
#       define LOGE_SDK(format, ...) printf(LIB_LOG_TAG(": error "));printf (format, ##__VA_ARGS__);printf ("\n")
#   else
////////////////////////////////Windows//////////////////////////////////////
#       include <windows.h>
#       include <stdio.h>
#       include <ctime>
#       define LOGI_SDK(...) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); \
fprintf(stderr,"%08d : ",clock());fprintf(stderr,__VA_ARGS__);fprintf(stderr,"\r\n");

#       define LOGD_SDK(...) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN | FOREGROUND_BLUE |  FOREGROUND_INTENSITY); \
fprintf(stderr,"%08d : ",clock());fprintf(stderr,__VA_ARGS__);fprintf(stderr,"\r\n");

#       define LOGE_SDK(...) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED |  FOREGROUND_INTENSITY); \
fprintf(stderr,"%08d : ",clock());fprintf(stderr,__VA_ARGS__);fprintf(stderr,"\r\n%s(%d)\r\n",__FILE__,__LINE__);

#   endif//defined(ANDROID) || defined(__ANDROID__)
#else
#   define LOGI_SDK(...)
#   define LOGD_SDK(...)
#   define LOGE_SDK(...)
#endif//DEBUG_LOG

#ifndef LOGD_SDK_SHOW
#   undef LOGD_SDK
#   define LOGD_SDK(...)
#endif

#define LOG_START_FUNC() LOGD("++(%s)++",__FUNCTION__)

#define LOG_END_FUNC() LOGD("--(%s)--",__FUNCTION__)

#endif//_H_MT_SEGMENT_LOGIC_LOG_H_
