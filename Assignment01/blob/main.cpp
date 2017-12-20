#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;

void processImage();
void displayGraphics();

Mat image;
Mat processedImage;
std::vector<Vec3b> colors;
Vec3b currColor;
int colorIndex = 0;

int main(int argc, char *argv[]) {
	if(argc > 1)
		image = imread(argv[1]);	
	else
		image = imread("blobs labelling.jpg");	

	if(image.empty())
		exit(1);

	// create our list of colors
	createColorTable(colors);
	colors.push_back(Vec3b(255, 0, 0));
	colors.push_back(Vec3b(0, 255, 0));
	colors.push_back(Vec3b(0, 0, 255));
	colors.push_back(Vec3b(255, 255, 0));
	colors.push_back(Vec3b(255, 0, 255));
	colors.push_back(Vec3b(0, 255, 255));
	colors.push_back(Vec3b(255, 255, 255));

	processImage();
	displayGraphics();

	// save image for external inspection
	imwrite("./res.png", processedImage);

	waitKey(0);
	return 0;
}

void displayGraphics() {
	imshow("Image", image);
	imshow("Processed Image", processedImage);
}

// we simply treat everything brighter than "half"
// as foreground
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

// keep checking pixels around the current pixel for being white and 
// match the current color, and continue with the just changed pixel
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

// loop over every pixel, but only call matchSurroundingColor if
// the pixel's white, and once that's done, change the color.
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
