//
//  AppDelegate.h
//  CroppingDoc
//
//  Created by My Star on 2/14/17.
//  Copyright © 2017 My Star. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CoreData/CoreData.h>

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (readonly, strong) NSPersistentContainer *persistentContainer;
@property (strong, nonatomic) UIImage * mem_image;
- (void)saveContext;


@end

