# OpenCV 3 installation guide for Windows

In order to get OpenCV 3 up and running under any Windows operating system, you should follow the guidelines below.

## Easiest way: using prebuilt libraries

The easiest way to install OpenCV 3 on Windows is by using prebuilt library packages provided by the OpenCV team. You can find those downloads at [the official download page](http://opencv.org/downloads.html) and at the [sourgeforge OpenCV download page](http://sourceforge.net/projects/opencvlibrary/files/opencv-win/). The latter has all the different stable versions up to OpenCV 3.0.0.

Once you have downloaded the package, in our case the 3.0.0 download, you need to apply the following steps
 1. Make sure you have admin rights. Unpack the self-extracting archive into a folder of your choosing, for example `C:\OpenCV\`. But this can be any path on your system.
 2. Make sure that you set the OpenCV environment variable correctly. This one will ensure that your system knows where OpenCV 3 is installed. To do this:
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

*Remark: Keep in mind that the prebuilt libraries of OpenCV do _NOT_ have CUDA/OPENCL support, nor are they optimized with the TBB parallel threading library. They only support the basic OpenCV library and thus have no access to the OpenCV contributed modules. To have all these advantages and extra functionality (as required in some chapters of the book), you will need to go to the advanced setup and built OpenCV 3 yourself.*

## A bit more advanced: building OpenCV 3 yourself on Windows

If you want to be able to run all the code in all chapters of this book, you will choose this option. Building OpenCV yourself on Windows is a bit more cumbersome, but this manual will most likely make your life easier.

### Installing dependencies

Since Windows requires you to install every single dependency manually, let us start with that.

TODO

### Obtaining the source code of OpenCV and the contributed modules

Let's suppose that we want to put the source code of OpenCV and the contributed modules in subdirectories of `C:\OpenCV`. In Command Prompt or Git Bash, use the following commands to create the parent directory and to get the latest source code from GitHub:

```
mkdir C:\OpenCV
cd C:\OpenCV
git clone https://github.com/Itseez/opencv.git opencv
git clone https://github.com/Itseez/opencv_contrib.git opencv_contrib
```

Now, we should have the following directory structure:

```
C:
  OpenCV
    opencv
    opencv_contrib
```

Alternatively, you may modify the preceding commands to check out the source code to other paths instead.

### Creating the Visual Studio solution with CMake

We can create either a 32-bit or a 64-bit OpenCV library project. The relevant commands differ slightly, as indicated in the following subsections.

#### 32-bit

In Command Prompt, run commands similar to the following (but change the paths if they differ in your setup):

```
mkdir C:\OpenCV\x86
cd C:\OpenCV\x86
cmake -DCMAKE_BUILD_TYPE=RELEASE ^
-DWITH_OPENGL=ON ^
-DWITH_TBB=ON ^
-DTBB_INCLUDE_DIR:PATH=C:/TBB/include ^
-DTBB_LIB_DIR:PATH=C:/TBB/lib/ia32/vc12 ^
-DPYTHON2_LIBRARY=C:/Python27/libs/python27.lib ^
-DPYTHON2_INCLUDE_DIR=C:/Python27/include ^
-DOPENCV_EXTRA_MODULES_PATH=C:/OpenCV/src/opencv_contrib ^
-G "Visual Studio 12 2013" ^
C:/OpenCV/src/opencv
```

This will create a Visual Studio 2013 solution at `C:\OpenCV\x86\OpenCV.sln`.

#### 64-bit

In Command Prompt, run commands similar to the following (but change the paths if they differ in your setup):

```
mkdir C:\OpenCV\x64
cd C:\OpenCV\x64
cmake -DCMAKE_BUILD_TYPE=RELEASE ^
-DWITH_OPENGL=ON ^
-DWITH_TBB=ON ^
-DTBB_INCLUDE_DIR:PATH=C:/TBB/include ^
-DTBB_LIB_DIR:PATH=C:/TBB/lib/intel64/vc12 ^
-DPYTHON2_LIBRARY=C:/Python27/libs/python27.lib ^
-DPYTHON2_INCLUDE_DIR=C:/Python27/include ^
-DOPENCV_EXTRA_MODULES_PATH=C:/OpenCV/src/opencv_contrib ^
-G "Visual Studio 12 2013 Win64" ^
C:/OpenCV/src/opencv
```

This will create a Visual Studio 2013 solution at `C:\OpenCV\x64\OpenCV.sln`.

### Building the Visual Studio solution

TODO

### Installing the build and configuring the environment

TODO

## Building and running a sample using Visual Studio

TODO
