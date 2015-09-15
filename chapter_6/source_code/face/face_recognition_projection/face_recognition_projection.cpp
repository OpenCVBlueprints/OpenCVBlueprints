/**********************************************************************************************
This code is part of the code supplied with the OpenCV Blueprints book.
It was written by Steven Puttemans, who can be contacted via steven.puttemans[at]kuleuven.be
***********************************************************************************************
Software for projecting back recognized faces on top of their dimensions

USAGE
./face_recognition_projection -train train_faces.txt -test test_faces.txt

STRUCTURE PREFERRED
You will need a train and test faces file which contains a sum of all data
You will want name <space> label

Based on the OpenCV tutorial http://docs.opencv.org/3.0-last-rst/modules/face/doc/facerec_tutorial.html
But changed until it works with the latest OpenCV3 interface where stuff changed a lot.
***********************************************************************************************/

#include "opencv2/opencv.hpp"
#include "opencv2/face.hpp"
#include <fstream>

using namespace std;
using namespace cv;
using namespace cv::face;

static Mat norm_0_255(InputArray _src) {
    Mat src = _src.getMat();
    // Create and return normalized image:
    Mat dst;
    switch(src.channels()) {
    case 1:
        cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
        break;
    case 3:
        cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
        break;
    default:
        src.copyTo(dst);
        break;
    }
    return dst;
}

int main( int argc, const char** argv )
{
    // If no parameters are give, then a usage template should be provided
    if(argc == 1){
	cout << "This is software for for projecting back recognized faces on top of their dimensions." << endl;
        cout << "USAGE ./face_recognition_projection -train train_faces.txt -test test_faces.txt" << endl;
     	   return 0;
    }

    // Read in the input arguments
    string train_faces_file = "", test_faces_file = "";
    for(int i = 1; i < argc; ++i )
    {
        if( !strcmp( argv[i], "-train" ) )
        {
            train_faces_file = argv[++i];
        }else if( !strcmp( argv[i], "-test" ) )
        {
            test_faces_file = argv[++i];
        }
	}

	// Process the data from the files
	ifstream train(train_faces_file.c_str());
	ifstream test(test_faces_file.c_str());

	// Read all the training data
	string current_line;
	vector<Mat> train_faces;
	vector<int> train_labels;
 	while( getline(train, current_line) ){
        vector<string> line_elements;
        stringstream temp_stream (current_line);
        string single_item;
        while ( getline(temp_stream, single_item, ' ') ) {
            line_elements.push_back(single_item);
        }

        // Process the data recovered
        Mat face = imread(line_elements[0], CV_LOAD_IMAGE_GRAYSCALE);
        int label = atoi(line_elements[1].c_str());
        train_faces.push_back(face);
        train_labels.push_back(label);
    }

    // Read all the testing data
    string current_line2;
	vector<Mat> test_faces;
	vector<int> test_labels;
 	while( getline(test, current_line2) ){
        vector<string> line_elements;
        stringstream temp_stream (current_line2);
        string single_item;
        while ( getline(temp_stream, single_item, ' ') ) {
            line_elements.push_back(single_item);
        }

        // Process the data recovered
        Mat face = imread(line_elements[0], CV_LOAD_IMAGE_GRAYSCALE);
        int label = atoi(line_elements[1].c_str());
        test_faces.push_back(face);
        test_labels.push_back(label);
    }

	// Open up a facerecognizer
	// Both a model for eigenfaces and for fisherfaces to see difference
	Ptr<BasicFaceRecognizer> face_model_eigen = createEigenFaceRecognizer();
	Ptr<BasicFaceRecognizer> face_model_fisher = createFisherFaceRecognizer();

	// Train the facerecognizer
	face_model_eigen->train(train_faces, train_labels);
	face_model_fisher->train(train_faces, train_labels);

	// Get the eigen data that is available for both models
	// This will allow us to do a reprojection to investigate the reconstructive power
    Mat eigenvectors_E = face_model_eigen->getEigenVectors();
    Mat mean_E = face_model_eigen->getMean();

    Mat eigenvectors_F = face_model_fisher->getEigenVectors();
    Mat mean_F = face_model_fisher->getMean();

    // ----------------------------------------------------------------------------------------------------------
    // REPROJECTION OF THE EIGENSPACE
    // For eigenfaces, we know that the AT&T database needs 300 eigenvectors for a good reconstruction of a face
    // Therefore we plot the in between steps while adding 25 vectors at a time
    // This will yield 12 results --> lets put them in a 2x6 matrix
    // ----------------------------------------------------------------------------------------------------------
    Mat container_E (test_faces[0].rows * 2, test_faces[0].cols * 6, CV_8UC1);
    int counter = 0;
    for(int amount_eigenvectors=25; amount_eigenvectors<=300; amount_eigenvectors+=25){
        // Perform the projection and reconstruction
        Mat needed_eigenvectors = Mat(eigenvectors_E, Range::all(), Range(0, amount_eigenvectors));
        Mat projection = cv::LDA::subspaceProject(needed_eigenvectors, mean_E, train_faces[0].reshape(1,1));
        Mat reconstruction = cv::LDA::subspaceReconstruct(needed_eigenvectors, mean_E, projection);

        // Now reshape it all
        Mat grayscale = norm_0_255(reconstruction.reshape(1, test_faces[0].rows));

        // Copy to the correct image container position
        if(counter < 6){
            grayscale.copyTo( container_E( Rect(counter * test_faces[0].cols, 0, test_faces[0].cols, test_faces[0].rows) ) );
            counter++;
        }else{
            grayscale.copyTo( container_E( Rect((counter-6) * test_faces[0].cols, test_faces[0].rows, test_faces[0].cols, test_faces[0].rows) ) );
            counter++;
        }
    }

    // Show the end result
    imshow("reprojection of first face", container_E);
    imwrite("/data/reprojection_eigenfaces.png", container_E);

    // ----------------------------------------------------------------------------------------------------------
    // REPROJECTION OF THE FISHERSPACE
    // Lets project the first 12 Fisherface dimensions - which has less dimensions than Eigenfaces
    // And the reconstruction of the first individual on top of that
    // ----------------------------------------------------------------------------------------------------------
    Mat container_F (test_faces[0].rows * 2, test_faces[0].cols * 6, CV_8UC1);
    counter = 0;
    for(int amount_eigenvectors=0; amount_eigenvectors<min(12, eigenvectors_F.cols); amount_eigenvectors++){
        // Perform the projection and reconstruction
        Mat current_fisherface = eigenvectors_F.col(amount_eigenvectors);
        Mat projection = cv::LDA::subspaceProject(current_fisherface, mean_F, train_faces[0].reshape(1,1));
        Mat reconstruction = cv::LDA::subspaceReconstruct(current_fisherface, mean_F, projection);

        // Now reshape it all
        Mat grayscale = norm_0_255(reconstruction.reshape(1, train_faces[0].rows));

        // Copy to the correct image container position
        if(counter < 6){
            grayscale.copyTo( container_F( Rect(counter * test_faces[0].cols, 0, test_faces[0].cols, test_faces[0].rows) ) );
            counter++;
        }else{
            grayscale.copyTo( container_F( Rect((counter-6) * test_faces[0].cols, test_faces[0].rows, test_faces[0].cols, test_faces[0].rows) ) );
            counter++;
        }
    }

    // Show the end result
    imshow("reprojection of first face", container_F);
    imwrite("/data/reprojection_fisherfaces.png", container_F);
    waitKey(0);

    return 0;
}

