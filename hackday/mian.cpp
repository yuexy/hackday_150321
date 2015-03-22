#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

const int lowThreshold = 700;
const int highThreshold = 250;
const int kernel_size = 3;

const int tt = 10;

int max_height = 0;
int max_width = 0;
int min_height = 0;
int min_width = 0;

int num = 0;
int p = 0;
int t = 0;

RNG rng(12345);

void getXYZ(Mat& m1, Mat& m2)
{
	cout << "num:  " << num++ << endl;
	int half_height = (max_height - min_height) / 16;
	int point_height = min_height + 9 * half_height;

	int half_width = (max_width - min_width) / 8;
	
	uchar* current = m2.ptr<uchar>(point_height);

	/*cout << "half_height" << half_height << endl;
	cout << "half_width" << half_width << endl;*/

	int i = min_width;
	for (; i < max_width; i++)
	{
		if (current[i] > 128)
			break;
	}

	if (i > (min_width + half_height))
	{
		if (p == 1)
		{
			t++;
			if (t < tt)
				return;
			t = 0;
		}
		cout << "1" << endl;
		circle(m1, Point(min_width + half_width, point_height), 1, Scalar( 0, 0, 255 ), 3, 0);
		//system("adb shell input tap 70 615");
		p = 1;
	}
	else
	{
		i = half_width + min_width;
		for (; i < max_width; i++)
		{
			if (current[i] < 128)
				break;
		}
		if (i < (min_width + half_width * 3))
		{
			if (p == 2)
			{
				t++;
				if (t < tt)
					return;
				t = 0;
			}
			cout << "2" << endl;
			circle(m1, Point(min_width + half_width * 3, point_height), 1, Scalar( 0, 0, 255 ), 3, 0);
			//system("adb shell input tap 200 615");
			p = 2;
		}
		else if (i < (min_width + half_width * 5))
		{
			if (p == 3)
			{
				t++;
				if (t < tt)
					return;
				t = 0;
			}
			cout << "3" << endl;
			circle(m1, Point(min_width + half_width * 5, point_height), 1, Scalar( 0, 0, 255 ), 3, 0);
			//system("adb shell input tap 350 615");
			p = 3;
		}
		else
		{
			if (p == 4)
			{
				t++;
				if (t < tt)
					return;
				t = 0;
			}
			cout << "4" << endl;
			circle(m1, Point(min_width + half_width * 7, point_height), 1, Scalar( 0, 0, 255 ), 3, 0);
			//system("adb shell input tap 460 615");
			p = 4;
		}
	}
}

Mat process(Mat img)
{
	Mat res;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	int max = 0;

	Canny(img, res, lowThreshold, highThreshold, kernel_size);

	findContours(res, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<RotatedRect> minRect(contours.size());

	Mat drawing = Mat::zeros( res.size(), CV_8UC3 );
	for( int i = 0; i< contours.size(); i++ )
	{
		minRect[i] = minAreaRect(Mat(contours[i]));

		if (minRect[i].size.area() > minRect[max].size.area())
			max = i;
	}
	Scalar color = Scalar( rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));

	Point2f rect_points[4];
	minRect[max].points(rect_points);

	if (rect_points[0].x > rect_points[1].x)
	{
		min_width = (int)rect_points[1].x;
		max_width = (int)rect_points[0].x;
	}
	else
	{
		min_width = (int)rect_points[0].x;
		max_width = (int)rect_points[1].x;
	}

	for (int j = 2; j < 4; j++)
	{
		if (rect_points[j].x < min_width)
			min_width = (int)rect_points[j].x;
		if (rect_points[j].x > max_width)
			max_width = (int)rect_points[j].x;
	}

	if (rect_points[0].y > rect_points[1].y)
	{
		min_height = (int)rect_points[1].y;
		max_height = (int)rect_points[0].y;
	}
	else
	{
		min_height = (int)rect_points[0].y;
		max_height = (int)rect_points[1].y;
	}

	for (int j = 2; j < 4; j++)
	{
		if (rect_points[j].y < min_height)
			min_height = rect_points[j].y;
		if (rect_points[j].y > max_height)
			max_height = rect_points[j].y;
	}

	//for( int j = 0; j < 4; j++ )
	//{
	//	line( drawing, rect_points[j], rect_points[(j+1)%4], color, 1, 8 );
	//}


	drawContours( drawing, contours, max, color, 2, 8, hierarchy, 0, Point());
	
	return drawing;
}

Mat process1(Mat img)
{
	Mat res;
	cvtColor(img, img, CV_BGR2GRAY);
	threshold(img, res, 220, 255, 0);
	return res;
}

void showVideo()
{
	VideoCapture video_show;
	video_show.open(0);

	namedWindow("HelloWorld");
	namedWindow("HelloWorld1");

	while (1)
	{
		Mat image;
		video_show >> image;
		Mat mat1 = process(image);
		Mat mat2 = process1(image);

		getXYZ(mat1, mat2);

		imshow("HelloWorld", mat1);
		imshow("HelloWorld1", mat2);
		//waitKey(0);
		if (waitKey(30) > 0) break;
	}
}

int main()
{
	showVideo();
	return 0;
}