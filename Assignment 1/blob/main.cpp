#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

//main functions
void processImage();
void displayGraphics();

//images
Mat image;
Mat processedImage;

int main(int argc, char *argv[]) {
  //load the image
  if(argc > 1)
  	image = imread(argv[1]);	
  else
  	image = imread("blobs labelling.jpg", CV_LOAD_IMAGE_GRAYSCALE);	

  if(image.empty())
  	exit(1);


  processImage();
  displayGraphics();

  waitKey(0);


  //no need to release memory
  return 0;
}

bool isForeground(Vec3b pixel) {
  int bgBright = 50;

  if (pixel[0] + pixel[1] + pixel[2] > bgBright)
    return true;
  return false;
}

void displayGraphics() {
  //display both images
  imshow("Image", image);
  imshow("Processed Image", processedImage);
}

Mat makeMonochrome(Mat input) {
  int x,y;
  for(y = 0; y < input.rows; y++) {
    for(x = 0; x < input.cols; x++) {
      Vec3b pixel = input.at<Vec3b>(y, x);
      if (isForeground(pixel)) {
        input.at<Vec3b>(y, x) = Vec3b(255,255,255);
      }
      else {
        input.at<Vec3b>(y, x) = Vec3b(0,0,0);
      }
    }
  }
  return input;
}

void processImage() {
  int x,y;
  Vec3b pixel;
  processedImage = makeMonochrome(image);
  //processedImage = image.clone();
  
  for(y = 0; y < processedImage.rows; y++) {
    for(x = 0; x < processedImage.cols; x++) {
      pixel = processedImage.at<Vec3b>(y, x);
      if (isForeground(pixel)) {
        processedImage.at<Vec3b>(y, x) = pixel;
      }
      else {
        processedImage.at<Vec3b>(y, x) = pixel;
      }
    }
  }
}
