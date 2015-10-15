# OpenCV 3 installation guide for Mac

On Mac, there are several ways to configure, build, and install OpenCV 3. For example, you could choose one of the following options:

1. Use the MacPorts package manager.
2. Use the Homebrew package manager.
3. Install dependencies and use OpenCV's CMake build system, without the help of a package manager.

This tutorial will focus on the use of MacPorts, which has well-supported, configurable packages for OpenCV, its required dependencies, and many of its optional dependencies.

## Installing MacPorts

MacPorts is a third-party, open-source tool, but it depends on Xcode and the Xcode Command Line Tools. To install MacPorts and its dependencies, follow the MacPorts [installation guide](https://www.macports.org/install.php).

## Installing a Port

After installing MacPorts, we can use Terminal commands to install packages, also known as **ports**. A port may have **variants**, which are configuration options. To install a port with specified variants, we can use a command in the following format:

    $ sudo port install <port_name> +<variant_0> +<variant_1> ...

MacPorts also takes care of installing any dependencies of the port and the selected variants.

To search for ports by name, run a command such as the following:

    $ port list *cv*

The preceding command will list all ports with "cv" in the name, notably the `opencv` port. To list this port's variants, we can run the following command:

    $ port variants opencv

This will produce output similar to the following:

```
opencv has the variants:
   dc1394: Use libdc1394 for FireWire camera. Breaks compatibility with Apple
           iSight FireWire camera.
   debug: Enable debug binaries
   eigen: Enable eigen support.
   java: Add Java bindings.
   opencl: Enable OpenCL support.
   openni: Enable OpenNI support.
   python27: Add Python 2.7 bindings.
     * conflicts with python34
   python34: Add Python 3.4 bindings.
     * conflicts with python27
   qt4: Build with Qt4 Backend support.
     * conflicts with qt5
   qt5: Build with Qt5 Backend support.
     * conflicts with qt4
   tbb: Use Intel TBB.
   universal: Build for multiple architectures
   vtk: Include VTK support.
```

A few variants from this list are relevant to OpenCV Blueprints. Some chapters use the Python bindings (your choice of Python 2.7 or 3.4). Also, some chapters use functionality that is greatly accelerated by OpenCV's TBB optimizations. (Intel Thread Building Blocks, or TBB, is a library for CPU multiprocessing.) Notably, these TBB optimizations are beneficial to LBP cascade classification. We will also enable optimizations that use Eigen (for CPU vector processing) and OpenCL (for GPU multiprocessing).

To install OpenCV with optimizations and Python 2.7 bindings, run the following command:

    $ sudo port install opencv +python27 +eigen +tbb +opencl

Alternatively, to install OpenCV with optimizations and Python 3.4 bindings, run the following command:

    $ sudo port install opencv +python34 +eigen +tbb +opencl

Dependencies, including Python, will also be installed.

## Running a Script with MacPorts Python

Note that MacPorts uses its own installation of Python (hereafter, "MacPorts Python"), rather than the Python installation that is part of the Mac system (hereafter, "Apple Python"). This is good because the system relies on the behavior of Apple Python and its libraries, so we should avoid modifying them. Moreover, system updates can wipe out any custom Apple Python libraries that the user may have installed.

Apple Python is located at /usr/bin/python2.6 or /usr/bin/python2.7. MacPorts Python is located at /opt/local/bin/python2.7 or /opt/local/bin/python3.4. When we run a Python script that uses OpenCV, we need to use MacPorts Python. We could fully specify the path to MacPorts Python, like this:

    $ /opt/local/bin/python2.7 my_opencv_sample.py

Alternatively, we could make MacPorts Python the default `python` executable. To make MacPorts Python 2.7 the default, we would use the following command:

    $ sudo port select python python27

Similarly, to make MacPorts Python 3.4 the default, we would use the following command: 

    $ sudo port select python python34

Now, we could run a script with MacPorts Python more concisely, like this:

    $ python my_opencv_sample.py

To revert to Apple Python 2.7 as the default, we would use this command:

    $ sudo port select python python27-apple

And so on for other versions. To see a list of the available Python versions, just run this:

    $ sudo port select python

## Making a Custom Port

As we have seen, the default MacPorts repository provides a nice OpenCV package that supports many configurations. However, as an advanced user, you might find that you need to customize OpenCV even further. To do so, you can copy and modify the set of files that define the OpenCV package.

TODO

Now, edit the file `/opt/local/etc/macports/sources.conf`. (If MacPorts is installed in a non-default location, this file's location will differ accordingly.) Find the line `` and, just above it, add the following line:

    file://<local_repository>

For example, if `<local_repository>` is `/Users/Me/Portfiles`, add this line:

    file:///Users/Me/Portfiles

Note that the third slash in `file:///` is required in order to refer to the root directory.

Save `sources.conf`. Now, MacPorts will search for Portfiles in `<local_repository>` first and the default online repository second.

TODO

    $ mkdir <local_repository>/graphics/
    $ cp /opt/local/var/macports/sources/rsync.macports.org/release/ports/graphics/opencv <local_repository>/graphics

TODO
