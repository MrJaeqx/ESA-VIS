#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

double rad2deg(double rad) {
    return (rad*(180/M_PI));
}

Mat getTransposeMatrix(Mat src) {
    Mat dst(src.cols, src.rows, CV_64FC1);

    for(double i = 0; i < src.cols; i++) {
        for(double j = 0; j < src.rows; j++) {
            dst.at<double>(i,j) = src.at<double>(j,i);
        }
    }

    return dst;
}

double findDeterminant(Mat src) {
    return 0;
}

Mat getInverseMatrix(Mat src) {

}

int main(int argc, char* argv[]) {
	Mat src1, src2;
	std::vector< std::vector<Point> > contours;
    std::vector<Vec4i> hierarchy;
    
    if (argc < 3) {
        src1 = imread("./ImageA.jpg", 1);
        src2 = imread("./ImageB.jpg", 1);
    }
    else {
        src1 = imread(argv[1], 1);
        src2 = imread(argv[2], 1);
    }
    
	if(!src1.data || !src2.data) {
		std::cout << "Oh no!" << std::endl;
		return -1;
    }

    double dA[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 ,12}; 

    Mat M = Mat(3,4, CV_64FC1, dA);
    Mat MTCV;
    Mat MTLJ = getTransposeMatrix(M);
    transpose(M, MTCV);

    Mat MSQUARE = M * MTCV;

    //double MDLJ = findDeterminant(MSQUARE);
    double MDCV = determinant(MSQUARE);


    //Mat MILJ = getInverseMatrix(MSQUARE);
    Mat MICV = MSQUARE.inv();
    
    
    

    std::cout << "M = "<< std::endl << " "  << M << std::endl << std::endl;

    std::cout << "M transposed custom= "<< std::endl << " "  << MTLJ << std::endl << std::endl;

    std::cout << "M transposed opencv= "<< std::endl << " "  << MTCV << std::endl << std::endl;

    std::cout << "M * M transposed= "<< std::endl << " "  << MSQUARE << std::endl << std::endl;

    //std::cout << "M * M transposed determinant custom = "<< std::endl << " "  << MDLJ << std::endl << std::endl;

    std::cout << "M * M transposed determinant opencv = "<< std::endl << " "  << MDCV << std::endl << std::endl;

    //std::cout << "M * M transposed inverse custom = "<< std::endl << " "  << MILJ << std::endl << std::endl;

    std::cout << "M * M transposed inverse opencv = "<< std::endl << " "  << MICV << std::endl << std::endl;
    
    /*
     * Code!
     */

	waitKey(0);
	return 0;
}
