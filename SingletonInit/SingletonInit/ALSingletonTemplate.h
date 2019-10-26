//
//  ALSingletonTemplate.h
//  SingletonInit
//
//  Created by apple on 2019/10/26.
//  Copyright © 2019 apple. All rights reserved.
//

#ifndef ALSingletonTemplate_h
#define ALSingletonTemplate_h

/**
 * A template code for define a singleton class.
 * Example:
    <code>
    // .h file
    @interface SingletionTest : NSObject
    AS_SINGLETON
    @end
 
    // .m file
    @implementation SingletionTest
    SYNTHESIZE_SINGLETON(SingletionTest)
    // IMPORTANT: DO NOT add `-init` in you singleton class!!! you should use `-singletonInit` instead!!!
    // and DONT FORGET to add `[super singletonInit]` in you singletonInit method.
    - (void)singletonInit {
        [super singletonInit];
        // your init code here ...
    }
 
    // your code here ...
    @end
 
    // usage:
    SingletionTest *singleton = [SingletionTest sharedInstance];
    // or: SingletionTest *singleton = [[SingletionTest alloc] init];
    // or: SingletionTest *singleton = [SingletionTest new];
    </code>
 */

///////////////////////////////////////////////////////////////////////////////////////////////
/// singleton
#undef AL_AS_SINGLETON
#if __has_feature(objc_arc)
    #define AL_AS_SINGLETON             \
        + (instancetype)sharedInstance; \
        + (void)al_destroySingleton;    \
        - (void)al_destroySingleton;
#else
    #define AL_AS_SINGLETON             \
        + (instancetype)sharedInstance;
#endif

/// weak singleton; only supports ARC
#if __has_feature(objc_arc)
    #undef AL_AS_WEAK_SINGLETON
    #define AL_AS_WEAK_SINGLETON  AL_AS_SINGLETON
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
#undef AL_SYNTHESIZE_SINGLETON
#if __has_feature(objc_arc)
    #undef AL_SYNTHESIZE_WEAK_SINGLETON
    #define AL_SYNTHESIZE_WEAK_SINGLETON(CLS)                               \
        static __weak CLS *__AL_SINGLETON_INSTANCE_FOR_CLASS(CLS) = nil;    \
        __AL_SYNTHESIZE_SINGLETON_ARC(CLS);

    #define AL_SYNTHESIZE_SINGLETON(CLS)                                    \
        static CLS *__AL_SINGLETON_INSTANCE_FOR_CLASS(CLS) = nil;           \
        __AL_SYNTHESIZE_SINGLETON_ARC(CLS);
#else
    #define AL_SYNTHESIZE_SINGLETON(CLS)                           \
        static CLS *__AL_SINGLETON_INSTANCE_FOR_CLASS(CLS) = nil;  \
        __AL_SYNTHESIZE_SINGLETON_MRC(CLS);
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
#undef __AL_SINGLETON_SEMAPHORE_FOR_CLASS
#define __AL_SINGLETON_SEMAPHORE_FOR_CLASS(cls)   __AL_SINGLETON_MACRO_CONCAT(__al_singleton_semaphore_, cls)

#undef __AL_SYNTHESIZE_SINGLETON_COMMON
#define __AL_SYNTHESIZE_SINGLETON_COMMON(cls)                                                                        \
    +(dispatch_semaphore_t) __AL_SINGLETON_SEMAPHORE_FOR_CLASS(cls) {                                                \
        static dispatch_semaphore_t semaphore;                                                                       \
        static dispatch_once_t onceToken;                                                                            \
        dispatch_once(&onceToken, ^{                                                                                 \
            semaphore = dispatch_semaphore_create(1);                                                                \
        });                                                                                                          \
        return semaphore;                                                                                            \
    }                                                                                                                \
                                                                                                                     \
    +(instancetype) sharedInstance {                                                                                 \
        if (self != cls.class) {                                                                                     \
            printf(                                                                                                  \
                "‼️ [SINGLETON] class `%s` invokes `%s` will return the instance of `%s`, which is not the one " \
                "you expected.\n\n",                                                                                     \
                NSStringFromClass(self).UTF8String, __PRETTY_FUNCTION__, #cls);                                      \
        }                                                                                                            \
        __block cls *strongRef = __AL_SINGLETON_INSTANCE_FOR_CLASS(cls);                                             \
        if (strongRef == nil) {                                                                                      \
            dispatch_semaphore_t semaphore = [cls __AL_SINGLETON_SEMAPHORE_FOR_CLASS(cls)];                          \
            __AL_SINGLETON_SEMAPHORE_WITH_TIMEOUT(semaphore,                                                         \
                                                  if (__AL_SINGLETON_INSTANCE_FOR_CLASS(cls) == nil) {               \
                                                      strongRef = [[super allocWithZone:NULL] init];                 \
                                                      __AL_SINGLETON_INSTANCE_FOR_CLASS(cls) = strongRef;            \
                                                  } else { strongRef = __AL_SINGLETON_INSTANCE_FOR_CLASS(cls); });   \
        }                                                                                                            \
        return strongRef;                                                                                            \
    }                                                                                                                \
                                                                                                                     \
        + (id) allocWithZone : (NSZone *) zone {                                                                     \
        if (self == cls.class) {                                                                                     \
            return [self sharedInstance];                                                                            \
        }                                                                                                            \
        return [super allocWithZone:zone];                                                                           \
    }                                                                                                                \
                                                                                                                     \
    -(instancetype) init {                                                                                           \
        static dispatch_semaphore_t semaphore;                                                                       \
        static dispatch_once_t onceToken;                                                                            \
        dispatch_once(&onceToken, ^{                                                                                 \
            semaphore = dispatch_semaphore_create(1);                                                                \
        });                                                                                                          \
                                                                                                                     \
        cls *strongRef = __AL_SINGLETON_INSTANCE_FOR_CLASS(cls);                                                     \
        __AL_SINGLETON_SEMAPHORE_WITH_TIMEOUT(semaphore, if (strongRef.class != self.class) {                        \
            self = [super init];                                                                                     \
            if (self.class == cls.class) {                                                                           \
                [self singletonInit];                                                                                \
            }                                                                                                        \
        });                                                                                                          \
        return self;                                                                                                 \
    }                                                                                                                \
                                                                                                                     \
    -(id) copyWithZone : (nullable NSZone *) zone {                                                                  \
        return self;                                                                                                 \
    }                                                                                                                \
    -(id) mutableCopyWithZone : (nullable NSZone *) zone {                                                           \
        return self;                                                                                                 \
    }

///////////////////////////////////////////////////////////////////////////////////////////////
#undef __AL_SYNTHESIZE_SINGLETON_ARC
#define __AL_SYNTHESIZE_SINGLETON_ARC(cls)                                                        \
    __AL_SYNTHESIZE_SINGLETON_COMMON(cls);                                                        \
    + (void)al_destroySingleton {                                                                 \
        printf("‼️ [SINGLETON] The singleton instance `%s` will be deallocated.\n",           \
               [self description].UTF8String);                                                    \
        dispatch_semaphore_t lock = [cls __AL_SINGLETON_SEMAPHORE_FOR_CLASS(cls)];                \
        __AL_SINGLETON_SEMAPHORE_WITH_TIMEOUT(lock,                                               \
            __AL_SINGLETON_INSTANCE_FOR_CLASS(cls) = nil;                                         \
        );                                                                                        \
    }                                                                                             \
    -(void) al_destroySingleton {                                                                 \
        [self.class al_destroySingleton];                                                         \
    };

///////////////////////////////////////////////////////////////////////////////////////////////
#undef __AL_SYNTHESIZE_SINGLETON_MRC
#define __AL_SYNTHESIZE_SINGLETON_MRC(cls)              \
    __AL_SYNTHESIZE_SINGLETON_COMMON(cls);              \
                                                        \
    - (instancetype)retain { return self; }             \
    - (oneway void)release{}                            \
    - (instancetype)autorelease {  return self; }       \
    - (NSUInteger)retainCount { return NSUIntegerMax; }

///////////////////////////////////////////////////////////////////////////////////////////////

#undef __AL_SINGLETON_MACRO_CONCAT_
#define __AL_SINGLETON_MACRO_CONCAT_(a, b) a##b
#undef __AL_SINGLETON_MACRO_CONCAT
#define __AL_SINGLETON_MACRO_CONCAT(a, b) __AL_SINGLETON_MACRO_CONCAT_(a, b)

#undef __AL_SINGLETON_INSTANCE_FOR_CLASS
#define __AL_SINGLETON_INSTANCE_FOR_CLASS(cls) __AL_SINGLETON_MACRO_CONCAT(__al_singleton_instance_, cls)

///
/// execute the code statements `jobStmt` in dispatch_semaphore.
/// Try to get the semaphore in 10 secods, if failed, that may means a deadlock is occured. and you should check you code.
/// @note DO NOT return in `jobStmt`, otherwise the samaphore will not be processed correctly.
///
#undef __AL_SINGLETON_SEMAPHORE_WITH_TIMEOUT
#define __AL_SINGLETON_SEMAPHORE_WITH_TIMEOUT(sema, jobStmt)                                                        \
    if (dispatch_semaphore_wait((sema), dispatch_time(DISPATCH_TIME_NOW, (int64_t)(10.f * NSEC_PER_SEC))) == 0) {   \
        jobStmt;                                                                                                    \
        dispatch_semaphore_signal((sema));                                                                          \
    } else {                                                                                                        \
        NSAssert(NO, @"[SINGLETON] %s: timeout while waiting to acquire the lock. Deadlock may occured!", __PRETTY_FUNCTION__); \
    }


#endif /* ALSingletonTemplate_h */
