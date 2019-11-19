//
//  MTBeijingSDKImageHelper.cpp
//  demo_photodetector
//
//  Created by ashtower on 2019/6/6.
//  Copyright © 2019年 meitu. All rights reserved.
//

#include "MTBeijingSdkImageHelper.hpp"

#include <vector>
#include <libyuv.h>

NAMESPACE_BYPASS_INDENT_OPEN

#ifdef USE_GPU_CODE
void reset_texture(GLuint texture_id, int width, int height)
{
    std::vector<GLubyte> emptyData(width * height * 4, 0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &emptyData[0]);
}
#endif


#ifdef NEED_SUPPORT_CROP_IMAGE
using namespace ImageCropHelper;

#ifdef USE_GPU_CODE
using namespace ImageCropHelper::GpuDetails;
#endif

void ImageCropHelper::expand_bouding_box(BoundingBox& box, float left, float right, float top, float bottom)
{
    auto clipper = [](float val) {return std::min(1.0f, std::max(0.0f, val));};
    
    float w = box.x2 - box.x1;
    float h = box.y2 - box.y1;
    box.x1 = clipper(box.x1 - left * w);
    box.y1 = clipper(box.y1 - top * h);
    box.x2 = clipper(box.x2 + right * w);
    box.y2 = clipper(box.y2 + bottom * h);
}

auto ImageCropHelper::get_cropped_image(mt_image_handle_t src_image, const BoundingBox &box)
-> std::shared_ptr<mt_image_t>
{
    int dst_w = (box.x2 - box.x1) * src_image->width;
    int dst_h = (box.y2 - box.y1) * src_image->height;
    auto ret = create_image_with_same_formmat(src_image, dst_w, dst_h);
    
        // mt_image_proc_clip_scale是最近邻（kFilterNone）插值，没有开接口出来设置插值方式
        // 先针对rgba弄一个插值的优化，使用双线性插值
    if (MT_PIX_FMT_RGBA8U == src_image->format) {
        int clip_x = box.x1 * src_image->width, clip_y =  box.y1 * src_image->height;
        int clip_width= (box.x2 - box.x1) * src_image->width;
        int clip_height= (box.y2 - box.y1) * src_image->height;
            // ARGBScaleClip好像不行，这里copy的mtimage的调用
        libyuv::ARGBScale(src_image->data + clip_y * src_image->stride + clip_x * 4
                          , src_image->stride, clip_width, clip_height
                          , ret->data, ret->stride, ret->width, ret->height
                          , libyuv::kFilterBilinear);
    } else {
        mt_image_proc_clip_scale(src_image, ret.get(), ret->stride, ret->width, ret->height
                                 , box.x1 * src_image->width, box.y1 * src_image->height
                                 , dst_w, dst_h);
    }
    
    return ret;
    
}

#ifdef USE_GPU_CODE
auto ImageCropHelper::GenerateTextureInfo_(GLuint txture_id, int width, int height)
-> ImageCropHelper::TextureInfo
{
    ImageCropHelper::TextureInfo tx_info;
    tx_info.fbo_id = 0;
    tx_info.texture_id = txture_id;
    tx_info.width = width;
    tx_info.height = height;
    
    return tx_info;
}

auto ImageCropHelper::get_cropped_image(TextureInfo input_texture, TextureInfo output_texture
                                        , const BoundingBox& box, GLuint &shader_program)
-> void
{
    int in_w = input_texture.width, in_h = input_texture.height;
    int out_w = output_texture.width, out_h = output_texture.height;
    
        // 纹理坐标和图片坐标的y轴方向是相反的：纹理正方向向上，图片的则正方向向下
    int x_left = box.x1 * input_texture.width, y_down = box.y1 * input_texture.height;
    int x_right = box.x2 * input_texture.width, y_up = box.y2 * input_texture.height;
    
    const float afVertexData[] =
    {
        0.0f,         0.0f,         0.0f,
        (float)out_w, 0.0f,         0.0f,
        0.0f,         (float)out_h, 0.0f,
        (float)out_w, (float)out_h, 0.0f // 操作输出纹理的全部
    };
    
    const float afTexCoordData[] =
    {
        1.0f/in_w * x_left,  1.0f/in_h * y_down,
        1.0f/in_w * x_right, 1.0f/in_h * y_down,
        1.0f/in_w * x_left,  1.0f/in_h * y_up,
        1.0f/in_w * x_right, 1.0f/in_h * y_up   // 操作输入纹理的一部分
    };
    
    resize_texture(input_texture, output_texture
                   , out_w, out_h
                   , afVertexData, afTexCoordData, shader_program);
}

auto ImageCropHelper::GpuDetails::resize_texture(
        TextureInfo input_texture, TextureInfo output_texture
        , int out_w, int out_h
        , const float afVertexData[], const float afTexCoordData[]
        , GLuint &shader_program)
-> void
{
    GLuint tmp_fbo = 0;
    if (0 == output_texture.fbo_id) {
        glGenFramebuffers(1, &tmp_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, tmp_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output_texture.texture_id, 0);
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, output_texture.fbo_id);
    }
    glViewport(0, 0, out_w, out_h);
    
    /***************编译着色器脚本******************/
    if (0 == shader_program) {
        std::string strVertShader = get_vertex_shader();
        std::string strTextShader = get_fragment_shader();
        shader_program = CreateProgramSource(strVertShader, strTextShader);
    }
    glUseProgram(shader_program);
    
    
    glBindTexture(GL_TEXTURE_2D, input_texture.texture_id);
    GLuint inputImageTexture = glGetUniformLocation(shader_program, "inputImageTexture");
    glUniform1i(inputImageTexture, 0);
    
    GLuint inVertex = glGetAttribLocation(shader_program, "inVertex");
    glEnableVertexAttribArray(inVertex);
    glVertexAttribPointer(inVertex, 3, GL_FLOAT, GL_FALSE, 0, afVertexData);
    
    GLuint inTexCoord = glGetAttribLocation(shader_program, "inTexCoord");
    glEnableVertexAttribArray(inTexCoord);
    glVertexAttribPointer(inTexCoord, 2, GL_FLOAT, GL_FALSE, 0, afTexCoordData);
    
    /*******************计算mvp矩阵****************/
    float m_aMVPMatTranspose[16] = {0};
    MatTransform(m_aMVPMatTranspose, 0.0f, (float)out_w, 0.0f, (float)out_h, -1.0f, 1.0f);
    
    GLuint MVP = glGetUniformLocation(shader_program, "MVP");
    glUniformMatrix4fv(MVP, 1, GL_FALSE, m_aMVPMatTranspose);
    
    /******************执行渲染*******************/
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    if (tmp_fbo != 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &tmp_fbo);
    }
}

auto ImageCropHelper::GpuDetails::get_fragment_shader()
-> std::string
{
    return R"=======(
#ifdef GL_ES//for discriminate GLES & GL
precision mediump float;
#else
#define highp      //openGL中要忽视这些精度关键字，否则会出错
#define mediump
#define lowp
#endif
    
    varying highp vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    void main()
    {
        vec2 xy0 = textureCoordinate.xy;
        vec4 cc = texture2D(inputImageTexture, xy0);
        gl_FragColor = cc;
    }
)=======";
}

auto ImageCropHelper::GpuDetails::get_vertex_shader()
-> std::string
{
    return R"=======(
#ifdef GL_ES//for discriminate GLES & GL
    precision mediump float;
#else
#   define highp   //openGL中要忽视这些精度关键字，否则会出错
#   define mediump
#   define lowp
#endif
    
    attribute highp vec3 inVertex;
    attribute highp vec2 inTexCoord;
    varying highp vec2 textureCoordinate;
    uniform highp mat4 MVP;
    
    void main()
    {
        gl_Position = MVP * vec4(inVertex, 1.0);
        textureCoordinate = inTexCoord;
    }
)=======";
}
    
auto ImageCropHelper::GpuDetails::MatTransform(float* dst
                                            , float l, float r
                                            , float b, float t
                                            , float n, float f)
-> void
{
    // init
    float src[16] = {
          1, 0, 0, 0
        , 0, 1, 0, 0
        , 0, 0, 1, 0
        , 0, 0, 0, 1};
    
    // setOrthoFrustum
    // https://www.cnblogs.com/hefee/p/3811099.html
    src[0]  =  2 / (r - l);
    src[3]  =  -(r + l) / (r - l);
    src[5]  =  2 / (t - b);
    src[7]  =  -(t + b) / (t - b);
    src[10] = -2 / (f - n);
    src[11] = -(f + n) / (f - n);
    
    // getTranspose
    dst[0] = src[0];   dst[1] = src[4];   dst[2] = src[8];   dst[3] = src[12];
    dst[4] = src[1];   dst[5] = src[5];   dst[6] = src[9];   dst[7] = src[13];
    dst[8] = src[2];   dst[9] = src[6];   dst[10]= src[10];  dst[11]= src[14];
    dst[12]= src[3];   dst[13]= src[7];   dst[14]= src[11];  dst[15]= src[15];
}
    
auto ImageCropHelper::GpuDetails::LoadShaderSource(GLenum shaderType, const std::string &pSource)
-> GLuint
{
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        const auto p_data = pSource.data();
        glShaderSource(shader, 1, &p_data, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*)malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE_SDK("Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
            }
            glDeleteShader(shader);
            shader = 0;
        }
    }
    return shader;
}

auto ImageCropHelper::GpuDetails::CreateProgramSource(const std::string &pVertexSource, const std::string &pFragmentSource)
-> GLuint
{
    GLuint vertexShader = LoadShaderSource(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        LOGE_SDK("ERROR in load vertex shader!\n");
        return 0;
    }
    
    GLuint pixelShader = LoadShaderSource(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        LOGE_SDK("ERROR in load pixel shader!\n");
        return 0;
    }
    
    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        glAttachShader(program, pixelShader);
        
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*)malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE_SDK("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    
    if (vertexShader) {
        glDeleteShader(vertexShader);
        vertexShader = 0;
    }
    
    if (pixelShader) {
        glDeleteShader(pixelShader);
        pixelShader = 0;
    }
    
    return program;
}
    
auto ImageCropHelper::recover_cropped_mask(TextureInfo input_texture, TextureInfo output_texture
                                           , const BoundingBox& box, GLuint &shader_program)
-> void
{
    int out_w = output_texture.width, out_h = output_texture.height;
    
        // 纹理坐标和图片坐标的y轴方向是相反的：纹理正方向向上，图片的则正方向向下
    int x_left = box.x1 * output_texture.width, y_down = box.y1 * output_texture.height;
    int x_right = box.x2 * output_texture.width, y_up = box.y2 * output_texture.height;
    
    /****************设置顶点和纹理坐标*****************/
    const float afVertexData[] =
    {
        (float)x_left,  (float)y_down, 0.0f,
        (float)x_right, (float)y_down, 0.0f,
        (float)x_left,  (float)y_up,   0.0f,
        (float)x_right, (float)y_up,   0.0f // 操作输出纹理的一部分
    };
    
    const float afTexCoordData[] =
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f // 操作输入纹理的全部
    };
    
    resize_texture(input_texture, output_texture
                   , out_w, out_h
                   , afVertexData, afTexCoordData, shader_program);
}

#endif // USE_GPU_CODE
    
auto ImageCropHelper::recover_cropped_mask(mt_image_handle_t src_image, mt_image_handle_t mask
                                           , const BoundingBox &box)
-> std::shared_ptr<mt_image_t>
{
    auto ret = CreateSharedMtimage(nullptr);
    
    ret = CreateSharedMtimage(mt_image_utils_create_from_gray(src_image->width, src_image->height, nullptr));
    std::fill_n(ret->data, ret->width * ret->height, 0);
    
    int offset = box.x1 + box.y1 * src_image->width;
    libyuv::ScalePlane(mask->data, mask->stride, mask->width, mask->height
                       , ret->data + offset, ret->stride, ret->width, ret->height
                       , libyuv::kFilterBilinear);
    return ret;
}

#endif  // NEED_SUPPORT_CROP_IMAGE

NAMESPACE_BYPASS_INDENT_CLOSE
