#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

double rad2deg(double rad) {
    return (rad*(180/M_PI));
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
    
    /*
     * Code!
     */

	waitKey(0);
	return 0;
}
