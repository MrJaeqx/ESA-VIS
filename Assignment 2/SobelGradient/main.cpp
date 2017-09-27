#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <math.h>

using namespace cv;

Vec3b red = (Vec3b(0, 0, 255));
Vec3b yel = (Vec3b(0, 255, 255));
Vec3b blu = (Vec3b(255, 0, 0));
Vec3b grn = (Vec3b(0, 255, 0));
Vec3b blk = (Vec3b(0, 0, 0));


int main(int argc, char ** argv) {
    Mat src, src_gray;
    Mat grad;
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;
  
    //load the image
    if(argc > 1) {
        src = imread(argv[1]);
    }
    else {
        src = imread("SobelGradient.png");
    }

    if(src.empty()) {
        exit(1);
    }

    cvtColor( src, src_gray, CV_BGR2GRAY );

    for (int y = 0; y < src_gray.rows; y++) {
        for (int x = 0; x < src_gray.cols; x++) {
            if (src_gray.at<uchar>(y, x) < 127) {
                src_gray.at<uchar>(y, x) = 0;
            }
            else {
                src_gray.at<uchar>(y, x) = 255;                
            }
        }
    }

    //imshow("Sobel B/W", src_gray );
    
    floodFill(src_gray, cv::Point(src_gray.cols/2,src_gray.rows/2), Scalar(255));
    
    
    Mat derp;
    cvtColor( src_gray, derp, CV_GRAY2BGR );
    
    GaussianBlur( src_gray, src_gray, Size(15,15), 0, 0, BORDER_DEFAULT );
    imshow("Sobel B/W", src_gray );
    
    Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    
    
    for (int y = 0; y < src_gray.rows; y++) {
        for (int x = 0; x < src_gray.cols; x++) {
            if (src_gray.at<uchar>(y, x) < 127) {
                continue;
            }
            
            short grad_x_px = grad_x.at<short>(y, x);
            float val_x_px = ((float)grad_x_px)/32767.0f;

            short grad_y_px = grad_y.at<short>(y, x);
            float val_y_px = ((float)grad_y_px)/32767.0f;

            float gradient = atan2(val_y_px, val_x_px);
            float degs = gradient * 180.0/M_PI;

            if (degs < 0) {
                degs = 360.0f + degs;                
            }

            if (degs >= 1.0f && degs < 45.0f) {
                derp.at<Vec3b>(y, x) = red;
            }
            else if (degs >= 45.0f && degs < 135.0f) {
                derp.at<Vec3b>(y, x) = yel;                
            }
            else if (degs >= 135.0f && degs < 225.0f) {
                derp.at<Vec3b>(y, x) = blu;                
            }
            else if (degs >= 225.0f && degs < 315.0f) {
                derp.at<Vec3b>(y, x) = grn;       
            }
            else if (degs >= 315.0f && degs < 360.0f) {
                derp.at<Vec3b>(y, x) = red;  
            }
            else {
                derp.at<Vec3b>(y, x) = blk;
            }
        }
    }
    
    imshow("Sobel Gradient", derp );
    imwrite("./res.png", derp);
    
    waitKey(0);    
    
    return 0;
}

