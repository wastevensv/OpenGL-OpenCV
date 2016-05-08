#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

RNG rng(12345);
const int thresh = 100;

Mat whiteFilter(const Mat& src) {
    assert(src.type() == CV_8UC3);
    
    Mat hsv;
    cvtColor(src,hsv,CV_BGR2HSV);
    
    Mat colorOnly;
    inRange(hsv, Scalar(0, 0, 200), Scalar(255, 50, 255), colorOnly);

    return colorOnly;
}

Mat equalizeIntensity(const Mat& inputImage)
{
    if(inputImage.channels() >= 3)
    {
        Mat hsv;
        cvtColor(inputImage,hsv,CV_BGR2HSV);

        vector<Mat> channels;
        split(hsv,channels);
        for(int i = 1; i < 3; i++) {
          equalizeHist(channels[i], channels[i]);
        }
        Mat result;
        merge(channels,result);

        Mat outputImage;
        cvtColor(result,outputImage,CV_HSV2BGR);
        return outputImage;
    }
    return Mat();
}

vector<float> findObject(const Mat& inputImage) {
    //Mat normFrame = equalizeIntensity(inputImage);
    //Mat filtered = whiteFilter(normFrame);       
    Mat filtered = whiteFilter(inputImage);       


    Mat canny_output;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    // Blur Image
    blur(filtered, filtered, Size(5,5));

    // Detect edges using canny
    Canny( filtered, canny_output, thresh, thresh*2, 3 );

    // Find contours
    findContours( canny_output, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    // Draw contours
    float cx = 0.0f;
    float cy = 0.0f;
    bool found = false;
    double maxarea = 50;
    for( int i = 0; i< contours.size(); i++ )
    {
        double area = contourArea(contours[i]);
        drawContours( filtered, contours, i, Scalar(127,127,127), 2, 8, hierarchy, 0, Point() ); 
        if(area > maxarea) {
            maxarea = area;
            Moments M = moments(contours[i]);
            cx = (M.m10/M.m00);
            cy = (M.m01/M.m00);
            found = true;
        }
    }
    imshow("filter", filtered);
    //Show in a window
    if(found) {
        circle(filtered, Point(cx, cy), 5, Scalar(127, 127, 127));
        cerr << cx << ", " << cy << endl;
        vector<float> point = vector<float>();
        point.push_back(cx);
        point.push_back(cy);
        return point;
    } else
        return vector<float>();
}
