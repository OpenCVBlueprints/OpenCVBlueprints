/**********************************************************************************************
This code is part of the code supplied with the OpenCV Blueprints book.
It was written by Steven Puttemans, who can be contacted via steven.puttemans[at]kuleuven.be
***********************************************************************************************
Software for detecting eyes inside face regions

USAGE
./face_normalisation -folder face_images/ -model_eye eye_model.xml -output aligned_faces/
***********************************************************************************************/

#include "opencv2/opencv.hpp"
#include <fstream>

using namespace std;
using namespace cv;

int main( int argc, const char** argv )
{
    // If no parameters are give, then a usage template should be provided
    if(argc == 1){
	cout << "This software can be used for etecting eyes inside face regions" << endl;
        cout << "USAGE ./face_normalisation -folder face_images/ -model_eye eye_model.xml -output aligned_faces/" << endl;
        return 0;
    }

    // Read in the input arguments
    string folder = "", eyemodel = "", output = "";
    for(int i = 1; i < argc; ++i )
    {
        if( !strcmp( argv[i], "-folder" ) )
        {
            folder = argv[++i];
        }else if( !strcmp( argv[i], "-model_eye" ) )
        {
            eyemodel = argv[++i];
        }else if( !strcmp( argv[i], "-output" ) )
        {
            output = argv[++i];
        }
	}

	// Read the contents of the folder so that we know which images to process
	vector<String> filenames;
	glob(folder, filenames);

	// Run over samples and process the data
	CascadeClassifier eye( eyemodel );
	for (size_t i=0; i<filenames.size(); i++){
		// Read and pre-process image
		Mat image, image_gray, image_hist;
		image = imread(filenames[i]);
		cvtColor(image, image_gray, COLOR_BGR2GRAY);
		equalizeHist(image_gray, image_hist);

		// Perform the eye detection
		vector<Rect> eyes_found;
		eye.detectMultiScale(image_hist, eyes_found, 1.1, 3);

		// Now let us assume only two eyes (both eyes and no FP) are found
		// Check if at least 2 eyes are found
		if (eyes_found.size() < 2){
			cerr << "The current image did not yield 2 eye detections";
			break;
		}

		// If so, continue and for the heck of it, consider only the 2 first hits to be eyes
		// You could do something smarter here, like asking for the confidence score and then keeping
		// only the 2 best hits, instead of just the first 2

		// Calculate the transformation
		double angle = atan( (eyes_found[0].y - eyes_found[1].y) / (eyes_found[0].x - eyes_found[1].x) );
		Point2f pt(image.cols/2, image.rows/2);
		Mat rotation = getRotationMatrix2D(pt, angle, 1.0);
		Mat rotated_face;

		// Apply the rotation
		warpAffine(image, rotated_face, rotation, Size(image.cols, image.rows));

		// Store the image
		stringstream location;
		location << output << i << ".png";
		imwrite(location.str(), rotated_face);
	}

    return 0;
}

