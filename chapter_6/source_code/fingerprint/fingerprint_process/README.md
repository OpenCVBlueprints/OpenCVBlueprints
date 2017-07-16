#Steps 

```
$ mkdir build && cd build 
$ cmake ../
$ make -j5
```
You should run into a message error as `xfeatures2d` will not be found. 
All new modules are published into a new repository `https://github.com/opencv/opencv_contrib`. 
You need to get `xfeatures2d` from `opencv_contrib`. 
You can either add the lib manually to your opencv installation folder or you can follow the steps as mentioned in the guide: 

```
$ cd build
$ cmake -DOPENCV_EXTRA_MODULES_PATH=<opencv_contrib>/modules <opencv_source_directory>
$ make -j5
$ ./fingerprint_process
```

`./fingerprint_process` doesn't accept a template as argument at the moment. Don't forget to copy the dataset into the folder: `chapter_6/source_code/fingerprint/fingerprint_process/data/fingerprints/` like `chapter_6/source_code/fingerprint/fingerprint_process/data/fingerprints/101_1.tif`
