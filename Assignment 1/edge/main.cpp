#include <opencv2/opencv.hpp>

using namespace cv;

void processImage4();
void processImage8();
void processImageUsingOpenCVFunctions();
void processImageGradient();
void displayGraphics();

Mat image, processedImage4, processedImage8, processedImageCV, processedGradient, kernel;
int kernel_size = 3;
int scale = 4;
int delta = 0;
int ddepth = -1;

int main(int argc, char *argv[])
{
  namedWindow("Image");
  namedWindow("Processed Image 4");
  namedWindow("Processed Image 8");
  namedWindow("Processed openCV");
  namedWindow("Processed Gradient");

//load the image
  if(argc > 1)
   image = imread(argv[1]);	
 else
   image = imread("edge detection.jpg");	

 if(image.empty())
   exit(1);


 processImage4();
 processImage8();
 processImageUsingOpenCVFunctions();
 processImageGradient();
 displayGraphics();

 waitKey(0);
 return 0;
}

void displayGraphics()
{
  imshow("Image", image);
  imshow("Processed Image 4", processedImage4);
  imshow("Processed Image 8", processedImage8);
  imshow("Processed openCV", processedImageCV);
  imshow("Processed Gradient", processedGradient);
}

void processImageUsingOpenCVFunctions()
{
  GaussianBlur( image, processedImageCV, Size(3,3), 0, 0, BORDER_DEFAULT );
  cvtColor( processedImageCV, processedImageCV, CV_BGR2GRAY );
  Laplacian( processedImageCV, processedImageCV, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( processedImageCV, processedImageCV );
}

void processImage4()
{
    kernel_size = 3;
    kernel = (Mat_<double>(3,3) << 0, -1, 0, -1, 4, -1, 0, -1, 0);
    cvtColor( image, processedImage4, CV_BGR2GRAY );
    filter2D(processedImage4, processedImage4, ddepth , kernel, Point( -1, -1 ), delta, BORDER_DEFAULT );
}

void processImage8()
{
    kernel_size = 3;
    kernel = (Mat_<double>(3,3) << 0, -1, 0, -1, 8, -1, 0, -1, 0);
    cvtColor( image, processedImage8, CV_BGR2GRAY );
    filter2D(processedImage8, processedImage8, ddepth , kernel, Point( -1, -1 ), delta, BORDER_DEFAULT );
}

void processImageGradient()
{
    kernel_size = 3;
    kernel = (Mat_<double>(3,3) << -1, 0, 1, -1, 0, 1, -1, 0, 1);
    cvtColor( image, processedGradient, CV_BGR2GRAY );
    filter2D(processedGradient, processedGradient, ddepth , kernel, Point( -1, -1 ), delta, BORDER_DEFAULT );
}