#ifndef _MTEGL_H_
#define _MTEGL_H_
#include <EGL/egl.h>	//for egl
#include <EGL/eglplatform.h>
#include <EGL/eglext.h>

#include <GLES2/gl2.h>
#include "MTBeijingSdkDefine.h"


typedef struct EGLEnvionment {
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
}EGLEnvionment;

int egl_init_display(struct EGLEnvionment* eglEnvionment,int envWidth,int envHeight,int contexClientVersion = 2);

void egl_term_display(struct EGLEnvionment* eglEnvionment);

#endif

