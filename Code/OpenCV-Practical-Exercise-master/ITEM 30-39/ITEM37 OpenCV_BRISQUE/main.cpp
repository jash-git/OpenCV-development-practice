#include "brisque.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <fstream>

// rescaling based on training data i libsvm
// ѵ�����ݻ�ã�ͼ�����ά�����ֵ����Сֵ
float rescale_vector[36][2];

using namespace std;

int read_range_file(string range_fname)
{
	//check if file exists
	char buff[100];
	int i;
	FILE* range_file = fopen(range_fname.c_str(), "r");
	if (range_file == NULL) return 1;
	//assume standard file format for this program
	fgets(buff, 100, range_file);
	fgets(buff, 100, range_file);
	//now we can fill the array
	for (i = 0; i < 36; ++i)
	{
		float a, b, c;
		fscanf(range_file, "%f %f %f", &a, &b, &c);
		rescale_vector[i][0] = b;
		rescale_vector[i][1] = c;
	}
	return 0;
}

int main()
{
	// use the pre-trained allmodel file
	// ʹ��Ԥѵ��ģ��
	string modelfile = "allmodel";

	// ѵ��ͼ���
	string range_fname = "allrange";
	// �����ͼ��·��
	string imagename = "./images/original-scaled-image.jpg";

	//read in the allrange file to setup internal scaling array
	if (read_range_file(range_fname))
	{
		cerr << "unable to open allrange file" << endl;
		return -1;
	}

	// �����������۵÷֣�����Խ��ͼ������Խ��
	float qualityscore = computescore(imagename, modelfile);
	cout << "Quality Score: " << qualityscore << endl;

	return 0;
}