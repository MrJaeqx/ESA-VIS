#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>

// Parameters for finding CCC targets
#define DPIXEL  3.0             // max distance between centroids
const int FRAME_WIDTH = 1440;
const int FRAME_HEIGHT = 1080;
const std::string windowName = "Video enzo";

// Find all CCC targets on the image.  Return a vector (list) of their locations.
std::vector<cv::Point2d> findTargets(cv::Mat Image)
{
    std::vector<cv::Point2d> targets;

    cv::Mat gray;
    cv::Mat imageThresh;

    cv::cvtColor( Image, gray, CV_BGR2GRAY );

    // Do Otsu global thresholding.
    cv::threshold(gray,
        imageThresh,        // output thresholded image
        0,                  // threshold value
        255,                // output value
        cv::THRESH_OTSU | cv::THRESH_BINARY_INV);       // threshold_type - invert

    // Apply morphological operations to get rid of small (noise) regions
    cv::Mat structuringElmt = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3,3));
    cv::Mat imageOpen;
    morphologyEx(imageThresh, imageOpen, cv::MORPH_OPEN, structuringElmt);
    cv::Mat imageClose;
    morphologyEx(imageOpen, imageClose, cv::MORPH_CLOSE, structuringElmt);

    // Now find connected components
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(
        imageClose,             // input image (is destroyed)
        contours,               // output vector of contours
        hierarchy,              // hierarchical representation
        cv::RETR_CCOMP,         // retrieve all contours 
        cv::CHAIN_APPROX_NONE); // all pixels of each contours

    // Analyze components and find CCCs
    for(unsigned int i1 = 0; i1<(unsigned int)contours.size(); i1++) {
        int i2 = hierarchy[i1][2];
        if (i2 < 0) continue;           // See if it has a child inside

        // Compute centroids of inner and outer regions
        cv::Moments mu1 = cv::moments(contours[i1]);
        cv::Point2d x1(mu1.m10/mu1.m00, mu1.m01/mu1.m00);
        cv::Moments mu2 = cv::moments(contours[i2]);
        cv::Point2d x2(mu2.m10/mu2.m00, mu2.m01/mu2.m00);

        // Check if centroids coincide
        if (norm(x1-x2) > DPIXEL)   continue;

        // Check the "circularity" ratio of the outer region, which is 
        // the ratio of area to perimeter squared:  R = 4*pi*A/P^2.
        // R is 1 for a circle, and pi/4 for a square.
        double P1 = arcLength(contours[i1], true);
        double A1 = contourArea(contours[i1]);
        if (4*3.1415*A1/(P1*P1) < 3.1415/4)
            // Let's say that we want our region to be at least as round as a square.
            continue;

        // This must be a valid target; add it to the output list.
        targets.push_back(x1);
    }

    return targets;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " image" << std::endl;
        return -1;
    }

    std::string filename(argv[1]);

    //Matrices
    cv::Mat cameraFeed;

    //video capture object to acquire webcam feed
    cv::VideoCapture capture;

    //open capture object at location zero (default location for webcam)
    capture.open(filename);

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
            std::cout << "No more frames..." << std::endl;
            continue;
        }

        std::vector<cv::Point2d> circles;
        circles = findTargets(cameraFeed);

        for( unsigned int i = 0; i < circles.size(); i++ ) {
            cv::Point2d center(cvRound(circles[i].x), cvRound(circles[i].y));
            // circle center
            circle( cameraFeed, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
            // circle outline
            circle( cameraFeed, center, 23, cv::Scalar(0,0,255), 3, 8, 0 );
            char text[32];
            int baseline = 1;
            sprintf(text, "Point %d", i);
            cv::Size size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);
            cv::putText(cameraFeed, text, center, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255,0,0), 1, CV_AA, false);
           }

        //show frames 
        cv::imshow(windowName, cameraFeed);

        //delay 30ms so that screen can refresh.
        //image will not appear without this waitKey() command
        cv::waitKey(30);
    }

    capture.release();

    return 0;
}