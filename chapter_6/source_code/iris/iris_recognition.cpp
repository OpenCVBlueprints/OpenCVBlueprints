/**********************************************************************************************
This code is part of the code supplied with the OpenCV Blueprints book.
It was written by Steven Puttemans, who can be contacted via steven.puttemans[at]kuleuven.be
***********************************************************************************************
Software for recognizing iris images over a database

USAGE
./iris_recognition.cpp

***********************************************************************************************/

#include "opencv2/opencv.hpp"
#include "opencv2/face.hpp"
#include <fstream>

using namespace std;
using namespace cv;
using namespace cv::face;

int main( int argc, const char** argv )
{
    // Read in the file with the images and labels
    // File should be in format name<space>label<enter>
    ifstream training("/data/iris_wrapped.txt");
    ifstream testing("/data/iris_wrapped_test.txt");

    // Read all the training data
	string current_line;
	vector<Mat> train_iris;
	vector<int> train_labels;
 	while( getline(training, current_line) ){
        vector<string> line_elements;
        stringstream temp_stream (current_line);
        string single_item;
        while ( getline(temp_stream, single_item, ' ') ) {
            line_elements.push_back(single_item);
        }
        // Process the data recovered
        Mat iris = imread(line_elements[0], CV_LOAD_IMAGE_GRAYSCALE);
        int label = atoi(line_elements[1].c_str());
        train_iris.push_back(iris);
        train_labels.push_back(label);
    }

    // Read all the testing data
    current_line;
	vector<Mat> test_iris;
	vector<int> test_labels;
 	while( getline(testing, current_line) ){
        vector<string> line_elements;
        stringstream temp_stream (current_line);
        string single_item;
        while ( getline(temp_stream, single_item, ' ') ) {
            line_elements.push_back(single_item);
        }
        // Process the data recovered
        Mat iris = imread(line_elements[0], CV_LOAD_IMAGE_GRAYSCALE);
        int label = atoi(line_elements[1].c_str());
        test_iris.push_back(iris);
        test_labels.push_back(label);
    }

    // Try using the facerecognizer interface for these irises
    // Choice of using LBPH --> local and good for finding texture
	Ptr<LBPHFaceRecognizer> iris_model = createLBPHFaceRecognizer();
    // Train the facerecognizer
	iris_model->train(train_iris, train_labels);
	// Loop over test images and match their labels
	int total_correct = 0, total_wrong = 0;
	for(int i=0; i<test_iris.size(); i ++){
        int predict = iris_model->predict(test_iris[i]);
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


