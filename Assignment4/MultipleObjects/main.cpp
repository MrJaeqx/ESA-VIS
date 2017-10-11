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
#include <opencv2/core/core.hpp>
#include <math.h>

//#include "opencv2/core.hpp"
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/imgcodecs/imgcodecs.hpp>

using namespace cv;

//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 180;
int S_MIN = 182;
int S_MAX = 256;
int V_MIN = 149;
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

void drawAxis(Mat& img, Point p, Point q, Scalar colour, const float scale = 0.2)
{
    double angle;
    double hypotenuse;
    angle = atan2( (double) p.y - q.y, (double) p.x - q.x ); // angle in radians
    hypotenuse = sqrt( (double) (p.y - q.y) * (p.y - q.y) + (p.x - q.x) * (p.x - q.x));
//    double degrees = angle * 180 / CV_PI; // convert radians to degrees (0-180 range)
//    cout << "Degrees: " << abs(degrees - 180) << endl; // angle in 0-360 degrees range
    // Here we lengthen the arrow by a factor of scale
    q.x = (int) (p.x - scale * hypotenuse * cos(angle));
    q.y = (int) (p.y - scale * hypotenuse * sin(angle));
    line(img, p, q, colour, 1, CV_AA);
    // create the arrow hooks
    p.x = (int) (q.x + 9 * cos(angle + CV_PI / 4));
    p.y = (int) (q.y + 9 * sin(angle + CV_PI / 4));
    line(img, p, q, colour, 1, CV_AA);
    p.x = (int) (q.x + 9 * cos(angle - CV_PI / 4));
    p.y = (int) (q.y + 9 * sin(angle - CV_PI / 4));
    line(img, p, q, colour, 1, CV_AA);
}
double getOrientation(const vector<Point> &pts, Mat &img)
{
    //Construct a buffer used by the pca analysis
    int sz = static_cast<int>(pts.size());
    Mat data_pts = Mat(sz, 2, CV_64FC1);
    for (int i = 0; i < data_pts.rows; ++i)
    {
        data_pts.at<double>(i, 0) = pts[i].x;
        data_pts.at<double>(i, 1) = pts[i].y;
    }
    //Perform PCA analysis
    PCA pca_analysis(data_pts, Mat(), CV_PCA_DATA_AS_ROW);
    //Store the center of the object
    Point cntr = Point(static_cast<int>(pca_analysis.mean.at<double>(0, 0)),
                      static_cast<int>(pca_analysis.mean.at<double>(0, 1)));
    //Store the eigenvalues and eigenvectors
    vector<Point2d> eigen_vecs(2);
    vector<double> eigen_val(2);
    for (int i = 0; i < 2; ++i)
    {
        eigen_vecs[i] = Point2d(pca_analysis.eigenvectors.at<double>(i, 0),
                                pca_analysis.eigenvectors.at<double>(i, 1));
        eigen_val[i] = pca_analysis.eigenvalues.at<double>(0, i);
    }
    // Draw the principal components
    circle(img, cntr, 3, Scalar(255, 0, 255), 2);
    Point p1 = cntr + 0.02 * Point(static_cast<int>(eigen_vecs[0].x * eigen_val[0]), static_cast<int>(eigen_vecs[0].y * eigen_val[0]));
    Point p2 = cntr - 0.02 * Point(static_cast<int>(eigen_vecs[1].x * eigen_val[1]), static_cast<int>(eigen_vecs[1].y * eigen_val[1]));
    drawAxis(img, cntr, p1, Scalar(0, 255, 0), 1);
    drawAxis(img, cntr, p2, Scalar(255, 255, 0), 5);
    double angle = atan2(eigen_vecs[0].y, eigen_vecs[0].x); // orientation in radians

	char buffer[64];
	sprintf(buffer, "Angle: %.3f", angle*(180/M_PI));
	p1.x += 10;
	p1.y -= 10;
	putText(img, buffer, p1, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,255), 1, CV_AA, false);
	

    return angle;
}

int erosion_elem = 0;
int erosion_size = 10;
int dilation_elem = 0;
int dilation_size = 10;
int const max_elem = 2;
int const max_kernel_size = 21;

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

	//matrix for canny
	Mat canny_output;

	//x and y values for the location of the object
	int x=0, y=0;

	//create slider bars for HSV filtering
	createTrackbars();

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

		if(!frame) {
			capture.set(CV_CAP_PROP_POS_FRAMES, 0);
			continue;
		}
		
		//convert frame from BGR to HSV colorspace
		cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
		
		//filter HSV image between values and store filtered image to
		//threshold matrix
		inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold);
		
		{
			int erosion_type;
			if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
			else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
			else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
		
			Mat element = getStructuringElement( erosion_type,
												Size( 2*erosion_size + 1, 2*erosion_size+1 ),
												Point( erosion_size, erosion_size ) );
		
			/// Apply the erosion operation
			erode( threshold, threshold, element );
		}

		GaussianBlur( threshold, threshold, Size(21,21), 2, 2 );		

		{
			int dilation_type;
			if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
			else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
			else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
		
			Mat element = getStructuringElement( dilation_type,
												Size( 2*dilation_size + 1, 2*dilation_size+1 ),
												Point( dilation_size, dilation_size ) );
			/// Apply the dilation operation
			dilate( threshold, threshold, element );
		}

	    vector<Vec4i> hierarchy;
	    vector<vector<Point> > contours;
	    findContours(threshold, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	    for (size_t i = 0; i < contours.size(); ++i)
	    {
	        // Calculate the area of each contour
	        double area = contourArea(contours[i]);
	        // Ignore contours that are too small or too large
	        if (area < 1e4 || 1e5 < area) continue;
	        // Draw each contour only for visualisation purposes
	        drawContours(cameraFeed, contours, static_cast<int>(i), Scalar(0, 0, 255), 2, 8, hierarchy, 0);
	        // Find the orientation of each shape
	        getOrientation(contours[i], cameraFeed);
	    }

		//show frames 
		imshow(windowName2,threshold);
		imshow(windowName,cameraFeed);
		imshow(windowName1,HSV);
		//imshow(windowName3,drawing);

		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(30);
	}

	return 0;
}

