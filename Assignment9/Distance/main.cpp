#include <opencv2/opencv.hpp>
using namespace cv;

//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

struct CameraParams {
	CameraParams(Mat matrix, Mat coeff, double focal, double res) : 
		camera_matrix(matrix), 
		distortion_coefficients(coeff), 
		focal(focal), 
		resolution(res) { }
	Mat camera_matrix;
	Mat distortion_coefficients;
	double focal;
	double resolution;
};

void checkerboardDistance(CameraParams params, Mat image, bool verbose = false) {
	Mat undistortedImage;

	undistort(image, undistortedImage, params.camera_matrix, params.distortion_coefficients);

	//std::cout << "undistorted\n";

	std::vector<Point> ptvec;

	bool found = findChessboardCorners( 
		undistortedImage, cv::Size(9, 6), 
		ptvec, CV_CALIB_CB_ADAPTIVE_THRESH );

	if (verbose) std::cout << "Found: " << found << " (" << ptvec.size() << ")" << "\n";

	std::vector<Point3f> foundBoardCorners;
	for (auto p : ptvec) {
		foundBoardCorners.push_back(Point3f(p.x, p.y, 0.0f));
	}

	cv::RotatedRect box = cv::minAreaRect(cv::Mat(ptvec));
	double size = box.size.width / params.resolution;
	if (verbose) std::cout << "size: " << size << "\n";

	double dist = 200.0 * params.focal / size;
	std::cout << "dist: " << dist << "\n";
}

void checkerboardVideo(CameraParams params) {
	Mat image;
	VideoCapture capture;

	capture.open(0);

	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);

	while (true) {
		//store image to matrix
		bool frame = capture.read(image);

		if(!frame) {
			capture.set(CV_CAP_PROP_POS_FRAMES, 0);
			continue;
		}
		checkerboardDistance(params, image);
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

	bool useVideo = false;
	if (argc < 2) {
		useVideo = true;
	}

	double f_x = camera_matrix.at<double>(0,0);
	double f_y = camera_matrix.at<double>(1,1);
	//double c_x = camera_matrix.at<double>(2,0);
	//double c_y = camera_matrix.at<double>(2,1);
	double focal = 3.67; // millimeters
	double m = ((f_x + f_y)/2.0)/focal;
	std::cout << m << "px/mm\n";

	CameraParams params(camera_matrix, distortion_coefficients, focal, m);

	if (useVideo) {
		checkerboardVideo(params);
	}
	else {
		singlePicture(params, argv[1]);
		return 0;
	}
}

