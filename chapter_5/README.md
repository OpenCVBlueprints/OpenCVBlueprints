# Chapter 5: Generic object detection for industrial applications.

In this subfolder you will find all software related to chapter 5 of OpenCVBlueprints.

## Samples folder
This folder contains all sample output that I generated for you
* The *HAAR* folder contains a visualisation of a HAAR wavelet based face detection model.
* The *LBP* folder contains a visualisation of a Local Binary Pattern based face detection model.
* The *rotation invariant detection* folder contains an output of applying this rotation invariant face detector to a given image.

## Source code folder
* ***average_dimensions.cpp***: defining the average dimensions for your model based on your training data
* ***count_samples.cpp***: counting the exact amount of training samples from a given annotation file
* ***detect_score.cpp***: perform object detection which outputs a certainty score
* ***detect_simple.cpp***: perform object detection without a certainty score outputted
* ***folder_listing.cpp***: list the contents of a folder in a txt file
* ***generate_negatives.cpp***: generate negative samples based on an annotation file of the positive samples
* ***object_annotation.cpp***: perform the annotation of objects using a simple OpenCV interface
* ***precision_recall.cpp***: calculate the precision recall on a given training and test dataset
* ***precision_recall.m***: visualise the precision recall on a given training and test dataset [MATLAB required]
* ***rotation_invariant_object_detection.cpp***: perform rotation invariant detection given a trained object model
* ***visualize_models.cpp***: perform a visualisation of the features and stages of a trained model
