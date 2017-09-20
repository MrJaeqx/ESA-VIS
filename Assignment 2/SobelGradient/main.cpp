#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <math.h>

using namespace cv;

Vec3b red = (Vec3b(0, 0, 255));
Vec3b yel = (Vec3b(0, 255, 255));
Vec3b blu = (Vec3b(255, 0, 0));
Vec3b grn = (Vec3b(0, 255, 0));

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

    //GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
    cvtColor( src, src_gray, CV_BGR2GRAY );

    /// Gradient X
    Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    //convertScaleAbs( grad_x, abs_grad_x );

    /// Gradient Y
    Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    //convertScaleAbs( grad_y, abs_grad_y );

    //addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

    //Mat angles;
    //phase(grad_x,grad_y,angles,true);
    
    Mat derp;
    derp = src.clone();

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
                //degs = degs * -1.0f;
                degs = 360.0f + degs;                
            }

            if (degs < 45.0f) {
                derp.at<Vec3b>(y, x) = red;
            }
            if (degs >= 45.0f && degs < 135.0f) {
                derp.at<Vec3b>(y, x) = yel;                
            }
            if (degs >= 135.0f && degs < 225.0f) {
                derp.at<Vec3b>(y, x) = blu;                
            }
            if (degs >= 225.0f && degs < 315.0f) {
                derp.at<Vec3b>(y, x) = grn;       
            }
            if (degs >= 315.0f && degs < 360.0f) {
                derp.at<Vec3b>(y, x) = red;  
            }
        }
    }

    /// Total Gradient (approximate)
    //addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
        
    imshow("Sobel Gradient", derp );
    //imshow("Sobel Gradient2", grad );
    

    waitKey(0);    
    
    return 0;
}
