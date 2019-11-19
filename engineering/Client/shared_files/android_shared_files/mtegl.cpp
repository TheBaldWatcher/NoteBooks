/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "mtegl.h"
#include <errno.h>
#include <malloc.h>

/**
 * Initialize an EGL context for the current display.
 */
int egl_init_display(struct EGLEnvionment* eglEnvionment,int envWidth,int envHeight,int contexClientVersion) {
    // initialize OpenGL ES and EGL
   
    EGLint w, h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY)
    {
        LOGE_SDK("eglGetDisplay failed.");
        return -1;
    }
    
    EGLint majorVersion,minorVersion;
    EGLBoolean ret = EGL_FALSE;
    
    ret = eglInitialize(display, &majorVersion, &minorVersion);
    if (ret != EGL_TRUE)
    {
        EGLint err = eglGetError();
        if (err != EGL_SUCCESS)
        {
            char errmsg[32];
            LOGE_SDK("eglInitialize failed. Error code: 0x%04x", errno);
            // EGL_BAD_DISPLAY      EGLDisplay参数错误
            // EGL_NOT_INITIALIZED  EGL不能初始化
            return -1;
        }  
    }
    
	LOGI_SDK("zcd eglInitialize majorVersion = %d, minorVersion = %d",majorVersion,minorVersion);
    
    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_ALPHA_SIZE,8,
        EGL_NONE
    };
    
	EGLBoolean success = EGL_FALSE;
    /* Here, the application chooses the configuration it desires. In this
     * sample, we have a very simplified selection process, where we pick
     * the first EGLConfig that matches our criteria */
	success = eglGetConfigs(display,NULL,0,&numConfigs);
   
	if(success == EGL_TRUE && numConfigs > 0)
    {
		//LOGD_SDK("zcd egl_YES_surface =");

	    EGLBoolean i = eglChooseConfig(display, attribs, &config, 1, &numConfigs);

		LOGI_SDK("zcd egl_no_surface = %0x,%d,numConfigs=%d",eglGetError(),i,numConfigs);
	}
	else
	{
		LOGE_SDK("zcd egl_no_surface =");

		return -1;
	}

	const GLuint atrrilist[] = {
		EGL_WIDTH,static_cast<GLuint>(envWidth),
		EGL_HEIGHT,static_cast<GLuint>(envHeight),
		EGL_LARGEST_PBUFFER,EGL_TRUE,
		EGL_NONE
	};

	LOGI_SDK("zcd eglCreatePbufferSurface");
    surface = eglCreatePbufferSurface(display,config,(const EGLint*)atrrilist);
	LOGI_SDK("zcd eglCreatePbufferSurface1");
	if(surface == NULL)
	{
		//EGL_BAD_CONFIG
		EGLint i = eglGetError();
		LOGE_SDK("zcd egl_no_surface = %0x",i);
		return -1;
	}

	const EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, contexClientVersion, EGL_NONE};
	LOGI_SDK("zcd egl_no_contextAttribs = %0x",eglGetError());
    context = eglCreateContext(display, config,  EGL_NO_CONTEXT, contextAttribs);
	if(context == NULL)
	{
        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
        }
		//EGL_BAD_CONFIG
		EGLint i = eglGetError();
		LOGE_SDK("zcd egl_no_context = %0x",i);
		return -1;
	}
    
	LOGI_SDK("zcd eglMakeCurrent");
    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
        }
        LOGE_SDK(" zcd Unable to eglMakeCurrent");
        return -1;
    }

	LOGI_SDK("zcd shader1");
    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);
	LOGI_SDK("zcd shader2 = %d,%d",w,h);

    eglEnvionment->display = display;
    eglEnvionment->context = context;
    eglEnvionment->surface = surface;
    eglEnvionment->width = w;
    eglEnvionment->height = h;
 
    return 0;
}

/**
 * Tear down the EGL context currently associated with the display.
 */
void egl_term_display(struct EGLEnvionment* eglEnvionment) {
    if (eglEnvionment->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(eglEnvionment->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (eglEnvionment->context != EGL_NO_CONTEXT) {
            eglDestroyContext(eglEnvionment->display, eglEnvionment->context);
        }
        if (eglEnvionment->surface != EGL_NO_SURFACE) {
            eglDestroySurface(eglEnvionment->display, eglEnvionment->surface);
        }
        eglTerminate(eglEnvionment->display);
    }

    eglEnvionment->display = EGL_NO_DISPLAY;
    eglEnvionment->context = EGL_NO_CONTEXT;
    eglEnvionment->surface = EGL_NO_SURFACE;
}

