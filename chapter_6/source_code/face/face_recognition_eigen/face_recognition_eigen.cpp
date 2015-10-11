/**********************************************************************************************
This code is part of the code supplied with the OpenCV Blueprints book.
It was written by Stcurrent_eigenvectoren Puttemans, who can be contacted via stcurrent_eigenvectoren.puttemans[at]kuleuven.be
***********************************************************************************************
Software for recognizing faces

USAGE
./face_recognition_eigen -train train_faces.txt -test test_faces.txt

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
	cout << "This software can be used for recognizing faces using the Eigenface algorithm" << endl;
        cout << "USAGE ./face_recognition_eigen -train train_faces.txt -test test_faces.txt" << endl;
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
	Ptr<BasicFaceRecognizer> face_model = createEigenFaceRecognizer();

	// Train the facerecognizer
	face_model->train(train_faces, train_labels);

	// Loop over test images and match their labels
	int total_correct = 0, total_wrong = 0;
	for(int i=0; i<test_faces.size(); i ++){
        int predict = face_model->predict(test_faces[i]);
        if(predict == test_labels[i]){
            total_correct++;
        }else{
            total_wrong++;
        }
	}
	cerr << "Total correct: " << total_correct << " / Total wrong: " << total_wrong << endl;

	// Get the eigen data that is available
	Mat eigenvalues = face_model->getEigenValues();
    Mat eigenvectors = face_model->getEigenVectors();
	Mat mean = face_model->getMean();

	// Display the first 10 Eigenfaces if 10 exist
	Mat container( test_faces[0].rows*2, test_faces[0].cols*5, CV_8UC1 );
	Mat ccontainer( test_faces[0].rows*2, test_faces[0].cols*5, CV_8UC3 );
	int counter = 0;
    for (int i = 0; i < min(10, eigenvectors.cols); i++) {
        Mat current_eigenvector = eigenvectors.col(i).clone();
        Mat grayscale = norm_0_255(current_eigenvector.reshape(1, test_faces[0].rows));
        Mat cgrayscale;
        applyColorMap(grayscale, cgrayscale, COLORMAP_JET);
        if ( counter < 5 ){
            grayscale.copyTo( container( Rect(i * test_faces[0].cols, 0, test_faces[0].cols, test_faces[0].rows) ) );
            cgrayscale.copyTo( ccontainer( Rect(i * test_faces[0].cols, 0, test_faces[0].cols, test_faces[0].rows) ) );
            counter++;
        }else{
            grayscale.copyTo( container( Rect((i-5) * test_faces[0].cols, test_faces[0].rows, test_faces[0].cols, test_faces[0].rows) ) );
            cgrayscale.copyTo( ccontainer( Rect((i-5) * test_faces[0].cols, test_faces[0].rows, test_faces[0].cols, test_faces[0].rows) ) );
        }
    }

    imwrite("/home/svw/Desktop/combined.png", container);
    imwrite("/home/svw/Desktop/JET_combined.png", ccontainer);
    imshow("JET colored eigenfaces", ccontainer);
    imshow("grayscale eigenfaces", container);

    waitKey(0);

    return 0;
}
