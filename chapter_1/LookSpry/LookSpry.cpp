/************
 * LookSpry *
 *****************************************************************************
 *
 * This demo uses FlyCapture2, OpenCV, and SDL2 in an optimized, real-time
 * video pipeline, capable of handling high-speed HD streams.
 *
 * The following key commands toggle the demo's modes:
 *   'd': Start or stop face [d]etection (and drawing of face rectangles).
 *   'm': [M]irror or un-mirror the video.
 *
 * Created 2015-01-30 by Joseph Howse.
 *
 * Copyright (c) 2015 Nummist Media Corporation Limited. All rights reserved.
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <flycapture/C/FlyCapture2_C.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <SDL2/SDL.h>


#ifdef _WIN32
#define snprintf sprintf_s
#endif

void showFC2Error(fc2Error error) {
  if (error != FC2_ERROR_OK) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "FlyCapture2 Error",
                             fc2ErrorToDescription(error), NULL);
  }
}

void showSDLError() {
  SDL_ShowSimpleMessageBox(
      SDL_MESSAGEBOX_ERROR, "SDL2 Error", SDL_GetError(), NULL);
}

int main(int argc, char *argv[]) {

  const unsigned int cameraIndex = 0u;
  const unsigned int numImagesPerFPSMeasurement = 240u;
  const int windowWidth = 1440;
  const int windowHeight = 900;
  const char cascadeFilename[] = "haarcascade_frontalface_alt.xml";
  const double detectionScaleFactor = 1.25;
  const int detectionMinNeighbours = 4;
  const int detectionFlags = CV_HAAR_SCALE_IMAGE;
  const cv::Size detectionMinSize(120, 120);
  const cv::Size detectionMaxSize;
  const cv::Scalar detectionDrawColor(255.0, 0.0, 255.0);
  char strBuffer[256u];
  const size_t strBufferSize = 256u;

  int matType;
  cv::Mat equalizedGrayMat;

#ifdef _WIN32
  snprintf(strBuffer, strBufferSize, "%s/../%s", argv[0], cascadeFilename);
  cv::CascadeClassifier detector(strBuffer);
#else
  cv::CascadeClassifier detector(cascadeFilename);
#endif
  if (detector.empty()) {
    snprintf(strBuffer, strBufferSize, "%s could not be loaded.",
              cascadeFilename);
    SDL_ShowSimpleMessageBox(
      SDL_MESSAGEBOX_ERROR, "Failed to Load Cascade File", strBuffer, NULL);
    return EXIT_FAILURE;
  }
  std::vector<cv::Rect> detectionRects;

  fc2Error error;

  fc2Image image;
  error = fc2CreateImage(&image);
  if (error != FC2_ERROR_OK) {
    showFC2Error(error);
    return EXIT_FAILURE;
  }

  fc2Context context;
  error = fc2CreateContext(&context);
  if (error != FC2_ERROR_OK) {
    showFC2Error(error);
    return EXIT_FAILURE;
  }
  
  fc2PGRGuid cameraGUID;
  error = fc2GetCameraFromIndex(context, cameraIndex, &cameraGUID);
  if (error != FC2_ERROR_OK) {
    showFC2Error(error);
    return EXIT_FAILURE;
  }
  
  error = fc2Connect(context, &cameraGUID);
  if (error != FC2_ERROR_OK) {
    showFC2Error(error);
    return EXIT_FAILURE;
  }

  error = fc2StartCapture(context);
  if (error != FC2_ERROR_OK) {
    fc2Disconnect(context);
    showFC2Error(error);
    return EXIT_FAILURE;
  }

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fc2StopCapture(context);
    fc2Disconnect(context);
    showSDLError();
    return EXIT_FAILURE;
  }

  SDL_Window *window = SDL_CreateWindow(
      "LookSpry", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      windowWidth, windowHeight, 0u);
  if (window == NULL) {
    fc2StopCapture(context);
    fc2Disconnect(context);
    showSDLError();
    return EXIT_FAILURE;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0u);
  if (renderer == NULL) {
    fc2StopCapture(context);
    fc2Disconnect(context);
    SDL_DestroyWindow(window);
    showSDLError();
    return EXIT_FAILURE;
  }
  
  SDL_RendererInfo rendererInfo;
  SDL_GetRendererInfo(renderer, &rendererInfo);

  if (strcmp(rendererInfo.name, "direct3d") == 0) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
  } else if (strcmp(rendererInfo.name, "opengl") == 0) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  }

  snprintf(strBuffer, strBufferSize, "LookSpry | %s", rendererInfo.name);
  SDL_SetWindowTitle(window, strBuffer);

  SDL_Texture *videoTex = NULL;
  void *videoTexPixels;
  int pitch;

  clock_t startTicks = clock();
  clock_t endTicks;
  unsigned int numImagesCaptured = 0u;

  bool running = true;
  bool detecting = true;
  bool mirroring = true;
  SDL_Event event;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
        break;
      } else if (event.type == SDL_KEYUP) {
        switch(event.key.keysym.sym) {
        // When 'd' is pressed, start or stop [d]etection.
        case SDLK_d:
          detecting = !detecting;
          break;
        // When 'm' is pressed, [m]irror or un-mirror the video.
        case SDLK_m:
          mirroring = !mirroring;
          break;
        default:
          break;
        }
      }
    }

    error = fc2RetrieveBuffer(context, &image);
    if (error != FC2_ERROR_OK) {
       fc2Disconnect(context);
       SDL_DestroyTexture(videoTex);
       SDL_DestroyRenderer(renderer);
       SDL_DestroyWindow(window);
       showFC2Error(error);
       return EXIT_FAILURE;
    }

    if (videoTex == NULL) {
      equalizedGrayMat.create(image.rows, image.cols, CV_8UC1);
      SDL_RenderSetLogicalSize(renderer, image.cols, image.rows);
      Uint32 videoTexPixelFormat;
      switch (image.format) {
        // For monochrome capture modes, plan to render captured data to the Y
        // plane of a planar YUV texture.
        case FC2_PIXEL_FORMAT_RAW8:
        case FC2_PIXEL_FORMAT_MONO8:
          videoTexPixelFormat = SDL_PIXELFORMAT_YV12;
          matType = CV_8UC1;
          break;
        // For color capture modes, plan to render captured data to the entire
        // space of a texture in a matching color format.
        case FC2_PIXEL_FORMAT_422YUV8:
          videoTexPixelFormat = SDL_PIXELFORMAT_UYVY;
          matType = CV_8UC2;
          break;
        case FC2_PIXEL_FORMAT_RGB:
          videoTexPixelFormat = SDL_PIXELFORMAT_RGB24;
          matType = CV_8UC3;
          break;
        case FC2_PIXEL_FORMAT_BGR:
          videoTexPixelFormat = SDL_PIXELFORMAT_BGR24;
          matType = CV_8UC3;
          break;
        default:
          fc2StopCapture(context);
          fc2Disconnect(context);
          SDL_DestroyTexture(videoTex);
          SDL_DestroyRenderer(renderer);
          SDL_DestroyWindow(window);
          SDL_ShowSimpleMessageBox(
              SDL_MESSAGEBOX_ERROR, "Unsupported FlyCapture2 Pixel Format",
              "LookSpry supports RAW8, MONO8, 422YUV8, RGB, and BGR.", NULL);
          return EXIT_FAILURE;
      }
      videoTex = SDL_CreateTexture(
          renderer, videoTexPixelFormat, SDL_TEXTUREACCESS_STREAMING,
          image.cols, image.rows);
      if (videoTex == NULL) {
        fc2StopCapture(context);
        fc2Disconnect(context);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        showSDLError();
        return EXIT_FAILURE;
      }
      snprintf(
          strBuffer, strBufferSize, "LookSpry | %s | %dx%d --> %dx%d",
          rendererInfo.name, image.cols, image.rows, windowWidth,
          windowHeight);
      SDL_SetWindowTitle(window, strBuffer);
    }

    cv::Mat srcMat(image.rows, image.cols, matType, image.pData, image.stride);
    if (detecting) {
      switch (image.format) {
        // For monochrome capture modes, just equalize.
        case FC2_PIXEL_FORMAT_RAW8:
        case FC2_PIXEL_FORMAT_MONO8:
          cv::equalizeHist(srcMat, equalizedGrayMat);
          break;
        // For color capture modes, convert to gray and equalize.
        case FC2_PIXEL_FORMAT_422YUV8:
          cv::cvtColor(srcMat, equalizedGrayMat, cv::COLOR_YUV2GRAY_UYVY);
          cv::equalizeHist(equalizedGrayMat, equalizedGrayMat);
          break;
        case FC2_PIXEL_FORMAT_RGB:
          cv::cvtColor(srcMat, equalizedGrayMat, cv::COLOR_RGB2GRAY);
          cv::equalizeHist(equalizedGrayMat, equalizedGrayMat);
          break;
        case FC2_PIXEL_FORMAT_BGR:
          cv::cvtColor(srcMat, equalizedGrayMat, cv::COLOR_BGR2GRAY);
          cv::equalizeHist(equalizedGrayMat, equalizedGrayMat);
          break;
        default:
          break;
      }
      // Run the detector on the equalized image.
      detector.detectMultiScale(
          equalizedGrayMat, detectionRects, detectionScaleFactor,
          detectionMinNeighbours, detectionFlags, detectionMinSize,
          detectionMaxSize);
      // Draw the resulting detection rectangles on the original image.
      for (cv::Rect detectionRect : detectionRects) {
        cv::rectangle(srcMat, detectionRect, detectionDrawColor);
      }
    }

    SDL_LockTexture(videoTex, NULL, &videoTexPixels, &pitch);

    switch (image.format) {
    case FC2_PIXEL_FORMAT_RAW8:
    case FC2_PIXEL_FORMAT_MONO8:
      // Make the planar YUV video gray by setting all bytes in its U and V
      // planes to 128 (the middle of the range).
      memset(((unsigned char *)videoTexPixels + image.dataSize), 128,
             image.dataSize / 2u);
      break;
    default:
      break;
    }

    if (mirroring) {
      // Flip the image data while copying it to the texture.
      cv::Mat dstMat(image.rows, image.cols, matType, videoTexPixels,
                     image.stride);
      cv::flip(srcMat, dstMat, 1);
    } else {
      // Copy the image data, as-is, to the texture.
      // Note that the PointGrey image and srcMat have pointers to the same
      // data, so the following code does reference the data that we modified
      // earlier via srcMat.
      memcpy(videoTexPixels, image.pData, image.dataSize);
    }

    SDL_UnlockTexture(videoTex);
    SDL_RenderCopy(renderer, videoTex, NULL, NULL);
    SDL_RenderPresent(renderer);

    numImagesCaptured++;
    if (numImagesCaptured >= numImagesPerFPSMeasurement) {
      endTicks = clock();
      snprintf(
          strBuffer, strBufferSize, "LookSpry | %s | %dx%d --> %dx%d | %ld FPS",
          rendererInfo.name, image.cols, image.rows, windowWidth,
          windowHeight,
          numImagesCaptured * CLOCKS_PER_SEC / (endTicks - startTicks));
      SDL_SetWindowTitle(window, strBuffer);
      startTicks = endTicks;
      numImagesCaptured = 0u;
    }
  }

  fc2StopCapture(context);
  fc2Disconnect(context);
  SDL_DestroyTexture(videoTex);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  return EXIT_SUCCESS;
}
