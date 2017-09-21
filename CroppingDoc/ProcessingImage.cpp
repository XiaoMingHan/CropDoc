//
//  ProcessingImage.cpp
//  CroppingDoc
//
//  Created by My Star on 2/14/17.
//  Copyright © 2017 My Star. All rights reserved.
//

#include "ProcessingImage.hpp"
#define PI 3.141592
int cmpY_bottom(const void * a, const void * b)
{
    // cout<<(*(Mark_bullet *)a).mark<<"  "<<(*(Mark_bullet *)b).mark <<endl;
    int mrk1=(*(cv::Point*)a).y;
    int mrk2=(*(cv::Point*)b).y;
    return ( mrk2-mrk1);
}
int cmpY_top(const void * a, const void * b)
{
    // cout<<(*(Mark_bullet *)a).mark<<"  "<<(*(Mark_bullet *)b).mark <<endl;
    int mrk1=(*(cv::Point*)a).y;
    int mrk2=(*(cv::Point*)b).y;
    return ( mrk1-mrk2);
}
int cmpX_left(const void * a, const void * b)
{
    // cout<<(*(Mark_bullet *)a).mark<<"  "<<(*(Mark_bullet *)b).mark <<endl;
    int mrk1=(*(cv::Point*)a).x;
    int mrk2=(*(cv::Point*)b).x;
    return ( mrk1-mrk2);
}
int cmpX_right(const void * a, const void * b)
{
    // cout<<(*(Mark_bullet *)a).mark<<"  "<<(*(Mark_bullet *)b).mark <<endl;
    int mrk1=(*(cv::Point*)a).x;
    int mrk2=(*(cv::Point*)b).x;
    return ( mrk2-mrk1);
}
float distance(cv::Point p1,cv::Point p2){
    int dx=p1.x-p2.x;
    int dy=p1.y-p2.y;
    
    float res=sqrt(dx*dx+dy*dy);
    if(res<1)res=1;
    return  res;
}
float getAngle(cv::Point p1,cv::Point p2){
    float dx=fabs(p1.x-p2.x);
    float dy=fabs(p1.y-p2.y);
    if(dy==0)return 100;
    return 90*atan(dx/dy)/3.14;
}
bool isBound(cv::Rect rect,float perc,cv::Point pt){
    int ww=rect.width*perc;
    int hh=rect.height*perc;
    int hht=0;//rect.height*0.08;
    int hhb=0;//hht*0.15/0.08;
    cv::Rect rect_=cv::Rect(cv::Point(rect.x+ww,rect.y+hh+hht),cv::Point(rect.x+rect.width-ww,rect.y+rect.height-hh-hhb));
    return rect_.contains(pt);
    
}
double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

float anglefromPts(cv::Point p2,cv::Point p1,cv::Point p3){
    float p12=    distance(p1,p2);
    float p23=    distance(p3,p2);
    float p31=    distance(p3,p1);
    if(fabs(p12)<1 || fabs(p31<1))return 0;
    float res=acos((p12*p12+p31*p31-p23*p23)/(2*p12*p31));
    return res;
}
Mat ProcessingImage::cropDocument(Mat& image){
    if(m_state!=NONE_DETECTION){
     cvtColor(image,image,CV_RGBA2RGB);
    std::vector<Point2f> quad_ptss;
    std::vector<Point2f> squre_pts;
    int cut_edge=8;
    quad_ptss.push_back(Point2f(mp1.x+cut_edge, mp1.y+cut_edge));
    quad_ptss.push_back(Point2f(mp2.x+cut_edge, mp2.y-cut_edge));
    quad_ptss.push_back(Point2f(mp4.x-cut_edge, mp4.y+cut_edge));
    quad_ptss.push_back(Point2f(mp3.x-cut_edge, mp3.y-cut_edge));
    Rect boundRect = boundingRect(quad_ptss);
    squre_pts.push_back(Point2f(boundRect.x, boundRect.y));
    squre_pts.push_back(Point2f(boundRect.x, boundRect.y + boundRect.height));
    squre_pts.push_back(Point2f(boundRect.x + boundRect.width, boundRect.y));
    squre_pts.push_back(Point2f(boundRect.x + boundRect.width, boundRect.y + boundRect.height));
    
    Mat transmtx = getPerspectiveTransform(quad_ptss, squre_pts);
    Mat transformed = Mat::zeros(image.rows, image.cols, CV_8UC3);
    warpPerspective(image, transformed, transmtx, image.size());

    cv::Mat res=transformed(boundRect);
    
    
    int ww=(distance(mp1,mp4)+distance(mp2, mp3))/2;
    
    float top=distance(mp1,mp4);
    float bottom=distance(mp2, mp3);
    int hh=(distance(mp1,mp2)+distance(mp4, mp3))/2+fabs(top-bottom)*0.5;
    float rt2=(float)hh/ww;
    int www=image.cols*0.97;
    int hhh=www*rt2;
    if (hhh>image.rows*0.97){
         hhh=image.rows*0.97;
         www=hhh/rt2;
    }
    Size size=Size(www,hhh);
    resize(res, res, size);
    cv::Rect bound=Rect((image.cols-www)/2,(image.rows-hhh)/2,size.width,size.height);
    Mat sd = Mat::zeros(image.rows, image.cols, CV_8UC3);
    sd=Scalar(255,255,255);
    cv::Mat dstROI=sd(bound);
    res.copyTo(dstROI);
    cvtColor(sd,sd,CV_RGB2RGBA);
           detect_start=false;
        return sd;
    }else{
        cv::Mat res;
        
        cvtColor(image,res,CV_RGB2RGBA);
        return res.clone();
    }
 
    return cv::Mat();
}

bool ProcessingImage::detectionOfRect(Mat &image){
    
    bool detection_flg=false;
    Mat src;//=image.clone();
    int ww=image.cols;
    int hh=image.rows;
    float ff=(float)hh/ww;
    float size=300;
 
    resize(image,src,cv::Size(size,size*ff));
    
    if (detect_start==false){
        stablize_p1=cv::Point(src.cols/2,src.rows/2);
        stablize_p2=cv::Point(src.cols/2,src.rows/2);
        stablize_p3=cv::Point(src.cols/2,src.rows/2);
        stablize_p1=cv::Point(src.cols/2,src.rows/2);
        mp1=stablize_p1;
        mp2=stablize_p1;
        mp3=stablize_p1;
        mp4=stablize_p1;

        quad_pts.clear();
        quad_pts.push_back(stablize_p1);
        quad_pts.push_back(stablize_p2);
        quad_pts.push_back(stablize_p3);
        quad_pts.push_back(stablize_p4);
        detect_start=true;
        
    }
    
    Mat thr;
    cvtColor(src, thr, CV_BGR2GRAY);
    
    Mat filter;
    blur(thr,thr,cv::Size(3,3));
    bilateralFilter(thr,filter, 11, 7, 7);
    
    Canny(filter,thr,0,125);
    blur(thr,thr,cv::Size(9,9));
    
    vector< vector <Point> > contours; // Vector for storing contour
    vector< Vec4i > hierarchy;
    int largest_contour_index = -1;
    int largest_area = 0;
    vector<Point> poly4;
    vector<Point> poly5;
    
    Mat dst(src.rows, src.cols, CV_8UC1, Scalar::all(0)); //create destination image
    
    findContours(thr.clone(), contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
    
    for (int i = 0; i< contours.size(); i++){
        
        float epsilon = 0.03*arcLength(Mat(contours[i]),true);
        
        vector<Point> ppl;
        
        approxPolyDP(Mat(contours[i]), ppl, epsilon, true);
          float area=contourArea(contours[i]);
        if (ppl.size() >= 4 && ppl.size() <= 8 && area>largest_area)
        {
            // Number of vertices of polygonal curve
            largest_area=area;
    
            largest_contour_index=i;
            
            poly4=ppl;
       }
    }
    
    if (largest_contour_index==-1)return detection_flg;
   

    if (poly4.size() >= 4 && poly4.size() <= 8){
        
        std::vector<Point> squre_pts;
        
        
   float area=contourArea(poly4);
    
    float area0=src.cols*src.rows/50;
        
            
            if(area>area0){

                bound_pts=poly4;
                detection_flg=true;

            }
 
    }
    return detection_flg;
}

void drawMask(Mat &mask,vector<cv::Point> pts ){
    
    std::vector<std::vector<cv::Point> > fillContAll;
    
    fillContAll.push_back(pts);
    
    cv::fillPoly( mask, fillContAll,  cv::Scalar( 255));

}
bool ProcessingImage::recheckOfRect(Mat &image){
    
  bool detection_flg=false;
    
    vector<cv::Point> bound_2(bound_pts.begin(),bound_pts.end());
    
    if (bound_pts.size() > 4 && bound_pts.size() <= 8){
      
        vector<cv::Point> bound_2(bound_pts.begin(),bound_pts.end());
        
        float size=300;
        
        int ww=image.cols;
        
        int hh=image.rows;
        
        float ff=(float)hh/ww;
        
         cv::Mat src(size*ff,300, CV_8UC1, cv::Scalar(0));
        
         cv::Mat src1(size*ff,300, CV_8UC1, cv::Scalar(0));
        
         cv::Mat src2(size*ff,300, CV_8UC1, cv::Scalar(0));
        
        drawMask(src, bound_pts);
        
        for ( int i=0;i<bound_pts.size();i++){
            
            int k1=i%bound_pts.size();
            
            int k2=(i+1)%bound_pts.size();
            
            int k3=(i+2)%bound_pts.size();
            
            int k4=(i+3)%bound_pts.size();
            
            float ang1=anglefromPts(bound_pts[k1], bound_pts[k2], bound_pts[k3]);
            
            float ang2=anglefromPts(bound_pts[k2], bound_pts[k3], bound_pts[k4]);
            
            if(ang1>1.2*PI/2 && ang2>1.2*PI/2){
                
                float x1=bound_pts[k1].x;float y1=bound_pts[k1].y;
                float x2=bound_pts[k2].x;float y2=bound_pts[k2].y;
                float x3=bound_pts[k3].x;float y3=bound_pts[k3].y;
                float x4=bound_pts[k4].x;float y4=bound_pts[k4].y;
                
                float xx3=((x1*y2-y1*x2)*(x3-x4)-(x1-x2)*(x3*y4-y3*x4))/((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
                float yy3=((x1*y2-y1*x2)*(y3-y4)-(y1-y2)*(x3*y4-y3*x4))/((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
                
                bound_pts[k2]=cv::Point(xx3,yy3);
                bound_pts.erase (bound_pts.begin()+k3);
                
            }
            if(bound_pts.size()==4)break;
            
        }
        
        drawMask(src1, bound_pts);
        float area1=sum(abs(src-src1)).val[0];
      
        for ( int i=0;i<bound_2.size();i++){
            
            int k1=(i+1)%bound_2.size();
            int k2=(i+2)%bound_2.size();
            int k3=(i+3)%bound_2.size();
            int k4=(i+4)%bound_2.size();
            
            float ang1=anglefromPts(bound_2[k1], bound_2[k2], bound_2[k3]);
            float ang2=anglefromPts(bound_2[k2], bound_2[k3], bound_2[k4]);
            
            if(ang1>=1.1*PI/2 && ang2>=1.1*PI/2){
                
                float x1=bound_2[k1].x;float y1=bound_2[k1].y;
                float x2=bound_2[k2].x;float y2=bound_2[k2].y;
                float x3=bound_2[k3].x;float y3=bound_2[k3].y;
                float x4=bound_2[k4].x;float y4=bound_2[k4].y;
                
                float xx3=((x1*y2-y1*x2)*(x3-x4)-(x1-x2)*(x3*y4-y3*x4))/((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
                float yy3=((x1*y2-y1*x2)*(y3-y4)-(y1-y2)*(x3*y4-y3*x4))/((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
                
                bound_2[k2]=cv::Point(xx3,yy3);
                bound_2.erase (bound_2.begin()+k3);
            }
            if(bound_2.size()==4)break;
            
        }
      
        drawMask(src2, bound_2);
        
        float area2=sum(abs(src-src2)).val[0];
          if(area1>area2){
            bound_pts=bound_2;
        }
        
    }
    
    if(bound_pts.size()<4 )return detection_flg;
    
    float ang1=anglefromPts(bound_pts[0], bound_pts[1], bound_pts[2]);
    float ang2=anglefromPts(bound_pts[1], bound_pts[2], bound_pts[3]);
    float ang3=anglefromPts(bound_pts[2], bound_pts[3], bound_pts[0]);
    float ang4=anglefromPts(bound_pts[3], bound_pts[0], bound_pts[1]);
    
    float sum=(ang1+ang2+ang3+ang4);
    float err=fabs(sum-PI*2);
    if(err>0.1)return detection_flg;
    float ddf=image.cols/300.f;
    
    quad_pts[0]=bound_pts[0]*ddf;
    quad_pts[1]=bound_pts[1]*ddf;
    quad_pts[2]=bound_pts[2]*ddf;
    quad_pts[3]=bound_pts[3]*ddf;
    
 //    float pensize=image.rows/300.0;
//    line(image, quad_pts[0], quad_pts[1], Scalar(0, 255, 0), pensize, CV_AA);
//    line(image, quad_pts[1], quad_pts[2], Scalar(0, 255, 0), pensize, CV_AA);
//    line(image, quad_pts[2], quad_pts[3], Scalar(0, 255, 0), pensize, CV_AA);
//    line(image, quad_pts[3], quad_pts[0], Scalar(0, 255, 0), pensize, CV_AA);

    return true;
}
int  ProcessingImage::confirmBound(Mat & image){
    
    if(quad_pts.size()!=4)return NONE_DETECTION;
    Point PP1=quad_pts[0];
    Point PP2=quad_pts[1] ;
    Point PP3=quad_pts[2] ;
    Point PP4=quad_pts[3];
    
    Point VP1;
    Point VP2;
    Point VP3;
    Point VP4;
    vector<cv::Point> svr;
    svr.push_back(PP1);
    svr.push_back(PP2);
    svr.push_back(PP3);
    svr.push_back(PP4);
    cv::Point center_rect=(PP1+PP2+PP3+PP4)/4;
    int count=0;
    for ( int i=0;i<svr.size();i++){
        if( svr[i].x<center_rect.x && svr[i].y<center_rect.y){
            VP1=svr[i];count++;
        }else if(svr[i].x<center_rect.x && svr[i].y>center_rect.y){
            VP2=svr[i];count++;
        }else if(svr[i].x>center_rect.x && svr[i].y>center_rect.y){
            VP3=svr[i];count++;
        }else if(svr[i].x>center_rect.x && svr[i].y<center_rect.y){
            VP4=svr[i];count++;
        }
    }
    cv::Rect rect=cv::Rect(0,0,image.cols,image.rows);
    float perc=0.02;
    int sensitive_detection=10;
    int result=-1;
    if(count ==4 && isBound(rect, perc, VP1) && isBound(rect, perc, VP2) && isBound(rect, perc, VP3) && isBound(rect, perc, VP4) ){
        cv::Point kp1=(VP1+VP4)/2;
        cv::Point kp2=(VP2+VP3)/2;
        float alpa=getAngle(kp1,kp2);
        if(alpa<5){
            
            
            if(count_time>sensitive_detection){
                stablize_p1=VP1;
                stablize_p2=VP2;
                stablize_p3=VP3;
                stablize_p4=VP4;
                count_time=0;
                
                mp1=VP1;
                mp2=VP2;
                mp3=VP3;
                mp4=VP4;
                
            }else{
                
                
                float fx=0.4;
                
                float radial_pos=5.0*image.rows/300;
                if (m_state==HOLD_STILL)radial_pos=1.0*image.rows/300;
                float dis1=distance(VP1,stablize_p1);
                float dis2=distance(VP2,stablize_p2);
                float dis3=distance(VP3,stablize_p3);
                float dis4=distance(VP4,stablize_p4);
                stablize_p1=dis1>radial_pos?stablize_p1*(1-fx)+VP1*fx:stablize_p1;
                stablize_p2=dis2>radial_pos?stablize_p2*(1-fx)+VP2*fx:stablize_p2;
                stablize_p3=dis3>radial_pos?stablize_p3*(1-fx)+VP3*fx:stablize_p3;
                stablize_p4=dis4>radial_pos?stablize_p4*(1-fx)+VP4*fx:stablize_p4;
                
                mp1=stablize_p1;
                mp2=stablize_p2;
                mp3=stablize_p3;
                mp4=stablize_p4;
                
                
            }
            
            float left_length=  distance(mp1,mp2);
            float right_length=  distance(mp3,mp4);
            float percent1=fabs(left_length-right_length)/max(left_length,right_length);
            
            float top_length=  distance(mp1,mp4);
            float bottom_length=  distance(mp3,mp2);
            float percent2=fabs(top_length-bottom_length)/max(top_length,bottom_length);
            
            float area=contourArea(svr);
         
            float kp_area=image.cols*image.rows*0.5 ;
            if(area<kp_area || distance(center_rect, cv::Point(image.cols/2,image.rows/2))>image.cols/6){
                result=MOVE_CLOSER;
                stabilize_time=0;
            }else if(percent1>0.15 || percent2>0.15){
                result=PERSPECTIVE;
                stabilize_time=0;
            }else{
                
                result=HOLD_STILL;
            }
                         count_time=0;
            
        }else{
           
            
                        if(count_time>sensitive_detection){
                            result=NONE_DETECTION;
                        }
                        count_time++;
        }
    }else{
        
                if(count_time>sensitive_detection){
                    result=NONE_DETECTION;
               }
        count_time++;
    }
    if (result!=-1){
        m_state=result;
    }

    return result;
}

void ProcessingImage::drawBound(Mat &image){
                float pensize=image.rows/300.0;
                line(image, mp1, mp2, Scalar(0, 255, 0), pensize, CV_AA);
                line(image, mp2, mp3, Scalar(0, 255, 0), pensize, CV_AA);
                line(image, mp3, mp4, Scalar(0, 255, 0), pensize, CV_AA);
                line(image, mp4, mp1, Scalar(0, 255, 0), pensize, CV_AA);

}
bool ProcessingImage::getDonotMoveState(Mat& image){
    bool res=false;
    cv::Point pcen=(quad_pts[0]+quad_pts[1]+quad_pts[2]+quad_pts[3])/4;
     float area=contourArea(quad_pts);
    static float m_area=area;
    float dis=distance(pcen, m_centerPoint);
    float errr=fabs(area-m_area)/max(area,m_area);
    float holdRadius=4*image.cols/100.f;
    if(dis<holdRadius && errr<0.041)
    {
        res=true;
    }else{
        res=false;
         m_area=0;
    }
      m_centerPoint=pcen;
    m_area=area;
    return res;
}
int ProcessingImage::detectEdge(Mat& image){
    
    bool isCapture=false;
       cvtColor(image,image,CV_RGBA2BGR);
    if(stageOfImageProcessing==DETECT_CONFIRM){
        isCapture=detectionOfRect(image);
        if(isCapture)stageOfImageProcessing=DETECT_ROUGH;
        else stageOfImageProcessing=DETECT_CONFIRM;
    }
    else if(stageOfImageProcessing==DETECT_ROUGH){if(recheckOfRect(image)) stageOfImageProcessing=DETECT_CHECK;else stageOfImageProcessing=DETECT_CONFIRM;}
    else if(stageOfImageProcessing==DETECT_CHECK){stageOfImageProcessing=DETECT_CONFIRM;}
     detection_state= confirmBound(image);
    if(detection_state!=NONE_DETECTION){
       drawBound(image);
    }
    if(detection_state==HOLD_STILL && isCapture==true ){
        if(getDonotMoveState(image)==true) stabilize_time++;else stabilize_time=0;
      
        //if(stabilize_time==1)m_centerPoint=pcen;
         if(stabilize_time>m_holdtime ){
           detection_state=PERFECT_HOID;
            stabilize_time=0;
         }
        
        
    }
    return detection_state;
}

