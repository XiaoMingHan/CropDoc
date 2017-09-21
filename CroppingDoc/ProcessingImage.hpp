//
//  ProcessingImage.hpp
//  CroppingDoc
//
//  Created by My Star on 2/14/17.
//  Copyright © 2017 My Star. All rights reserved.
//

#ifndef ProcessingImage_hpp
#define ProcessingImage_hpp

#include <stdio.h>
#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;
enum{
    NONE_DETECTION=0,
    MOVE_CLOSER=1,
    PERSPECTIVE=2,
    HOLD_STILL=3,
    PERFECT_HOID=4,
    
    
};
enum{
    DETECT_ROUGH=0,
    DETECT_CHECK=1,
    DETECT_CONFIRM=2,
    
};
class ProcessingImage{
public:
    ProcessingImage(){
        m_area=0;
        count_time=0;
        detect_start=false;
        stabilize_time=0;
        m_state=-1;
        detection_state=-1;
        stageOfImageProcessing=DETECT_CONFIRM;
        m_sensitive_detection=10;
        m_holdtime=15;
        
    };
    Mat cropDocument(Mat& image);
    int detectEdge(Mat& image);
    void detectEdge1(Mat& image);    
    void setSensitive(float sens){
        m_sensitive_detection=sens;
    }
    void setHoldTime(float hold){
       m_holdtime=hold;
    }
private:
    bool getDonotMoveState(Mat& image);
    bool detectionOfRect(Mat &image);
    bool recheckOfRect(Mat &image);
    int confirmBound(Mat & image);
    void drawBound(Mat &image);
    float m_sensitive_detection;
    float m_holdtime;
    cv::Point mp1;
    cv::Point mp2;
    cv::Point mp3;
    cv::Point mp4;
    
    cv::Point m_centerPoint;
    cv::Point stablize_p1;
    cv::Point stablize_p2;
    cv::Point stablize_p3;
    cv::Point stablize_p4;
    std::vector<cv::Point> quad_pts;
    std::vector<cv::Point> bound_pts;
    std::vector<cv::Point> polys_pts;
    int stageOfImageProcessing;
    int m_state;
    int detection_state;
    int stabilize_time;
    bool detect_start;
    int count_time;
    float m_area;
};
#endif /* ProcessingImage_hpp */
