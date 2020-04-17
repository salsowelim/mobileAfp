/*Copyright (c) 2020, Suliman Alsowelim
All rights reserved.
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree. 
*/
/*
This is a modified version of an original implementation. 
see it here https://github.com/salsowelim/dejavu_cpp_port 
*/
#include <jni.h>
#include <string>
#include <vector>
#include <math.h>   /* log10 */
#import <opencv2/opencv.hpp>
#include <android/log.h>


using namespace std;
/*extern "C" JNIEXPORT jstring JNICALL
Java_com_mobileafp_RecAndFp_stringFromJNI(JNIEnv* env, jobject instance ) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}*/

std::vector<std::vector<float>> stride_windows(const std::vector<float>& data, size_t blocksize, size_t overlap){
  //https://stackoverflow.com/questions/21344296/striding-windows/21345055
  std::vector<std::vector<float>> res;
  size_t minlen = (data.size() - overlap)/(blocksize - overlap);
  auto start = data.begin();
  for (size_t i=0; i<blocksize; ++i)
  {
    res.emplace_back(std::vector<float>());
    std::vector<float>& block = res.back();
    auto it = start++;
    for (size_t j=0; j<minlen; ++j)
    {
      block.push_back(*it);
      std::advance(it,(blocksize-overlap));
    }
  }
  return res;
}

int detrend(std::vector<std::vector<float>>& data){
  // remove the mean of all elements from them all
  size_t nocols = data[0].size();
  size_t norows = data.size();
  float mean = 0;
  for (size_t i=0; i<nocols; ++i){
    for (size_t j=0; j<norows; ++j){
      mean = mean + data[j][i];
    }
  }
  mean = mean/(norows*nocols); // calculate mean
  for (size_t i=0; i<nocols; ++i){
    for (size_t j=0; j<norows; ++j){
      data[j][i] = data[j][i] - mean;
    }
  }
  return 0;
}
std::vector<float> create_window(int wsize){
  std::vector<float> res;
  float multiplier;
  for (int i = 0; i < wsize; i++) {
    multiplier = 0.5 - 0.5 *(cos(2.0*M_PI*i/(wsize-1)));
    res.emplace_back(multiplier);
  }
  return res;
}

void apply_window(std::vector<float> &hann_window,std::vector<std::vector<float>>& data){
  size_t nocols = data[0].size();
  size_t norows = data.size();
  for (size_t i=0; i<nocols; ++i){
    for (size_t j=0; j<norows; ++j){
      data[j][i] = data[j][i] * hann_window[j];
    }
  }
  
}

std::string generate_hashes(vector<pair<int,int>> &v_in){
  int DEFAULT_FAN_VALUE = 15;
  int MIN_HASH_TIME_DELTA = 0;
  int MAX_HASH_TIME_DELTA = 200;
  //sorting
  //https://stackoverflow.com/questions/279854/how-do-i-sort-a-vector-of-pairs-based-on-the-second-element-of-the-pair
  std::sort(v_in.begin(), v_in.end(), [](auto &left, auto &right) {
    if (left.second == right.second)
      return left.first < right.first;
    return left.second < right.second;
  });
  std::ostringstream buf;
  buf << "[";
  for(int i=0; i<v_in.size(); i++){
    for(int j=1; j<DEFAULT_FAN_VALUE; j++){
      if ((i+j) < v_in.size()){
        int freq1 = v_in[i].first;
        int freq2 = v_in[i+j].first;
        int time1 = v_in[i].second;
        int time2 = v_in[i+j].second;
        int t_delta = time2 - time1;
        if ((t_delta >= MIN_HASH_TIME_DELTA) and (t_delta <= MAX_HASH_TIME_DELTA)){
          std::stringstream ss;
          ss << freq1;
          std::string freq1st = ss.str();
          ss.str("");
          ss << freq2;
          std::string freq2st = ss.str();
          ss.str("");
          ss << t_delta;
          std::string t_deltast = ss.str();
          
          std::string compositest = freq1st+"|"+freq2st+"|"+t_deltast;
          if(buf.str() != "["){
            buf << ",";
          }
           buf << "{\"hash\": \"" << compositest << "\" , " << "\"offset\": \"" << time1 << "\" }";
        }
      }
    }
  }
  buf << "]";
  cout << buf.str() << endl;
  return buf.str();
}

vector<pair<int,int>> detect_peaks (cv::Mat  data){
  int PEAK_NEIGHBORHOOD_SIZE = 20; // default is 20
  float DEFAULT_AMP_MIN = 10; // default is 10
  /* generate binary structure and apply maximum filter*/
  cv::Mat tmpkernel = cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(3,3),cv::Point(-1,-1));
  cv::Mat kernel = cv::Mat(PEAK_NEIGHBORHOOD_SIZE*2+1,PEAK_NEIGHBORHOOD_SIZE*2+1, CV_8U, uint8_t(0));
  kernel.at<uint8_t>(PEAK_NEIGHBORHOOD_SIZE,PEAK_NEIGHBORHOOD_SIZE) = uint8_t(1);
  cv::dilate(kernel, kernel, tmpkernel,cv::Point(-1, -1), PEAK_NEIGHBORHOOD_SIZE,1,1);
  cv::Mat d1;
  cv::dilate(data, d1, kernel);/* d1 now contain m1 with max filter applied */
  /* generate eroded background */
  cv::Mat background = (data == 0); // 255 if element == 0 , 0 otherwise
  cv::Mat local_max = (data == d1); // 255 if true, 0 otherwise
  cv::Mat eroded_background;
  cv::erode(background, eroded_background, kernel);
  cv::Mat detected_peaks = local_max - eroded_background;
  vector<pair<int,int>> freq_time_idx_pairs;
  for(int i=0; i<data.rows; ++i){
    for(int j=0; j<data.cols; ++j){
      if ((detected_peaks.at<uint8_t>(i, j) == 255) and (data.at<float>(i,j) > DEFAULT_AMP_MIN)) {
        freq_time_idx_pairs.push_back(std::make_pair(i,j));
      }
    }
  }
  return freq_time_idx_pairs;
}



extern "C" JNIEXPORT jstring JNICALL
Java_com_mobileafp_RecAndFp_passingDataToJni(JNIEnv *env, jobject instance, jfloatArray floatarray, jint intValue) {
  int DEFAULT_WINDOW_SIZE = 4096;
  float DEFAULT_OVERLAP_RATIO = 0.5;
  float FS = 8000.0;
  int max_freq = 0; //onesided
  if (DEFAULT_WINDOW_SIZE % 2 == 0){
    max_freq =  int(std::floor(DEFAULT_WINDOW_SIZE / 2)) + 1;
  }else{
      max_freq =  int(std::floor((DEFAULT_WINDOW_SIZE+1) / 2));
  }
  jfloat *dataArray = env->GetFloatArrayElements(floatarray, 0);
  int data_size = (int)intValue;
  std::vector<float> vec(&dataArray[0], dataArray + data_size);
  std::vector<std::vector<float>> blocks = stride_windows(vec, DEFAULT_WINDOW_SIZE, DEFAULT_WINDOW_SIZE*DEFAULT_OVERLAP_RATIO);
  std::vector<float> hann_window = create_window(DEFAULT_WINDOW_SIZE);
  apply_window(hann_window,blocks);

  cv::Mat dst(blocks[0].size(),blocks.size(), CV_32F);
  for(int i=0; i<dst.rows; ++i)
    for(int j=0; j<dst.cols; ++j){
      dst.at<float>(i, j) = blocks[j][i];
    }
  cv::dft(dst,dst,cv::DftFlags::DFT_COMPLEX_OUTPUT+cv::DftFlags::DFT_ROWS,0);
  cv::mulSpectrums(dst,dst,dst,0,true);// conj() * result
  cv::Mat dst2(max_freq,blocks.at(0).size(), CV_32F);
  for(int i=0; i<max_freq; ++i)
    for(int j=0; j<dst2.cols; ++j){
      dst2.at<float>(i, j) = dst.ptr<float>(j)[2*i];
    }
  for(int i=1; i<dst2.rows -1; ++i)
    for(int j=0; j<dst2.cols; ++j)
      dst2.at<float>(i, j) = dst2.at<float>(i, j)*2;
  
  dst2 = dst2 * (1.0/FS);
  float sum = 0.0;
  float tmp = 0.0;
  for(unsigned int i = 0; i < hann_window.size(); i++)
  {
    if(hann_window[i] < 0)
      tmp = hann_window[i]* -1;
    else
      tmp = hann_window[i];
    sum = sum + (tmp*tmp);
  }
  dst2 = dst2 * (1.0/sum);
  float threshold = 0.00000001;
  for(int i=0; i<dst2.rows; ++i){
    for(int j=0; j<dst2.cols; ++j){
      if ((dst2.at<float>(i, j)) < threshold){
        dst2.at<float>(i, j) = threshold;
      }
      dst2.at<float>(i, j) = 10 * log10(dst2.at<float>(i, j));
    }
  }
  vector<pair<int,int>> v_in = detect_peaks(dst2);
  std::string json = generate_hashes(v_in);
 env->ReleaseFloatArrayElements(floatarray, dataArray, 0);
  return env->NewStringUTF(json.c_str());
 }