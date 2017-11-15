#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

int thresh = 100;
RNG rng(12345);

int main(int argc, char* argv[]) {
	Mat src, HSV, threshold_output;
	std::vector< std::vector<Point> > contours;
    std::vector<Vec4i> hierarchy;
    
    if (argc < 2) {
        src = imread("./hands.jpg", 1);
    }
    else {
        src = imread(argv[1], 1);
    }
    
	if(!src.data) {
		std::cout << "Please add an image" << std::endl;
		return -1;
    }
    
	// Convert it to hsv
	cvtColor(src, HSV, COLOR_BGR2HSV);
    
	// Reduce the noise so we avoid false noise detection
	GaussianBlur(HSV, HSV, Size(11, 11), 2, 2);

	inRange(HSV, Scalar(0,0,0), Scalar(255,255,191), HSV);
    imshow("HSV", HSV);

    
	/// Detect edges using Threshold
    threshold( HSV, threshold_output, thresh, 255, THRESH_BINARY );
    
    /// Find contours
    findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    
    /// Find the convex hullp object for each contour
    std::vector< std::vector<Point> > hullp(contours.size());
    std::vector< std::vector<int>   > hulli(contours.size());
    std::vector< std::vector<Vec4i> > defects(contours.size()); 

    for( int i = 0; i < contours.size(); i++ ) {  
        convexHull( Mat(contours[i]), hullp[i], true );
        convexHull( Mat(contours[i]), hulli[i], true );
        if (hulli[i].size() > 3)
            convexityDefects(contours[i], hulli[i], defects[i]);
    }

    for( int i = 0; i < contours.size(); i++ ) {  
        std::cout << "Hull[" << i << "] size: " << hullp[i].size() << "\n";
        std::cout << "Defects[" << i << "] size: " << defects[i].size() << "\n";
    }

    /// Draw contours + hullp results
    Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ ) {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );
        drawContours( drawing, hullp, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );
    
        /// Draw convexityDefects
        for(int j=0; j<defects[i].size(); ++j) {
            const Vec4i& v = defects[i][j];
            float depth = v[3] / 256;
            //  filter defects by depth
            if (depth > 10) {
                int startidx = v[0]; Point ptStart(contours[i][startidx]);
                int endidx = v[1]; Point ptEnd(contours[i][endidx]);
                int faridx = v[2]; Point ptFar(contours[i][faridx]);

                line(drawing, ptStart, ptEnd, Scalar(0, 255, 0), 1);
                line(drawing, ptStart, ptFar, Scalar(0, 255, 0), 1);
                line(drawing, ptEnd, ptFar, Scalar(0, 255, 0), 1);
                circle(drawing, ptFar, 4, Scalar(0, 255, 0), 2);
            }
        }
    }
    
	// Show your results
	namedWindow("Convex Hull", CV_WINDOW_AUTOSIZE);
	imshow("Convex Hull", drawing);
    
	waitKey(0);
	return 0;
}
