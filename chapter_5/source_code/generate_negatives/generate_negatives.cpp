/****************************************************************************************************
This code is part of the code supplied with the OpenCV Blueprints book.
It was written by Steven Puttemans, who can be contacted via steven.puttemans[at]kuleuven.be

License can be found at https://github.com/OpenCVBlueprints/OpenCVBlueprints/blob/master/license.txt
*****************************************************************************************************
This is software for generating negative windows based on your positive dataset.

USAGE:
./generate_negatives -annotations <file.txt> -output <folder>
***********************************************************************************************/

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <fstream>

using namespace std;
using namespace cv;

int main( int argc, const char** argv )
{
    // If no parameters are give, then a usage template should be provided
    if(argc == 1){
	cout << "This is software for generating negative windows based on your positive dataset." << endl;
        cout << "./generate_negatives -annotations <file.txt> -output <folder>" << endl;
        return 0;
    }    

    // Retrieve the input parameters supplied as arguments to the function
    string annotation_file;
    string storage_folder;
    for(int i = 1; i < argc; ++i )
    {
        if( !strcmp( argv[i], "-annotations" ) )
        {
            annotation_file = argv[++i];
        }else if( !strcmp( argv[i], "-output" ) )
        {
            storage_folder = argv[++i];
        }
    }

    cerr<< annotation_file << " " << storage_folder << endl;

	// Read through the txt file and process the given information
	// Each annotation needs to be replaced by a blackend region
	ifstream my_annotations ( annotation_file.c_str() );
    string current_line;
    int counter = 0;
	while(!my_annotations.eof()){
		// Read the first line and check if it has a value, else break out of loop
		getline(my_annotations, current_line, '\n');
		if (current_line == ""){
		    break;
        }

		// Split the string into parameters
		vector<string> data;
		string element = "";
		stringstream temp_stream( current_line.c_str() );
		while(getline(temp_stream, element,' ')){
			data.push_back(element);
		}

		// Read in the original image
		Mat img = imread( data[0] );

		// Retrieve rectangle bounding box parameters
		// Parameterposition ignores the location and amount of detections and is then used for looping over bounding boxes
		int amount_detections = atoi(data[1].c_str());
		int x = 0, y = 0, width = 0, height = 0;
		for(int i = 0; i < amount_detections; i ++){
			// Grab each bounding box from annotations
			x = atoi(data[(i*4)+2].c_str());
			y = atoi(data[(i*4)+3].c_str());
			width = atoi(data[(i*4)+4].c_str());
			height = atoi(data[(i*4)+5].c_str());

			// Add a black filled rectangle to the image based on the information
			rectangle(img, Point(x,y), Point(x+width,y+height), Scalar(0,0,0), CV_FILLED);
		}

		// After all rectangles have been added, save the result
		stringstream out;
		stringstream date;

		// Create a unique blueprint for the name!
		time_t t = time(0);
		struct tm * now = localtime( & t );
		date << (now->tm_year + 1900) << (now->tm_mon + 1) << now->tm_mday << now->tm_hour << now->tm_min << now->tm_sec;

		out << storage_folder << "negative_" << date.str() << "_" << counter << ".png";
		imwrite(out.str(), img);

		// Add image index
		counter++;
	}
}
