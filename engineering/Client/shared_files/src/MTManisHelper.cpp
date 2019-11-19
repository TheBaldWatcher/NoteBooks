#include "MTManisHelper.hpp"

#include "MTBeijingSdkDefine.h"
#include <vector>

#define CHECK_STAT_AND_TRY_THROW(stat) \
if (manis::STATUS_SUCCESS != stat) {    \
LOGE_SDK("fail to init! error %d , %s, %d", stat, __FUNCTION__, __LINE__);   \
throw ("fail to init!");                              \
}

#if 0
#import <UIKit/UIKit.h>
#import <Uikit/UIImagePickerController.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <string>
#endif

NAMESPACE_BYPASS_INDENT_OPEN

MTManisHelper::MTManisHelper(const char *model_path
                             , const std::string &recommended_model
                             ,manis::DeviceType device_type
                             , void *mp_obj
                             , manis::OpenglColorMode cvt_mode
                             , bool is_gpu_sp
                             , manis::OpenglShaderPrecision precision_type
                             , const char *shader_file, void *sf_obj
                             , bool enable_flag)
: device_type_(device_type)
{
    manis::FileStorage stream;
    try {
        if (!model_path || !stream.LoadFile(reinterpret_cast<const int8_t *>(model_path))) {
            LOGE_SDK("Fail to load model, model path %s", model_path);
            throw "";
        } else {
            LOGI_SDK("Load model file");
        }
        
        DoInit_(stream, precision_type, shader_file, mp_obj, sf_obj, is_gpu_sp, cvt_mode, enable_flag, recommended_model);
    } catch (...){
        throw ManisModelLoadError((std::string{"Fail to load model, model path"} + model_path).c_str());
    }
}


MTManisHelper::MTManisHelper(const char *model_data, const size_t model_size
                             , const std::string &recommended_model
                             , manis::DeviceType device_type, void *mp_obj
                             , manis::OpenglColorMode cvt_mode
                             , bool is_gpu_sp
                             , manis::OpenglShaderPrecision precision_type
                             , const char *shader_file, void *sf_obj
                             , bool enable_flag)
: device_type_(device_type)
{
    manis::FileStorage stream;
    try {
        if (!model_data || 0 == model_size
            || !stream.LoadData(reinterpret_cast<const int8_t *>(model_data)
                                , static_cast<uint32_t>(model_size)) )
        {
            LOGE_SDK("Fail to load model, model data %p, size %zu", model_data, model_size);
            throw "";
        } else {
            LOGI_SDK("Load model data");
        }
        
        DoInit_(stream, precision_type, shader_file, mp_obj, sf_obj, is_gpu_sp, cvt_mode, enable_flag,recommended_model);
    } catch (...) {
        throw ManisModelLoadError("Fail to load model data");
    }
    
}

void MTManisHelper::DoInit_(const manis::FileStorage &stream
                            , manis::OpenglShaderPrecision precision_type
                            , const char *shader_file
                            , void *mp_obj, void *sf_obj
                            , bool is_gpu_sp
                            , manis::OpenglColorMode cvt_mode
                            , bool enable_flag
                            , const std::string &recommended_model)
{
    InitNet_(stream, precision_type, shader_file, mp_obj, sf_obj, is_gpu_sp, cvt_mode, enable_flag);
    
    // 初始化参数
    InitParam_(recommended_model);
}

void MTManisHelper::InitNet_(const manis::FileStorage &stream
                             , manis::OpenglShaderPrecision precision_type
                             , const char *shader_file
                             , void *mp_obj, void *sf_obj
                             , bool is_gpu_sp
                             , manis::OpenglColorMode cvt_mode
                             , bool enable_falg)
{
    // manis2.1.0.1开始支持wrapper来管理资源，但目前需要在manis2.0.5.1上跑，所以还是要手动释放
    auto deleter = [=](manis::ManisWrapper *manis_wrapper){
        if (!is_using_shared_memory_pool_) {
            manis_wrapper->FreeEventObj(mp_obj_);
        }
#ifdef USE_GPU_CODE
        if (sf_obj_ && !is_using_shared_shader_factory_) {
            manis_wrapper->FreeEventObj(sf_obj_);
        }
#endif  //USE_GPU_CODE
        delete manis_wrapper;
    };
    manis_wrapper_ = std::shared_ptr<manis::ManisWrapper>(new manis::ManisWrapper(), deleter);
    
    auto manis_info = manis_wrapper_->CreateManisInfo(device_type_);
    manis_info->SetInfo(false);
    net_ = manis_wrapper_->CreateManis(manis_info);
    
    SetMemoryPool_(mp_obj);

#ifdef USE_GPU_CODE
    if (manis::DeviceType::DEVICE_OPENGL == device_type_) {
        GpuConfig_(precision_type, shader_file, static_cast<manis::EventObjBase *>(sf_obj), is_gpu_sp, cvt_mode, enable_falg);
    }
#endif  //USE_GPU_CODE
    
    auto error = net_->Init(&stream);
    
    if (manis::STATUS_SUCCESS != error) {
        LOGE_SDK("fail to load model!, error %d", error);
        throw("fail to load model!, error %d");
    };
    
}

void MTManisHelper::InitParam_(const std::string &recommended_model)
{
    manis::NetInfo net_info;
    net_->GetNetInfo(&net_info);
    auto *input_shape_0 = net_info.input_lst.front().shape;
    input_bacth_ = input_shape_0->GetValue(manis::Shape_N_);
    input_width_ = input_shape_0->GetValue(manis::Shape_W_);
    input_height_ = input_shape_0->GetValue(manis::Shape_H_);
    input_channel_ = input_shape_0->GetValue(manis::Shape_C_);
    input_c4_ = input_shape_0->GetValue(manis::Shape_C4_);
    input_len_ = input_width_ * input_height_ * input_channel_;
    
    auto *output_shape_0 = net_info.output_lst.front().shape;
    output_size_ = net_info.output_lst.size();
    output_bacth_ = output_shape_0->GetValue(manis::Shape_N_);
    output_width_ = output_shape_0->GetValue(manis::Shape_W_);
    output_height_ = output_shape_0->GetValue(manis::Shape_H_);
    output_channel_ = output_shape_0->GetValue(manis::Shape_C_);
    output_c4_ = output_shape_0->GetValue(manis::Shape_C4_);
    output_len_ = output_width_ * output_height_ * output_channel_;
    
    model_name_ = std::string{(const char*)net_info.name};
    LOGI_SDK("Model Name : %s", model_name_.c_str());
    if(!recommended_model.empty()){
        auto idx = recommended_model.find(model_name_);
        if(idx == std::string::npos ){
           LOGE_SDK("Model does not match SDK version, the effect may be problematic");
        }
    }
}

MTManisHelper::~MTManisHelper() noexcept
{
    manis_wrapper_->FreeManis(net_);//manis对象释放后，才能释放脚本管理对象和内存池对象
    /**
     背景：mp和sf共享时，SDK_A的mp_A/sf_A共享给SDK_B，此时wrapper_A需要比SDK_B的wrapper_B的生命周期长。
     
     这里需要显式释放wrapper_：
        SDK_B拿到mp_A时，会持有一个wrapper_A（mp_manis_helper），以保证wrapper_A不会在SDK_B还在使用时，
            SDK_A在析构时会析构wrapper_A进而把mp_A析构掉；
        但这里有个边界情况，即析构时，如果SDK_A已析构，SDK_B中先释放掉mp_manis_helper在释放wrapper_B，
            会导致wrapper_A生命周期≤wrapper_B，所以需要显式释放wrapper
     */
    manis_wrapper_.reset();
}

#ifdef USE_GPU_CODE

void MTManisHelper::GpuConfig_(manis::OpenglShaderPrecision precision_type, const char *shader_file, manis::EventObjBase *sf_obj, bool is_gpu_sp, manis::OpenglColorMode cvt_mode, bool enable_falg)
{
    LOGI_SDK(__FUNCTION__);

    SetShaderFactory_(precision_type, sf_obj);
    
    auto stat = manis::ManisStatus::STATUS_UNKNOWN_ERROR;
    bool is_shader_loaded = false;
    
    //将脚本管理对象传给manis对象
    auto op_info = manis_wrapper_->CreateOpInfo(manis::DEVICE_OPENGL);
    if(op_info){
        op_info->SetInfo(sf_obj_, enable_falg, manis::OPENGL_TEXTURE_TYPE_FLOAT32, -1, cvt_mode);
        stat = net_->SetConfig(manis::Configure_OpInfo, (void *)op_info);
        CHECK_STAT_AND_TRY_THROW(stat)
    }
    
    //配置为sp模式
    if (is_gpu_sp) {
        auto net_info = manis_wrapper_->CreateNetInfo(manis::DEVICE_OPENGL);
        if(net_info){
            net_info->SetInfo(manis::GLRun_SP);
            stat = net_->SetConfig(manis::Configure_NetInfo, (void *)net_info);
            CHECK_STAT_AND_TRY_THROW(stat)
        }
    }
    
    // 安装APP时、或者读取shder文件发现其损坏后，会将编译的shader进行一次保存，以加快下次加载的速度
    // 加载shader
    if (shader_file) {
        long b[2] {(long)(sf_obj_), (long)(shader_file)};
        auto shader_flag = manis_wrapper_->ExecEvent(manis::Event_GL_ShadersFactory_Load, (void *)b);
        is_shader_loaded = nullptr != shader_flag;
    } else {
        LOGI_SDK("Opening shader file faile, No shader file path.");
    };
    // 保存shader
    if (shader_file) {
        long b[2] = {static_cast<long>((long)sf_obj_),static_cast<long>((long)shader_file)};
        if (nullptr != manis_wrapper_->ExecEvent(manis::Event_GL_ShadersFactory_Save,(void *)b)) {
            is_shader_saved_ = true;
        }
    }
}

void MTManisHelper::SetShaderFactory_(manis::OpenglShaderPrecision precision_type, void *sf_obj)
{
        // precision
    auto arg = manis_wrapper_->CreateParamInfo(manis::Param_GL_ShaderFactory);
    arg->SetInfo(precision_type, -1);
    
        // 如果外部没传入，创建脚本管理对象
    auto src_manis_helper = static_cast<MTManisHelper *>(sf_obj);
    if (src_manis_helper) {
        sf_obj_ = src_manis_helper->sf_obj_;
        sf_manis_wrapper_ = src_manis_helper->manis_wrapper_;
        is_using_shared_shader_factory_ = true;
    } else {
        sf_obj_ = manis_wrapper_->ExecEvent(manis::Event_GL_ShadersFactory_Register, (void *)arg);
        is_using_shared_shader_factory_ = false;
    }
}

void *MTManisHelper::GetGlFactory () noexcept
{
    return static_cast<void *>(this);// data_core_
}

void MTManisHelper::GenerateTextures() const noexcept
{
        //run前创建所有的纹理
    if (mp_obj_) {
        manis::GLTextureEventType et1 = manis::Excute_GL_CreateAllTexture;
        long c1[2] = {static_cast<long>((long)mp_obj_), static_cast<long>((long)&et1)};
        manis_wrapper_->ExecEvent(manis::Event_GL_Texture, (void *)c1);
    }
}

void MTManisHelper::DeleteTextures() const noexcept
{
        //删除init时创建的所有纹理
    if (mp_obj_) {
        manis::GLTextureEventType et = manis::Excute_GL_DeleteAllTexture;
        long c[2] = {static_cast<long>((long)mp_obj_), static_cast<long>((long)&et)};
        manis_wrapper_->ExecEvent(manis::Event_GL_Texture, (void *)c);
    }
}

bool MTManisHelper::IsSupportGlCaffe() noexcept
{
    manis::ManisWrapper manis_detect;
    int gl_detect_result[2] = {0, 0};
    
        // is_support_gl    0---》当前平台不支持manis openGL版本， 1---》当前平台支持manis openGL版本
        // support_bit    16--->支持16位   32--->支持32位
    int support_bit = 0;
    bool is_support_gl = (bool)manis_detect.ExecEvent(manis::Event_GL_IsSupportGl, (void *)&support_bit);
    manis_detect.ExecEvent(manis::Event_GL_IsSupportGl, (void *)gl_detect_result);
    
    return is_support_gl;
}

//void loadTextureToUIImage(GLuint texture, int width, int height, std::string file_name, int channel, bool is_mask)
//{
//    GLuint fbo;
//    glGenFramebuffers(1, &fbo);
//        //绑定framebuffer
//    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
//    glViewport(0, 0, width, height);
//    unsigned char *pData = new unsigned char[width * height << 2];
//    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pData);
//    auto pp = pData;
//
//    auto dhf_heler = [](NSString *fileCompent) {
//        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
//        return [[NSString stringWithFormat:@"%@/debug_%@", [paths objectAtIndex:0], fileCompent] UTF8String];
//    };
//
//
//    std::string out_name;
//    if (is_mask) {
//        for (int i = 0 ; i < width * height; ++i) {
//            pp[3] = 255;
//            pp +=4;
//        }
//        out_name = file_name + "_output.png";
//    } else {
//        auto dhf_in_origin_name = file_name + "_input_origin.txt";
//        auto dhf_in_origin_ios_dir = dhf_heler([NSString stringWithUTF8String:dhf_in_origin_name.c_str()]);
//        std::ofstream of (dhf_in_origin_ios_dir, std::ios::binary);
//        of.write((char*)pData, width * height <<2);
//
//
//        out_name = file_name + "_input.png";
//    }
//    auto ios_dir = dhf_heler([NSString stringWithUTF8String:out_name.c_str()]);
//    stbi_write_png(ios_dir, width, height, channel, pData, width*4);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glDeleteFramebuffers(1, &fbo);
//}

void MTManisHelper::Run(GLuint inputTextrueID, GLuint outputTextrueID
                        , int gl_output_width, int gl_output_height
                        , float alpha
                        , const float *pfInputTexCoordTrans, const float *pfOutputTexCoordTrans)
{
    auto inputInfo = manis_wrapper_->CreateParamInfo(manis::Param_GL_SPInOutPut);
    inputInfo->SetInfo(inputTextrueID, outputTextrueID, pfInputTexCoordTrans);
    auto outputInfo = manis_wrapper_->CreateParamInfo(manis::Param_GL_SPInOutPut);
    outputInfo->SetInfo( outputTextrueID, gl_output_width, gl_output_height, alpha, pfOutputTexCoordTrans);
    
    manis::ManisData<float> input_tensor_gpu,output_tensor_gpu;
    
    input_tensor_gpu.SetData(inputInfo, sizeof(*inputInfo));
    output_tensor_gpu.SetData(outputInfo, sizeof(*outputInfo));
    
        // inputs_gpu、outputs_gpu
    manis::ManisDataLst inputs_gpu;
    manis::ManisDataLst outputs_gpu;
    inputs_gpu.push_back(&input_tensor_gpu);
    outputs_gpu.push_back(&output_tensor_gpu);
    
    BindTextureToFbo();
    
    net_->Run(inputs_gpu, outputs_gpu);
    
//    static int dhf_cnt = 0;
//    if (dhf_cnt & 1) {
//        loadTextureToUIImage(inputTextrueID, GetInputWidth(), GetInputHeight(), std::to_string(dhf_cnt), 4, false);
//        loadTextureToUIImage(outputTextrueID, gl_output_width, gl_output_height, std::to_string(dhf_cnt), 4, true);
//    }
//    dhf_cnt++;
//    if (dhf_cnt % 5 == 0)
//        LOGE_SDK("====%s %d  %d", __FUNCTION__, __LINE__, dhf_cnt);
}

void MTManisHelper::BindTextureToFbo(bool force)
{
    if (force || pre_thread_id_ != std::this_thread::get_id()) {
        pre_thread_id_ = std::this_thread::get_id();
        manis::GLTextureEventType et = manis::Excute_GL_BindFbo;
        long c[2] = {static_cast<long>((long)mp_obj_), static_cast<long>((long)&et)};
        manis_wrapper_->ExecEvent(manis::Event_GL_Texture, (void *)c);
    }
}

#endif  //USE_GPU_CODE

manis::ManisDataLst MTManisHelper::Run(manis::PixelsBuffer *input_buffer, bool release_input_buffer)
{
    manis::ManisDataLst inputs, outputs;
    manis::ManisData<float> input_tensor_float;
    input_tensor_float.SetData(input_buffer->get_data(), static_cast<uint32_t>(input_len_));
    
        // 实际输入宽高可能和模型文件中写的不一致，因此要用input_buffer的数据
    input_tensor_float.SetShape(static_cast<int>(GetInputBatch()), static_cast<int>(GetInputChannel())
                                , input_buffer->get_hegiht(), input_buffer->get_width()
                                , static_cast<int>(GetInputC4()));
    inputs.push_back(&input_tensor_float);
    
    net_->Run(inputs, outputs);
    
    CheckAndUpdateOutputParams_(input_buffer, outputs);
    
    if (release_input_buffer) {
        manis::release_pixels_buffer(input_buffer);
    }
    
    return outputs;
}

void MTManisHelper::SetMemoryPool_(void *mp_obj)
{
    // 如果外部没传入,创建内存池对象
    auto src_manis_helper = static_cast<MTManisHelper *>(mp_obj);
    if (src_manis_helper) {
        mp_obj_ = src_manis_helper->mp_obj_;
        mp_manis_wrapper_ = src_manis_helper->manis_wrapper_;
        is_using_shared_memory_pool_ = true;
    } else {
        mp_obj_ = manis_wrapper_->ExecEvent(manis::Event_DataCore, (void*)&device_type_);
        is_using_shared_memory_pool_ = false;
    }
    
    // 将内存池对象传给manis对象
    auto dc_info = manis_wrapper_->CreateDataCoreInfo(device_type_);
    if(dc_info){
        dc_info->SetInfo(device_type_, mp_obj_);
        auto stat = net_->SetConfig(manis::Configure_DataCoreInfo, (void *)dc_info);
        CHECK_STAT_AND_TRY_THROW(stat);
    }
    
    // mp共享时，需要如下设置
    manis::TensorInfoBase* tensor_info = manis_wrapper_->CreateTensorInfo(device_type_);
    if(tensor_info){
        tensor_info->SetInfo(manis::Light_);
        auto stat = net_->SetConfig(manis::Configure_TensorInfo, (void*)tensor_info);
        CHECK_STAT_AND_TRY_THROW(stat);
    }
}

void *MTManisHelper::GetMemoryPool() noexcept
{
    return static_cast<void *>(this);
}

#ifdef USE_YUV_MTIMAGE
std::shared_ptr<mt_image_t>
create_image_with_same_formmat(mt_image_handle_t src_image, int dst_w, int dst_h)
{
    auto ret = CreateSharedMtimage(nullptr);
    
    switch (src_image->format){
        case MT_PIX_FMT_RGBA8U: {
            ret = CreateSharedMtimage(mt_image_utils_create_from_rgba(dst_w, dst_h, nullptr));
            break;
        }
        case MT_PIX_FMT_BGRA8U: {
            ret = CreateSharedMtimage(mt_image_utils_create_from_bgra(dst_w, dst_h, nullptr));
            break;
        }
        case MT_PIX_FMT_GRAY8U: {
            ret = CreateSharedMtimage(mt_image_utils_create_from_gray(dst_w, dst_h, nullptr));
            break;
        }
        case MT_PIX_FMT_NV12: {
            ret = CreateSharedMtimage(mt_image_utils_create_from_nv12(dst_w, dst_h, nullptr, nullptr));
            break;
        }
        case MT_PIX_FMT_NV21: {
            ret = CreateSharedMtimage(mt_image_utils_create_from_nv21(dst_w, dst_h, nullptr, nullptr));
            break;
        }
        default:{
            LOGE_SDK("Unsupported image format!, format %d", src_image->format);
            throw;
        }
    }
    return ret;
}
#endif


void MTManisHelper::CheckAndUpdateOutputParams_(manis::PixelsBuffer *input_buffer, const manis::ManisDataLst& outputs)
{
    if (input_buffer->get_width() != input_width_
        || input_buffer->get_hegiht() != input_height_)
    {
        auto shape0 = outputs[0]->GetShape();
        output_width_ = shape0->GetValue(manis::Shape_W_);
        output_height_ = shape0->GetValue(manis::Shape_W_);
    }
}

NAMESPACE_BYPASS_INDENT_CLOSE
