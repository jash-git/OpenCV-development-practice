#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>

// Defining the dimensions of checkerboard
// �������̸�ĳߴ磬w=6,h=9
int CHECKERBOARD[2]{ 6,9 };

int main()
{
	// Creating vector to store vectors of 3D points for each checkerboard image
	// ����vector�Դ洢ÿ������ͼ���3D��ʸ��
	std::vector<std::vector<cv::Point3f> > objpoints;

	// Creating vector to store vectors of 2D points for each checkerboard image
	// ����vector�Դ洢ÿ������ͼ���2D��ʸ��
	std::vector<std::vector<cv::Point2f> > imgpointsL, imgpointsR;

	// Defining the world coordinates for 3D points
	// ������ά�����������
	std::vector<cv::Point3f> objp;
	// ��ʼ���㣬cv::Point3f(j, i, 0)�������x,y,z���ꡣ
	for (int i{ 0 }; i < CHECKERBOARD[1]; i++)
	{
		for (int j{ 0 }; j < CHECKERBOARD[0]; j++)
		{
			objp.push_back(cv::Point3f(j, i, 0));
		}
	}

	// Extracting path of individual image stored in a given directory
	// ��ȡ�洢�ڸ���Ŀ¼�еĵ���ͼ���·��
	std::vector<cv::String> imagesL, imagesR;
	// Path of the folder containing checkerboard images
	// ��������ͼ����ļ��е�·��
	// pathL��pathR�ֱ�Ϊ��������ͷ�ڶ��ʱ�������ͼ��
	std::string pathL = "./data/stereoL/*.png";
	std::string pathR = "./data/stereoR/*.png";

	// ��ȡ�����ļ��� ���е�ͼ��
	cv::glob(pathL, imagesL);
	cv::glob(pathR, imagesR);

	cv::Mat frameL, frameR, grayL, grayR;
	// vector to store the pixel coordinates of detected checker board corners
	// ���ڴ洢��⵽�����̽ǵ���������������
	std::vector<cv::Point2f> corner_ptsL, corner_ptsR;
	bool successL, successR;

	// Looping over all the images in the directory
	// ����Ŀ¼�е�����ͼ��
	for (int i{ 0 }; i < imagesL.size(); i++)
	{
		// ��ȡͬһʱ�̷ֱ�����������ͷ�ĵ�����Ƭ
		frameL = cv::imread(imagesL[i]);
		cv::cvtColor(frameL, grayL, cv::COLOR_BGR2GRAY);

		frameR = cv::imread(imagesR[i]);
		cv::cvtColor(frameR, grayR, cv::COLOR_BGR2GRAY);

		// Finding checker board corners
		// Ѱ������ͼ���ڽǵ�λ��
		// If desired number of corners are found in the image then success = true
		// �����ͼ�����ҵ�����Ľ�������success=true
		// ���庯��ʹ�ý��ܼ�https://blog.csdn.net/h532600610/article/details/51800488
		successL = cv::findChessboardCorners(
			grayL,
			cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]),
			corner_ptsL);
		// cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);

		successR = cv::findChessboardCorners(
			grayR,
			cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]),
			corner_ptsR);
		// cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);

		// �����⵽����Ľǵ��������ϸ���������겢������ʾ�����̸�ͼ����
		if ((successL) && (successR))
		{
			// TermCriteria��������㷨��ֹ��������
			// ����ʹ�ü� https://www.jianshu.com/p/548868c4d34e
			cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.001);

			// refining pixel coordinates for given 2d points.
			// ϸ������2D����������꣬cornerSubPix���������ؽǵ���
			// ���������https://blog.csdn.net/guduruyu/article/details/69537083
			cv::cornerSubPix(grayL, corner_ptsL, cv::Size(11, 11), cv::Size(-1, -1), criteria);
			cv::cornerSubPix(grayR, corner_ptsR, cv::Size(11, 11), cv::Size(-1, -1), criteria);

			// Displaying the detected corner points on the checker board
			// drawChessboardCorners���ڻ������̸�ǵ�ĺ���
			cv::drawChessboardCorners(frameL, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_ptsL, successL);
			cv::drawChessboardCorners(frameR, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_ptsR, successR);

			// ���������Թ�����ʹ��
			// ������ά������
			objpoints.push_back(objp);
			// ����ǵ���Ϣ
			imgpointsL.push_back(corner_ptsL);
			imgpointsR.push_back(corner_ptsR);
		}

		//cv::imshow("ImageL", frameL);
		//cv::imshow("ImageR", frameR);
		//cv::waitKey(0);
	}

	// �ر����д���
	cv::destroyAllWindows();

	// ͨ��������֪��ά�㣨objpoints����ֵ�ͼ�⵽�Ľǵ㣨imgpoints������Ӧ����������ִ�����У׼
	// mtxL,mtxRΪ�ڲ������� distL��distRΪ�������
	// R_L��R_RΪ��ת������T_L��T_RΪλ������
	cv::Mat mtxL, distL, R_L, T_L;
	cv::Mat mtxR, distR, R_R, T_R;

	cv::Mat new_mtxL, new_mtxR;

	// Calibrating left camera
	// У��������

	// ����궨����
	// ����ʹ�ü�https://blog.csdn.net/u011574296/article/details/73823569
	cv::calibrateCamera(objpoints,
		imgpointsL,
		grayL.size(),
		mtxL,
		distL,
		R_L,
		T_L);

	// ȥ���䣬�Ż�����ڲΣ���һ����ѡ
	// getOptimalNewCameraMatrix����ʹ�ü�https://www.jianshu.com/p/df78749b4318
	new_mtxL = cv::getOptimalNewCameraMatrix(mtxL,
		distL,
		grayL.size(),
		1,
		grayL.size(),
		0);

	// Calibrating right camera
	// У���ұ����
	cv::calibrateCamera(objpoints,
		imgpointsR,
		grayR.size(),
		mtxR,
		distR,
		R_R,
		T_R);

	new_mtxR = cv::getOptimalNewCameraMatrix(mtxR,
		distR,
		grayR.size(),
		1,
		grayR.size(),
		0);

	// Here we fix the intrinsic camara matrixes so that only Rot, Trns, Emat and Fmat
	// are calculated. Hence intrinsic parameters are the same.
	// ��������������˹��е�camara�����Ա�ֻ����Rot��Trns��Emat��Fmat��������ڲ�������ͬ�ġ�
	cv::Mat Rot, Trns, Emat, Fmat;

	int flag = 0;
	flag |= cv::CALIB_FIX_INTRINSIC;

	// This step is performed to transformation between the two cameras and calculate Essential and Fundamenatl matrix
	// ͬʱ�궨��������ͷ���������ܼ�https://www.cnblogs.com/zyly/p/9373991.html
	cv::stereoCalibrate(objpoints,
		imgpointsL,
		imgpointsR,
		new_mtxL,
		distL,
		new_mtxR,
		distR,
		grayR.size(),
		Rot,
		Trns,
		Emat,
		Fmat,
		flag,
		cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 30, 1e-6));

	cv::Mat rect_l, rect_r, proj_mat_l, proj_mat_r, Q;

	// Once we know the transformation between the two cameras we can perform stereo rectification
	// һ������֪�����������֮��ı任�����ǾͿ��Խ�������У��
	// stereoRectifyͬʱУ��������������������ܼ�https://www.cnblogs.com/zyly/p/9373991.html
	cv::stereoRectify(new_mtxL,
		distL,
		new_mtxR,
		distR,
		grayR.size(),
		Rot,
		Trns,
		rect_l,
		rect_r,
		proj_mat_l,
		proj_mat_r,
		Q,
		1);

	// Use the rotation matrixes for stereo rectification and camera intrinsics for undistorting the image
	// Compute the rectification map (mapping between the original image pixels and
	// their transformed values after applying rectification and undistortion) for left and right camera frames
	// ���������Ŀ�궨�õ����ڲ��Լ�stereoRectify���������ֵ������������������У����ӳ��任����
	cv::Mat Left_Stereo_Map1, Left_Stereo_Map2;
	cv::Mat Right_Stereo_Map1, Right_Stereo_Map2;

	// �������ܼ�https://www.cnblogs.com/zyly/p/9373991.html
	cv::initUndistortRectifyMap(new_mtxL,
		distL,
		rect_l,
		proj_mat_l,
		grayR.size(),
		CV_16SC2,
		Left_Stereo_Map1,
		Left_Stereo_Map2);

	cv::initUndistortRectifyMap(new_mtxR,
		distR,
		rect_r,
		proj_mat_r,
		grayR.size(),
		CV_16SC2,
		Right_Stereo_Map1,
		Right_Stereo_Map2);

	// ����У����Ϣ
	cv::FileStorage cv_file = cv::FileStorage("data/params_cpp.xml", cv::FileStorage::WRITE);
	cv_file.write("Left_Stereo_Map_x", Left_Stereo_Map1);
	cv_file.write("Left_Stereo_Map_y", Left_Stereo_Map2);
	cv_file.write("Right_Stereo_Map_x", Right_Stereo_Map1);
	cv_file.write("Right_Stereo_Map_y", Right_Stereo_Map2);
	cv_file.release();
	return 0;
}