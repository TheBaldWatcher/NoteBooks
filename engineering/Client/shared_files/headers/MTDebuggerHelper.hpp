//
// Created by ysy on 2019/4/19.
//

#ifndef MT_DEBUGGER_HELPER_HPP
#define MT_DEBUGGER_HELPER_HPP
#include <iostream>
#include <vector>
#include <fstream>

//OpenCV
#ifdef UES_OPENCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#endif

class MTDebuggerHelper
{
public:
    
#ifdef UES_OPENCV
    static void SaveImgWithOpencv(std::string save_name, unsigned char * img_rgba, int img_witdh , int img_height, bool img_show)
    {
        cv::Mat frame = cv::Mat(img_height, img_witdh, CV_8UC4, img_rgba);
        cv::cvtColor(frame, frame, CV_RGBA2BGR);
        cv::imwrite(save_name, frame);
        if(img_show){
            cv::imshow("", frame);
            cv::waitKey(500);
        }
    }
#endif
};


#endif //MT_DEBUGGER_HELPER_HPP
