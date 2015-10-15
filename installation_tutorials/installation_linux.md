# OpenCV 3 installation guide for Linux

This is a step-by-step installation guide for installing OpenCV 3 on a Linux system. This installation guide was checked on Ubuntu 12.04 and Ubuntu 14.04. If, however, the installation does not work on your specific Linux distribution, then do open up an issues page, so we can try to help you find a solution.

## Preparing your system

You should start by firing up a console window. The complete guide will be based on console commands, since Linux distributions do not all cover a graphical interface. Once you have fired up the console window, you can start by installing some basic packages. This can be done with the following installation commands:

```C++
  // Install some basic libraries used by the OpenCV software
  // Start with a good compiler
  apt-get install build-essentials
  // Now install the required packages for installing OpenCV in linux
  apt-get install cmake cmake-qt-gui git pkg-config libgtk2.0-dev libavcodec-dev libformat-dev libswscale-dev
  // If you want python support you should install python and the numpy library
  apt-get install python-dev python-numpy
  // If you want some free optimization packages then run this line
  apt-get install libtbb2 libtbb-dev
  // If you want support for the most standard image formats then run this line
  apt-get install libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
  // If you want Eigen support for mathematical calculations
  apt-get install libeigen-dev libeigen2-dev libeigen3-dev
```

Thanks to the geniusness of OpenCV, most optimization packages of OpenCV are included as source files and built during installation of OpenCV, so you are not expected to add anything other than the above to get a basic system to work.

**Installing CUDA or OpenCL support**

If you want to use the GPU-powered functionality that is embedded into OpenCV, then please download the corresponding packages.
* CUDA can be downloaded from the [NVIDIA website](https://developer.nvidia.com/cuda-downloads). Install it in your system and remember where you installed it, since OpenCV might ask for the package location.
* OpenCL comes preinstalled on most Linux based systems, or comes as a package with your library. There is no need to install this seperately.

## Start by grabbing the latest OpenCV 3 installation 

Now that we have our basic system configured, we will start by setting up a folder structure. Make sure that somewhere on your system you have the following folder layout:

--> root directory

------> opencv

----------> build

------> opencv_contrib

Then, we will start by grabbing the latest master branch for the main OpenCV repository. This can be done by the following command from the root director:

`git clone https://github.com/Itseez/opencv.git opencv`

This will download the compelete repository into your local drive, so give it some time to finish. Once it is done, you need to make sure that the correct branch is selected. Therefore, execute the command:

`git checkout master`

Now, go back to the root folder and execute the command needed for retrieving the OpenCV "Contrib" (contributed) repository, since several chapters of this book use techniques and functionality from these contributed modules.

`git clone https://github.com/Itseez/opencv_contrib.git opencv_contrib`

Again, make sure that you have the correct branch by doing:

`git checkout master`

Now, your basic system is set up to go and be installed. 

## Running CMake to configure, build and install OpenCV 3

Start by navigating to the build directory inside your OpenCV folder. Fire up the building process by executing

`cmake-gui ..`

which basically fires up the CMake graphical interface (a bit easier to configure for new users). Of course, if you have a non-graphical Linux system, you might want to try the `ccmake` package which can be installed through `sudo apt-get install cmake-curses-gui`.

The first window you will get is, a window asking you to define where your source folder and build folder are. However, since we ran the `cmake` command from the correct folder, pointing it to the source folder which is one level up using the `..` syntax, we are already set up correctly. Now, hit the `Configure` button!

![start](https://github.com/OpenCVBlueprints/OpenCVBlueprints/blob/master/installation_tutorials/images/start.png)

The configuration will first ask you what generator must be used for this project. Make sure you select `Use default native compilers` as option and from the dropdown list `Unix Makefiles`. Now, the tool will check all the packages installed on your system and give you the following output:

![first run](https://github.com/OpenCVBlueprints/OpenCVBlueprints/blob/master/installation_tutorials/images/first_run.png)

The top part, all in red, contains all the fields where CMake still wants your input. The bottom part is a wrap-up of all the things that will be configured in your OpenCV installation. The basic rule is, as long CMake keeps asking for input with red fields, the configure stage is not complete yet. Now, start browsing the top part and make sure that you find the field `OPENCV_EXTRA_MODULES_PATH` and make sure that it points to your opencv_contrib modules folder. In our case, this will be `/home/spu/Documents/github/opencv_contrib/modules`. Now hit the configure button again!

Now, in our second run, we will make the following adaptations:
* From the new modules that are introduced, we only select `BUILD_opencv_face, BUILD_opencv_xfeatures2d and BUILD_opencv_ximproc`. All other new modules that are in red can be disabled. We do not need them for the book and they can only cause things to break down.
* Go down to the field `CMAKE_BUILD_TYPE` and select `Debug`. This ensures that we get meaningful errors when OpenCV decides to crash.
* *IF YOU NEED CUDA*: Find the field `CUDA_ARCH_BIN and CUDA_ARCH_PTX` and leave only the value specific for your card, which can be found [here](https://developer.nvidia.com/cuda-gpus). This ensures that building goes way faster! For example, my card are Kepler K2000 cards and I set both values to 3.0.
* *IF YOU DO __NOT__ NEED CUDA*: Make sure that the following fields are disabled: `WITH_CUBLAS, WITH_CUDA and WITH_CUFFT`
* *IF YOU DO __NOT__ NEED OPENCL*: Make sure that the following fields are disabled: `WITH_OPENCL, WITH_OPENCLAMDBLAS and WITH_OPENCLAMDFFT`

Now hit the configure button again! Cross your fingers that there is not a single red field anymore. In that case hit the generate button. If there still a red field and you cannot solve it yourself, open up a question at our [issues page](https://github.com/OpenCVBlueprints/OpenCVBlueprints/issues) and we will help you as soon as possible. If all went fine, you can close down CMake.

The following step is to actually build OpenCV for your system. This is done by

`sudo make -j 12 install`

Replace the number with the number of cores your system has. The install option will push everything into the standard directories after building, which is easier when compiling your programs later on. Wait till it finishes, again fingers crossed that it works without errors.

## Useful optimizations for OpenCV 3 to improve performance

OpenCV 3 supports several optimization technologies to improve the speed of its core algorithms.
* IPP or Integrated Performance Primitives is a technology for Intel processors to speed up processing. OpenCV 3 provides a partial free version of IPP, provided by Intel, which is enabled by default if you have an Intel processor. You can enable or disable it by setting the `WITH_IPP` option in CMake. However, if you have access to the [complete IPP package](https://software.intel.com/en-us/intel-ipp), then enable the option `WITH_IPP_A` which will give you an even larger performance increase.
* TBB or Thread Building Blocks is software that enables parallelism on multicore CPUs. Several functions of OpenCV have been heavily optimized using this technique. To get this performance speed advantage, you will have to [download](https://www.threadingbuildingblocks.org/) and install TBB on your system, and then enable the `WITH_TBB` option in CMake. This ensures you get the latest TBB version out there. If you are fine with an older version, then also enable the `BUILD_TBB` option in CMake, which will use the third-party package of TBB included in the OpenCV repository.

## Testing your OpenCV installation

Finally, you can open up a test.cpp file in your favorite editor and add the following lines of code:

```C++
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

int main( int argc, char* argv[] )
{
    // Load in test image
    Mat image = imread("/some/path/to/image", IMREAD_GRAYSCALE);
    imshow("original", image); waitKey(0);

    return 0;
}
```

Make sure that you replace the image path with an actual image path that exists! Then, configure your editor to include the build and installed OpenCV repositories. Under Ubuntu, you can do that by pointing your linker to <pre>`pkg-config opencv --libs`</pre> which will auto grab all the dependencies. Since all headers are installed into `/usr/local/lib/`, you do not need to explicitly tell that to the system, since Ubuntu can handle that itself.

**Enjoy your working OpenCV 3 installation!**
