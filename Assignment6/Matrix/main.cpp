#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

double rad2deg(double rad) {
    return (rad*(180/M_PI));
}

std::vector<Point> getCorners(std::string name, Mat src) {
    Mat drawing = src.clone();

    std::cout << "Bla\n";

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

    for( int i = 0; i < contours.size(); i++ ) {  
        if (contourArea(contours[i]) > 20000) contours2.push_back(contours[i]);
    }
    
    Scalar color = Scalar( 0, 0, 255 );

    for( int i = 0; i < contours2.size(); i++ ) {  
        std::cout << i << " " << contours2[i].size() << "\n";
        drawContours( drawing, contours2, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );
    }

    std::vector< std::vector<Point> > hullp(contours2.size());
    for( int i = 0; i < contours2.size(); i++ ) {  
        convexHull( Mat(contours2[i]), hullp[i], false );
    }

    color = Scalar( 255, 0, 0 );

    std::vector<Point> dingetje;

    for( int i = 0; i < hullp.size(); i++ ) {  
        drawContours( drawing, hullp, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );

        std::vector<Point> approx;
        approxPolyDP(Mat(hullp[i]), approx, arcLength(Mat(hullp[i]), true)*0.02, true);
        if (approx.size() == 4) dingetje = approx;
    }

    std::cout << "Dinges sz: " << dingetje.size() << "\n";

    color = Scalar( 0, 255, 0 );
    std::vector< std::vector<Point> > dingetje2;
    dingetje2.push_back(dingetje);

    for (int i = 0; i < dingetje2.size(); i++) {
        drawContours( drawing, dingetje2, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );
    }

    namedWindow(name.c_str(), WINDOW_NORMAL);
    resizeWindow(name.c_str(), 800, 480);
	imshow(name.c_str(), drawing);
    
    return dingetje;
}

Mat getTransposeMatrix(Mat src) {
    Mat dst(src.cols, src.rows, CV_64FC1);

    for(int i = 0; i < src.cols; i++) {
        for(int j = 0; j < src.rows; j++) {
            dst.at<double>(i,j) = src.at<double>(j,i);
        }
    }

    return dst;
}

double determinant(Mat src, int n) {
    Mat temp(n, n, CV_64FC1);
    double det = 0.0;
    int refCol, tempIndexRow, tempIndexCol;
    
    if(n == 1) {
        return src.at<double>(0,0);
    } else if (n == 2) {
        return (src.at<double>(0,0) * src.at<double>(1,1) - src.at<double>(0,1) * src.at<double>(1,0));
    } else {  
        for(refCol = 0; refCol < n; refCol++) {  
            tempIndexRow = 0;  

            for(int srcIndexRow = 1; srcIndexRow < n; srcIndexRow++) {  
                tempIndexCol = 0;

                for(int srcIndexCol = 0; srcIndexCol < n; srcIndexCol++) {
                    if (srcIndexCol == refCol) {
                        continue;
                    }

                    temp.at<double>(tempIndexRow,tempIndexCol) = src.at<double>(srcIndexRow,srcIndexCol);
                    tempIndexCol++;
                }
                tempIndexRow++;
            }
        det = det + (pow(-1,refCol) * src.at<double>(0,refCol) * determinant(temp, n - 1));
        }
        return det;
    }
}

double findDeterminant(Mat src) {
    if(src.rows == src.cols) {
        return determinant(src.clone(), src.rows);
    } else {
        return -1;
    }
}

Mat getMinor(Mat src, int row, int col) {
    Mat min(src.rows - 1, src.cols - 1, CV_64FC1);
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
                min.at<double>(tempIndexRow,tempIndexCol) = src.at<double>(i,j);
            }
        }
    }
    return min;
}

Mat getCofactor(Mat src) {
    Mat cofactor(src.rows, src.cols, CV_64FC1);
    for(int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; ++j)
        {
            Mat min = getMinor(src, i, j);
            cofactor.at<double>(i,j) = (min.at<double>(0,0) * min.at<double>(1,1) - min.at<double>(0,1) * min.at<double>(1,0)) * (double) pow(-1, i + j);
        }
    }
   return cofactor; 
}

Mat getInverseMatrix(Mat src) {
    if (src.rows != src.cols) {
        CV_Error( CV_StsBadArg, "Matrix needs to be square");
    }

    Mat inv(src.rows, src.cols, CV_64FC1);
    Mat cofactor = getCofactor(getTransposeMatrix(src));

    double det = findDeterminant(src);

    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            inv.at<double>(i,j) = cofactor.at<double>(i,j) / det;
        }
    }

    return inv;
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

    double dA[] = {1, 2, 3, 4, 0, 1, 4, 7, 5, 6, 0, 8};

    Mat M = Mat(3, 4, CV_64FC1, dA);

    Mat MTCV;
    Mat MTLJ = getTransposeMatrix(M).clone();
    transpose(M, MTCV);

    Mat MSQUARE = M.clone();
    //M.mul(MTLJ);

    //gemm(M, MTLJ, 1, MTLJ, 0, MSQUARE, 0);
    MSQUARE = (M*MTLJ);

    double MDLJ = findDeterminant(MSQUARE);
    double MDCV = determinant(MSQUARE);


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
    //std::vector<Point> imageAPoints = getCorners("A", src1);
    //std::vector<Point> imageBPoints = getCorners("B", src2);

    //auto transform_Matrix = getMatrix(imageAPoints, imageBPoints);

	waitKey(0);
	return 0;
}
