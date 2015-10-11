#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/ml/ml.hpp>
using namespace cv;
using namespace std;

void mlp(int num_of_label, Mat train_features, Mat train_labels, vector<string> train_paths, Mat test_features, Mat test_labels, vector<string> test_paths );
void svm(int num_of_label, Mat train_features, Mat train_labels, vector<string> train_paths, Mat test_features, Mat test_labels, vector<string> test_paths );
void knn(int num_of_label, Mat train_features, Mat train_labels, vector<string> train_paths, Mat test_features, Mat test_labels, vector<string> test_paths, int K);
void bayes(int num_of_label, Mat train_features, Mat train_labels, vector<string> train_paths, Mat test_features, Mat test_labels, vector<string> test_paths);

int main(int argc, char* argv[]) {
    cout << "============= TRAIN =============" << endl;

    if(argc != 7){
        cout << "Usage: " << endl;
        cout << argv[0] << " -algo <algorithm_name> -src <input_features> -dest <output_folder>" << endl;
        cout << "Example: " << endl;
        cout << argv[0] << " -algo mlp -src /Volumes/Data/Dataset/JAFFE/output/sift_features.yml -dest /Volumes/Data/Dataset/JAFFE/output" << endl;
        return 1;
    }

    // ********************
    // Get input parameters
    // ********************

    string algorithm_name;
    string input_file;
    string output_folder;

    int i;
    for(i = 1; i < argc ; i++)
    {
        if( strcmp(argv[i], "-algo") == 0 ){
            if(i + 1 >= argc) return -1;
            algorithm_name = argv[i + 1];
        }

        if( strcmp(argv[i], "-src") == 0 ){
            if(i + 1 >= argc) return -1;
            input_file = argv[i + 1];
        }

        if( strcmp(argv[i], "-dest") == 0 ){
            if(i + 1 >= argc) return -1;
            output_folder = argv[i + 1];
        }
    }

    cout << algorithm_name << " : " << input_file << " : " << output_folder <<  endl;

    // READ YML FILES
    FileStorage in( input_file, FileStorage::READ);
    int num_of_image = 0, num_of_train = 0, num_of_test = 0, feature_size = 0, num_of_label;
    in["num_of_image"] >> num_of_image;
    in["num_of_train"] >> num_of_train;
    in["num_of_test"] >> num_of_test;
    in["feature_size"] >> feature_size;
    in["num_of_label"] >> num_of_label;

    Mat feature;
    string path;
    int label;
    bool isTrain;
    Mat train_features(num_of_train, feature_size, CV_32FC1);
    Mat train_labels(num_of_train, 1, CV_32FC1);
    vector<string> train_paths;

    Mat test_features(num_of_test, feature_size, CV_32FC1);
    Mat test_labels(num_of_test, 1, CV_32FC1);
    vector<string> test_paths;

    int train_idx = 0, test_idx = 0;
    for(int i = 0 ; i < num_of_image; i ++){
        in["image_feature_" + to_string(i)] >> feature;
        in["image_label_" + to_string(i)] >> label;
        in["image_path_" + to_string(i)] >> path;
        in["image_is_train_" + to_string(i)] >> isTrain;

        if(isTrain){
            feature.copyTo(train_features.row(train_idx));
            train_labels.at<int>(train_idx, 0) = label;
            train_paths.push_back(path);
            train_idx += 1;
        }else{
            feature.copyTo(test_features.row(test_idx));
            test_labels.at<int>(test_idx, 0) = label;
            test_paths.push_back(path);
            test_idx += 1;
        }
    }

    // START TRAINING
    if( algorithm_name.compare( "mlp" ) == 0 ){
        mlp(num_of_label, train_features, train_labels, train_paths, test_features, test_labels, test_paths);
    } else if (algorithm_name.compare( "svm" ) == 0){
        svm(num_of_label, train_features, train_labels, train_paths, test_features, test_labels, test_paths);
    } else if (algorithm_name.compare( "knn" ) == 0){
        knn(num_of_label, train_features, train_labels, train_paths, test_features, test_labels, test_paths, 2);
    } else if (algorithm_name.compare( "bayes" ) == 0){
        bayes(num_of_label, train_features, train_labels, train_paths, test_features, test_labels, test_paths);
    }

    return 0;
}
// accuracy
float evaluate(cv::Mat& predicted, cv::Mat& actual) {
    assert(predicted.rows == actual.rows);
    int t = 0;
    int f = 0;
    for(int i = 0; i < actual.rows; i++) {

        float max = -1000000000000.0f;
        int cls = -1;

        for(int j = 0 ; j < predicted.cols ; j++)
        {
            float value = predicted.at<float>(i, j);

            if(value > max)
            {
                max = value;
                cls = j;
            }
        }

        int truth = (int) actual.at<int>(i, 0);
        if(cls ==  truth)
            t++;
        else
            f++;
    }
    return (t * 1.0) / (t + f) * 100;
}
void mlp(int num_of_label, Mat train_features, Mat train_labels, vector<string> train_paths, Mat test_features, Mat test_labels, vector<string> test_paths ){
    cout << "Training MLP: trainset: " << train_features.size() << " testset: " << test_features.size() << endl;

    Mat labels = Mat::zeros( train_labels.rows, num_of_label, CV_32FC1);
    for(int i = 0 ; i < train_labels.rows; i ++){
        int idx = train_labels.at<int>(i, 0);
        labels.at<float>(i, idx) = 1.0f;
    }

    Mat labels_test = Mat::zeros( test_labels.rows, 1, CV_32S);
    for(int i = 0 ; i < test_labels.rows; i ++){
        labels_test.at<unsigned int>(i, 0) = test_labels.at<int>(i, 0);
    }

    cv::Mat layers = cv::Mat(3, 1, CV_32S);

    layers.row(0) = cv::Scalar(train_features.cols);
    layers.row(1) = cv::Scalar(20);
    layers.row(2) = cv::Scalar(num_of_label);

    Ptr<ml::ANN_MLP> mlp = ml::ANN_MLP::create();
    mlp->setLayerSizes(layers);
    mlp->setTrainMethod(ml::ANN_MLP::BACKPROP);
    mlp->setActivationFunction(ml::ANN_MLP::SIGMOID_SYM, 0, 0);
    mlp->setTermCriteria(TermCriteria(TermCriteria::EPS+TermCriteria::COUNT, 100000, 0.00001f));

    // train
    Ptr<ml::TrainData> trainData = ml::TrainData::create(train_features, ml::SampleTypes::ROW_SAMPLE, labels);
    mlp->train(trainData);

    cv::Mat response(1, num_of_label, CV_32FC1);
    cv::Mat predicted(test_labels.rows, num_of_label, CV_32FC1);
    for(int i = 0; i < test_features.rows; i++) {
        cv::Mat sample = test_features.row(i);
        mlp->predict(sample, response);
        response.copyTo(predicted.row(i));
    }

    cout << "PREDICT: " << predicted << endl;
    cout << "LABEL: " << labels_test.t() << endl;

    cout << "Accuracy_{MLP} = " << evaluate(predicted, labels_test) << endl;

}

void svm(int num_of_label, Mat train_features, Mat train_labels, vector<string> train_paths, Mat test_features, Mat test_labels, vector<string> test_paths ){

    cout << "Training SVM: trainset: " << train_features.size() << " testset: " << test_features.size() << endl;

    Mat labels = Mat::zeros( train_labels.rows, 1, CV_32S);
    for(int i = 0 ; i < train_labels.rows; i ++){
        labels.at<unsigned int>(i, 0) = train_labels.at<int>(i, 0);
    }

    Mat labels_test = Mat::zeros( test_labels.rows, 1, CV_32S);
    for(int i = 0 ; i < test_labels.rows; i ++){
        labels_test.at<unsigned int>(i, 0) = test_labels.at<int>(i, 0);
    }

    Ptr<ml::SVM> svm = ml::SVM::create();
//    svm->setType(SVM::C_SVC);
//    svm->setKernel(SVM::RBF);
    Ptr<ml::TrainData> trainData = ml::TrainData::create(train_features, ml::SampleTypes::ROW_SAMPLE, labels);
    svm->trainAuto(trainData);

    cv::Mat predicted = Mat::zeros(test_labels.rows, num_of_label, CV_32F);
    for(int i = 0; i < test_features.rows; i++) {
        cv::Mat sample = test_features.row(i);

        float predict = svm->predict(sample);

        predicted.at<float>(i, (int) predict) = 1.0f;
    }

    cout << "PREDICT: " << predicted << endl;
    cout << "LABEL: " << labels_test.t() << endl;

    cout << "Accuracy_{SVM} = " << evaluate(predicted, labels_test) << endl;
}

void knn(int num_of_label, Mat train_features, Mat train_labels, vector<string> train_paths, Mat test_features, Mat test_labels, vector<string> test_paths, int K){
    cout << "Training KNN: trainset: " << train_features.size() << " testset: " << test_features.size() << endl;

    Mat labels = Mat::zeros( train_labels.rows, 1, CV_32SC1);
    for(int i = 0 ; i < train_labels.rows; i ++){
        labels.at<unsigned int>(i, 0) = train_labels.at<int>(i, 0);
    }

    Mat labels_test = Mat::zeros( test_labels.rows, 1, CV_32SC1);
    for(int i = 0 ; i < test_labels.rows; i ++){
        labels_test.at<int>(i, 0) = test_labels.at<int>(i, 0);
    }

    Ptr<ml::KNearest> knn = ml::KNearest::create();
    Ptr<ml::TrainData> trainData = ml::TrainData::create(train_features, ml::SampleTypes::ROW_SAMPLE, labels);
    knn->train(trainData);

    cv::Mat predicted = Mat::zeros(test_labels.rows, num_of_label, CV_32F);
    for(int i = 0; i < test_features.rows; i++) {
        cv::Mat sample = test_features.row(i);
        Mat bestLabels;
        knn->findNearest(sample, K, bestLabels);
        predicted.at<float>(i, bestLabels.at<float>(0,0)) = 1.0f;
    }

    cout << "PREDICT: " << predicted << endl;
    cout << "LABEL: " << labels_test.t() << endl;

    cout << "Accuracy_{KNN} = " << evaluate(predicted, labels_test) << endl;
}
void bayes(int num_of_label, Mat train_features, Mat train_labels, vector<string> train_paths, Mat test_features, Mat test_labels, vector<string> test_paths){
    cout << "Training Bayes: trainset: " << train_features.size() << " testset: " << test_features.size() << endl;

    Mat labels = Mat::zeros( train_labels.rows, 1, CV_32S);
    for(int i = 0 ; i < train_labels.rows; i ++){
        labels.at<unsigned int>(i, 0) = train_labels.at<int>(i, 0);
    }

    cout << "Labels: " << labels.t() << endl;

    Mat labels_test = Mat::zeros( test_labels.rows, 1, CV_32SC1);
    for(int i = 0 ; i < test_labels.rows; i ++){
        labels_test.at<int>(i, 0) = test_labels.at<int>(i, 0);
    }

    Ptr<ml::NormalBayesClassifier> bayes = ml::NormalBayesClassifier::create();
    Ptr<ml::TrainData> trainData = ml::TrainData::create(train_features, ml::SampleTypes::ROW_SAMPLE, labels);
    bayes->train(trainData);
    cout << "Start testing" << endl;
    cv::Mat predicted = Mat::zeros(test_labels.rows, num_of_label, CV_32F);
    for(int i = 0; i < test_features.rows; i++) {
        cv::Mat sample = test_features.row(i);

        Mat output, outputProb;
        bayes->predictProb(sample, output, outputProb);

        predicted.at<float>(i, output.at<unsigned int>(0, 0)) = 1.0f;
    }

    cout << "PREDICT: " << predicted << endl;
    cout << "LABEL: " << labels_test.t() << endl;

    cout << "Accuracy_{Bayes} = " << evaluate(predicted, labels_test) << endl;
}