# OpenCV 3 installation guide for Windows

In order to get OpenCV 3 up and running under any Windows operating system, you should follow the guidelines beneath.

## Easiest way: using prebuilt libraries

The most easiest way to use OpenCV 3 on Windows is by using prebuilt library packages provided by the OpenCV team themselves. You can find those downloads at [the official download page](http://opencv.org/downloads.html) and at the [sourgeforge OpenCV download page](http://sourceforge.net/projects/opencvlibrary/files/opencv-win/). The latter has all the different stable versions up to OpenCV 3.0.0.

Once you have downloaded the package, in our case the 3.0.0 download, you need to apply the following steps
 1. Make sure you have admin rights. Unpack the self-extracting archive into a folder of your chosing, for example `D:\OpenCV\`. But this can be any path on your system.
 2. Make sure that you set the OpenCV environment variable correctly. This one will ensure that your system knows where OpenCV 3 is installed. To do this you:
  * Start by opening a command line interface.
  * Type in the command that corresponds to your system configuration.
  ```CPP
   x86 bit & Visual Studio 2010: `setx -m OPENCV_DIR D:\OpenCV\Build\x86\vc10`
   x64 bit & Visual Studio 2010: `setx -m OPENCV_DIR D:\OpenCV\Build\x64\vc10`
   x86 bit & Visual Studio 2012: `setx -m OPENCV_DIR D:\OpenCV\Build\x86\vc11`
   x64 bit & Visual Studio 2012: `setx -m OPENCV_DIR D:\OpenCV\Build\x64\vc11`
   x86 bit & Visual Studio 2013: `setx -m OPENCV_DIR D:\OpenCV\Build\x86\vc12`
   x64 bit & Visual Studio 2013: `setx -m OPENCV_DIR D:\OpenCV\Build\x64\vc12`
   x86 bit & Visual Studio 2015: `setx -m OPENCV_DIR D:\OpenCV\Build\x86\vc13`
   x64 bit & Visual Studio 2015: `setx -m OPENCV_DIR D:\OpenCV\Build\x64\vc13`
  ```
 3. Now make sure that your system can find the path of OpenCV 3 dynamic libraries (which are built by default). 
  * For Windows Vista & Windows 7: go to `Desktop > Computer > Right Click > Properties > Advanced System Settings > Advanced Tab > Environment Variables` and edith the `PATH` variable by adding `;%OPENCV_DIR%\bin` at the end of the line.
  * For Windows 8 & Windows 10: go to very bottom left corner and right-click to get the `Power User Task Menu`. Then click `System > Advanced System Settings > Advanced Tab > Environment Variables` and edith the `PATH` variable by adding `;%OPENCV_DIR%\bin` at the end of the line.

Your system is now up and running for configuring Visual Studio and running your first example. This can be found at the bottom of the page.

*Remark: Keep in mind that the prebuilt libraries of OpenCV do _NOT_ have CUDA/OPENCL support, nor are they optimized with parallel threading libraries like TBB or IPP. They only support the basic OpenCV library and thus have no access to the OpenCV contributed modules. To have all these advantages or extra functionalities (which are needed for some chapters of the book), you will need to go to the advanced setup and built OpenCV 3 yourself.*

## A bit more advanced: building OpenCV 3 yourself on Windows

Since you will probably want to be able to run all the code pieces of this book assigned with all chapters, you will choose for this option. Building OpenCV yourself on Windows is a bit more cumbersome, but this manual will mostlikely help to make your life easier.

### Installing dependencies

Since Windows requires you to install every single dependency manually, let us start with that.
* 

## Running a sample using Visual Studio environment
