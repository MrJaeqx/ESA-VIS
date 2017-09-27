#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <math.h>

using namespace cv;

const Vec3b red = (Vec3b(0, 0, 255));
const Vec3b yel = (Vec3b(0, 255, 255));
const Vec3b blu = (Vec3b(255, 0, 0));
const Vec3b grn = (Vec3b(0, 255, 0));
const Vec3b blk = (Vec3b(0, 0, 0));

void colorGradient(Mat src_gray, Mat grad_x, Mat grad_y, Mat &derp) {
    for (int y = 0; y < src_gray.rows; y++) {
        for (int x = 0; x < src_gray.cols; x++) {
            if (src_gray.at<uchar>(y, x) < 127) {
                continue;
            }
            
            // scale results to 0.0 to 1.0
            short grad_x_px = grad_x.at<short>(y, x);
            double val_x_px = ((double)grad_x_px)/32767.0;

            short grad_y_px = grad_y.at<short>(y, x);
            double val_y_px = ((double)grad_y_px)/32767.0;

            // use atan2 to calculate a in tan(a) = dy/dx
            double gradient = atan2(val_y_px, val_x_px);

            // use degrees because it's easy to work with
            double degs = gradient * 180.0/M_PI;

            // since atan gives back between -pi, pi, and we scale 
            // to degrees and expect 0, 360, so just flip that region 
            if (degs < 0.0) {
                degs = 360.0 + degs;
            }

            if (degs > 0.0 && degs < 45.0) {
                derp.at<Vec3b>(y, x) = red;
            }
            else if (degs >= 45.0 && degs < 135.0) {
                derp.at<Vec3b>(y, x) = yel;
            }
            else if (degs >= 135.0 && degs < 225.0) {
                derp.at<Vec3b>(y, x) = blu;
            }
            else if (degs >= 225.0 && degs < 315.0) {
                derp.at<Vec3b>(y, x) = grn;
            }
            else if (degs >= 315.0 && degs < 360.0) {
                derp.at<Vec3b>(y, x) = red;
            }
            else {
                derp.at<Vec3b>(y, x) = blk;
                // we know that atan2(0, x) is always 0 and it doesn't
                // matter what x is, so we can't distinguish between a
                // vertical line and a white plane. We still have the 
                // data itself, so we can check for this separately. This
                // fixes the left edge being detected as black.
                if (val_y_px == 0.0 && val_x_px > 0.0) {
                    derp.at<Vec3b>(y, x) = red;                    
                }
            }
        }
    }
} 

int main(int argc, char ** argv) {
    Mat src, src_gray;
    Mat grad;
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;
  
    if(argc > 1) {
        src = imread(argv[1]);
    }
    else {
        src = imread("SobelGradient.png");
    }

    if(src.empty()) {
        printf("No image\n");
        exit(1);
    }

    // load the image as a grayscale
    cvtColor( src, src_gray, CV_BGR2GRAY );

    // make it either black or white for sharp edges
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
    imwrite("./sobel_step0.png", src_gray);    
    
    // fill the circle so there's just one edge
    floodFill(src_gray, cv::Point(src_gray.cols/2,src_gray.rows/2), Scalar(255));

    imshow("Sobel B/W", src_gray );
    imwrite("./sobel_step1.png", src_gray);
    
    Mat derp;
    cvtColor( src_gray, derp, CV_GRAY2BGR );

    // blur the image to get a bigger edge
    GaussianBlur( src_gray, src_gray, Size(15,15), 0, 0, BORDER_DEFAULT );
    imshow("Sobel B/W blurred", src_gray );
    imwrite("./sobel_step2.png", src_gray);
    
    // use the sobel function to generate the gradients
    Sobel( src_gray, grad_x, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT );
    Sobel( src_gray, grad_y, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT );
    
    // color the image with the gradient data
    colorGradient(src_gray, grad_x, grad_y, derp);
    
    imshow("Sobel Gradient", derp );
    imwrite("./sobel_res.png", derp);
    
    waitKey(0);    
    
    return 0;
}

