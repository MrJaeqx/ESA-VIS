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
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>

using namespace cv;

//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 170;
int S_MAX = 256;
int V_MIN = 120;
int V_MAX = 256;

//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=50;

//minimum and maximum object area
const int MIN_OBJECT_AREA = 20*20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;

//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Blur";
const string trackbarWindowName = "Trackbars";

void on_trackbar( int, void* ) {
	//This function gets called whenever a
	// trackbar position is changed
}

string intToString(int number){
	std::stringstream ss;
	ss << number;
	return ss.str();
}

void createTrackbars(){
	//create window for trackbars

    namedWindow(trackbarWindowName,0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf( TrackbarName, "H_MIN %d", H_MIN);
	sprintf( TrackbarName, "H_MAX %d", H_MAX);
	sprintf( TrackbarName, "S_MIN %d", S_MIN);
	sprintf( TrackbarName, "S_MAX %d", S_MAX);
	sprintf( TrackbarName, "V_MIN %d", V_MIN);
	sprintf( TrackbarName, "V_MAX %d", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
    createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar );
    createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar );
    createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar );
    createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar );
    createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar );
    createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar );
}

int main(int argc, char* argv[]) {
	//some boolean variables for different functionality within this
	//program
    bool trackObjects = false;
    bool useMorphOps = false;

	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;

	//matrix storage for HSV image
	Mat HSV;

	//matrix storage for binary threshold image
	Mat threshold;

	//matrix for HoughCircles
	Mat circletje;

	//x and y values for the location of the object
	int x=0, y=0;

	//create slider bars for HSV filtering
	createTrackbars();

	//video capture object to acquire webcam feed
	VideoCapture capture;

	//open capture object at location zero (default location for webcam)
	capture.open("SingleColors.mp4");

	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);

	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop
	while(1) {
		//store image to matrix
		bool frame = capture.read(cameraFeed);

		if(!frame) {
			capture.set(CV_CAP_PROP_POS_FRAMES, 0);
			continue;
		}
		
		//convert frame from BGR to HSV colorspace
		cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
		
		//filter HSV image between values and store filtered image to
		//threshold matrix
		inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold);
		
		/// Reduce the noise so we avoid false circle detection
		GaussianBlur( threshold, circletje, Size(9, 9), 2, 2);

		vector<Vec3f> circles;

		/// Apply the Hough Transform to find the circles
		HoughCircles( circletje, circles, CV_HOUGH_GRADIENT, 1, circletje.rows/8, 200, 30, 0, 35);

		/// Draw the circles detected
		for( size_t i = 0; i < circles.size(); i++ )
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
		      // circle center
			circle( cameraFeed, center, 3, Scalar(0,255,0), -1, 8, 0 );
		      // circle outline
			circle( cameraFeed, center, radius, Scalar(0,0,255), 3, 8, 0 );
		}

		//show frames 
		imshow(windowName2,threshold);
		imshow(windowName,cameraFeed);
		imshow(windowName1,HSV);
		imshow(windowName3,circletje);

		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(30);
	}

	return 0;
}

