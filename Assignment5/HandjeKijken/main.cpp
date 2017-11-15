#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace cv;

int thresh = 100;
RNG rng(12345);

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

	inRange(HSV,Scalar(0,0,220),Scalar(255,255,255),HSV);

	// Reduce the noise so we avoid false circle detection
	GaussianBlur(HSV, HSV, Size(11, 11), 2, 2);


	/// Detect edges using Threshold
   threshold( HSV, threshold_output, thresh, 255, THRESH_BINARY );

   /// Find contours
   findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

   /// Find the convex hull object for each contour
   std::vector< std::vector<Point> > hull(contours.size());
   for( int i = 0; i < contours.size(); i++ )
      {  convexHull( Mat(contours[i]), hull[i], false ); }

   /// Draw contours + hull results
   Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
   for( int i = 0; i< contours.size(); i++ )
      {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );
        drawContours( drawing, hull, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );
      }


	// Show your results
	namedWindow("Convex Hull", CV_WINDOW_AUTOSIZE);
	imshow("Convex Hull", drawing);

	waitKey(0);
	return 0;
}
