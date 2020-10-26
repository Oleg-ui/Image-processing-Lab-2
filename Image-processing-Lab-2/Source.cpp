#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>

using namespace cv;
using namespace std;

int Clamp(int n)
{
	n = n > 255 ? 255 : n;
	return n < 0 ? 0 : n;
}

Mat addNoise(const Mat& src1, double Mean = 0.0, double StdDev = 10.0)
{
	if (src1.empty())
	{
		cout << "Image Empty!";
		exit(0);
	}

	Mat noise = Mat(src1.size(), CV_16SC3);
	Mat result(src1.size(), src1.type());

	randn(noise, Mean, StdDev);

	for (int Rows = 0; Rows < src1.rows; Rows++)
	{
		for (int Cols = 0; Cols < src1.cols; Cols++)
		{
			Vec3b Source_Pixel = src1.at<Vec3b>(Rows, Cols);
			Vec3s Noise_Pixel = noise.at<Vec3s>(Rows, Cols);
			Vec3b& Result_Pixel = result.at<Vec3b>(Rows, Cols);

			for (int i = 0; i < 3; i++)
				Result_Pixel.val[i] = Clamp(Source_Pixel.val[i] + Noise_Pixel.val[i]);
		}
	}
	return result;
}

Mat MedianFilter(const Mat& src1) 
{
	if (src1.empty())
	{
		cout << "Image Empty!";
		exit(0);
	}

	Mat result = src1.clone();

	int width = src1.cols;
	int height = src1.rows;
	Vec3b pix[9];

	for (int i = 1; i < height - 1; ++i)
	{
		for (int j = 1; j < width - 1; ++j)
		{
			pix[0] = src1.at<Vec3b>(i - 1, j - 1);
			pix[1] = src1.at<Vec3b>(i, j - 1);
			pix[2] = src1.at<Vec3b>(i + 1, j - 1);
			pix[3] = src1.at<Vec3b>(i - 1, j);
			pix[4] = src1.at<Vec3b>(i, j);
			pix[5] = src1.at<Vec3b>(i + 1, j);
			pix[6] = src1.at<Vec3b>(i - 1, j + 1);
			pix[7] = src1.at<Vec3b>(i, j + 1);
			pix[8] = src1.at<Vec3b>(i + 1, j + 1);

			std::sort(pix, pix + 9, [](const Vec3b& v1, const Vec3b& v2) -> bool 
				{ 
					int sum1 = v1[0] + v1[1] + v1[2];
					int sum2 = v2[0] + v2[1] + v2[2];
					return sum1 > sum2;
				});
			result.at<Vec3b>(i, j) = pix[4];
		}
	}
	return result;
}

Vec3b KillMePlease(const Mat& src1, int x, int y)
{
	Vec3b result{};
	Vec3b tmp{};

	for (int i = x - 1; i < x + 2; ++i)
	{
		for (int j = y - 1; j < y + 2; ++j)
		{
			result += src1.at<Vec3b>(i, j) * 1/9;
		}
	}
	return result;
}

Mat BoxFilter(const Mat& src1)
{
	if (src1.empty())
	{
		cout << "Image Empty!";
		exit(0);
	}

	Mat result = src1.clone();

	int width = src1.cols;
	int height = src1.rows;

	for (int i = 1; i < height - 1; ++i)
	{
		for (int j = 1; j < width - 1; ++j)
		{
			result.at<Vec3b>(i, j) = KillMePlease(src1, i, j);
		}
	}
	return result;
}

int main()
{
	//Task 1
	Mat src1 = imread("pic.jpg");
	imshow("Original image", src1);
	Mat src2(src1.size(), src1.type());
	src2 = addNoise(src1, 0, 50.0);
	imshow("Image with noise", src2);

	//Task 2
	Mat src11 = src2.clone();;
	unsigned int start_time1 = clock();
	src11 = MedianFilter(src2);
	unsigned int end_time1 = clock();
	unsigned int search_time1 = end_time1 - start_time1;
	imshow("MedianFilter", src11);
	cout << "Working time of our MedianFilter: " << search_time1 << endl;

	Mat src22 = src2.clone();
	unsigned int start_time2 = clock();
	src22 = BoxFilter(src2);
	unsigned int end_time2 = clock();
	unsigned int search_time2 = end_time2 - start_time2;
	cout << "Working time of our BoxFilter: " << search_time2 << endl;
	imshow("BoxFilter", src22);

	//Task 3
	Mat src111(src2.size(), src2.type());
	unsigned int start_time3 = clock();
	GaussianBlur(src2, src111, Size(5, 5), 0);
	unsigned int end_time3 = clock();
	unsigned int search_time3 = end_time3 - start_time3;
	cout << "Working time of OpenCV filter: " << search_time3 << endl;
	imshow("Noise removal OpenCV", src111);

	//Task 4


	waitKey(0);
	return 0;
}