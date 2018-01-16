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

// (C) 2018 Oswald Figaroa
// This function tries to find the 5-target pattern that looks like this
//  1  2  3
//  4     5
// It puts the targets in that order, and returns them.
std::vector<cv::Point2d> orderTargets(std::vector<cv::Point2d> allTargets) {
	std::vector<cv::Point2d> orderedTargets;
	unsigned int i1,i2,i3,i4,i5;
	unsigned int nCCC = allTargets.size();

	// Find 3 CCCs that are in a line. ATTENTION !!! These values are not the corrected ones !!
	double dMin = 1e9;		// distance from a CCC to the midpt between points 1,3
	double d13 = 1;			// the distance between points 1,3
	for (unsigned int i = 0; i<nCCC; i++)	{
		for (unsigned int j = i + 1; j<nCCC; j++)	{
			// Get the mid point between i,j.
			cv::Point2d midPt = (allTargets[i] + allTargets[j]) * 0.5;

			// Find the CCC that is closest to this midpoint.
			for (unsigned int k = 0; k<nCCC; k++)	{
				if (k == i || k == j)	continue;
				double d = norm(allTargets[k] - midPt);	// distance from midpoint

				if (d < dMin)	{
					// This is the minimum found so far; save it.
					dMin = d;
					i1 = i;
					i2 = k;
					i3 = j;
					d13 = norm(allTargets[i] - allTargets[j]);
				}
			}
		}
	}
	// If the best distance from the midpoint is < 30% of the distance between
	// the two other points, then we probably have our colinear set.
	if (dMin/d13 > 0.3)	return orderedTargets;	// return an empty list

	// We have found 3 colinear targets:  p1 -- p2 -- p3.
	// Now find the one closest to p1; call it p4.
	dMin = 1e9; // 
	for (unsigned int i=0; i<nCCC; i++)	{
		if (i!=i1 && i!=i2 && i!=i3)	{
			double d = norm(allTargets[i]-allTargets[i1]);
			if (d < dMin)	{
				dMin = d;
				i4 = i;
			}
		}
	}
	if (dMin > 1e7)	return orderedTargets;	// return an empty list

	// Now find the one closest to p3; call it p5.
	dMin = 1e9;
	for (unsigned int i=0; i<nCCC; i++)	{
		if (i!=i1 && i!=i2 && i!=i3 && i!=i4)	{
			double d = norm(allTargets[i]-allTargets[i3]);
			if (d < dMin)	{
				dMin = d;
				i5 = i;
			}
		}
	}
	if (dMin > 1e7)	return orderedTargets;	// return an empty list

	// Now, check to see where p4 is with respect to p1,p2,p3.  If the
	// signed area of the triangle p1-p3-p4 is negative, then we have
	// the correct order; ie
	//		1   2   3
	//		4		5
	// Otherwise we need to switch the order; ie
	//		3	2	1
	//		5		4

	// Signed area is the determinant of the 2x2 matrix [ p4-p1, p3-p1 ]
	cv::Vec2d p41 = allTargets[i4] - allTargets[i1];
	cv::Vec2d p31 = allTargets[i3] - allTargets[i1];
	double m[2][2] = { {p41[0], p31[0]}, {p41[1], p31[1]} };
	double det = m[0][0]*m[1][1] - m[0][1]*m[1][0];

	// Put the targets into the output list.
	if (det < 0)	{
		orderedTargets.push_back(allTargets[i1]);
		orderedTargets.push_back(allTargets[i2]);
		orderedTargets.push_back(allTargets[i3]);
		orderedTargets.push_back(allTargets[i4]);
		orderedTargets.push_back(allTargets[i5]);
	} else	{
		orderedTargets.push_back(allTargets[i3]);
		orderedTargets.push_back(allTargets[i2]);
		orderedTargets.push_back(allTargets[i1]);
		orderedTargets.push_back(allTargets[i5]);
		orderedTargets.push_back(allTargets[i4]);
	}

	return orderedTargets;
}

// (C) 2018 Oswald Figaroa
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

cv::Mat putImageInCorner(cv::Mat& p_OriginalImage, 
                         std::vector<cv::Point2d> p_Points) {
    cv::Mat warped, small_image;
    cv::Mat l_ResultImage = p_OriginalImage.clone();

    if (p_Points.size() < 5) return p_OriginalImage;

    cv::Point2f pointsFromImage[4];
    pointsFromImage[0] = (p_Points[0]);
    pointsFromImage[1] = (p_Points[2]);
    pointsFromImage[2] = (p_Points[3]);
    pointsFromImage[3] = (p_Points[4]);

    cv::Point2f pointsToMoveTo[4];
    pointsToMoveTo[0].x = 0;
    pointsToMoveTo[0].y = 0;

    pointsToMoveTo[2].x = 0;
    pointsToMoveTo[2].y = l_ResultImage.rows;

    pointsToMoveTo[1].x = l_ResultImage.cols;
    pointsToMoveTo[1].y = 0;

    pointsToMoveTo[3].x = l_ResultImage.cols;
    pointsToMoveTo[3].y = l_ResultImage.rows;

    cv::Mat l_Matrix = getPerspectiveTransform(pointsFromImage, pointsToMoveTo);
    warpPerspective(p_OriginalImage, warped, l_Matrix, l_ResultImage.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT);

    cv::Size size(FRAME_WIDTH/4,FRAME_HEIGHT/5);
    resize(warped,small_image,size);
    small_image.copyTo(l_ResultImage(cv::Rect(0,0,small_image.cols, small_image.rows)));

    return l_ResultImage;
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
        circles = orderTargets(circles);

        for( unsigned int i = 0; i < circles.size(); i++ ) {
            cv::Point2d center(cvRound(circles[i].x), cvRound(circles[i].y));
            // circle center
            circle( cameraFeed, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
            // circle outline
            circle( cameraFeed, center, 23, cv::Scalar(0,0,255), 3, 8, 0 );
            char text[32];
            sprintf(text, "Point %d", i);
            cv::putText(cameraFeed, text, center, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255,0,0), 1, CV_AA, false);
        }
        auto l_ResultImage = putImageInCorner(cameraFeed, circles);
		cv::imshow(windowName, l_ResultImage);

        //show frames 
        //cv::imshow(windowName, cameraFeed);

        //delay 30ms so that screen can refresh.
        //image will not appear without this waitKey() command
        cv::waitKey(30);
    }

    capture.release();

    return 0;
}