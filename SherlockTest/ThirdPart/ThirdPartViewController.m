//
//  ThirdPartViewController.m
//  SherlockTest
//
//  Created by Robbie on 2017/2/22.
//  Copyright © 2017年 Robbie. All rights reserved.
//

#import "ThirdPartViewController.h"

@protocol ThirdPartDelegate <NSObject>

@end

typedef void(^ThirdPartBlock)(void);


@interface ThirdPartViewController ()

@property (nonatomic, strong) NSTimer *timer;

@property (nonatomic, strong) id<ThirdPartDelegate> delegate;

@property (nonatomic, strong) ThirdPartBlock block;

@property (nonatomic, copy) NSMutableArray *mutableArr;

@property (nonatomic, strong) NSString *str;

@property (nonatomic, unsafe_unretained) NSNumber *num;

@property (atomic, strong) NSDictionary *dic;

@end

@implementation ThirdPartViewController

- (instancetype)initWithDic:(NSDictionary *)dic {
    if (self) {}
    return self;
}

- (instancetype)initWithArr:(NSArray *)arr {
    if (self) {}
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.timer = [NSTimer scheduledTimerWithTimeInterval:0.5 target:self selector:@selector(placeHolderMethod) userInfo:nil repeats:YES];
    
    if ([super respondsToSelector:@selector(viewDidLoad)]) {
        //answer is YES
    }
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(placeHolderMethod) name:@"ErrorNotification" object:nil];
    
    [self.view addObserver:self forKeyPath:@"alpha" options:NSKeyValueObservingOptionNew context:nil];
    [self.view addObserver:self forKeyPath:@"opaque" options:NSKeyValueObservingOptionNew context:nil];
}

- (void)dealloc {
    [self.view removeObserver:self forKeyPath:@"alpha"];
}

- (void)placeHolderMethod{}

@end
