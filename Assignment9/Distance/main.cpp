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

	double f_x = camera_matrix.at<double>(0,0);
	double f_y = camera_matrix.at<double>(1,1);
	double c_x = camera_matrix.at<double>(2,0);
	double c_y = camera_matrix.at<double>(2,1);
	double focal = 3.67; // millimeters
	auto m = ((f_x + f_y)/2.0)/focal;
	std::cout << m << "px/mm\n";

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

	cv::RotatedRect box = cv::minAreaRect(cv::Mat(ptvec));
	double size = box.size.width / m;
	std::cout << "size: " << size << "\n";

	double dist = 200.0 * focal / size;
	std::cout << "dist: " << dist << "\n";
	// std::vector<Point3f> rvec;
	// std::vector<Point3f> tvec;

	// cv::solvePnP(ptvec, foundBoardCorners, camera_matrix,
    //                  distortion_coefficients, rvec, tvec);
	
	// std::cout << "tvec: " << tvec << "\n\n";
	// std::cout << "rvec: " << rvec << "\n";
	
	return 0;
}

