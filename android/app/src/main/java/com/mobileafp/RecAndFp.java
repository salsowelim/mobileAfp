/*Copyright (c) 2020, Suliman Alsowelim
All rights reserved.
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree. 
*/
package com.mobileafp;
import android.widget.Toast;
import com.facebook.react.bridge.NativeModule;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.Promise;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import java.util.Map;
import java.util.HashMap;
import android.util.Log;

public class RecAndFp extends ReactContextBaseJavaModule {
  static {
        System.loadLibrary("fingerprint");
    }
  private static ReactApplicationContext reactContext;
  boolean recordingOn;
  float[] floatarray;
  int arcounter;
  RecAndFp(ReactApplicationContext context) {
    super(context);
    reactContext = context;
  }

  @Override
  public String getName() {
    return "RecAndFp";
  }
  @Override
  public Map<String, Object> getConstants() {
    final Map<String, Object> constants = new HashMap<>();
    return constants;
  }
  @ReactMethod
  public void start_recording() {
    Toast.makeText(getReactApplicationContext(), "Now recording", 6).show();
    final int SAMPLE_RATE = 8000;
    recordingOn = true;
    new Thread(new Runnable() {
        @Override
        public void run() {
            android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_AUDIO);
            int bufferSize = AudioRecord.getMinBufferSize(SAMPLE_RATE,
                    AudioFormat.CHANNEL_IN_MONO,
                    AudioFormat.ENCODING_PCM_16BIT);
            if (bufferSize == AudioRecord.ERROR || bufferSize == AudioRecord.ERROR_BAD_VALUE) {
                bufferSize = SAMPLE_RATE * 2;
            }

            short[] audioBuffer = new short[bufferSize / 2];
            floatarray = new float[9000000]; // warning this a fixed size array adjust it to your needs
            arcounter = 0;
            AudioRecord record = new AudioRecord(MediaRecorder.AudioSource.UNPROCESSED,
                    SAMPLE_RATE,
                    AudioFormat.CHANNEL_IN_MONO,
                    AudioFormat.ENCODING_PCM_16BIT,
                    bufferSize);

            if (record.getState() != AudioRecord.STATE_INITIALIZED) {
              Toast.makeText(getReactApplicationContext(), "Error in audio recorder state", 6).show();
              return;
            }
            record.startRecording();
            while (recordingOn) {
                int numberOfShort = record.read(audioBuffer, 0, audioBuffer.length);
                for (int i = 0; i < audioBuffer.length; i++) {
                   floatarray[arcounter] = (float)audioBuffer[i];
                   arcounter++;
                }
            }
            record.stop();
            record.release();
        }
    }).start();
}

  @ReactMethod
  public void stop_recording() {
    Toast.makeText(getReactApplicationContext(), "stopped", 6).show();
    recordingOn = false;
  }
  @ReactMethod
  public void fp( Promise promise) {
    String test = passingDataToJni(floatarray,arcounter);
    //String test = stringFromJNI();
    promise.resolve(test);
  }

  //public native String stringFromJNI();
  public native String passingDataToJni(float[] audio_data, int array_length);
}