#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
int main(int argc, char ** argv) {
	FileStorage fs("out_camera_data.yml", FileStorage::READ);
	Mat camera_matrix, distortion_coefficients;
	fs["camera_matrix"] >> camera_matrix;
	fs["distortion_coefficients"] >> distortion_coefficients;

	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " image" << std::endl;
		return -1;
	}

	string filename(argv[1]);
	Mat image = imread(filename);
	Mat undistortedImage;

	undistort(image, undistortedImage, camera_matrix, distortion_coefficients);

	std::cout << "undistorted\n";

	std::vector<Point> ptvec;

	bool found = findChessboardCorners( 
		undistortedImage, cv::Size(9, 6), 
		ptvec, CV_CALIB_CB_ADAPTIVE_THRESH );

	std::cout << "Found: " << found << " (" << ptvec.size() << ")" << "\n";

	std::vector<Point3f> foundBoardCorners;
	for (auto p : ptvec) {
		foundBoardCorners.push_back(Point3f(p.x, p.y, 0.0f));
	}

	std::vector<Point3f> rvec;
	std::vector<Point3f> tvec;

	cv::solvePnP(ptvec, foundBoardCorners, camera_matrix,
                     distortion_coefficients, rvec, tvec);
	
	std::cout << "tvec: " << tvec << "\n\n";
	std::cout << "rvec: " << rvec << "\n";
	
	return 0;
}

