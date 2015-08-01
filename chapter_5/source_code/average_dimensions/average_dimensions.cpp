/****************************************************************************************************
This code is part of the code supplied with the OpenCV Blueprints book.
It was written by Steven Puttemans, who can be contacted via steven.puttemans[at]kuleuven.be

License can be found at https://github.com/OpenCVBlueprints/OpenCVBlueprints/blob/master/license.txt
*****************************************************************************************************
Software for defining average dimensions of a given set of annotations

USAGE
./average_dimensions -annotations annotations.txt
*****************************************************************************************************/

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <stdlib.h>

using namespace std;

int main( int argc, const char** argv )
{
    // If no parameters are give, then a usage template should be provided
    if(argc == 1){
	cout << "Software for defining average dimensions of a given set of annotations" << endl;
        cout << "USAGE ./average_dimensions -annotations annotations.txt" << endl;
     	   return 0;
    }

    // Read in the parameters supplied
    string annotation_file;
    for(int i = 1; i < argc; ++i )
    {
        if( !strcmp( argv[i], "-annotations" ) )
        {
            annotation_file = argv[++i];
        }
    }

	ifstream input ( annotation_file.c_str() );
	string current_line;
	vector<int> w_dimensions;
	vector<int> h_dimensions;

	while ( getline(input, current_line) ){
	    // A complete line is read, now check how many detections happened
	    // First split the string into elements
        vector<string> line_elements;
        stringstream temp (current_line);
        string item;
        while ( getline(temp, item, ' ') ) {
            line_elements.push_back(item);
        }

        // Now use the elements to grab all w and h values
        int number_rectangles = atoi( line_elements[1].c_str() );
        for (int i = 0; i < number_rectangles; i++){
            w_dimensions.push_back( atoi( line_elements[(i*4)+4].c_str() ) );
            h_dimensions.push_back( atoi( line_elements[(i*4)+5].c_str() ) );
        }
    }
    input.close();

	// Calculate the average value of the vectors
    int sum_w = 0, sum_h = 0;
    for (int i = 0; i < (int)w_dimensions.size(); i++){
        sum_w += w_dimensions[i];
        sum_h += h_dimensions[i];
    }
    int average_w = sum_w / w_dimensions.size();
    int average_h = sum_h / h_dimensions.size();

    // Output the result
    cout << "The average dimensions are: w = " << average_w << " and h = " << average_h << "." << endl;
    system("wait");

    return 0;
}
