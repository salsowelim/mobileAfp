/*Copyright (c) 2020, Suliman Alsowelim
All rights reserved.
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree. 
*/

import UIKit
import AVFoundation
import Accelerate
import Foundation
@objc(RecAndFp)
class RecAndFp: NSObject,AVAudioRecorderDelegate {
  private var audio_session:AVAudioSession!
  private var audio_recorder:AVAudioRecorder!
  private var filePath:URL!
  func audioRecorderDidFinishRecording(_ recorder: AVAudioRecorder,successfully flag: Bool){
    //Called by the system when a recording is stopped or has finished due to reaching its time limit.
    print ("in audioRecorderDidFinishRecording")
  }
  func audioRecorderEncodeErrorDidOccur(_ recorder: AVAudioRecorder,error: Error?){
    //Called when an audio recorder encounters an encoding error during recording.
    print ("audioRecorderEncodeErrorDidOccur")
  
  }

  @objc func start_recording() {
    print("Inside start recording")
    
    audio_session = AVAudioSession.sharedInstance()
    switch audio_session.recordPermission {
    case AVAudioSession.RecordPermission.granted:
      print("permission granted")
      break
    case AVAudioSession.RecordPermission.denied:
      print("permission denied")
      break
    case AVAudioSession.RecordPermission.undetermined:
      print("permission undetermined")
      audio_session.requestRecordPermission({(granted: Bool)-> Void in
        if granted {
          print("undetermined - granted")
        } else{
          print("undetermined not granted")
        }
      })
      break
    default:
      break
    }
    let paths = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask)
    let formatter = DateFormatter()
    formatter.dateFormat = "yyyy_MM_dd_hh_mm_ss"
    var filename = (formatter.string(from: Date()) as NSString) as String
    filename = filename + ".wav"
    filePath = paths[0].appendingPathComponent(filename)
    let settings = [
      AVFormatIDKey: Int(kAudioFormatLinearPCM),
      AVSampleRateKey: 44100,
      AVNumberOfChannelsKey: 1,
      AVLinearPCMBitDepthKey: 16,
      AVLinearPCMIsBigEndianKey: 0,
      AVLinearPCMIsFloatKey: 0,
      AVEncoderAudioQualityKey:AVAudioQuality.high.rawValue
    ]
    do {
       try audio_session.setCategory(AVAudioSession.Category.playAndRecord,mode: AVAudioSession.Mode.default)
      try audio_session.setActive(true)
      audio_recorder = try AVAudioRecorder(url:filePath, settings: settings)
      audio_recorder.delegate = self
      audio_recorder.isMeteringEnabled = true
      audio_recorder.prepareToRecord()
      let recording_started = audio_recorder.record()
      if (recording_started){
        print ("record return true")
      }else{
        print("record return false")
      }

    }catch{
      print ("im catching error")
      print(error)
    }
  }
  
  @objc func stop_recording() {
    audio_recorder.stop()
    print("inside stop_recording .. recording stopped")
  }
  
  @objc func fp(_ resolve: @escaping RCTPromiseResolveBlock,
                rejecter reject: @escaping RCTPromiseRejectBlock ){
    var to_return_string = "test fp";
    print("inside Fp method")
    let fpcpp = fingerprint()
    //fpcpp.isThisWorking()
     let url = filePath!
    let file: AVAudioFile!
    do{
      file = try AVAudioFile(forReading: url,commonFormat: .pcmFormatInt16, interleaved: false)
      let totSamples = file.length
      let format = AVAudioFormat(commonFormat: .pcmFormatInt16, sampleRate: file.fileFormat.sampleRate, channels:  1, interleaved: false)!
      let buffer = AVAudioPCMBuffer(pcmFormat: format, frameCapacity: AVAudioFrameCount(totSamples+1000))!
      try file.read(into: buffer) // return true if reading is good.
      let samples_array = Array(UnsafeBufferPointer(start: buffer.int16ChannelData?[0], count:Int(buffer.frameLength)))
      var samples_array_float = [Float32](repeating: 0, count: samples_array.count) // sample arry to float
      for i in 0 ..< samples_array.count {
        samples_array_float[i] = Float32(samples_array[i])
      }
      
      let cstr = fpcpp.get_fingerprints(&samples_array_float,data_size: Int32(samples_array_float.count))
      to_return_string = String(cString: cstr)
      free(UnsafeMutablePointer(cstr))
    }catch {
      print("Error info: \(error)")
    }
    resolve(to_return_string)
  }

 }
 

