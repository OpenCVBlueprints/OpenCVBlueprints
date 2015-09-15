/**********************************************************************************************
This code is part of the code supplied with the OpenCV Blueprints book.
It was written by Steven Puttemans, who can be contacted via steven.puttemans[at]kuleuven.be
***********************************************************************************************
Software for detecting faces inside a webcam image and storing them to the disk

USAGE
./face_detection -model face_model.xml
***********************************************************************************************/

#include "opencv2/opencv.hpp"
#include <fstream>

using namespace std;
using namespace cv;

int main( int argc, const char** argv )
{
    // If no parameters are give, then a usage template should be provided
    if(argc == 1){
	cout << "This software can be used for detecting faces inside a webcam image and storing them to the disk" << endl;
        cout << "USAGE ./face_detection -model face_model.xml" << endl;
        return 0;
    }

    // Read in the input arguments
    string model = "";
    for(int i = 1; i < argc; ++i )
    {
        if( !strcmp( argv[i], "-model" ) )
        {
            model = argv[++i];
        }
    }

    // Initialize the interface for webcam grabbing
    // Grab a single frame
    Mat image, image_gray, image_hist;
    VideoCapture webcam(0);
    webcam >> image;

    // Pre-process the grabbed frame
    resize(image, image, Size(640,360));
    cvtColor(image, image_gray, COLOR_BGR2GRAY);
    equalizeHist(image_gray, image_hist);

    // Perform the face detection
    CascadeClassifier cascade(model);
    vector<Rect> faces;
    cascade.detectMultiScale(image_hist, faces, 1.1, 3);

    // Visualize the data
    Mat clone_image = image.clone();
    for (size_t i=0; i < faces.size(); i++){
	rectangle(clone_image, faces[i], Scalar(0,0,255), 2);
    }
    imshow("all face detections on original image", clone_image);
    waitKey(0);

    // Crop the face regions for further processing
    // Here we will store them on a system folder for face collection
    int counter = 0;
    for(size_t i=0; i<faces.size(); i++){
	stringstream output;
	Rect current_face = faces[i];
	Mat face_region = image( current_face ).clone();
	output << "/data/faces/" << counter << ".png";
	imwrite(output.str(), face_region);
	i++;
    }

    return 0;
}

