#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace cv;

int thresh = 100;
int dilation_size = 5;
Scalar red = Scalar(0, 0, 255);
Scalar green = Scalar(0, 255, 0);

int main(int argc, char* argv[]) {
	Mat src, HSV, threshold_output;
	std::vector< std::vector<Point> > contours;
	std::vector<Vec4i> hierarchy;

	// Read the image
	src = imread(argv[1], 1);

	if(!src.data) {
		std::cout << "Please add an image" << std::endl;
		return -1;
	}

	// Convert it to gray
	cvtColor(src, HSV, COLOR_BGR2HSV);

	inRange(HSV,Scalar(0,0,40),Scalar(10,30,230),HSV);

	// Reduce the noise so we avoid false circle detection
	GaussianBlur(HSV, HSV, Size(11, 11), 2, 2);


	// Detect edges using Threshold
	threshold( HSV, threshold_output, thresh, 255, THRESH_BINARY );

	Mat element = getStructuringElement( MORPH_RECT,
                                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                       Point( dilation_size, dilation_size ) );
  /// Apply the dilation operation
  dilate( threshold_output, threshold_output, element );

	// Find contours
	findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	// Find the convex hull object for each contour
	std::vector< std::vector<Point> > hull(contours.size());
	for( int i = 0; i < contours.size(); i++ ) {
		convexHull( Mat(contours[i]), hull[i], false );
		std::cout << "Ding" << std::endl;
	}

	///Draw contours + hull results
	Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
	for( int i = 0; i< contours.size(); i++ ) {
		drawContours( drawing, contours, i, red, 1, 8, std::vector<Vec4i>(), 0, Point() );
		drawContours( drawing, hull, i, green, 1, 8, std::vector<Vec4i>(), 0, Point() );
	}

	// Show your results
	namedWindow("Convex Hull", CV_WINDOW_AUTOSIZE);
	imshow("Convex Hull", HSV);

	waitKey(0);
	return 0;
}
