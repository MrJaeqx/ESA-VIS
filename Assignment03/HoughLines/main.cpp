// Assignment 3 - Hough Lines
// Authors: Lars Jaeqx / Minh-Triet Diep
// Original from
// http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/hough_lines/hough_lines.html

#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <string>

using namespace cv;

double rad2deg(double rad) {
    return (rad*(180/M_PI));
}

struct LineThing {
    LineThing(double a_, double b_) : a(a_), b(b_) { }
    double a;
    double b;
};

void help() {
    std::cout << "\nThis program demonstrates line finding with the Hough transform.\n"
            "Usage:\n"
            "./houghlines <image_name>, Default is pic1.jpg\n" << std::endl;
}

bool isAboutEqual(LineThing a, LineThing b) {
    if  ((a.a > b.a - 0.10 && a.a < b.a + 0.10) &&
        (a.b > b.b - 10.0 && a.b < b.b + 10.0)) {
        return true;
    }
    return false;
}

int main(int argc, char** argv) {
    const char* filename = argc >= 2 ? argv[1] : "HoughPicture.jpg";

    Mat src = imread(filename, 0);
    if(src.empty()) {
        help();
        std::cout << "can not open " << filename << std::endl;
        return -1;
    }

    Mat dst, cdst;
    Canny(src, dst, 50, 200, 3);
    cvtColor(dst, cdst, CV_GRAY2BGR);
    GaussianBlur( cdst, cdst, Size(9,9), 2, 2 );
    
    std::vector<Vec4i> lines;
    HoughLinesP(dst, lines, 1, CV_PI/180.0, 50, 50, 10 );

    std::vector<LineThing> lineThings;

    // We use HoughLinesP to make OpenCV detect line pieces. This results in multiple lines
    // however, so we need to find out if the line pieces are on the line.
    for( size_t i = 0; i < lines.size(); i++ ) {
        Vec4i l = lines[i];
        line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 1, CV_AA);

        double x1 = lines[i][0];
        double y1 = lines[i][1];
        double x2 = lines[i][2];
        double y2 = lines[i][3];

        double a = (y2-y1)/(x2-x1);
        double b = y2 - (a * x2);

        lineThings.push_back(LineThing(a, b));
    }

    printf("LineThings: %d\n", (int)lineThings.size());

    // We make a new vector with lines that occur just once. This is FinalLines.
    std::vector<int> okayIndexes;
    for ( int i = 0; i < lineThings.size(); i++ ) {
        for (int j = 0; j < lineThings.size(); j++) {
            if (i == j) continue;
            if (isAboutEqual(lineThings[i], lineThings[j])) {
                if (std::find(okayIndexes.begin(), okayIndexes.end(), i) == okayIndexes.end() &&
                    std::find(okayIndexes.begin(), okayIndexes.end(), j) == okayIndexes.end()) {
                    okayIndexes.push_back(i);
                }
            }
        }
    }

    printf("okayIndexes: %d\n", (int)okayIndexes.size());
    
    std::vector<LineThing> finalLines;
    for (int i = 0; i < okayIndexes.size(); i++) {
        finalLines.push_back(lineThings[okayIndexes[i]]);
    }

    // We print the line formulas for the final lines
    for (int i = 0; i < finalLines.size(); i++) {
        printf("Line %d\n", i);
        printf("y = %lfx + %lf\n", finalLines[i].a, finalLines[i].b);
    }

    std::vector<Vec2i> finalLineVectors;    
    // And then we draw them.
    for( size_t i = 0; i < finalLines.size(); i++ ) {
        LineThing l = finalLines[i];
        // y = a*x+b
        double y1 = l.a*0.0+l.b;
        double y2 = l.a*cdst.cols+l.b;

        line( cdst, Point(0, (int)y1), Point(cdst.cols, (int)y2), Scalar(255,0,0), 1, CV_AA);
        finalLineVectors.push_back(Vec2i(cdst.cols-0, (int)y2-(int)y1));
    }

    // We now calculate the intersections of the lines (within the window).
    std::vector<Point> intersects;
    std::vector<double> angles;
    for (int i = 0; i < finalLines.size(); i++) {
        for (int j = 0; j < finalLines.size(); j++) {
            if (i == j) continue;
            double x = (finalLines[j].b - finalLines[i].b)/(finalLines[i].a - finalLines[j].a);
            double y = (finalLines[i].a*x + finalLines[i].b);
            if (x > 0.0 && x < (double)cdst.cols && y > 0.0 && y < (double)cdst.rows) {
                if (std::find(intersects.begin(), intersects.end(), Point(x, y)) == intersects.end()) {
                    intersects.push_back(Point(x, y));
                    circle(cdst , Point(x, y), 10, Scalar(0, 255, 0), 1, 8, 0);
                }
                // dot product of two vectors
                double numerator = finalLineVectors[i][0] * finalLineVectors[j][0] +finalLineVectors[i][1] * finalLineVectors[j][1];
                double denominator = sqrt(pow(finalLineVectors[i][0], 2)+pow(finalLineVectors[i][1], 2)) * sqrt(pow(finalLineVectors[j][0], 2)+pow(finalLineVectors[j][1], 2));
                double angle = acos(numerator/denominator);
                if (std::find(angles.begin(), angles.end(), angle) == angles.end()) {
                    angles.push_back(angle);
                }
            }
        }   
    }

    printf("We have %lu intersections\n", intersects.size());

    for (int i = 0; i < intersects.size(); i++) {
        printf("P%d:\n", i);
        printf("\tx: %d\n", intersects[i].x);
        printf("\ty: %d\n", intersects[i].y);

        double angle1 = rad2deg(angles[i]);
        double angle2 = 180.0 - angle1;

        // We can't know which directions the vectors need to be,
        // so we just give both angles. 
        printf("\tangle: %.2lf° or %.2lf°\n", angle1, angle2);

        char intersectTag[64];
        sprintf(intersectTag, "%d", i);
        int baseline = 1;
        Size size = getTextSize(intersectTag, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);
    
        bool flip = false;
        if (intersects[i].x + size.width >= cdst.cols) {
            flip = true;
        }

        putText(cdst, intersectTag, intersects[i], FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,255), 1, CV_AA, flip);
    }

    imshow("source", src);
    imshow("detected lines", cdst);
    imwrite("./hough_res.png", cdst);
    
    waitKey();

    return 0;
}
