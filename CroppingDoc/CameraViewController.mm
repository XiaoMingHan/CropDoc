//
//  CameraViewController.m
//  CroppingDoc
//
//  Created by My Star on 2/14/17.
//  Copyright © 2017 My Star. All rights reserved.
//

#import "CameraViewController.h"
#import "AppDelegate.h"
@interface CameraViewController ()

@end

@implementation CameraViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
   
    _videoCamera = [[CvVideoCamera alloc]
                   initWithParentView:_imageView];
    _videoCamera.delegate = self;
    _videoCamera.defaultAVCaptureDevicePosition =  AVCaptureDevicePositionBack;
    _videoCamera.defaultAVCaptureSessionPreset =    AVCaptureSessionPresetPhoto;
    _videoCamera.defaultAVCaptureVideoOrientation =    AVCaptureVideoOrientationPortrait;
    _videoCamera.defaultFPS = 20;
       self.processingImage = new ProcessingImage();
    
    flash_turn=NO;
    _autoCamera.selected=YES;
    
    [_warningMark setHidden:YES];
    _processingImage->setSensitive(12);

    _processingImage->setHoldTime(10);
   }
- (BOOL)prefersStatusBarHidden {
    
    return YES;
}
-(void)gotoCameraPreview{
    UIViewController *vc = [self.storyboard instantiateViewControllerWithIdentifier:@"previewCrop"];
    
    AppDelegate* appDelegate;
    appDelegate = (AppDelegate*)[UIApplication sharedApplication].delegate;
    
    appDelegate.mem_image=_imageView.image;
   // [self dismissViewControllerAnimated:YES completion:NULL];
    [self presentViewController:vc animated:YES completion:nil];

}
- (IBAction)TakePhoto:(id)sender {
    
    if(_autoCamera.selected==NO){
    if(isCapturing==NO){
        
         isCapturing = YES;
        //  _videoCamera.defaultAVCaptureSessionPreset =    AVCaptureSessionPreset3840x2160;
    }
    
   else {
      // _videoCamera.defaultAVCaptureSessionPreset =    AVCaptureSessionPreset640x480;
       isCapturing=NO;
      [_videoCamera start];
   }
    }
 }
-(void)viewDidAppear:(BOOL)animated{
    
    
    isCapturing = NO;
   
     [self flashTurnoffon:flash_turn];
     [_videoCamera start];
    [self autoFocus];
}


-(void)flashTurnoffon:(BOOL) flg{
    AVCaptureDevice *flashLight = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    if ([flashLight isTorchAvailable] && [flashLight isTorchModeSupported:AVCaptureTorchModeOn])
    {
        BOOL success = [flashLight lockForConfiguration:nil];
        if (success)
        {
            if ([flashLight isTorchActive] && flg==NO)
            {
                // [btnFlash setTitle:@"TURN ON" forState:UIControlStateNormal];
                [flashLight setTorchMode:AVCaptureTorchModeOff];
            }
            else if( flg==YES)
            {
                // [btnFlash setTitle:@"TURN OFF" forState:UIControlStateNormal];
                [flashLight setTorchMode:AVCaptureTorchModeOn];
            }
            [flashLight unlockForConfiguration];
        }
    }
 
}
- (IBAction)FlashToggle:(id)sender {
    flash_turn=!flash_turn;
    //if(flash_turn==YES)
    [self flashTurnoffon: flash_turn];
    _flashBtn.selected=!_flashBtn.isSelected;
    _flashBtn.tintColor=[UIColor colorWithRed:100 green:200 blue:100 alpha:0.8];
    
}


- (IBAction)AutomaticDetection:(id)sender {

    _autoCamera.selected=!_autoCamera.isSelected;
    if(_autoCamera.isSelected==YES)
        [_labelBtn setTextColor:[UIColor colorWithRed:50 green:150 blue:150 alpha:1]];
    else
        [_labelBtn setTextColor:[UIColor colorWithRed:255 green:255 blue:255 alpha:1]];
    if(_autoCamera.isSelected==YES)
        [_captureBtn setImage:[UIImage imageNamed:@"capture1.png"] forState:UIControlStateNormal];
    else
        [_captureBtn setImage:[UIImage imageNamed:@"capture.png"] forState:UIControlStateNormal];

}
-(bool)getFocus{
     AVCaptureDevice *device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    return [device isAdjustingFocus];
}
-(void)autoFocus{
   AVCaptureDevice *device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    
    if ([device lockForConfiguration:nil]) {
        
        [device setFocusMode:AVCaptureFocusModeAutoFocus];
         [device setFocusMode:AVCaptureFocusModeContinuousAutoFocus];
        [device unlockForConfiguration];
    }
    else{
        NSLog(@"error while configuring focusMode");
    }
    
    }
- (void)processImage:(cv::Mat&)image
{
    if (isCapturing==NO){
    if(_autoCamera.selected==YES){

        mem_image=image.clone();
        int res= _processingImage->detectEdge(image);
  
       
                    dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), ^{
                       
                        
                        dispatch_async(dispatch_get_main_queue(), ^{
                            
                          if(res>-1){
                            
                           if(res==MOVE_CLOSER){
                           [_warningMark setHidden:NO];
                            _warningMark.text=@"Move Closer";
                           }else if(res==PERSPECTIVE){
                               [_warningMark setHidden:NO];
                               _warningMark.text=@"Perspective";
                           }else if(res==HOLD_STILL){
                               [_warningMark setHidden:NO];
                               _warningMark.text=@"Hold still...";
                           }else if (res==NONE_DETECTION){
                               [_warningMark setHidden:NO];
                               _warningMark.text=@"Reaserchng Document";
                               
                           }
                              if(res==PERFECT_HOID && ![self getFocus]){
                        

                               [_warningMark setHidden:YES];
                               isCapturing=YES;
                              // [_videoCamera stop];
                               Mat res= _processingImage->cropDocument(image);
                               _imageView.image= [self UIImageFromCVMat:res];
                               [self gotoCameraPreview];

                           }
                            
                            }
                            
                        });
                            
                       
                        
                    });
    }else{
        
        dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), ^{
            
            
            dispatch_async(dispatch_get_main_queue(), ^{

             [_warningMark setHidden:YES];
            });
            
            
            
        });

    }
    }else{
        
        
        dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), ^{
            
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [self flashTurnoffon:NO];
                isCapturing=YES;
                [_videoCamera stop];
                [_warningMark setHidden:YES];
                cvtColor(image, image, CV_BGRA2RGBA);
                _imageView.image= [self UIImageFromCVMat:image];
                 [self gotoCameraPreview];
                

            });
            
            
            
        });

        
    }
}
-(UIImage *)UIImageFromCVMat:(cv::Mat)cvMat
{
    NSData *data = [NSData dataWithBytes:cvMat.data length:cvMat.elemSize()*cvMat.total()];
    CGColorSpaceRef colorSpace;
    
    if (cvMat.elemSize() == 1) {
        colorSpace = CGColorSpaceCreateDeviceGray();
    } else {
        colorSpace = CGColorSpaceCreateDeviceRGB();
    }
    
    CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef)data);
    
    // Creating CGImage from cv::Mat
    CGImageRef imageRef = CGImageCreate(cvMat.cols,                                 //width
                                        cvMat.rows,                                 //height
                                        8,                                          //bits per component
                                        8 * cvMat.elemSize(),                       //bits per pixel
                                        cvMat.step[0],                            //bytesPerRow
                                        colorSpace,                                 //colorspace
                                        kCGImageAlphaNone|kCGBitmapByteOrderDefault,// bitmap info
                                        provider,                                   //CGDataProviderRef
                                        NULL,                                       //decode
                                        false,                                      //should interpolate
                                        kCGRenderingIntentDefault                   //intent
                                        );
    
    
    // Getting UIImage from CGImage
    UIImage *finalImage = [UIImage imageWithCGImage:imageRef];
    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);
    
    return finalImage;
}
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
