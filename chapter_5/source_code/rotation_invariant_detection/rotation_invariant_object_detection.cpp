/****************************************************************************************************
This code is part of the code supplied with the OpenCV Blueprints book.
It was written by Steven Puttemans, who can be contacted via steven.puttemans[at]kuleuven.be

License can be found at https://github.com/OpenCVBlueprints/OpenCVBlueprints/blob/master/license.txt
*****************************************************************************************************
This is software for performing rotation invariant object detection using a single orientation 
model trained with the OpenCV3.0 cascade classifier training tool.

USAGE:
./rotation_invariant_detection -model <model.xml> -image <window.png>
                               -max_angle <degrees> -step_angle <degrees>
***********************************************************************************************/

#include <string.h>
#include <iostream>
#include <sys/time.h>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/objdetect.hpp"

using namespace std;
using namespace cv;

typedef unsigned long long timestamp_t;

static timestamp_t get_timestamp ()
{
  struct timeval now;
  gettimeofday (&now, NULL);
  return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}

// Return the rotation matrices for each rotation
// The angle parameter is expressed in degrees!
void rotate(Mat& src, double angle, Mat& dst)
{
    Point2f pt(src.cols/2., src.rows/2.);
    Mat r = getRotationMatrix2D(pt, angle, 1.0);
    warpAffine(src, dst, r, cv::Size(src.cols, src.rows));
}

int main(int argc, char* argv[])
{
    // If no parameters are give, then a usage template should be provided
    if(argc == 1){
	cout << "This is software for performing rotation invariant object detection using a single orientation model trained with the OpenCV3.0 cascade classifier training tool." << endl;
        cout << "USAGE ./rotation_invariant_detection -model <model.xml> -image <window.png> -max_angle <degrees> -step_angle <degrees>" << endl;
        return 0;
    }

    // Timing
    timestamp_t t0 = get_timestamp();

    // Read in the input arguments
    string model = "";
    string input_image = "";
    int max_angle = 0, step_angle = 0;
    for(int i = 1; i < argc; ++i )
    {
        if( !strcmp( argv[i], "-model" ) )
        {
            model = argv[++i];
        }else if( !strcmp( argv[i], "-image" ) ){
            input_image = argv[++i];
        }else if( !strcmp( argv[i], "-max_angle" ) ){
            max_angle = atoi(argv[++i]);
        }else if( !strcmp( argv[i], "-step_angle" ) ){
            step_angle = atoi(argv[++i]);
        }
    }

    // Create the cascade classifier
    CascadeClassifier cascade(model);
    if (cascade.empty()){
        cerr << "The given model could not be loaded!" << endl;
        return -1;
    }

    // Create the 3D model matrix of the input image
    Mat image = imread(input_image);
    int steps = max_angle / step_angle;
    vector<Mat> rotated_images;
    // Preprocess the images already, since we will need it for detection purposes
    cvtColor(rotated, rotated, CV_BGR2GRAY);
    equalizeHist( rotated, rotated );
    for (int i = 0; i < steps; i ++){
        // Rotate the image
        Mat rotated = image.clone();
        rotate(image, (i+1)*step_angle, rotated);
        // Add to the collection of rotated and processed images
        rotated_images.push_back(rotated);
    }

    // Perform detection on each image seperately
    // Store all detection in a vector of vector detections
    vector< vector<Rect> > detections;
    for (int i = 0; i < (int)rotated_images.size(); i++){
        vector<Rect> objects;
        // You can reduce the scale range, downscale step or score threshold here if prefered
        cascade.detectMultiScale(rotated_images[i], objects, 1.05, 25, 0, Size(), Size());
        detections.push_back(objects);
    }

    // Visualize detections on the input frame and show in the given window
    // Select a color for each orientation
    RNG rng(12345);
    for( int i = 0; i < (int)detections.size(); i++ ){
        vector<Rect> temp = detections[i];
        Scalar color(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
        for( int j = 0; j < (int)temp.size(); j++ )
        {
                // Use a rectangle representation on the frame but warp back the coordinates
                // Retrieve the 4 corners detected in the rotation image
                Point p1 ( temp[j].x, temp[j].y ); // Top left
                Point p2 ( (temp[j].x + temp[j].width), temp[j].y ); // Top right
                Point p3 ( (temp[j].x + temp[j].width), (temp[j].y + temp[j].height) ); // Down right
                Point p4 ( temp[j].x, (temp[j].y + temp[j].height) ); // Down left

                // Add the 4 points to a matrix structure
                Mat coordinates = (Mat_<double>(3,4) << p1.x, p2.x, p3.x, p4.x,\
                                                        p1.y, p2.y, p3.y, p4.y,\
                                                        1   , 1  ,  1   , 1    );

                // Apply a new inverse tranformation matrix
                Point2f pt(image.cols/2., image.rows/2.);
                Mat r = getRotationMatrix2D(pt, -(step_angle*(i+1)), 1.0);
                Mat result = r * coordinates;

                // Retrieve the new coordinates from the tranformed matrix
                Point p1_back, p2_back, p3_back, p4_back;
                p1_back.x=(int)result.at<double>(0,0);
                p1_back.y=(int)result.at<double>(1,0);

                p2_back.x=(int)result.at<double>(0,1);
                p2_back.y=(int)result.at<double>(1,1);

                p3_back.x=(int)result.at<double>(0,2);
                p3_back.y=(int)result.at<double>(1,2);

                p4_back.x=(int)result.at<double>(0,3);
                p4_back.y=(int)result.at<double>(1,3);

                // Draw a rotated rectangle by lines, using the reverse warped points
                line(image, p1_back, p2_back, color, 2);
                line(image, p2_back, p3_back, color, 2);
                line(image, p3_back, p4_back, color, 2);
                line(image, p4_back, p1_back, color, 2);
            }
    }

    // Timing
    timestamp_t t1 = get_timestamp();
    double secs = (t1 - t0) / 1000000.0L;
    cerr << "Processing took " << secs << "seconds." << endl;

    // Show the actual frame
    imshow("Detection result", image);
    waitKey(0);

    return 0;
}
