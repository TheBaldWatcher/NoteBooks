//
//  MTBeijingSDKImageHelper.hpp
//  demo_photodetector
//
//  Created by ashtower on 2019/6/6.
//  Copyright © 2019年 meitu. All rights reserved.
//

#pragma once

#include <stdio.h>

#include <mt_image_utils.h>
#include <mt_image_proc.h>
#include <memory>

#include "MTBeijingSdkDefine.h"
#include "MTManisHelper.hpp"    // 省略一些gpu的include，复用MTManisHelper.hpp的

NAMESPACE_BYPASS_INDENT_OPEN

#ifdef USE_GPU_CODE
void reset_texture(GLuint texture_id, int width, int height);
#endif


/******************************************************************
 *                          图像处理
 ******************************************************************/
/**
 根据src_image类型，构造一个相同格式的图像
 
 @param src_image 原图
 @param dst_w 新图宽
 @param dst_h 新图高
 @return 新图
 */
auto create_image_with_same_formmat(mt_image_handle_t src_image, int dst_w, int dst_h)
-> std::shared_ptr<mt_image_t>;


#ifdef NEED_SUPPORT_CROP_IMAGE
/******************************************************************
 *                  裁图逻辑，目前只有分割有这个裁图需求
 ******************************************************************/

namespace ImageCropHelper {
    
    struct BoundingBox
    {
        float x1 = .0f;
        float y1 = .0f;
        float x2 = .0f;
        float y2 = .0f;
        
        bool is_used = true;   // 是否已经用这个框计算过一次了。一般来说，两个帧的框不应该相同
    };
    
#ifdef USE_GPU_CODE
    struct TextureInfo
    {
        GLuint fbo_id = 0;
        GLuint texture_id = 0;
        int width = 0;
        int height = 0;
    };
#endif
    
    /**
     将框按比例扩大
     
     @param box 框数据
     @param left 向左扩大的比例
     @param right 向右扩大的比例
     @param top 向上扩大的比例
     @param bottom 向下扩大的比例
     */
    void expand_bouding_box(BoundingBox& box, float left, float right, float top, float bottom);
    
        ///< 根据框，裁剪图像
    auto get_cropped_image(mt_image_handle_t src_image, const BoundingBox &box)
    -> std::shared_ptr<mt_image_t>;
    
    auto recover_cropped_mask(mt_image_handle_t src_image, mt_image_handle_t mask
                              , const BoundingBox &box)
    -> std::shared_ptr<mt_image_t>;
    
#ifdef USE_GPU_CODE
    auto get_cropped_image(TextureInfo input_texture, TextureInfo output_texture
                           , const BoundingBox& box, GLuint &shader_program)
    -> void;
    
    auto recover_cropped_mask(TextureInfo input_texture, TextureInfo output_texture
                              , const BoundingBox& box, GLuint &shader_program)
    -> void;
    
    auto GenerateTextureInfo_(GLuint txture_id, int width, int height)
    -> ImageCropHelper::TextureInfo;
    
    namespace GpuDetails {
        auto get_fragment_shader()
        -> std::string;
        auto get_vertex_shader()
        -> std::string;
        
        auto resize_texture(TextureInfo input_texture, TextureInfo output_texture
                            , int out_w, int out_h
                            , const float afVertexData[], const float afTexCoordData[], GLuint &shader_program)
        -> void;
        
        auto LoadShaderSource(GLenum shaderType, const std::string &pSource)
        -> GLuint;
        auto CreateProgramSource(const std::string &pVertexSource, const std::string &pFragmentSource)
        -> GLuint;
        auto MatTransform(float* dst, float l, float r, float b, float t, float n, float f)
        -> void;
    }   // namespace Details
#endif  //USE_GPU_CODE
    
}   // namespace ImageCropHelper
#endif

NAMESPACE_BYPASS_INDENT_CLOSE
