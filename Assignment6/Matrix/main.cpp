#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

float rad2deg(float rad) {
    return (rad*(180/M_PI));
}

std::vector<Point> getCorners(std::string name, Mat src) {
    Mat drawing = src.clone();

    Mat threshold_output;
    std::vector<Vec4i> hierarchy;
    std::vector<Point> corners;
	cvtColor(src, src, COLOR_BGR2HSV);
    GaussianBlur(src, src, Size(5, 5), 0, 0);

    inRange(src, Scalar(22,0,0), Scalar(255,255,255), src);

    threshold( src, threshold_output, 100, 255, THRESH_BINARY );

    std::vector< std::vector<Point> > contours;
    findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    
    std::vector< std::vector<Point> > contours2;

    for( uint i = 0; i < contours.size(); i++ ) {  
        if (contourArea(contours[i]) > 20000) contours2.push_back(contours[i]);
    }
    
    Scalar color = Scalar( 0, 0, 255 );

    for( uint i = 0; i < contours2.size(); i++ ) {  
        drawContours( drawing, contours2, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );
    }

    std::vector< std::vector<Point> > hullp(contours2.size());
    for( uint i = 0; i < contours2.size(); i++ ) {  
        convexHull( Mat(contours2[i]), hullp[i], false );
    }

    color = Scalar( 255, 0, 0 );

    std::vector<Point> dingetje;

    for( uint i = 0; i < hullp.size(); i++ ) {  
        drawContours( drawing, hullp, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );

        std::vector<Point> approx;
        approxPolyDP(Mat(hullp[i]), approx, arcLength(Mat(hullp[i]), true)*0.02, true);
        if (approx.size() == 4) dingetje = approx;
    }

    color = Scalar( 0, 255, 0 );
    std::vector< std::vector<Point> > dingetje2;
    dingetje2.push_back(dingetje);

    for (uint i = 0; i < dingetje2.size(); i++) {
        drawContours( drawing, dingetje2, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );
    }

    namedWindow(name.c_str(), WINDOW_NORMAL);
    resizeWindow(name.c_str(), 800, 480);
	imshow(name.c_str(), drawing);
    
    return dingetje;
}

Mat getTransposeMatrix(Mat src) {
    Mat dst(src.cols, src.rows, CV_32FC1);

    for(int i = 0; i < src.cols; i++) {
        for(int j = 0; j < src.rows; j++) {
            dst.at<float>(i,j) = src.at<float>(j,i);
        }
    }

    return dst;
}

float determinant(Mat src, int n) {
    Mat temp(n, n, CV_32FC1);
    float det = 0.0;
    int refCol, tempIndexRow, tempIndexCol;
    
    if(n == 1) {
        return src.at<float>(0,0);
    } else if (n == 2) {
        return (src.at<float>(0,0) * src.at<float>(1,1) - src.at<float>(0,1) * src.at<float>(1,0));
    } else {  
        for(refCol = 0; refCol < n; refCol++) {  
            tempIndexRow = 0;  

            for(int srcIndexRow = 1; srcIndexRow < n; srcIndexRow++) {  
                tempIndexCol = 0;

                for(int srcIndexCol = 0; srcIndexCol < n; srcIndexCol++) {
                    if (srcIndexCol == refCol) {
                        continue;
                    }

                    temp.at<float>(tempIndexRow,tempIndexCol) = src.at<float>(srcIndexRow,srcIndexCol);
                    tempIndexCol++;
                }
                tempIndexRow++;
            }
        det = det + (pow(-1,refCol) * src.at<float>(0,refCol) * determinant(temp, n - 1));
        }
        return det;
    }
}

float findDeterminant(Mat src) {
    if(src.rows == src.cols) {
        return determinant(src.clone(), src.rows);
    } else {
        return -1;
    }
}

Mat getMinor(Mat src, int row, int col) {
    Mat min(src.rows - 1, src.cols - 1, CV_32FC1);
    int tempIndexRow = 0;
    int tempIndexCol = 0;

    for (int i = 0; i < src.rows; i++) {
        tempIndexRow = i;
        if (i > row) {
            tempIndexRow--;
        }
        for (int j = 0; j < src.cols; j++) {
            tempIndexCol = j;
            if (j > col) {
                tempIndexCol--;
            }
            if (i != row && j != col) {
                min.at<float>(tempIndexRow,tempIndexCol) = src.at<float>(i,j);
            }
        }
    }
    return min;
}

Mat getCofactor(Mat src) {
    Mat cofactor(src.rows, src.cols, CV_32FC1);
    for(int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; ++j)
        {
            Mat min = getMinor(src, i, j);
            cofactor.at<float>(i,j) = (min.at<float>(0,0) * min.at<float>(1,1) - min.at<float>(0,1) * min.at<float>(1,0)) * (float) pow(-1, i + j);
        }
    }
   return cofactor; 
}

Mat getInverseMatrix(Mat src) {
    if (src.rows != src.cols) {
        CV_Error( CV_StsBadArg, "Matrix needs to be square");
    }

    Mat inv(src.rows, src.cols, CV_32FC1);
    Mat cofactor = getCofactor(getTransposeMatrix(src));

    float det = findDeterminant(src);

    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            inv.at<float>(i,j) = cofactor.at<float>(i,j) / det;
        }
    }

    return inv;
}

float getRotationCos(Mat src) {
    return acos(src.at<float>(0,0));
}

float getRotationSin(Mat src) {
    return asin(src.at<float>(1,0));
}

float getTransformationX(Mat src) {
    return src.at<float>(2,0);
}

float getTransformationY(Mat src) {
    return src.at<float>(3,0);
}

Mat getTransformationMatrix(Mat a, Mat b) {
    Mat aT = getTransposeMatrix(a);

    std::cout << "a = "<< std::endl << " "  << a << std::endl << std::endl;

    std::cout << "aT = "<< std::endl << " "  << aT << std::endl << std::endl;


    Mat aPseudo = a * aT;

    std::cout << "aPseudo = "<< std::endl << " "  << aPseudo << std::endl << std::endl;

    Mat aInv = getInverseMatrix(aPseudo);

    std::cout << "aInv = "<< std::endl << " "  << aInv << std::endl << std::endl;

    Mat aDing = aInv * aT;

    std::cout << "aDing = "<< std::endl << " "  << aDing << std::endl << std::endl;

    //Mat aDingMetB = aDing * b.clone();

    //std::cout << "aDingMetB = "<< std::endl << " "  << aDingMetB << std::endl << std::endl;

//    Mat aInv = getInverseMatrix(a * aT).clone();
//    return aInv * (aT * b);
    return aPseudo;
}

int main(int argc, char* argv[]) {
	Mat src1, src2;
    
    if (argc < 3) {
        src1 = imread("./ImageA.jpg", 1);
        src2 = imread("./ImageB.jpg", 1);
    }
    else {
        src1 = imread(argv[1], 1);
        src2 = imread(argv[2], 1);
    }
    
	if(!src1.data || !src2.data) {
		std::cout << "Oh no!" << std::endl;
		return -1;
    }

    float dAasdas[] = {1, 2, 3, 4, 0, 1, 4, 7, 5, 6, 0, 8};

    Mat M = Mat(3, 4, CV_32FC1, dAasdas);

    Mat MTCV;
    Mat MTLJ = getTransposeMatrix(M).clone();
    transpose(M, MTCV);

    Mat MSQUARE = M.clone();
    
    MSQUARE = (M*MTLJ);

    float MDLJ = findDeterminant(MSQUARE);
    float MDCV = determinant(MSQUARE);


    Mat MILJ = getInverseMatrix(MSQUARE);
    Mat MICV;
    invert(MSQUARE, MICV, DECOMP_SVD);

    Mat MPLJ = MTLJ * MILJ;
    Mat MPCV = MTCV * MICV;

    std::cout << "M = "<< std::endl << " "  << M << std::endl << std::endl;

    std::cout << "M transposed custom= "<< std::endl << " "  << MTLJ << std::endl << std::endl;

    std::cout << "M transposed opencv= "<< std::endl << " "  << MTCV << std::endl << std::endl;

    std::cout << "M * M transposed= "<< std::endl << " "  << MSQUARE << std::endl << std::endl;

    std::cout << "M * M transposed determinant custom = "<< std::endl << " "  << MDLJ << std::endl << std::endl;

    std::cout << "M * M transposed determinant opencv = "<< std::endl << " "  << MDCV << std::endl << std::endl;

    std::cout << "M * M transposed inverse custom"<< std::endl << " "  << MILJ << std::endl << std::endl;

    std::cout << "M * M transposed inverse opencv = "<< std::endl << " "  << MICV << std::endl << std::endl;
    
    std::cout << "(M * Mt) * Mt custom = "<< std::endl << " "  << MPLJ << std::endl << std::endl;

    std::cout << "(M * Mt) * Mt opencv = "<< std::endl << " "  << MPCV << std::endl << std::endl;

    /*
     * Find corners
     */
    std::vector<Point> imageAPoints = getCorners("A", src1);
    std::vector<Point> imageBPoints = getCorners("B", src2);

    std::vector<Point2f> iAP2;
    std::vector<Point2f> iBP2;
    
    for (auto p : imageAPoints) {
        iAP2.push_back(Point2d((float)p.x, (float)p.y));
    }
    for (auto p : imageBPoints) {
        iBP2.push_back(Point2d((float)p.x, (float)p.y));
    }
    
    

    float dA[] = {
        iAP2[0].x, -iAP2[0].y, 1, 0,
        iAP2[1].y, iAP2[1].x, 0, 1,
        iAP2[2].x, -iAP2[2].y, 1, 0,
        iAP2[3].y, iAP2[3].x, 0, 1
    };

    Mat pointsA = Mat(4, 4, CV_32FC1, dA);

    float dB[] {
        iBP2[0].x, iBP2[0].y,
        iBP2[1].x, iBP2[1].y,
        iBP2[2].x, iBP2[2].y,
        iBP2[3].x, iBP2[3].y
    };

    Mat pointsB = Mat(8, 1, CV_32FC1, dB);
    
    auto transformMatrix = getTransformationMatrix(pointsA, pointsB);

    Mat cvTransformMatrix = getPerspectiveTransform(iAP2, iBP2);

    std::cout << "transformMatrix = "<< std::endl << " "  << transformMatrix << std::endl << std::endl;
    std::cout << "cvTransformMatrix = "<< std::endl << " "  << cvTransformMatrix << std::endl << std::endl;

	waitKey(0);
	return 0;
}
