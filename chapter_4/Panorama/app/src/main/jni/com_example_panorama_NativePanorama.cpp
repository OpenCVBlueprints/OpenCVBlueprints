#include "com_example_panorama_NativePanorama.h"
#include "opencv2/opencv.hpp"
#include "opencv2/stitching.hpp"

using namespace std;
using namespace cv;

JNIEXPORT void JNICALL Java_com_example_panorama_NativePanorama_processPanorama
  (JNIEnv * env, jclass clazz, jlongArray imageAddressArray, jlong outputAddress){

  // Get the length of the long array
  jsize a_len = env->GetArrayLength(imageAddressArray);

  // Convert the jlongArray to an array of jlong
  jlong *imgAddressArr = env->GetLongArrayElements(imageAddressArray,0);

  // Create a vector to store all the image
  vector< Mat > imgVec;
  for(int k=0;k<a_len;k++)
  {
    // Get the image
    Mat & curimage=*(Mat*)imgAddressArr[k];
    Mat newimage;

    // Convert to a 3 channel Mat to use with Stitcher module
    cvtColor(curimage, newimage, CV_BGRA2RGB);

    // Reduce the resolution for fast computation
    float scale = 1000.0f / curimage.rows;
    resize(newimage, newimage, Size(scale * curimage.rows, scale * curimage.cols));

    imgVec.push_back(newimage);
  }

  Mat & result  = *(Mat*) outputAddress;

  Stitcher stitcher = Stitcher::createDefault();
  stitcher.stitch(imgVec, result);

  env->ReleaseLongArrayElements(imageAddressArray, imgAddressArr ,0);
}