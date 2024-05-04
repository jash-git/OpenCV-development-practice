#include "pch.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//�׶����
void fillHoles(Mat &mask)
{
	Mat maskFloodfill = mask.clone();
	//��ˮ���
	floodFill(maskFloodfill, cv::Point(0, 0), Scalar(255));
	Mat mask2;
	//��ɫ
	bitwise_not(maskFloodfill, mask2);
	//������
	mask = (mask2 | mask);
}

int main()
{
	// Read image ����ɫͼ��
	Mat img = imread("./image/red_eyes.jpg", CV_LOAD_IMAGE_COLOR);

	// Output image ���ͼ��
	Mat imgOut = img.clone();

	// Load HAAR cascade ��ȡhaar������
	CascadeClassifier eyesCascade("./model/haarcascade_eye.xml");

	// Detect eyes ����۾�
	std::vector<Rect> eyes;
	//ǰ�ĸ�����������ͼ���۾��������ʾÿ��ͼ��ߴ��С�ı�������ʾÿһ��Ŀ������Ҫ����⵽4�β��������
	//������������0 | CASCADE_SCALE_IMAGE��ʾ��ͬ�ļ��ģʽ����С���ߴ�
	eyesCascade.detectMultiScale(img, eyes, 1.3, 4, 0 | CASCADE_SCALE_IMAGE, Size(100, 100));

	// For every detected eye ÿֻ�۾������д���
	for (size_t i = 0; i < eyes.size(); i++)
	{
		// Extract eye from the image. ��ȡ�۾�ͼ��
		Mat eye = img(eyes[i]);

		// Split eye image into 3 channels. ��ɫ����
		vector<Mat>bgr(3);
		split(eye, bgr);

		// Simple red eye detector ���ۼ��������ý����ģ
		Mat mask = (bgr[2] > 150) & (bgr[2] > (bgr[1] + bgr[0]));

		// Clean mask ������ģ
		//���׶�
		fillHoles(mask);
		//����׶�
		dilate(mask, mask, Mat(), Point(-1, -1), 3, 1, 1);

		// Calculate the mean channel by averaging the green and blue channels
		//����bͨ����gͨ���ľ�ֵ
		Mat mean = (bgr[0] + bgr[1]) / 2;
		//�øþ�ֵͼ�񸲸�ԭͼ��ģ����ͼ��
		mean.copyTo(bgr[2], mask);
		mean.copyTo(bgr[0], mask);
		mean.copyTo(bgr[1], mask);

		// Merge channels
		Mat eyeOut;
		//ͼ��ϲ�
		cv::merge(bgr, eyeOut);

		// Copy the fixed eye to the output image.
		// �۲�ͼ���滻
		eyeOut.copyTo(imgOut(eyes[i]));
	}

	// Display Result
	imshow("Red Eyes", img);
	imshow("Red Eyes Removed", imgOut);
	waitKey(0);
	return 0;
} 