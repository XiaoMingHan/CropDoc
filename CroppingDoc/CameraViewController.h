//
//  CameraViewController.h
//  CroppingDoc
//
//  Created by My Star on 2/14/17.
//  Copyright © 2017 My Star. All rights reserved.
//

#import <UIKit/UIKit.h>
#include <OpenCV2/OpenCV.hpp>
#import <opencv2/videoio/cap_ios.h>
#include "ProcessingImage.hpp"
@interface CameraViewController : UIViewController<CvVideoCameraDelegate>
{
  
    BOOL isCapturing;
    cv::Mat mem_image;
    bool processing_flg;
    
    BOOL flash_turn;
    
}
@property (weak, nonatomic) IBOutlet UIButton *autoCamera;
@property (weak, nonatomic) IBOutlet UIButton *flashBtn;
@property (weak, nonatomic) IBOutlet UILabel *labelBtn;
@property (weak, nonatomic) IBOutlet UIButton *captureBtn;

@property (nonatomic, strong) CvVideoCamera* videoCamera;
@property (nonatomic) ProcessingImage * processingImage;
@property (nonatomic, strong) IBOutlet UIImageView* imageView;
@property (weak, nonatomic) IBOutlet UILabel *warningMark;

@end
