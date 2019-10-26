//
//  NSObject+ALSingletonInit.h
//  SingletonInit
//
//  Created by apple on 2019/10/26.
//  Copyright © 2019 apple. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSObject (ALSingletonInit)

- (void)singletonInit NS_REQUIRES_SUPER;

@end

NS_ASSUME_NONNULL_END
