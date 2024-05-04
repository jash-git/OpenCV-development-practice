#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

// Defining the dimensions of checkerboard
// �������̸�ĳߴ�
int CHECKERBOARD[2]{ 6,9 };

int main()
{
	// Creating vector to store vectors of 3D points for each checkerboard image
	// ����ʸ���Դ洢ÿ������ͼ�����ά��ʸ��
	std::vector<std::vector<cv::Point3f> > objpoints;

	// Creating vector to store vectors of 2D points for each checkerboard image
	// ����ʸ���Դ洢ÿ������ͼ��Ķ�ά��ʸ��
	std::vector<std::vector<cv::Point2f> > imgpoints;

	// Defining the world coordinates for 3D points
	// Ϊ��ά�㶨����������ϵ
	std::vector<cv::Point3f> objp;
	for (int i{ 0 }; i < CHECKERBOARD[1]; i++)
	{
		for (int j{ 0 }; j < CHECKERBOARD[0]; j++)
		{
			objp.push_back(cv::Point3f(j, i, 0));
		}
	}

	// Extracting path of individual image stored in a given directory
	// ��ȡ�洢�ڸ���Ŀ¼�еĵ���ͼ���·��
	std::vector<cv::String> images;

	// Path of the folder containing checkerboard images
	// ��������ͼ����ļ��е�·��
	std::string path = "./images/*.jpg";

	// ʹ��glob������ȡ����ͼ���·��
	cv::glob(path, images);

	cv::Mat frame, gray;

	// vector to store the pixel coordinates of detected checker board corners
	// �洢��⵽������ת�����������ʸ��
	std::vector<cv::Point2f> corner_pts;
	bool success;

	// Looping over all the images in the directory
	// ѭ����ȡͼ��
	for (int i{ 0 }; i < images.size(); i++)
	{
		frame = cv::imread(images[i]);
		if (frame.empty())
		{
			continue;
		}
		if (i == 40)
		{
			int b = 1;
		}
		cout << "the current image is " << i << "th" << endl;
		cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

		// Finding checker board corners
		// Ѱ�ҽǵ�
		// If desired number of corners are found in the image then success = true
		// �����ͼ�����ҵ����������Ľǣ���success = true
		// opencv4���°汾��flag����ΪCV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE
		success = cv::findChessboardCorners(gray, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);

		/*
		 * If desired number of corner are detected,
		 * we refine the pixel coordinates and display
		 * them on the images of checker board
		*/
		// �����⵽���������Ľǵ㣬���ǽ�ϸ���������겢������ʾ������ͼ����
		if (success)
		{
			// �����OpenCV4���°汾����һ������ΪCV_TERMCRIT_EPS | CV_TERMCRIT_ITER
			cv::TermCriteria criteria(TermCriteria::EPS | TermCriteria::Type::MAX_ITER, 30, 0.001);

			// refining pixel coordinates for given 2d points.
			// Ϊ�����Ķ�ά��ϸ����������
			cv::cornerSubPix(gray, corner_pts, cv::Size(11, 11), cv::Size(-1, -1), criteria);

			// Displaying the detected corner points on the checker board
			// ����������ʾ��⵽�Ľǵ�
			cv::drawChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);

			objpoints.push_back(objp);
			imgpoints.push_back(corner_pts);
		}

		//cv::imshow("Image", frame);
		//cv::waitKey(0);
	}

	cv::destroyAllWindows();

	cv::Mat cameraMatrix, distCoeffs, R, T;

	/*
	 * Performing camera calibration by
	 * passing the value of known 3D points (objpoints)
	 * and corresponding pixel coordinates of the
	 * detected corners (imgpoints)
	*/
	// ͨ��������֪3D�㣨objpoints����ֵ�ͼ�⵽�Ľǵ㣨imgpoints������Ӧ����������ִ�����У׼
	cv::calibrateCamera(objpoints, imgpoints, cv::Size(gray.rows, gray.cols), cameraMatrix, distCoeffs, R, T);

	// �ڲξ���
	std::cout << "cameraMatrix : " << cameraMatrix << std::endl;
	// ͸������ϵ��
	std::cout << "distCoeffs : " << distCoeffs << std::endl;
	// rvecs
	std::cout << "Rotation vector : " << R << std::endl;
	// tvecs
	std::cout << "Translation vector : " << T << std::endl;

	return 0;
}