/****************************************************************************************************
This code is part of the code supplied with the OpenCV Blueprints book.
It was written by Steven Puttemans, who can be contacted via steven.puttemans[at]kuleuven.be

License can be found at https://github.com/OpenCVBlueprints/OpenCVBlueprints/blob/master/license.txt
*****************************************************************************************************
This is a software alpha release of the OpenCV interface for visualising object models and
the used features to perform their detections.

USAGE:
./visualise_models -model <model.xml> -image <ref.png> -data <output folder>

LIMITS
- Only handles cascade classifier models
- Handles stumps only for the moment
- Needs a valid training/test sample window with the original model dimensions
- Can handle HAAR and LBP features
***********************************************************************************************/

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <fstream>
#include <iostream>

using namespace std;
using namespace cv;

struct rect_data{
    int x;
    int y;
    int w;
    int h;
    float weight;
};

int main( int argc, const char** argv )
{
    // If no parameters are give, then a usage template should be provided
    if(argc == 1){
	cout << "This is a software alpha release of the OpenCV interface for visualising object models and the used features to perform their detections." << endl;
        cout << "USAGE ./visualise_models -model <model.xml> -image <ref.png> -data <output folder>" << endl;
	cout << "LIMITS: only cascade classifier models / only stump features for now / valid sample window with model dimensions needed / HAAR and LBP features" << endl;
     	return 0;
    }

    // Read in the input arguments
    string model = "";
    string output_folder = "";
    string image_ref = "";
    for(int i = 1; i < argc; ++i )
    {
        if( !strcmp( argv[i], "-model" ) )
        {
            model = argv[++i];
        }else if( !strcmp( argv[i], "-image" ) ){
            image_ref = argv[++i];
        }else if( !strcmp( argv[i], "-data" ) ){
            output_folder = argv[++i];
        }
    }

    // Value for timing
    int timing = 1;

    // Value for cols of storing elements
    int cols_prefered = 5;

    // Open the XML model
    FileStorage fs;
    fs.open(model, FileStorage::READ);

    // Get a the required information
    // First decide which feature type we are using
    FileNode cascade = fs["cascade"];
    string feature_type = cascade["featureType"];
    bool haar = false, lbp = false;
    if (feature_type.compare("HAAR") == 0){
        haar = true;
    }
    if (feature_type.compare("LBP") == 0){
        lbp = true;
    }
    if ( feature_type.compare("HAAR") != 0 && feature_type.compare("LBP")){
        cerr << "The model is not an HAAR or LBP feature based model!" << endl;
        cerr << "Please select a model that can be visualized by the software." << endl;
        return -1;
    }

    // We make a visualisation mask - which increases the window to make it at least a bit more visible
    int resize_factor = 10;
    int resize_storage_factor = 10;
    Mat reference_image = imread(image_ref, CV_LOAD_IMAGE_GRAYSCALE);
    Mat visualization;
    resize(reference_image, visualization, Size(reference_image.cols * resize_factor, reference_image.rows * resize_factor), 0, 0, CV_INTER_CUBIC);

    // First recover for each stage the number of weak features and their index
    // Important since it is NOT sequential when using LBP features
    vector< vector<int> > stage_features;
    FileNode stages = cascade["stages"];
    FileNodeIterator it_stages = stages.begin(), it_stages_end = stages.end();
    int idx = 0;
    for( ; it_stages != it_stages_end; it_stages++, idx++ ){
        vector<int> current_feature_indexes;
        FileNode weak_classifiers = (*it_stages)["weakClassifiers"];
        FileNodeIterator it_weak = weak_classifiers.begin(), it_weak_end = weak_classifiers.end();
        vector<int> values;
        for(int idy = 0; it_weak != it_weak_end; it_weak++, idy++ ){
            (*it_weak)["internalNodes"] >> values;
            current_feature_indexes.push_back( (int)values[2] );
        }
        stage_features.push_back(current_feature_indexes);
    }

    // If the output option has been chosen than we will store a combined image plane for
    // each stage, containing all weak classifiers for that stage.
    bool draw_planes = false;
    stringstream output_video;
    output_video << output_folder << "model_visualization.avi";
    VideoWriter result_video;
    if( output_folder.compare("") != 0 ){
        draw_planes = true;
        result_video.open(output_video.str(), CV_FOURCC('X','V','I','D'), 15, Size(reference_image.cols * resize_factor, reference_image.rows * resize_factor), false);
    }

    if(haar){
        // Grab the corresponding features dimensions and weights
        FileNode features = cascade["features"];
        vector< vector< rect_data > > feature_data;
        FileNodeIterator it_features = features.begin(), it_features_end = features.end();
        for(int idf = 0; it_features != it_features_end; it_features++, idf++ ){
            vector< rect_data > current_feature_rectangles;
            FileNode rectangles = (*it_features)["rects"];
            int nrects = (int)rectangles.size();
            for(int k = 0; k < nrects; k++){
                rect_data current_data;
                FileNode single_rect = rectangles[k];
                current_data.x = (int)single_rect[0];
                current_data.y = (int)single_rect[1];
                current_data.w = (int)single_rect[2];
                current_data.h = (int)single_rect[3];
                current_data.weight = (float)single_rect[4];
                current_feature_rectangles.push_back(current_data);
            }
            feature_data.push_back(current_feature_rectangles);
        }

        // Loop over each possible feature on its index, visualise on the mask and wait a bit,
        // then continue to the next feature.
        // If visualisations should be stored then do the in between calculations
        Mat image_plane;
        Mat metadata = Mat::zeros(150, 1000, CV_8UC1);
        vector< rect_data > current_rects;
        for(int sid = 0; sid < (int)stage_features.size(); sid ++){
            if(draw_planes){
                int features = stage_features[sid].size();
                int cols = cols_prefered;
                int rows = features / cols;
                if( (features % cols) > 0){
                    rows++;
                }
                image_plane = Mat::zeros(reference_image.rows * resize_storage_factor * rows, reference_image.cols * resize_storage_factor * cols, CV_8UC1);
            }
            for(int fid = 0; fid < (int)stage_features[sid].size(); fid++){
                stringstream meta1, meta2;
                meta1 << "Stage " << sid << " / Feature " << fid;
                meta2 << "Rectangles: ";
                Mat temp_window = visualization.clone();
                Mat temp_metadata = metadata.clone();
                int current_feature_index = stage_features[sid][fid];
                current_rects = feature_data[current_feature_index];
                Mat single_feature = reference_image.clone();
                resize(single_feature, single_feature, Size(), resize_storage_factor, resize_storage_factor);
                for(int i = 0; i < (int)current_rects.size(); i++){
                    rect_data local = current_rects[i];
                    if(draw_planes){
                        if(local.weight >= 0){
                            rectangle(single_feature, Rect(local.x * resize_storage_factor, local.y * resize_storage_factor, local.w * resize_storage_factor, local.h * resize_storage_factor), Scalar(0), CV_FILLED);
                        }else{
                            rectangle(single_feature, Rect(local.x * resize_storage_factor, local.y * resize_storage_factor, local.w * resize_storage_factor, local.h * resize_storage_factor), Scalar(255), CV_FILLED);
                        }
                    }
                    Rect part(local.x * resize_factor, local.y * resize_factor, local.w * resize_factor, local.h * resize_factor);
                    meta2 << part << " (w " << local.weight << ") ";
                    if(local.weight >= 0){
                        rectangle(temp_window, part, Scalar(0), CV_FILLED);
                    }else{
                        rectangle(temp_window, part, Scalar(255), CV_FILLED);
                    }
                }
                imshow("features", temp_window);
                putText(temp_window, meta1.str(), Point(15,15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255));
                result_video.write(temp_window);
                // Copy the feature image if needed
                if(draw_planes){
                    single_feature.copyTo(image_plane(Rect(0 + (fid%cols_prefered)*single_feature.cols, 0 + (fid/cols_prefered) * single_feature.rows, single_feature.cols, single_feature.rows)));
                }
                putText(temp_metadata, meta1.str(), Point(15,15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255));
                putText(temp_metadata, meta2.str(), Point(15,40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255));
                imshow("metadata", temp_metadata);
                waitKey(timing);
            }
            //Store the stage image if needed
            if(draw_planes){
                stringstream save_location;
                save_location << output_folder << "stage_" << sid << ".png";
                imwrite(save_location.str(), image_plane);
            }
        }
    }

    if(lbp){
        // Grab the corresponding features dimensions and weights
        FileNode features = cascade["features"];
        vector<Rect> feature_data;
        FileNodeIterator it_features = features.begin(), it_features_end = features.end();
        for(int idf = 0; it_features != it_features_end; it_features++, idf++ ){
            FileNode rectangle = (*it_features)["rect"];
            Rect current_feature ((int)rectangle[0], (int)rectangle[1], (int)rectangle[2], (int)rectangle[3]);
            feature_data.push_back(current_feature);
        }

        // Loop over each possible feature on its index, visualise on the mask and wait a bit,
        // then continue to the next feature.
        Mat image_plane;
        Mat metadata = Mat::zeros(150, 1000, CV_8UC1);
        for(int sid = 0; sid < (int)stage_features.size(); sid ++){
            if(draw_planes){
                int features = stage_features[sid].size();
                int cols = cols_prefered;
                int rows = features / cols;
                if( (features % cols) > 0){
                    rows++;
                }
                image_plane = Mat::zeros(reference_image.rows * resize_storage_factor * rows, reference_image.cols * resize_storage_factor * cols, CV_8UC1);
            }
            for(int fid = 0; fid < (int)stage_features[sid].size(); fid++){
                stringstream meta1, meta2;
                meta1 << "Stage " << sid << " / Feature " << fid;
                meta2 << "Rectangle: ";
                Mat temp_window = visualization.clone();
                Mat temp_metadata = metadata.clone();
                int current_feature_index = stage_features[sid][fid];
                Rect current_rect = feature_data[current_feature_index];
                Mat single_feature = reference_image.clone();
                resize(single_feature, single_feature, Size(), resize_storage_factor, resize_storage_factor);

                // VISUALISATION
                // The rectangle is the top left one of a 3x3 block LBP constructor
                Rect resized(current_rect.x * resize_factor, current_rect.y * resize_factor, current_rect.width * resize_factor, current_rect.height * resize_factor);
                meta2 << resized;
                // Top left
                rectangle(temp_window, resized, Scalar(0), 1);
                // Top middle
                rectangle(temp_window, Rect(resized.x + resized.width, resized.y, resized.width, resized.height), Scalar(0), 1);
                // Top right
                rectangle(temp_window, Rect(resized.x + 2*resized.width, resized.y, resized.width, resized.height), Scalar(0), 1);
                // Middle left
                rectangle(temp_window, Rect(resized.x, resized.y + resized.height, resized.width, resized.height), Scalar(0), 1);
                // Middle middle
                rectangle(temp_window, Rect(resized.x + resized.width, resized.y + resized.height, resized.width, resized.height), Scalar(0), CV_FILLED);
                // Middle right
                rectangle(temp_window, Rect(resized.x + 2*resized.width, resized.y + resized.height, resized.width, resized.height), Scalar(0), 1);
                // Bottom left
                rectangle(temp_window, Rect(resized.x, resized.y + 2*resized.height, resized.width, resized.height), Scalar(0), 1);
                // Bottom middle
                rectangle(temp_window, Rect(resized.x + resized.width, resized.y + 2*resized.height, resized.width, resized.height), Scalar(0), 1);
                // Bottom right
                rectangle(temp_window, Rect(resized.x + 2*resized.width, resized.y + 2*resized.height, resized.width, resized.height), Scalar(0), 1);

                if(draw_planes){
                    Rect resized(current_rect.x * resize_storage_factor, current_rect.y * resize_storage_factor, current_rect.width * resize_storage_factor, current_rect.height * resize_storage_factor);
                    // Top left
                    rectangle(single_feature, resized, Scalar(0), 1);
                    // Top middle
                    rectangle(single_feature, Rect(resized.x + resized.width, resized.y, resized.width, resized.height), Scalar(0), 1);
                    // Top right
                    rectangle(single_feature, Rect(resized.x + 2*resized.width, resized.y, resized.width, resized.height), Scalar(0), 1);
                    // Middle left
                    rectangle(single_feature, Rect(resized.x, resized.y + resized.height, resized.width, resized.height), Scalar(0), 1);
                    // Middle middle
                    rectangle(single_feature, Rect(resized.x + resized.width, resized.y + resized.height, resized.width, resized.height), Scalar(0), CV_FILLED);
                    // Middle right
                    rectangle(single_feature, Rect(resized.x + 2*resized.width, resized.y + resized.height, resized.width, resized.height), Scalar(0), 1);
                    // Bottom left
                    rectangle(single_feature, Rect(resized.x, resized.y + 2*resized.height, resized.width, resized.height), Scalar(0), 1);
                    // Bottom middle
                    rectangle(single_feature, Rect(resized.x + resized.width, resized.y + 2*resized.height, resized.width, resized.height), Scalar(0), 1);
                    // Bottom right
                    rectangle(single_feature, Rect(resized.x + 2*resized.width, resized.y + 2*resized.height, resized.width, resized.height), Scalar(0), 1);

                    single_feature.copyTo(image_plane(Rect(0 + (fid%cols_prefered)*single_feature.cols, 0 + (fid/cols_prefered) * single_feature.rows, single_feature.cols, single_feature.rows)));
                }

                putText(temp_metadata, meta1.str(), Point(15,15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255));
                putText(temp_metadata, meta2.str(), Point(15,40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255));
                imshow("metadata", temp_metadata);
                imshow("features", temp_window);
                putText(temp_window, meta1.str(), Point(15,15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255));
                result_video.write(temp_window);

                waitKey(timing);
            }

            //Store the stage image if needed
            if(draw_planes){
                stringstream save_location;
                save_location << output_folder << "stage_" << sid << ".png";
                imwrite(save_location.str(), image_plane);
            }
        }
    }
    return 0;
}
