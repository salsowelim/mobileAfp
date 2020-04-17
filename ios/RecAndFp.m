/*Copyright (c) 2020, Suliman Alsowelim
All rights reserved.
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree. 
*/
#import "React/RCTBridgeModule.h"
@interface RCT_EXTERN_MODULE(RecAndFp, NSObject)

  RCT_EXTERN_METHOD(start_recording)
  RCT_EXTERN_METHOD(stop_recording)
  RCT_EXTERN_METHOD(fp : (RCTPromiseResolveBlock)resolve
                    rejecter : (RCTPromiseRejectBlock)reject)

@end
