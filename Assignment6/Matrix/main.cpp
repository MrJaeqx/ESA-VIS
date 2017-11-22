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

std::vector<Point> getCorners(std::string name, Mat src) {
    Mat drawing = src.clone();

    std::cout << "Bla\n";

    Mat threshold_output;
    std::vector<Vec4i> hierarchy;
    std::vector<Point> corners;
	cvtColor(src, src, COLOR_BGR2HSV);
    GaussianBlur(src, src, Size(5, 5), 0, 0);

    inRange(src, Scalar(22,0,0), Scalar(255,255,255), src);

    threshold( src, threshold_output, 100, 255, THRESH_BINARY );

    std::vector< std::vector<Point> > contours;
    findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    
    std::vector< std::vector<Point> > contours2;

    for( int i = 0; i < contours.size(); i++ ) {  
        if (contourArea(contours[i]) > 20000) contours2.push_back(contours[i]);
    }
    
    Scalar color = Scalar( 0, 0, 255 );

    for( int i = 0; i < contours2.size(); i++ ) {  
        std::cout << i << " " << contours2[i].size() << "\n";
        drawContours( drawing, contours2, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );
    }

    std::vector< std::vector<Point> > hullp(contours2.size());
    for( int i = 0; i < contours2.size(); i++ ) {  
        convexHull( Mat(contours2[i]), hullp[i], false );
    }

    color = Scalar( 255, 0, 0 );

    std::vector<Point> dingetje;

    for( int i = 0; i < hullp.size(); i++ ) {  
        drawContours( drawing, hullp, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );

        std::vector<Point> approx;
        approxPolyDP(Mat(hullp[i]), approx, arcLength(Mat(hullp[i]), true)*0.02, true);
        if (approx.size() == 4) dingetje = approx;
    }

    std::cout << "Dinges sz: " << dingetje.size() << "\n";

    color = Scalar( 0, 255, 0 );
    std::vector< std::vector<Point> > dingetje2;
    dingetje2.push_back(dingetje);

    for (int i = 0; i < dingetje2.size(); i++) {
        drawContours( drawing, dingetje2, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );
    }

    namedWindow(name.c_str(), WINDOW_NORMAL);
    resizeWindow(name.c_str(), 800, 480);
	imshow(name.c_str(), drawing);
    
    return corners;
}

int main(int argc, char* argv[]) {
	Mat src1, src2;
    
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
     * Find corners
     */
    std::vector<Point> imageAPoints = getCorners("A", src1);
    std::vector<Point> imageBPoints = getCorners("B", src2);

    //auto transform_Matrix = getMatrix(imageAPoints, imageBPoints);

	waitKey(0);
	return 0;
}
