// Assignment 3 - Hough Lines
// Authors: Lars Jaeqx / Minh-Triet Diep
// Original from
// http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/hough_lines/hough_lines.html

#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

using namespace cv;

#define eps 0.1

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

    vector<Vec4i> lines;
    HoughLinesP(dst, lines, 1, 2*CV_PI/180, 50, 50, 10 );

    vector<LineThing> lineThings;

    for( size_t i = 0; i < lines.size(); i++ ) {
        Vec4i l = lines[i];
        line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 1, CV_AA);
        //printf("Line %d\n", i);

        double x1 = lines[i][0];
        double y1 = lines[i][1];
        double x2 = lines[i][2];
        double y2 = lines[i][3];

        // printf("x1: %d\n", x1);
        // printf("y1: %d\n", y1);
        // printf("x2: %d\n", x2);
        // printf("y2: %d\n", y2);        
        // y = a*x + b
        double a = (y2-y1)/(x2-x1);
        double b = y2 - (a * x2);

        //printf("y = %lfx + %lf\n", a, b);
        lineThings.push_back(LineThing(a, b));
    }

    printf("LineThings: %d\n", (int)lineThings.size());

    vector<int> okayIndexes;
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
    
    vector<LineThing> finalLines;
    for (int i = 0; i < okayIndexes.size(); i++) {
        finalLines.push_back(lineThings[okayIndexes[i]]);
    }

    for (int i = 0; i < finalLines.size(); i++) {
        printf("Line %d\n", i);
        printf("y = %lfx + %lf\n", finalLines[i].a, finalLines[i].b);
    }

    for( size_t i = 0; i < finalLines.size(); i++ ) {
        LineThing l = finalLines[i];
        // y = a*x+b
        double y1 = l.a*0.0+l.b;
        double y2 = l.a*cdst.cols+l.b;

        line( cdst, Point(0, (int)y1), Point(cdst.cols, (int)y2), Scalar(255,127,255), 1, CV_AA);
        
    }

    imshow("source", src);
    imshow("detected lines", cdst);

    waitKey();

    return 0;
}