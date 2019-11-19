#include <iostream>
#include <time.h>
#include <string.h>
//get directory path
#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
#include <mtbodypose.h>
//load stb_image
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
using namespace cv;
//mtcvlite
#include "mtcvlite/base/Mat/mat.h"
#include "mtcvlite/base/ImageUtil/cvtColor.h"
#undef SAFE_DELETE
#define SAFE_DELETE(x) if((x)!=NULL){delete (x);(x)=NULL;}

//Function Statement
struct BPS {
    float x;
    float y;
    float score;
};
clock_t startTime, endTime;
std::string GetCurrentPath();
void DrawPose(cv::Mat& frame, mtbodypose::MTPointsPose *body_pose_date);
void DrawBox(cv::Mat &im,  mtbodypose::MTBodyBoxs *body_boxs);
////渲染颜色设置，格式GBR
cv::Scalar DarkRed=cv::Scalar(101,67,254);
cv::Scalar LightYellow=cv::Scalar(173,205,249);
cv::Scalar LightGreen=cv::Scalar(155,175,131);
int img_width, img_height;


int person_num = 0;

//#define STATIC_TYPE  //拍后与实时切换

#ifdef STATIC_TYPE
int main()
{
    //获取当前工程路径
    std::string CurrentPathPrefix=GetCurrentPath() + "/../../../../";
    std::string PicPath = CurrentPathPrefix + "demo/res/picture/1.jpg";
    std::string BodyPosePath = CurrentPathPrefix + "model/RealtimeModel/realtime2.2.1.0_pose_5ae2.manis";
    std::string DetectionAPath = CurrentPathPrefix + "model/RealtimeModel/realtime2.0.0.0_96_detectionA_6eb2.manis";
    std::string DetectionBPath = CurrentPathPrefix + "model/RealtimeModel/realtime2.0.0.0_96_detectionB_66d5.manis";
    auto body_pose = std::make_shared<mtbodypose::MTBodyPose>(BodyPosePath.c_str(),DetectionAPath.c_str(),DetectionBPath.c_str());
    auto body_pose_date = std::make_shared<mtbodypose::MTPointsPose>();
    auto body_boxs = std::make_shared<mtbodypose::MTBodyBoxs>();
    bool is_multi_person=false;
    auto mode = mtbodypose::EstimateStat::kStaticImage;
    body_pose->SetEstimateStat(mode);
    struct BPS body_pose_struct[14];

    //load image
    int nWidth, nHeight, nChannels;
    unsigned char* pInPicData = stbi_load(PicPath.c_str(), &nWidth, &nHeight, &nChannels, 4);
     //run
    person_num = body_pose->Run(0, pInPicData, 4*nWidth, nWidth, nHeight, mtbodypose::PixelOrder::RGBA, body_pose_date.get(), 1, is_multi_person);
    body_pose->GetBodyBoxs(body_boxs.get());
    if (0 != person_num) {
        for (int i = 0; i < 14; i++) {
            body_pose_struct[i].x = body_pose_date->point_array[i].x*nWidth;
            body_pose_struct[i].y = body_pose_date->point_array[i].y*nHeight;
            body_pose_struct[i].score = body_pose_date->score_array[i];
            std::cout << "bodyposedate["<<i<<"].x=" << body_pose_struct[i].x << "\tbodyposedate[" << i << "].y=" << body_pose_struct[i].y <<"  score: "<<body_pose_date->score_array[i]<<std::endl;
            
        }
        for(int j=0; j<body_boxs->box_num; j++){
            std::cout<<"body_boxs: x1: "<<body_boxs->box_array[j].x1<<"\t y1: "<<body_boxs->box_array[j].y1<<"\t x2: "
            <<body_boxs->box_array[j].x2<<"\t y2: "<<body_boxs->box_array[j].y2<<"\t score:"<<body_boxs->box_array[j].score<<std::endl;
        }
        // DrawLine(frame, body_pose_struct);
    }
    stbi_image_free(pInPicData);
    return 0;
}
#else
int main()
{
    //获取当前工程路径
    std::string CurrentPathPrefix=GetCurrentPath() + "/../../../../";
    std::string BodyPosePath = CurrentPathPrefix + "model/RealtimeModel/realtime2.2.1.0_pose_5ae2.manis";
    std::string DetectionAPath = CurrentPathPrefix + "model/RealtimeModel/realtime2.0.0.0_96_detectionA_6eb2.manis";
    std::string DetectionBPath = CurrentPathPrefix + "model/RealtimeModel/realtime2.0.0.0_96_detectionB_66d5.manis";
//    std::string DetectionAPath = CurrentPathPrefix + "model/realtime2.0.0.0_128_detectionA_357c.manis";
//    std::string DetectionBPath = CurrentPathPrefix + "model/realtime2.0.0.0_128_detectionB_a719.manis";
    auto body_pose = std::make_shared<mtbodypose::MTBodyPose>(BodyPosePath.c_str(),DetectionAPath.c_str(),DetectionBPath.c_str());
    auto body_pose_date = std::make_shared<mtbodypose::MTPointsPose>();
    auto body_boxs = std::make_shared<mtbodypose::MTBodyBoxs>();
    bool is_multi_person=false;
    auto mode = mtbodypose::EstimateStat::kStaticImage;
    body_pose->SetEstimateStat(mode);
    struct BPS body_pose_struct[14];
    /**********************************************************************
     *OpenCV设置，可选择摄像头和视频作为数据源，
     *路径采用相对路径，请设置当前工作路径为xcodeproj根目录
     *设置方法是：product-Edit Scheme-Run-Options-勾选Use custom working directory-设置本地路径
     *加载分割模型，路径自行设置
     ***********************************************************************/
    namedWindow("BodyPose",0);
    
    VideoCapture capture;
    VideoWriter writer;
    //********加载摄像头数据********//
    //    capture.open(0);
    //********加载视频文件数据********//
    
    capture.open(CurrentPathPrefix + "demo/res/video/1.mp4");
    Mat frame;
    capture>>frame;
    int iBitmapwidth = frame.cols;
    int iBitmapHeight = frame.rows;
    resizeWindow("BodyPose", iBitmapwidth, iBitmapHeight);
    bool bRotate = false;
    bool bStopFlag =false;
    /**********************************************************************
     *渲染循环
     ***********************************************************************/
    while(!bStopFlag)
    {
        if (!capture.read(frame))
            break;
        else{
            cv::resize(frame, frame, cv::Size(), 0.3, 0.3); // use to show result
            if (bRotate)
                cv::flip(frame.t(),frame,1);
            if(!writer.isOpened()) {
                writer.open("output_video.avi", CV_FOURCC('M', 'J', 'P', 'G'), 25, frame.size(), true);
            }
        }
        mtcvlite::Mat image_cvlite = mtcvlite::Mat(frame.rows, frame.cols, MT_8UC3, frame.data);
        mtcvlite::Mat image_rgba;
        mtcvlite::cvtColor(image_cvlite, image_rgba, mtcvlite::COLOR_BGR2RGBA);
        /**********************************************************************
         *Run-轮廓点计算
         ***********************************************************************/
        auto nWidth = image_rgba.cols;
        auto nHeight = image_rgba.rows;
        img_width = nWidth;
        img_height =nHeight;
        if(image_rgba.data!=0)
        {
            startTime = clock();
            person_num = body_pose->Run(0, image_rgba.data, 4*nWidth, nWidth, nHeight, mtbodypose::PixelOrder::RGBA, body_pose_date.get(), 1, is_multi_person);
            body_pose->GetBodyBoxs(body_boxs.get());
            endTime = clock();
            std::cout << "run cost time: " << (endTime - startTime) / 1000 << " ms" << std::endl;
        }
        if (person_num) {
            for (int i = 0; i < 14; i++) {
                body_pose_struct[i].x = body_pose_date->point_array[i].x*nWidth;
                body_pose_struct[i].y = body_pose_date->point_array[i].y*nHeight;
                body_pose_struct[i].score = body_pose_date->score_array[i];
                std::cout << "bodyposedate["<<i<<"].x=" << body_pose_struct[i].x << "\tbodyposedate[" << i << "].y=" << body_pose_struct[i].y <<"  score: "<<body_pose_date->score_array[i]<<std::endl;

            }
            for(int j=0; j<body_boxs->box_num; j++){
                std::cout<<"body_boxs: x1: "<<body_boxs->box_array[j].x1<<"\t y1: "<<body_boxs->box_array[j].y1<<"\t x2: "
                <<body_boxs->box_array[j].x2<<"\t y2: "<<body_boxs->box_array[j].y2<<"\t score:"<<body_boxs->box_array[j].score<<std::endl;
            }
            DrawBox(frame,body_boxs.get());
            DrawPose(frame, body_pose_date.get());
        }
        imshow("BodyPose", frame);
        if (waitKey(30) >= 0) break;
        writer << frame;
    }
    capture.release();
    writer.release();
    return 0;
}
#endif

void DrawBox(cv::Mat &im,  mtbodypose::MTBodyBoxs *body_boxs)
{
    int i =0;
    if (body_boxs->box_array[i].score==0){
        std::cout << "NO Box Data !!!"<<std::endl;
    }
    float x1 = body_boxs->box_array[i].x1;
    float y1 = body_boxs->box_array[i].y1;
    float x2 = body_boxs->box_array[i].x2;
    float y2 = body_boxs->box_array[i].y2;
    //    std::cout<<"x1:"<<x1<<"y1:"<<y1<<"x2:"<<x2<<"y2:"<<y2<<"score:"<<contour_box->score<<std::endl;
    if (body_boxs->box_array[i].score>0.2){
        rectangle(im, cv::Point2f(x1, y1), cv::Point2f(x2, y2), cv::Scalar(255, 255, 255), 2, 8);
    }
}

void DrawPose(cv::Mat& frame, mtbodypose::MTPointsPose *body_pose_date){
    {
        const std::vector<int> pairs{ 0,1,1,2,1,3,2,4,4,6,3,5,5,7,2,8,3,9,8,10,9,11,10,12,11,13 };
//        const std::vector<int> pairs{ 0,1,1,2,1,3,2,8,3,9,8,10,9,11,10,12,11,13 };
        int thicknessLine = 4;
        int lineType = 8;
        int shift = 0;
        for (int pair = 0; pair < pairs.size(); pair += 2)
        {
            int index1 = pairs[pair];
            int index2 = pairs[pair+1];
            if(body_pose_date->score_array[index1] > 0.2f && body_pose_date->score_array[index2]> 0.2f){
                const cv::Point keypoint1(body_pose_date->point_array[index1].x * img_width, body_pose_date->point_array[index1].y * img_height);
                const cv::Point keypoint2(body_pose_date->point_array[index2].x * img_width, body_pose_date->point_array[index2].y * img_height);
                cv::line(frame, keypoint1, keypoint2, Scalar(255,255,255), thicknessLine, lineType, shift);
            }
        }
    }
}

std::string GetCurrentPath(){
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
    {
        throw "can not get current Paht!";
    }
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
    std::string CurrentPath(cCurrentPath);
    return CurrentPath;
}
