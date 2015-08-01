# Chapter 5: Generic object detection for industrial applications.

In this subfolder you will find all software related to chapter 5 of OpenCVBlueprints.

## Samples folder
This folder contains all sample output that I generated for you
* The *HAAR* folder contains a visualisation of a HAAR wavelet based face detection model.
* The *LBP* folder contains a visualisation of a Local Binary Pattern based face detection model.
* The *rotation invariant detection* folder contains an output of applying this rotation invariant face detector to a given image.

## Source code folder
* ***average_dimensions***: defining the average dimensions for your model based on your training data
* ***count_samples***: counting the exact amount of training samples from a given annotation file
* ***CPU_GPU_comparison***: compare CPU and GPU processing speeds on a given input image
* ***detect_score***: perform object detection which outputs a certainty score
* ***detect_simple***: perform object detection without a certainty score outputted
* ***folder_listing***: list the contents of a folder in a txt file
* ***generate_negatives***: generate negative samples based on an annotation file of the positive samples
* ***object_annotation***: perform the annotation of objects using a simple OpenCV interface
* ***precision_recall***: calculate the precision recall on a given training and test dataset
* ***precision_recall.m***: visualise the precision recall on a given training and test dataset [MATLAB required]
* ***rotation_invariant_object_detection***: perform rotation invariant detection given a trained object model
* ***visualize_models***: perform a visualisation of the features and stages of a trained model
