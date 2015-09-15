# Chapter 3: Recognizing facial expressions with machine learning.

## Build
```
mkdir build
cd build
cmake .. 
make
```

## Example
* Facial components:
```
./facial_components -src /Volumes/Data/Dataset/JAFFE/jaffe -dest /Volumes/Data/Dataset/JAFFE/result
```
* Feature Extraction
```
./feature_extraction -feature surf -src /Volumes/Data/Dataset/JAFFE/result -dest /Volumes/Data/Dataset/JAFFE/result
```
* Train
```
./train -algo svm -src /Volumes/Data/Dataset/JAFFE/result/surf_features.yml -dest /Volumes/Data/Dataset/JAFFE/result
```