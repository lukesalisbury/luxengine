//
//  luxengineAppDelegate.m
//  luxengine
//
//  Created by user on 12/22/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "luxengineAppDelegate.h"
#import "luxengineViewController.h"

@implementation luxengineAppDelegate

@synthesize window;
@synthesize viewController;

- (void)applicationWillResignActive:(UIApplication *)application
{
    [viewController stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    [viewController startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [viewController stopAnimation];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Handle any background procedures not related to animation here.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Handle any foreground procedures not related to animation here.
}

- (void)dealloc
{
    [viewController release];
    [window release];
    
    [super dealloc];
}

@end
