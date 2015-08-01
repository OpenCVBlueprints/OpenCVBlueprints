/****************************************************************************************************
This code is part of the code supplied with the OpenCV Blueprints book.
It was written by Steven Puttemans, who can be contacted via steven.puttemans[at]kuleuven.be

License can be found at https://github.com/OpenCVBlueprints/OpenCVBlueprints/blob/master/license.txt
*****************************************************************************************************
This is software that calculates precision and recall from detection and ground truth files

USAGE:
./precision_recall -groundtruth <file.txt> -detections <file.txt> -output <folder>
                   -minthresh <lowest_score> -maxthresh <max_score> -stepthresh <step_score>
***********************************************************************************************/

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

struct scored_rect {
    string filename;
    Rect region;
    double score;
};

int main( int argc, const char** argv )
{
    // If no parameters are give, then a usage template should be provided
    if(argc == 1){
	cout << "This is software that calculates precision and recall from detection and ground truth files" << endl;
        cout << "USAGE ./precision_recall -groundtruth <file.txt> -detections <file.txt> -output <folder> -minthresh <lowest_score> -maxthresh <max_score> -stepthresh <step_score>" << endl;
        return 0;
    }

    // Read in the input arguments
    string ground_truth_file = "";
    string detection_file = "";
    string output = "";
    double min_thresh = 0, max_thesh = 0, step_thresh = 0;
    for(int i = 1; i < argc; ++i )
    {
        if( !strcmp( argv[i], "-groundtruth" ) )
        {
            ground_truth_file = argv[++i];
        }else if( !strcmp( argv[i], "-detections" ) ){
            detection_file = argv[++i];
        }else if( !strcmp( argv[i], "-output" ) ){
            output = argv[++i];
        }else if( !strcmp( argv[i], "-minthresh" ) ){
            min_thresh = atof(argv[++i]);
        }else if( !strcmp( argv[i], "-maxthresh" ) ){
            max_thesh = atof(argv[++i]);
        }else if( !strcmp( argv[i], "-stepthresh" ) ){
            step_thresh = atof(argv[++i]);
        }
    }

    // Groundtruth and detections need to be in the standard OpenCV format!
    // This means for each image an entry following this structure:
    // filename #detections x1 y1 w1 h1 ...... xN yN wN hN

    // Retrieve ground truth of the algorithm
    // Stored as a collection over the images, with a collection of rectangles for each image
    // Score set to -1 since ground truth doesn't have a certainty score
    ifstream read_ground_truth(ground_truth_file.c_str());
    string current_line;
    vector< vector<scored_rect> > ground_truth;
    while ( getline(read_ground_truth, current_line) ){
        // Switch a complete line to elements for further processing
        stringstream temp (current_line);
        vector<string> line_elements;
        string item;
        while ( getline(temp, item, ' ') ) {
            line_elements.push_back(item);
        }

        // Now convert the data to the needed rectangles
        int amount_rectangles = atoi(line_elements[1].c_str());
        vector<scored_rect> image_rectangles;
        for(int i = 0; i < amount_rectangles; i ++){
            scored_rect single_rectangle;
            single_rectangle.filename = line_elements[0];
            single_rectangle.region = Rect(atoi(line_elements[2+(i*4)].c_str()), atoi(line_elements[3+(i*4)].c_str()), atoi(line_elements[4+(i*4)].c_str()), atoi(line_elements[5+(i*4)].c_str()));
            single_rectangle.score = -1.0;
            image_rectangles.push_back(single_rectangle);
        }

        // add the detection vector of the single image to the complete collection
        ground_truth.push_back(image_rectangles);
    }
    read_ground_truth.close();

    // Retrieve detections of the algorithm
    ifstream read_detections(detection_file.c_str());
    vector< vector<scored_rect> > detections;
    while ( getline(read_detections, current_line) ){
        // Switch a complete line to elements for further processing
        stringstream temp (current_line);
        vector<string> line_elements;
        string item;
        while ( getline(temp, item, ' ') ) {
            line_elements.push_back(item);
        }

        // Now convert the data to the needed rectangles
        int amount_rectangles = atoi(line_elements[1].c_str());
        vector<scored_rect> image_rectangles;
        for(int i = 0; i < amount_rectangles; i ++){
            scored_rect single_rectangle;
            single_rectangle.filename = line_elements[0];
            single_rectangle.region = Rect(atoi(line_elements[2+(i*5)].c_str()), atoi(line_elements[3+(i*5)].c_str()), atoi(line_elements[4+(i*5)].c_str()), atoi(line_elements[5+(i*5)].c_str()));
            single_rectangle.score = atof(line_elements[6+(i*5)].c_str());
            image_rectangles.push_back(single_rectangle);
        }

        // add the detection vector of the single image to the complete collection
        detections.push_back(image_rectangles);
    }
    read_detections.close();

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Now we compare the ground truth versus the detections based on
    //      - TP = detection that is in ground truth
    //      - FP = detection that is not in ground truth
    //      - FN = in ground truth but no detection
    // We want to define two values for each threshold, being recall and precision
    //      - recall        TP/(TP+FN)
    //      - precision     TP/(TP+FP)
    // Threshold is defined on the certainty score of the detection output
    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Vector for storing curve points for precision and recall
    vector<Point2f> curve_points;
    vector<double> score_levels;

    // All the above is done by iterating over the score threshold
    for(double threshold = min_thresh; threshold <= max_thesh; threshold = threshold + step_thresh){
        // We need to reset the values for each threshold defined
        int TP = 0, FP = 0, FN = 0;

        // Start by looping over all images
        for(int imageIdx = 0; imageIdx < (int)detections.size(); imageIdx++){
            vector<scored_rect> current_detections = detections[imageIdx];
            vector<scored_rect> current_annotations;

            // Now loop over all annotation files and make sure that you have the same image annotations based on the filename
            for(int annotationsIdx = 0; annotationsIdx < (int)ground_truth.size(); annotationsIdx++){
                current_annotations = ground_truth[annotationsIdx];
                if(current_detections[0].filename.compare(current_annotations[0].filename) == 0){
                    break;
                }
            }

            // Start comparing

            // First check how much TP and FP are available - done by comparing detections with the ground truth
            for(int rectIdx = 0; rectIdx < (int)current_detections.size(); rectIdx++){
                Rect region_det = current_detections[rectIdx].region;
                // Only use a detection if it is equal or above a certain threshold value
                if (current_detections[rectIdx].score >= threshold){
                    bool false_positive = true;
                    // For each detection, see if there is a match with all of the annotations
                    for (int annoIdx = 0; annoIdx < (int)current_annotations.size(); annoIdx++){
                        Rect region_anno = current_annotations[annoIdx].region;

                        // Definining of a matching pair of regions is a match is done based on overlapping area
                        int x_overlap = std::max(0, std::min(region_det.x + region_det.width, region_anno.x + region_anno.width) - std::max(region_det.x, region_anno.x));
                        int y_overlap = std::max(0, std::min(region_det.y + region_det.height, region_anno.y + region_anno.height) - std::max(region_det.y, region_anno.y));

                        int surface_det = region_det.width * region_det.height;
                        int surface_anno = region_anno.width * region_anno.height;
                        int surface_overlap = x_overlap * y_overlap;

                        // Rule to define what to increase - dependend on 50% overlap for a good match AND a max 50% larger detection
                        if( (surface_overlap > 0.5 * surface_det) && (surface_overlap > 0.5 * surface_anno) && (surface_det < (1.5 * surface_anno)) && (surface_det > (0.5 * surface_anno))){
                             TP++;
                             false_positive = false;
                        }
                    }
                    // Check if a TP was given, else a FP needs to be added
                    if (false_positive){
                         FP++;
                    }
                }
            }

            // Secondly check how much FN are available - done by comparing all ground truth to the detections - no matches means that a FN is detected
            for (int annoIdx = 0; annoIdx < (int)current_annotations.size(); annoIdx++){
                Rect region_anno = current_annotations[annoIdx].region;
                bool no_matches = true;
                for(int rectIdx = 0; rectIdx < (int)current_detections.size(); rectIdx++){
                    if (current_detections[rectIdx].score >= threshold){
                        Rect region_det = current_detections[rectIdx].region;

                        // Definining of a matching pair of regions is a match is done based on overlapping area
                        int x_overlap = std::max(0, std::min(region_det.x + region_det.width, region_anno.x + region_anno.width) - std::max(region_det.x, region_anno.x));
                        int y_overlap = std::max(0, std::min(region_det.y + region_det.height, region_anno.y + region_anno.height) - std::max(region_det.y, region_anno.y));

                        int surface_det = region_det.width * region_det.height;
                        int surface_anno = region_anno.width * region_anno.height;
                        int surface_overlap = x_overlap * y_overlap;

                        // Rule to define what to increase - dependend on 50% overlap for a good match AND a max 50% larger detection
                        if( (surface_overlap > 0.5 * surface_det) && (surface_overlap > 0.5 * surface_anno) ){
                             TP++;
                             no_matches = false;
                        }
                    }
                }
                // Add a FN increment if there are no detections matching the ground truth annotation
                if (no_matches) {
                    FN++;
                }

            }
        }

        // Now calculate the precision recall based on the TP FP FN
        // Then add both results to the curve points vector
        double precision = (double)TP / (double)(TP + FP);
        double recall = (double)TP / (double)(TP + FN);

        Point2f location (precision, recall);
        curve_points.push_back( location );
        cerr << location;
        score_levels.push_back( threshold );

        // Both elements are zero than we have a devision by zero and a point with no meaning has been added
        if(TP == 0 && FP == 0){ curve_points.pop_back(); }
        if(TP == 0 && FN == 0){ curve_points.pop_back(); }
    }

    // Now store the curvature points
    stringstream location;
    location << output << "coordinates.txt";
    ofstream result(location.str().c_str());
    for(int i = 0; i < (int)curve_points.size(); i ++){
        result << curve_points[i].x << " " << curve_points[i].y << " " << score_levels[i] << endl;
    }

	return 0;
}

