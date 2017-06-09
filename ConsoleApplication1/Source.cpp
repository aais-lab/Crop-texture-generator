#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <windows.h>

using namespace cv;
using namespace std;

#define THRESHOLD 50
#define THRESHOLD_MAX_VALUE 255

int main(int argc, char **argv)
{
	HANDLE hFind;
	WIN32_FIND_DATA win32fd;

	hFind = FindFirstFile("image\\*.jpg", &win32fd); //jpgファイル検索

	if (hFind == INVALID_HANDLE_VALUE) {
		return 1;
	}
	int num = 0;
	std::vector<std::string> strList;


	/*** テンプレートマッチング用マーカイメージ　ここから ***/
	IplImage *templateImage_1 = cvLoadImage("./Marker/marker_1.jpg", CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	IplImage *templateImage_2 = cvLoadImage("./Marker/marker_2.jpg", CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);

	IplImage *templateGrayImage_1 = cvCreateImage(cvGetSize(templateImage_1), IPL_DEPTH_8U, 1);
	IplImage *templateGrayImage_2 = cvCreateImage(cvGetSize(templateImage_2), IPL_DEPTH_8U, 1);

	IplImage *templateBinaryImage_1 = cvCreateImage(cvGetSize(templateImage_1), IPL_DEPTH_8U, 1);
	IplImage *templateBinaryImage_2 = cvCreateImage(cvGetSize(templateImage_2), IPL_DEPTH_8U, 1);

	cvCvtColor(templateImage_1, templateGrayImage_1, CV_BGR2GRAY);
	cvCvtColor(templateImage_2, templateGrayImage_2, CV_BGR2GRAY);

	cvThreshold(templateGrayImage_1, templateBinaryImage_1, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY);
	cvThreshold(templateGrayImage_2, templateBinaryImage_2, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY);
	/*** テンプレートマッチング用マーカイメージ　ここまで ***/

	CvPoint minLocation_1;
	CvPoint minLocation_2;

	std::string fullpath;
	std::string num_str;

	std::string NEW_IMG_PATH;
	std::string OUTPUT_PATH;

	const char* cstr;

	int y_1, x_1, y_2, x_2; 
	int ul_y, ul_x;

	do {
		if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		}
		else {
			fullpath = win32fd.cFileName;

			strList.push_back(fullpath);

			std::cout << strList.at(num) << endl;

			if (num < 10) {
				num_str = "0" + std::to_string(num);
			}
			else {
				num_str = std::to_string(num);
			}

			NEW_IMG_PATH = "./Image/" + fullpath; //変換したいファイルのディレクトリパス
			OUTPUT_PATH = "./Texture/" + num_str + ".jpg"; //出力先

			/*** テンプレートマッチング　ここから ***/

			cstr = NEW_IMG_PATH.c_str();

			IplImage *sourceImage = cvLoadImage(cstr, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);

			IplImage *sourceGrayImage = cvCreateImage(cvGetSize(sourceImage), IPL_DEPTH_8U, 1);

			IplImage *sourceBinaryImage = cvCreateImage(cvGetSize(sourceImage), IPL_DEPTH_8U, 1);

			IplImage *differenceMapImage_1 = cvCreateImage(cvSize(sourceImage->width - templateImage_1->width +1, sourceImage->height - templateImage_1->height +1), IPL_DEPTH_32F, 1);
			IplImage *differenceMapImage_2 = cvCreateImage(cvSize(sourceImage->width - templateImage_2->width + 1, sourceImage->height - templateImage_2->height + 1), IPL_DEPTH_32F, 1);

			cvCvtColor(sourceImage, sourceGrayImage, CV_BGR2GRAY);

			cvThreshold(sourceGrayImage, sourceBinaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_OTSU);

			cvMatchTemplate(sourceBinaryImage, templateBinaryImage_1, differenceMapImage_1, CV_TM_SQDIFF);
			cvMatchTemplate(sourceBinaryImage, templateBinaryImage_2, differenceMapImage_2, CV_TM_SQDIFF);

			cvMinMaxLoc(differenceMapImage_1, NULL, NULL, &minLocation_1, NULL, NULL );
			cvMinMaxLoc(differenceMapImage_2, NULL, NULL, &minLocation_2, NULL, NULL);

			y_1 = minLocation_1.y; //右上y座標
			x_1 = minLocation_1.x; //右上x座標
			y_2 = minLocation_2.y + templateImage_2->height; //左下y座標
			x_2 = minLocation_2.x + templateImage_2->width; //左下x座標

			ul_y = y_2 - y_1; //切り取り範囲y軸
			ul_x = x_2 - x_1; //切り取り範囲x軸

			printf("右上y：%d, 右上x：%d, 左下y：%d, 左下x：%d\n", y_1, x_1, y_2, x_2);

			/*** テンプレートマッチング　ここまで ***/

			/*** 新規画像読み込み ***/
			cv::Mat image_raw(imread(NEW_IMG_PATH));
			
			/*** 切り抜きここから ***/
			cv::Mat image(image_raw, cv::Rect(x_1, y_1, ul_y, ul_x));
			/*** 切り抜きここまで ***/

			/*** 正方形リサイズここから ***/
			resize(image, image, cv::Size(), 1024.0 / image.cols, 1024.0 / image.rows, cv::INTER_CUBIC);
			/*** 正方形リサイズここまで ***/

			/*** 画像保存 ***/
			imwrite(OUTPUT_PATH, image);

			num = num + 1;
			printf("%d枚目\n", num);

			cvReleaseImage(&sourceImage);
			cvReleaseImage(&sourceGrayImage);
			cvReleaseImage(&sourceBinaryImage);
			cvReleaseImage(&differenceMapImage_1);
			cvReleaseImage(&differenceMapImage_2);

		}
	} while (FindNextFile(hFind, &win32fd)); 

	cvReleaseImage(&templateImage_1);
	cvReleaseImage(&templateImage_2);
	cvReleaseImage(&templateGrayImage_1);
	cvReleaseImage(&templateGrayImage_2);
	cvReleaseImage(&templateBinaryImage_1);
	cvReleaseImage(&templateBinaryImage_2);

	FindClose(hFind);

	return 0;
}