/**********************************************************************************************
This code is part of the code supplied with the OpenCV Blueprints book.
It was written by Steven Puttemans, who can be contacted via steven.puttemans[at]kuleuven.be
***********************************************************************************************
Software for recognizing faces

USAGE
./face_recognition_LBPH -train train_faces.txt -test test_faces.txt

STRUCTURE PREFERRED
You will need a train and test faces file which contains a sum of all data
You will want name <space> label
***********************************************************************************************/

#include "opencv2/opencv.hpp"
#include "opencv2/face.hpp"
#include <fstream>

using namespace std;
using namespace cv;
using namespace cv::face;

int main( int argc, const char** argv )
{
    // If no parameters are give, then a usage template should be provided
    if(argc == 1){
	cout << "This software can be used for recognizing faces using the LBPH algorithm" << endl;
        cout << "USAGE ./face_recognition_LBPH -train train_faces.txt -test test_faces.txt" << endl;
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
	Ptr<LBPHFaceRecognizer> face_model = createLBPHFaceRecognizer();

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
	cerr << endl;
	cerr << "Total correct: " << total_correct << " / Total wrong: " << total_wrong << endl;
	cerr << endl;

    return 0;
}


