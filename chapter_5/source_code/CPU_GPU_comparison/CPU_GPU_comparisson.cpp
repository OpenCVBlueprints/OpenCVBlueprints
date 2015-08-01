/****************************************************************************************************
This code is part of the code supplied with the OpenCV Blueprints book.
It was written by Steven Puttemans, who can be contacted via steven.puttemans[at]kuleuven.be

License can be found at https://github.com/OpenCVBlueprints/OpenCVBlueprints/blob/master/license.txt
*****************************************************************************************************
This software can be used for comparing CPU and GPU object detection performance on a given sample
****************************************************************************************************/

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/cudaobjdetect.hpp>

using namespace std;
using namespace cv;

int main( int argc, const char** argv )
{
    // If no parameters are give, then a usage template should be provided
    if(argc == 1){
	cout << "This software can be used for comparing CPU and GPU object detection performance on a given sample" << endl;
        cout << "USAGE ./CPU_GPU_comparison -image example.jpg" << endl;
        return 0;
    }
    
    // Process the arguments
    // Read in the parameters supplied
    string location;
    for(int i = 1; i < argc; ++i )
    {
        if( !strcmp( argv[i], "-image" ) )
        {
            location = argv[++i];
        }
    }

    // Read in an original image
    // Already apply processing
    Mat image = imread(location, IMREAD_GRAYSCALE);
    Mat hist;
    equalizeHist(image, hist);

    // Perform the CPU detector when downscaling 5 times
    cerr << "---------------------------------------------------------------------" << endl;
    cerr << "CPU PROCESSING - each time 10 images processed" << endl;
    cerr << "---------------------------------------------------------------------" << endl;
    CascadeClassifier cpu_detector("/home/spu/Documents/github/opencv_CUDA/data/haarcascades/haarcascade_fullbody.xml");
    if(cpu_detector.empty()){
        cerr << "NOT LOADED";
        return 0;
    }
    for(int scale = 2; scale<6; scale++){
        Mat current;
        resize(hist, current, Size(image.rows/scale, image.cols/scale));
        // Start timing here
        int64 t0 = getTickCount();
        for(int j=0; j<10; j++){
            vector<Rect> objects;
            cpu_detector.detectMultiScale(current, objects, 1.1, 0);
        }
        // End timing here and output
        int64 t1 = getTickCount();
        double secs = (t1-t0)/getTickFrequency();
        cerr << "Image dimensions = [" << current.rows << " " << current.cols << "]" << endl;
        cerr << "Measurement - division by " << scale << ": time = " << secs << " seconds"<< endl;
    }

    // Set the CUDA device to avoid this influencing the time calculations
    cuda::setDevice(0);

    cerr << "---------------------------------------------------------------------" << endl;
    cerr << "GPU PROCESSING - each time 10 images processed" << endl;
    cerr << "---------------------------------------------------------------------" << endl;
    // Perform the GPU detector
    Ptr<cuda::CascadeClassifier> cascade_gpu = cuda::CascadeClassifier::create("/home/spu/Documents/github/opencv_CUDA/data/haarcascades_cuda/haarcascade_fullbody.xml");
    cascade_gpu->setScaleFactor(1.1);
    cascade_gpu->setMinNeighbors(0);
    // Since my GPU NVIDIA Quadro K2000 has only 1GB of dedicated memory I cannot process more than 4000x4000 pixels in a single run.
    // Therefore we need to split the image, and do a double run, and thus a double pushing from CPU<->GPU
    for(int scale = 2; scale<6; scale++){
        Mat current;
        resize(hist, current, Size(image.rows/scale, image.cols/scale));
        // Start timing here
        int64 t0 = getTickCount();
        // We need to include the time for pushing and retrieving the data to and from the GPU
        for(int j=0; j<10; j++){
            cuda::GpuMat image_gpu(current);
            cuda::GpuMat objbuf;
            cascade_gpu->detectMultiScale(image_gpu, objbuf);
            std::vector<Rect> detections;
            cascade_gpu->convert(objbuf, detections);
        }
        // End timing here and output
        int64 t1 = getTickCount();
        double secs = (t1-t0)/getTickFrequency();
        cerr << "Image dimensions = [" << current.rows << " " << current.cols << "]" << endl;
        cerr << "Measurement - division by " << scale << ": time = " << secs << " seconds"<< endl;
    }

    cerr << endl << endl;
    cerr << "---------------------------------------------------------------------" << endl;
    cerr << "CPU-GPU PROCESSING on the original, 10 times" << endl;
    cerr << "---------------------------------------------------------------------" << endl;
    Mat original_cpu = hist.clone();

    // CPU processing
    // Directly processing it
    int64 t0 = getTickCount();
    for(int i=0; i < 10; i++){
        vector<Rect> objects_cpu;
        cpu_detector.detectMultiScale(original_cpu, objects_cpu, 1.1, 0);
    }
    int64 t1 = getTickCount();
    double secs = (t1-t0)/getTickFrequency();
    cerr << "CPU Measurement - [8000x4000] pixels: time = " << secs << " seconds"<< endl;

    // GPU processing
    t0 = getTickCount();
    for(int i=0; i < 10; i++){
        // We first need to split the image and add them to a vector
        vector<Mat> data_gpu;
        data_gpu.push_back( original_cpu( Rect(0, 0, 4000, 4000) ).clone() );
        data_gpu.push_back( original_cpu( Rect(3999, 0, 4000, 4000) ).clone() );
        // Now process the data in a loop
        for(size_t j=0; j<data_gpu.size(); j++){
            cuda::GpuMat image_gpu(data_gpu[j]);
            cuda::GpuMat objbuf;
            cascade_gpu->detectMultiScale(image_gpu, objbuf);
            vector<Rect> detections;
            cascade_gpu->convert(objbuf, detections);
        }
    }
    t1 = getTickCount();
    secs = (t1-t0)/getTickFrequency();
    cerr << "GPU Measurement - [8000x4000] pixels: time = " << secs << " seconds"<< endl;

    return 0;
}

