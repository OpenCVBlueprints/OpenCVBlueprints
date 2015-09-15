#include <iostream>
#include <string>
#include <dirent.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

#include "flandmark_detector.h"

using namespace std;
using namespace cv;

void processJAFFE(string input, string output);
vector<string> listFile(string folder);
CascadeClassifier loadCascade(string cascadePath);

int main(int argc, char* argv[]) {
    cout << "============= FACIAL COMPONENTS =============" << endl;

    if(argc != 5){
        cout << "Usage: " << endl;
        cout << argv[0] << " -src <input_folder> -dest <output_folder>" << endl;
        return 1;
    }

    // ********************
    // Get input parameters
    // ********************

    string input;
    string output_folder;

    for(int i = 0; i < argc ; i++)
    {
        if( strcmp(argv[i], "-src") == 0 ){
            if(i + 1 >= argc) return -1;
            input = argv[i + 1];
        }

        if( strcmp(argv[i], "-dest") == 0 ){
            if(i + 1 >= argc) return -1;
            output_folder = argv[i + 1];
        }
    }

    // ********************
    // JAFFE Dataset
    // ********************
    processJAFFE(input, output_folder);

    return 0;
}

void processJAFFE(string input, string output) {
    cout << "Process JAFFE: " << input << endl;

    vector<string> imgPath = listFile(input);

    int num_of_image = imgPath.size();

    int EYE_IMG_WIDTH = 100;
    int EYE_IMG_HEIGHT = 100;
    int MOUTH_IMG_WIDTH = 60;
    int MOUTH_IMG_HEIGHT = 40;
    int FACE_IMG_WIDTH = 160;
    int FACE_IMG_HEIGHT = 160;

    // ------ load cascade files ----
    CascadeClassifier face_cascade = loadCascade("haarcascade_frontalface_alt.xml");
    if(face_cascade.empty()) return;

    FLANDMARK_Model * model = flandmark_init("flandmark_model.dat");
    int num_of_landmark = model->data.options.M;
    double *points = new double[2 * num_of_landmark];

    FileStorage fs( output + "/list.yml" , FileStorage::WRITE);
    fs << "num_of_image" << num_of_image;

    for(int img_id = 0 ; img_id < num_of_image; img_id++) {
        Mat img, img_gray;
        // load image
        img = imread(imgPath[img_id], CV_LOAD_IMAGE_COLOR);
        cvtColor(img, img_gray, CV_RGB2GRAY);
        equalizeHist(img_gray, img_gray);

        vector<Rect> faces;
        face_cascade.detectMultiScale( img_gray, faces, 1.1, 3);

        // Get the largest face region
        int i = 0;
        int max_width = 0;
        for(int index = 0 ; index < faces.size() ; index++){
            if(faces[i].width > max_width){
                i = index;
                max_width = faces[i].width;
            }
        }

        int bbox[4] = { faces[i].x, faces[i].y, faces[i].x + faces[i].width, faces[i].y + faces[i].height };
        flandmark_detect(new IplImage(img_gray), bbox, model, points);

        // left eye
        Point centerLeft = Point( (int) (points[2 * 6] + points[2 * 2]) / 2, (int) (points[2 * 6 + 1] + points[2 * 2 + 1]) / 2 );
        int widthLeft = abs(points[2 * 6] - points[2 * 2]);


        // right eye
        Point centerRight = Point( (int) (points[2 * 1] + points[2 * 5]) / 2, (int) (points[2 * 1 + 1] + points[2 * 5 + 1]) / 2 );
        int widthRight = abs(points[2 * 1] - points[2 * 5]);

        // face
        int widthFace = (centerLeft.x + widthLeft) - (centerRight.x - widthRight);
        int heightFace = widthFace * 1.2;
        Mat face = img(Rect( centerRight.x - widthFace/4  , centerRight.y - heightFace/4, widthFace, heightFace ));

        //
        // extract label
        cout << "ImagePath: " << imgPath[img_id] << endl;
        string fileName = imgPath[img_id].substr(input.length() + 1, imgPath[i].length());

        // save image
        string curFileName = fileName;

        curFileName = fileName;
        curFileName.replace(fileName.length() - 4, 4, "face.tiff");

        resize(face, face, Size(FACE_IMG_WIDTH, FACE_IMG_HEIGHT));
        imwrite( output + "/" + curFileName, face);
        fs << "img_" + to_string(img_id) + "_face" << output + "/" + curFileName;

    }

}

CascadeClassifier loadCascade(string cascadePath){
    CascadeClassifier cascade;
    if (!cascade.load(cascadePath)) {
        cout << "--(!)Error loading cascade: " << cascadePath << endl;
    };
    return cascade;
}
vector<string> listFile(string folder){
    vector<string> imgPath;
    DIR *pDIR;
    struct dirent *entry;
    if( pDIR=opendir(folder.c_str()) ){
        while(entry = readdir(pDIR)){
            if (entry->d_type == DT_REG){		// if entry is a regular file
                std::string fname = entry->d_name;	// filename
                std::string::size_type size = fname.find(".tiff");
                if(size != std::string::npos){
                    imgPath.push_back(folder + "/" + fname);
                }
            }
        }
    }
    return imgPath;
}