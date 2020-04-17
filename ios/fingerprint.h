/*Copyright (c) 2020, Suliman Alsowelim
All rights reserved.
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree. 
*/
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface fingerprint : NSObject
- (void)isThisWorking;
- (char *) get_fingerprints: (float *) data
    data_size:(int)data_size;

@end

NS_ASSUME_NONNULL_END
