//
//  luxengineAppDelegate.h
//  luxengine
//
//  Created by user on 12/22/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class luxengineViewController;

@interface luxengineAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    luxengineViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet luxengineViewController *viewController;

@end

