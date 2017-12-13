#include <opencv2/opencv.hpp>
using namespace cv;

//default capture width and height
//const int FRAME_WIDTH = 640;
//const int FRAME_HEIGHT = 480;
const std::string windowName = "Distance";
const double true_size = 210.0; //mm
const double focal = 3.67; // mm

struct CameraParams {
	CameraParams(Mat matrix, Mat coeff, double focal, double res, Size2i size) : 
		camera_matrix(matrix), 
		distortion_coefficients(coeff), 
		focal(focal), 
		resolution(res),
		size(size) { }
	Mat camera_matrix;
	Mat distortion_coefficients;
	double focal;
	double resolution;
	Size2i size;
};

double checkerboardDistance(CameraParams params, Mat &image, bool verbose = false) {
	Mat undistortedImage;

	undistort(image, undistortedImage, params.camera_matrix, params.distortion_coefficients);

	std::vector<Point> ptVec;
	std::vector<Point2f> pt2fVec;

	bool found = findChessboardCorners( 
		undistortedImage, cv::Size(9, 6), 
		ptVec, CV_CALIB_CB_ADAPTIVE_THRESH );

	if (verbose) {
		std::cout << "Found: " << found << " (" << ptVec.size() << ")" << "\n";
	}

	for (auto p : ptVec) {
		pt2fVec.push_back(Point2f(p.x, p.y));
	}

	Scalar color = Scalar( 0, 0, 255 );

	cv::RotatedRect box = cv::minAreaRect(cv::Mat(ptVec));
	double size = box.size.width / params.resolution;
	if (verbose) {
		std::cout << "size: " << size << "\n";
	}

	double dist = true_size * params.focal / size;
	if (found) {
		drawChessboardCorners(image, cv::Size(9, 6), pt2fVec, found);
		putText(image, "Dist: " + std::to_string(dist), ptVec[0], FONT_HERSHEY_SIMPLEX, 0.5, color, 1, CV_AA, false);
		std::cout << "Dist: " << dist << "\n";
	}
	else {
		std::cout << "Not found" << "\n";
	}

	return dist;
}

void checkerboardVideo(CameraParams params) {
	Mat image;
	VideoCapture capture;

	capture.open(0);

	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH, params.size.width);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, params.size.height);

	while (true) {
		bool frame = capture.read(image);

		if(!frame) {
			capture.set(CV_CAP_PROP_POS_FRAMES, 0);
			continue;
		}
		checkerboardDistance(params, image);

		imshow(windowName, image);
		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(30);
	}
}

void singlePicture(CameraParams params, std::string fileName) {
	Mat image = imread(fileName);
	checkerboardDistance(params, image, true);
}

int main(int argc, char ** argv) {
	FileStorage fs("out_camera_data.yml", FileStorage::READ);
	Mat camera_matrix, distortion_coefficients;
	fs["camera_matrix"] >> camera_matrix;
	fs["distortion_coefficients"] >> distortion_coefficients;
	int width, height;
	fs["image_width"] >> width;
	fs["image_height"] >> height;
	Size2i size(width, height);

	bool useVideo = false;
	if (argc < 2) {
		useVideo = true;
	}

	double f_x = camera_matrix.at<double>(0,0);
	double f_y = camera_matrix.at<double>(1,1);
	//double c_x = camera_matrix.at<double>(2,0);
	//double c_y = camera_matrix.at<double>(2,1);
	double resolution = ((f_x + f_y)*0.5)/focal;
	std::cout << resolution << "px/mm\n";

	CameraParams params(camera_matrix, distortion_coefficients, focal, resolution, size);

	if (useVideo) {
		checkerboardVideo(params);
	}
	else {
		singlePicture(params, argv[1]);
		return 0;
	}
}
