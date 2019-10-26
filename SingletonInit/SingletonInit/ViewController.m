//
//  ViewController.m
//  SingletonInit
//
//  Created by apple on 2019/10/26.
//  Copyright © 2019 apple. All rights reserved.
//

#import "ViewController.h"
#import "MapManager.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    NSLog(@"=======%@=======%@=======%@", MapManager.sharedInstance, MapManager.sharedInstance, [[MapManager alloc] init]);
    
}


@end
