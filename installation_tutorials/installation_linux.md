# OpenCV3.0 installation guide for Linux based systems

This is a step-by-step installation guide for installing OpenCV3 on a Linux based system. This installation guide was checked on Ubuntu 12.02 and Ubuntu 14.04. If however the installation does not work on your specific Linux distribution, then do open up an issues page, so we can try to help you find a solution.

## Preparing your system

You should start by firing up a console. The complete guide will be based on console commands, since Linux distributions do not all cover a graphical interface. Once you have fired up the console window you can start by installing some basic packages. This can be done with the following installation commands.

<pre>
  // Install some basic libraries used by the OpenCV software
  // Start with a good compiler
  apt-get install build-essentials
  // Now install the required packages for installing OpenCV in linux
  apt-get install cmake cmake-gui git pkg-config libgtk2.0-dev libavcodec-dev libformat-dev libswscale-dev
  // If you want python support you should install python and the numpy library
  apt-get install python-dev python-numpy 
  // If you want some free optimization packages then run this line
  apt-get install libtbb2 libtbb-dev
  // If you want support for the most standard image formats then run this line
  apt-get install libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
  // If you want Eigen support for mathematical calculations
  apt-get install libeigen-dev libeigen2-dev libeigen3-dev
</pre>

Now you should decide which big optimization packages that you want to install in your OpenCV3 installation. 

**Installing CUDA support for NVIDIA GPU functionality**

**Installing OpenCL support for non-NVIDIA GPU functionality**

## Start by grabbing the latest OpenCV3 installation 

## Running CMAKE to configure, build and install OpenCV3

## Testing your OpenCV installation
