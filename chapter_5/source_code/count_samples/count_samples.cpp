/****************************************************************************************************
This code is part of the code supplied with the OpenCV Blueprints book.
It was written by Steven Puttemans, who can be contacted via steven.puttemans[at]kuleuven.be

License can be found at https://github.com/OpenCVBlueprints/OpenCVBlueprints/blob/master/license.txt
*****************************************************************************************************
This is software for counting exactly how many object samples are available in a
predefined annotations file.

USAGE:
./count_samples -annotations <annotations.txt>
***********************************************************************************************/

// Includes for standard file input output functionality
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
	cout << "This is software for counting exactly how many object samples are available in a predefined annotations file." << endl;
        cout << "USAGE ./count_samples -annotations <annotations.txt>" << endl;
        return 0;
    }

    // Get arguments
    string annotation_file;
    for(int i = 1; i < argc; ++i )
    {
        if( !strcmp( argv[i], "-annotations" ) )
        {
            annotation_file = argv[++i];
        }
    }

    ifstream input(annotation_file.c_str());
    string current_line;
    int total_samples = 0;
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
        total_samples = total_samples + number_rectangles;
    }
    input.close();

    // Output the result
    cout << "The total number of samples = " << total_samples << "." << endl;
    system("wait");

    return 0;
}
