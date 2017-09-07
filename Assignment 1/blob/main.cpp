#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;

//main functions
void processImage();
void displayGraphics();

//images
Mat image;
Mat processedImage;
std::vector<Vec3b> colors;
Vec3b currColor;
int colorIndex = 0;

int main(int argc, char *argv[]) {
	colors.push_back(Vec3b(255, 0, 0));
	colors.push_back(Vec3b(0, 255, 0));
	colors.push_back(Vec3b(0, 0, 255));
	colors.push_back(Vec3b(255, 255, 0));
	colors.push_back(Vec3b(255, 0, 255));
	colors.push_back(Vec3b(0, 255, 255));

	//load the image
	if(argc > 1)
		image = imread(argv[1]);	
	else
		image = imread("blobs labelling.jpg");	

	if(image.empty())
		exit(1);


	processImage();
	displayGraphics();

	imwrite("./res.png", processedImage);

	waitKey(0);


	//no need to release memory
	return 0;
}

void displayGraphics() {
	//display both images
	imshow("Image", image);
	imshow("Processed Image", processedImage);

}

bool isForeground(Vec3b &pixel) {
	int bgBright = 127*3;
	int total = pixel[0] + pixel[1] + pixel[2];
	if (total >= bgBright)
		return true;
	return false;
}

Mat makeMonochrome(Mat input) {
	Mat cpy = input.clone();
	int x,y;
	for(y = 0; y < cpy.rows; y++) {
		for(x = 0; x < cpy.cols; x++) {
			Vec3b &pixel = cpy.at<Vec3b>(y, x);
			if (isForeground(pixel)) {
				pixel = Vec3b(255,255,255);
			}
			else {
				pixel = Vec3b(0,0,0);
			}
		}
	}
	return cpy;
}

bool isWhite(Vec3b &pixel) {
	if (pixel[0] == 255 && 
		pixel[1] == 255 && 
		pixel[2] == 255)
		return true;
	return false;
}

void matchSurroundingColor(Vec3b &pixel, int y, int x) {
	for (int j = -1; j <= 1; j++) {
		if (y+j < 0 || y+j >= processedImage.rows) continue;
		for (int i = -1; i <= 1; i++) {
			if (x+i < 0 || x+i >= processedImage.cols) continue;
			Vec3b &newPixel = processedImage.at<Vec3b>(y+j, x+i);
			if (isWhite(newPixel)) {
				pixel = currColor;
				matchSurroundingColor(newPixel, y+j, x+i);
			}
		}
	}
}

void processImage() {
	int x,y;
	processedImage = makeMonochrome(image);
	currColor = colors[colorIndex];

	for(y = 0; y < processedImage.rows; y++) {
		for(x = 0; x < processedImage.cols; x++) {
			Vec3b &pixel = processedImage.at<Vec3b>(y, x);
			if (isWhite(pixel)) {
				matchSurroundingColor(pixel, y, x);
				colorIndex + 1 >= colors.size() ? colorIndex = 0 : colorIndex++;
				currColor = colors[colorIndex];
			}
		}
	}
}
