# OpenCV 3 installation guide for Mac OS X based systems

## With MacPorts

On Mac, the MacPorts package manager provides an easy way to build and install OpenCV 3, with several configuration options. MacPorts is a third-party, open-source tool and it depends on Xcode and the Xcode Command Line Tools. To install MacPorts and its dependencies, following the MacPorts [installation guide](https://www.macports.org/install.php).

After installing MacPorts, we can use Terminal commands to install packages, also known as **ports**. A port may have **variants**, which are configuration options. To install a port with specified variants, we can use a command in the following format:

    $ sudo port install <package> +<variant_0> +<variant_1> ...

MacPorts also takes care of installing any dependencies of the port and the selected variants.

To search for ports by name, run a command such as the following:

    $ port list *cv*

The preceding command will list all ports with "cv" in the name, notably the `opencv` port. To list this port's variants, we can run the following command:

    $ port variants opencv

TODO

    $ sudo port install opencv +python27 +eigen +tbb +opencl

TODO

    $ sudo port install opencv +python34 +eigen +tbb +opencl

TODO

    $ sudo port select python python27

TODO

    $ sudo port select python python34

TODO
