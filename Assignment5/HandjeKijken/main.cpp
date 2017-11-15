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

void drawConvexityDefects(Mat & image , 
                          std::vector< std::vector<Point> > contours,     
                          std::vector< std::vector<Vec4i> > defects, 
                          std::vector< Point > & points,
                          int index, 
                          const Scalar & color, 
                          int lineThickness = 1, 
                          int lineType = 8) {
    for(int j=0; j<defects[index].size(); ++j) {
        const Vec4i& v = defects[index][j];
        float depth = v[3] / 256;
        //  filter defects by depth
        if (depth > 10) {
            int startidx = v[0]; Point ptStart(contours[index][startidx]);
            int endidx = v[1]; Point ptEnd(contours[index][endidx]);
            int faridx = v[2]; Point ptFar(contours[index][faridx]);
            points.push_back(ptEnd);
            line(image, ptStart, ptFar, color, lineThickness, lineType);
            line(image, ptEnd, ptFar, color, lineThickness, lineType);
            circle(image, ptFar, 4, color, 2*lineThickness, lineType);
        }
    }

}


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
        convexHull( Mat(contours[i]), hullp[i], false );
        convexHull( Mat(contours[i]), hulli[i], true );
        if (hulli[i].size() > 3)
            convexityDefects(contours[i], hulli[i], defects[i]);
    }

    for( int i = 0; i < contours.size(); i++ ) {  
        std::cout << "Hull[" << i << "] size: " << hullp[i].size() << "\n";
        std::cout << "Defects[" << i << "] size: " << defects[i].size() << "\n";
    }

    /// Draw contours + hullp results
    Mat drawing = src;
    std::vector<Point> points;
    for( int i = 0; i< contours.size(); i++ ) {
        Scalar color = Scalar( 0, 0, 255 );
        drawContours( drawing, contours, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );
        color = Scalar( 0, 255, 0 );
        drawContours( drawing, hullp, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );
        
        for (int j = 0; j < hullp[i].size(); j++) {
            circle(drawing, hullp[i][j], 8, color);
        }
    
        color = Scalar( 0, 255, 255 );
        drawConvexityDefects(drawing, contours, defects, points, i, color, 2);
        
    }



    /// Get the moments
    vector<Moments> mu(contours.size() );
    for( int i = 0; i < contours.size(); i++ ) { 
        mu[i] = moments( contours[i], false ); 
    }

    ///  Get the mass centers:
    vector<Point2f> mc( contours.size() );
    Point com;
    for( int i = 0; i < contours.size(); i++ ) { 
        Scalar color = Scalar( 0, 0, 255 );
        mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); 
        circle(drawing, mc[i], 8, color);
        com = mc[i];
    }

    for (auto p : points) {
        Scalar color = Scalar( 255, 0, 255 );
        circle(drawing, p, 8, color);
        line(drawing, com, p, color, 5);
    }

	// Show your results
	namedWindow("Convex Hull", WINDOW_NORMAL);
	imshow("Convex Hull", drawing);
    
	waitKey(0);
	return 0;
}
