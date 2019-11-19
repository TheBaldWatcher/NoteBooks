//
//  ManisHelperCpu.hpp
//  imagegroup
//
//  Created by ashtower on 2019/2/27.
//  Copyright © 2019年 meitu. All rights reserved.
//

#ifndef MTManisHelper_hpp
#define MTManisHelper_hpp

#include <stdio.h>
#include <memory>
#include <thread>
//#include <stdexcept>

#include "MTBeijingSdkDefine.h"

#ifdef USE_GPU_CODE
#   if PLATFORM_IOS
#       if TARGET_OS_SIMULATOR //PLATFORM_APPLE_SIMULATOR
#           include <OpenGLES/ES3/gl.h>
#           include <OpenGLES/gltypes.h>
#       elif TARGET_OS_IPHONE  // PLATFORM_IOS
#           include <OpenGLES/ES3/gl.h>
#           include <OpenGLES/gltypes.h>
#       elif TARGET_OS_MAC  //PLATFORM_MacOS
#           include <OpenGL/gl.h>
#           include <OpenGL/gltypes.h>
#       endif
#   elif defined(PLATFORM_WINDOWS)
#       include <gl/glew.h>            // OpenGL Extension "autoloader"
#   elif defined(PLATFORM_ANDROID)
#       include "mtcvlite/glbase/glUtil/gl3stub.h"
#       include <android/log.h>
#       include <jni.h>
#       if !defined(MEITU_PHONE)
#           include <GLES2/gl2.h>
#       endif
#   elif defined(__APPLE__) && defined(__MACH__)
#       if TARGET_IPHONE_SIMULATOR == 1
#           include <OpenGLES/ES3/gl.h>
#           include <OpenGLES/gltypes.h>
#       elif TARGET_OS_IPHONE == 1
#           include <OpenGLES/ES3/gl.h>
#           include <OpenGLES/gltypes.h>
#       elif TARGET_OS_MAC == 1 //PLATFORM_MacOS
#           include <OpenGL/gl.h>
#       endif
#   endif
#endif // USE_GPU_CODE

#include <manis/manis_wrapper.hpp>
#include <manis/manis.hpp>
#include <manis/image_util.hpp>

#ifdef USE_YUV_MTIMAGE
    #include <libyuv.h>
    #include <mt_image_utils.h>
    #include <mt_image_proc.h>
    #include "MTBeijingSdkImageHelper.hpp"
#endif

NAMESPACE_BYPASS_INDENT_OPEN

//// TODO @dhf 看看这里：ios编译有问题，暂时手动using一下绕过
//using GLuint = unsigned int;
//using GLenum = uint32_t;


/******************************************************************
 *                          异常
 ******************************************************************/
class ManisModelLoadError : public std::runtime_error
{
    public :
    ManisModelLoadError(const char *msg): runtime_error(msg){};
    using runtime_error::what;
};


/******************************************************************
 *                      Manis调用
 ******************************************************************/
class MTManisHelper {
public:
    /**
     @param [in] model_path 模型文件路径
     @param [in] recommended_model SDK推荐模型版本，不一致发出警告
     @param [in] device_type 模式：CPU、GPU、GPU_SP
     @param [in] mp_obj 用以共享的memory pool
     **************** 以下为GPU_SP模式参数 *************
     @param [in] precision_type gl shadel precision type
     @param [in] shader_file load or save shader file path
     @param [in] sf_obj 用以共享的shader factory
     @param [in] enable_flag 只有ios单线程要设置为false，ios多线程、android单/多线程均为true。原因是，ios单线程为true时，manis在关闭顶点时，将GPUImage的顶点也关闭
     */
    explicit MTManisHelper(const char *model_path
                  , const std::string &recommended_model = ""
                  , manis::DeviceType device_type = manis::DeviceType::DEVICE_CPU
                  , void *mp_obj = nullptr
                  // these fallowing parameters only apply for ComputeType::GPU_SP
                  , manis::OpenglColorMode cvt_mode = manis::OPENGL_CVT_MODE_RGBA2BGRA
                  , bool is_gpu_sp = false
                  , manis::OpenglShaderPrecision precision_type = manis::OpenglShaderPrecision::OPENGL_SHADER_PRECISION_HIGH
                  , const char *shader_file = nullptr
                  , void *sf_obj = nullptr
                  , bool enable_flag = true);
    
    /**
     * 构造函数二 传入模型的二进制数据，而不是模型路径
     @param [in] model_data 模型文件数据
     @param [in] model_size 模型文件尺寸
     @param [in] recommended_model SDK推荐模型版本，不一致发出警告
     @param [in] device_type 模式：CPU、GPU、GPU_SP
     @param [in] mp_obj 用以共享的memory pool
     **************** 以下为GPU_SP模式参数 *************
     @param [in] precision_type gl shadel precision type
     @param [in] shader_file load or save shader file path
     @param [in] sf_obj 用以共享的shader factory
     @param [in] enable_flag 只有ios单线程要设置为false，ios多线程、android单/多线程均为true。原因是，ios单线程为true时，manis在关闭顶点时，将GPUImage的顶点也关闭
     */
    explicit MTManisHelper(const char *model_data, const size_t model_size
                  , const std::string &recommended_model = ""
                  , manis::DeviceType device_type = manis::DeviceType::DEVICE_CPU
                  , void *mp_obj = nullptr
                  // these fallowing parameters only apply for ComputeType::GPU_SP
                  , manis::OpenglColorMode cvt_mode = manis::OPENGL_CVT_MODE_RGBA2BGRA
                  , bool is_gpu_sp = false
                  , manis::OpenglShaderPrecision precision_type = manis::OpenglShaderPrecision::OPENGL_SHADER_PRECISION_HIGH
                  , const char *shader_file = nullptr, void *sf_obj= nullptr
                  , bool enable_flag = true);
    
    // 复制、移动似乎没有意义，如果有需求，再分析
    MTManisHelper(const MTManisHelper &other) = delete;
    MTManisHelper(MTManisHelper &&other) = delete;
    MTManisHelper& operator=(const MTManisHelper &other) = delete;
    MTManisHelper& operator=(MTManisHelper &&other) = delete;
    ~MTManisHelper() noexcept;

    
    /**
     跑前向，输入为PixelsBuffer*格式

     @param [in] input_buffer 输入
     @param [in] release_input_buffer 是否在前向完之后，释放掉input_buffer。一般来说都需要释放，除非要用同一个输入跑多次前向。如不释放，在from_pixel_resize这种接口，可能会有内存泄漏
     @return 前向结果
     */
    manis::ManisDataLst Run(manis::PixelsBuffer *input_buffer, bool release_input_buffer = false);
    
    manis::ManisStatus Run(const manis::ManisDataLst& inputs, manis::ManisDataLst& outputs) {
        return net_->Run(inputs, outputs);
    };
    
#ifdef USE_GPU_CODE
    void Run(GLuint inputTextrueID, GLuint outputTextrueID
             , int gl_output_width, int gl_output_height
             , float alpha
             , const float *pfInputTexCoordTrans, const float *pfOutputTexCoordTrans);
    void BindTextureToFbo(bool force = false);
    bool IsShaderSaved() const noexcept{return is_shader_saved_;}
    
    /********* GPU/GPU_SP 优化内存相关接口 *********/
    /**
     * 共享GL shader脚本:调用该接口以获得某个已存在的分割器的shader脚本，传给新的分割器构造函数，以进行共享
     * 调用该接口后，需在最后一个对象上设置NeedToDeleteGlFactory，以标记需要在其析构时进行释放
     */
    void *GetGlFactory() noexcept;
    
    // 使用时生成纹理，使用完后删除纹理，即不对纹理进行缓存，减少美图手机内存负担
    void GenerateTextures() const noexcept;
    void DeleteTextures() const noexcept;
    
    static bool IsSupportGlCaffe() noexcept;
#endif // USE_GPU_CODE
    
    static bool IsSupportCaffe2() noexcept { return false;};
    // 内存池相关接口，调用类似于GlFactory
    void *GetMemoryPool() noexcept;
    
    /**
       注册回调，获取中间层数据的获取

     @param op_name 中间层n名字
     @param func 回调函数，该函数只有一个参数
     @param user_data 回调函数的第一个参数
     @return 注册是否成功
     */
    manis::ManisStatus RegisterOpOutputCB(const int8_t* op_name, const manis::OpOutputCB func, void* user_data) {
        return net_->RegisterOpOutputCB(op_name, func, user_data);
    };
    
    size_t GetInputBatch() const noexcept {return input_bacth_;};
    size_t GetInputChannel() const noexcept {return input_channel_;};
    size_t GetInputWidth() const noexcept {return input_width_;};
    size_t GetInputHeight() const noexcept {return input_height_;};
    size_t GetInputC4() const noexcept {return input_c4_;};
    size_t GetInputLen() const noexcept {return input_len_;};

    size_t GetOutputSize() const noexcept {return output_size_;};
    size_t GetOutputBatch() const noexcept {return output_bacth_;};
    size_t GetOutputChannel() const noexcept {return output_channel_;};
    size_t GetOutputWidth() const noexcept {return output_width_;};
    size_t GetOutputHeight() const noexcept {return output_height_;};
    size_t GetOutputC4() const noexcept {return output_c4_;};
    size_t GetOutputLen() const noexcept {return output_len_;};
    
    const std::string& GetModelName(){return model_name_;};
    
private:
    void DoInit_(const manis::FileStorage &stream, manis::OpenglShaderPrecision precision_type
                 , const char *shader_file
                 , void *mp_obj, void *sf_obj
                 , bool is_gpu_sp, manis::OpenglColorMode cvt_mode
                 , bool enable_falg
                 , const std::string &recommended_model = "");
    
    void InitNet_(const manis::FileStorage& stream, manis::OpenglShaderPrecision precision_type
                  , const char *shader_file
                  , void *mp_obj, void *sf_obj
                  , bool is_gpu_sp, manis::OpenglColorMode cvt_mode
                  , bool enable_falg);
    
    void InitParam_(const std::string &recommended_model = "");
    
#ifdef USE_GPU_CODE
    void GpuConfig_(manis::OpenglShaderPrecision precision_type
                    , const char *shader_file, manis::EventObjBase *sf_obj
                    , bool is_gpu_sp, manis::OpenglColorMode cvt_mode
                    , bool enable_falg);
    void SetShaderFactory_(manis::OpenglShaderPrecision precision_type, void *sf_obj);
    
#endif  //USE_GPU_CODE
    
    void SetMemoryPool_(void *mp_obj);
    
    void CheckAndUpdateOutputParams_(manis::PixelsBuffer *input_buffer, const manis::ManisDataLst& outputs); // 输入尺寸和模型文件的不一致时，输出的尺寸需要更新一下
    
    size_t input_bacth_ = 0;
    size_t input_channel_ = 0;
    size_t input_width_ = 0;
    size_t input_height_ = 0;
    size_t input_c4_ = 0;
    size_t input_len_ = 0;

    size_t output_size_ = 0;
    size_t output_bacth_ = 0;
    size_t output_channel_ = 0;
    size_t output_width_ = 0;
    size_t output_height_ = 0;
    size_t output_c4_ = 0;
    size_t output_len_ = 0;
    
    std::string model_name_;
    
    manis::DeviceType device_type_ = manis::DeviceType::DEVICE_CPU;

    std::shared_ptr<manis::ManisWrapper>  manis_wrapper_ = nullptr;
    manis::Manis *net_ = nullptr; // manis_wrapper_负责管理内存
    
    /************************************************************
     *              美图手机，共享内存、shader相关接口
     ************************************************************/
    // manis_wrapper_析构时，会释放mp和sf，因此如果有sdk持有了共享的mp或sf，共享源sdk的manis_wrapper_
    // 就不能析构。这里用mp_manis_wrapper_/sf_manis_wrapper_来增加manis_wrapper_的引用计数，保证其
    // 生命周期长于mp和sf
    manis::EventObjBase *mp_obj_ = nullptr; // 内存池管理对象，cpu/gpu
    std::shared_ptr<manis::ManisWrapper>  mp_manis_wrapper_ = nullptr;
    
    // manis2.1.0.1开始支持wrapper来管理资源，但目前需要在manis2.0.5.1上跑，所以还是要手动释放
    bool is_using_shared_memory_pool_ = false;
    
#ifdef USE_GPU_CODE
    manis::EventObjBase *sf_obj_ = nullptr; // sf_obj_是openGL脚本管理对象，gpu才有
    std::shared_ptr<manis::ManisWrapper>  sf_manis_wrapper_ = nullptr;
    
    bool is_shader_saved_ = false;
    bool is_using_shared_shader_factory_ = false;   ///< 类似于is_using_shared_memory_pool_
    
    // 构造和Run不在同一线程、Run和上一次Run不在同一线程时，需要bind fbo
    std::thread::id pre_thread_id_;
    
#endif  //USE_GPU_CODE
};

NAMESPACE_BYPASS_INDENT_CLOSE

#endif /* MTManisHelper_hpp */

