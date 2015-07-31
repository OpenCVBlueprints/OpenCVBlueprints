/******************
 * Unblinking Eye *
 *****************************************************************************
 *
 * This demo uses OpenCV to record slow-motion video based on high-speed
 * capture from a PlayStation Eye camera.
 *
 * Created 2015-02-14 by Joseph Howse.
 *
 * Copyright (c) 2015 Nummist Media Corporation Limited. All rights reserved.
 *
 *****************************************************************************/

#include <stdio.h>
#include <time.h>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>


int main(int argc, char *argv[]) {

  const int cameraIndex = 0;
  const bool isColor = false;
  const int w = 320;
  const int h = 240;
  const double captureFPS = 187.0;
  const double writerFPS = 60.0;
  const char filename[] = "SlowMo.avi";
  const int fourcc = cv::VideoWriter::fourcc('M','J','P','G');
  const unsigned int numFrames = 3750;

  cv::Mat mat;

  cv::VideoCapture capture(cameraIndex);
  if (!isColor) {
    capture.set(cv::CAP_PROP_MODE, cv::CAP_MODE_GRAY);
  }
  capture.set(cv::CAP_PROP_FRAME_WIDTH, w);
  capture.set(cv::CAP_PROP_FRAME_HEIGHT, h);
  capture.set(cv::CAP_PROP_FPS, captureFPS);

  cv::VideoWriter writer(
      filename, fourcc, writerFPS, cv::Size(w, h), isColor);

  clock_t startTicks = clock();
  for (unsigned int i = 0; i < numFrames;) {
    if (capture.read(mat)) {
      writer.write(mat);
      i++;
    }
  }
  clock_t endTicks = clock();
  double actualFPS =
      numFrames * CLOCKS_PER_SEC / (double)(endTicks - startTicks);
  printf("FPS: %.1f\n", actualFPS);
}
