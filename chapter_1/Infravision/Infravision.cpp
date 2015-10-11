/***************
 * Infravision *
 *****************************************************************************
 *
 * This demo uses OpenNI2 and OpenCV to capture and display infrared images.
 *
 * Created 2015-02-16 by Joseph Howse.
 *
 * Copyright (c) 2015 Nummist Media Corporation Limited. All rights reserved.
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <OpenNI.h>

int main(int argc, char *argv[]) {

  const openni::SensorType sensorType = openni::SENSOR_IR;
//  const openni::SensorType sensorType = openni::SENSOR_COLOR;
//  const openni::SensorType sensorType = openni::SENSOR_DEPTH;
  const char windowName[] = "Infravision";

  int srcMatType;
  if (sensorType == openni::SENSOR_COLOR) {
    srcMatType = CV_8UC3;
  } else {
    srcMatType = CV_16U;
  }

  openni::Status status;

  status = openni::OpenNI::initialize();
  if (status != openni::STATUS_OK) {
    printf(
        "Failed to initialize OpenNI:\n%s\n",
        openni::OpenNI::getExtendedError());
    return EXIT_FAILURE;
  }

  openni::Device device;
  status = device.open(openni::ANY_DEVICE);
  if (status != openni::STATUS_OK) {
    printf(
        "Failed to open device:\n%s\n",
        openni::OpenNI::getExtendedError());
    openni::OpenNI::shutdown();
    return EXIT_FAILURE;
  }

  const openni::SensorInfo *sensorInfo =
      device.getSensorInfo(sensorType);
  if (sensorInfo == NULL) {
    printf("Failed to find sensor of appropriate type\n");
    device.close();
    openni::OpenNI::shutdown();
    return EXIT_FAILURE;
  }

  openni::VideoStream stream;
  status = stream.create(device, sensorType);
  if (status != openni::STATUS_OK) {
    printf(
        "Failed to create stream:\n%s\n",
        openni::OpenNI::getExtendedError());
    device.close();
    openni::OpenNI::shutdown();
    return EXIT_FAILURE;
  }

  // Select the video mode with the highest resolution.
  {
    const openni::Array<openni::VideoMode> *videoModes =
        &sensorInfo->getSupportedVideoModes();
    int maxResolutionX = -1;
    int maxResolutionIndex = 0;
    for (int i = 0; i < videoModes->getSize(); i++) {
      int resolutionX = (*videoModes)[i].getResolutionX();
      if (resolutionX > maxResolutionX) {
        maxResolutionX = resolutionX;
        maxResolutionIndex = i;
      }
    }
    stream.setVideoMode((*videoModes)[maxResolutionIndex]);
  }

  status = stream.start();
  if (status != openni::STATUS_OK) {
    printf(
        "Failed to start stream:\n%s\n",
        openni::OpenNI::getExtendedError());
    stream.destroy();
    device.close();
    openni::OpenNI::shutdown();
    return EXIT_FAILURE;
  }

  openni::VideoFrameRef frame;
  cv::Mat dstMat;
  cv::namedWindow(windowName);

  // Capture and display frames until any key is pressed.
  while (cv::waitKey(1) == -1) {
    status = stream.readFrame(&frame);
    if (frame.isValid()) {
      cv::Mat srcMat(
          frame.getHeight(), frame.getWidth(), srcMatType,
          (void *)frame.getData(), frame.getStrideInBytes());
      if (sensorType == openni::SENSOR_COLOR) {
        cv::cvtColor(srcMat, dstMat, cv::COLOR_RGB2BGR);
      } else {
        srcMat.convertTo(dstMat, CV_8U);
      }
      cv::imshow(windowName, dstMat);
    }
  }

  cv::destroyWindow(windowName);

  stream.stop();
  stream.destroy();
  device.close();
  openni::OpenNI::shutdown();
}
