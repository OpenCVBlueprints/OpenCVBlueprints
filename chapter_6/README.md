# Chapter 6: Efficient person identification.

In this subfolder you will find all software related to chapter 6 of OpenCVBlueprints.

## Datasets folder
This folder contains all data that was used to test the generated code with the chapter
* The *faces* folder contains the open access part of the AT&T face database.
* The *fingerprints* folder contains the database used for fingerprint recognition.
* The *iris* folder contains a sample of the publicly available CASIA iris database [ACCESS needs to be requested].

## Source code folder
### Face recognition
* ***face_detection.cpp***: detecting any given face in an image using a standard OpenCV LBP face model
* ***face_normalisation.cpp***: align images of a face based on the OpenCV haar eye detection model
* ***face_recognition_LBPH.cpp***: face recognition interface using the LBPH technique
* ***face_recognition_eigen.cpp***: face recognition interface using the Eigenface technique
* ***face_recognition_fisher.cpp***: face recognition interface using the Fisherface technique
* ***face_recognition_projection.cpp***: software that projects a given face on the trained model and its dimensions
* ***face_to_ELBP.cpp***: software for making an ELBP representation of any given face image

### Fingerprint recognition
* ***fingerprint_process.cpp***: software for encoding and matching fingerprint scans

### Iris recognition
* ***iris_processing.cpp***: software for unwrapping any given iris image to an encodable form
* ***iris_processing_batch.cpp***: same unwrapping software for a batch of files
* ***iris_recognition.cpp***: matching unwrapped iris images against a database of iris images



