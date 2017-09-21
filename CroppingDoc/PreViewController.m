//
//  PreViewController.m
//  CroppingDoc
//
//  Created by My Star on 2/21/17.
//  Copyright © 2017 My Star. All rights reserved.
//

#import "PreViewController.h"
#import "AppDelegate.h"
@interface PreViewController ()
@property (weak, nonatomic) IBOutlet UIImageView *imageView;

@end

@implementation PreViewController

- (void)viewDidLoad {
    
    [super viewDidLoad];
    AppDelegate* appDelegate;
    appDelegate = (AppDelegate*)[UIApplication sharedApplication].delegate;
    _imageView.image=appDelegate.mem_image;

    // Do any additional setup after loading the view.
}
- (BOOL)prefersStatusBarHidden {
    
    return YES;
}
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
- (IBAction)ReTkae:(id)sender {
      [self dismissViewControllerAnimated:YES completion:NULL];
    
}
- (IBAction)usePhoto:(id)sender {
    
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
