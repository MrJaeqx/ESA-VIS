//objectTrackingTutorial.cpp

//Written by  Kyle Hounslow 2013

//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software")
//, to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// Used with permission as described above
// MINES-VIS  "Object Tracking"   Fontys University of Applied Science ICT - Technology 

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//IN THE SOFTWARE.

#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv/cv.h>

using namespace cv;

//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

//names that will appear at the top of each window
const std::string windowName = "Ball detection";

void drawCircles(Mat image, std::vector<Vec3f> circles) {
	for( size_t i = 0; i < circles.size(); i++ ) {
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
	      // circle center
		circle( image, center, 3, Scalar(0,255,0), -1, 8, 0 );
	      // circle outline
		circle( image, center, radius, Scalar(0,0,255), 3, 8, 0 );
	}
}

int main(int argc, char* argv[]) {
	//some boolean variables for different functionality within this
	//program
    bool trackObjects = false;
    bool useMorphOps = false;

	//Matrices
	Mat cameraFeed;
	Mat HSV;
	Mat threshold;
	Mat circletje;

	// Vectors for detected circles
	std::vector<Vec3f> circles;

	//video capture object to acquire webcam feed
	VideoCapture capture;

	//open capture object at location zero (default location for webcam)
	capture.open("MultipleColors.mp4");

	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);

	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop
	while(1) {
		//store image to matrix
		bool frame = capture.read(cameraFeed);

		// If end of video restart
		if(!frame) {
			capture.set(CV_CAP_PROP_POS_FRAMES, 0);
			continue;
		}
		
		//convert frame from BGR to HSV colorspace
		cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);

		//RED BALL
		inRange(HSV,Scalar(0,151,97),Scalar(3,256,148),threshold);
		GaussianBlur( threshold, circletje, Size(9, 9), 2, 2);
		HoughCircles( circletje, circles, CV_HOUGH_GRADIENT, 1, circletje.rows/8, 200, 30, 0, 35);
		drawCircles(cameraFeed, circles);

		//YELLOW BALL
		inRange(HSV,Scalar(5,116,109),Scalar(107,256,256),threshold);
		GaussianBlur( threshold, circletje, Size(9, 9), 2, 2);
		HoughCircles( circletje, circles, CV_HOUGH_GRADIENT, 1, circletje.rows/8, 200, 30, 0, 35);
		drawCircles(cameraFeed, circles);

		// BLUE BALL
		inRange(HSV,Scalar(78,148,0),Scalar(133,225,256),threshold);
		GaussianBlur( threshold, circletje, Size(9, 9), 2, 2);
		HoughCircles( circletje, circles, CV_HOUGH_GRADIENT, 1, circletje.rows/8, 200, 30, 0, 35);
		drawCircles(cameraFeed, circles);

		//show frames 
		imshow(windowName,cameraFeed);

		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(30);
	}

	return 0;
}