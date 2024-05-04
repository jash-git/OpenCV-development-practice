#include "pch.h"
#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc.hpp>
#include <iostream>
#include <ctime>

using namespace cv;
using namespace cv::ximgproc::segmentation;

int main()
{
	// speed-up using multithreads ʹ�ö��߳�
	//����CPU��Ӳ��ָ���Ż�����
	setUseOptimized(true);
	setNumThreads(4);

	// read image ��ͼ
	Mat im = imread("./image/dogs.jpg");
	if (im.empty())
	{
		return 0;
	}
	// resize image ͼ���С����
	int newHeight = 200;
	int newWidth = im.cols*newHeight / im.rows;
	resize(im, im, Size(newWidth, newHeight));

	// create Selective Search Segmentation Object using default parameters Ĭ�ϲ�������ѡ����������
	Ptr<SelectiveSearchSegmentation> ss = createSelectiveSearchSegmentation();
	// set input image on which we will run segmentation Ҫ���зָ��ͼ��
	ss->setBaseImage(im);

	// Switch to fast but low recall Selective Search method ��������(�ٶȿ죬�ٻ��ʵ�)
	//ss->switchToSelectiveSearchFast();
	//��׼����(�ٶ������ٻ��ʸ�)
	ss->switchToSelectiveSearchQuality();

	// run selective search segmentation on input image �����������Ŀ򣬰������ԴӸߵ�������
	std::vector<Rect> rects;
	ss->process(rects);
	std::cout << "Total Number of Region Proposals: " << rects.size() << std::endl;

	// number of region proposals to show ��ͼ���б�����ٿ�
	int numShowRects = 100;

	while (1)
	{
		// create a copy of original image ��һ��ͼ��ͼ�񿽱�
		Mat imOut = im.clone();

		// itereate over all the region proposals ����ǰnumShowRects��
		for (int i = 0; i < numShowRects; i++)
		{
			rectangle(imOut, rects[i], Scalar(0, 255, 0));
		}

		// show output
		imshow("Output", imOut);

		waitKey(0);
	}
	return 0;
}