#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <algorithm>
#include <filesystem> 
#include <strstream>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <vector>
#include <stdexcept>


using namespace cv;
using namespace std;

#define THRESHOLD 50
#define THRESHOLD_MAX_VALUE 255

#define LINE_THICKNESS 10
#define LINE_TYPE 8
#define SHIFT 0


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

	do {
		if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			//printf("%s (DIR)\n", win32fd.cFileName);
		}
		else {
			//printf("%s\n", win32fd.cFileName);
			std::string fullpath = win32fd.cFileName;
			//int path_i = fullpath.find_last_of("\\") + 1; //7
			//int ext_i = fullpath.find_last_of("."); //10
			//std::string filename = fullpath.substr(path_i, ext_i - path_i); // 7文字目から3文字切り出す　"bbb"

			strList.push_back(fullpath);

			std::cout << strList.at(num) << endl;

			std::string num_str;

			if (num < 10) {
				num_str = "0" + std::to_string(num);
			}
			else {
				num_str = std::to_string(num);
			}

			std::string NEW_IMG_PATH = "./image/" + fullpath; //変換したいファイルのディレクトリパス
			std::string MASK_IMG_PATH = "./mask/mask.jpg"; //マスクに使うファイルのパス（拡張子.jpgまで）
			//std::string OUTPUT_PATH = "./Texture/" + num_str + ".jpg";
			std::string OUTPUT_PATH = "./masked/" + num_str + ".jpg";
			//std::string OUTPUT_PATH_2 = "./Template/tmp.jpg";
			//std::string OUTPUT_PATH = "./masked/image_" + num_str + ".png"; //出力するファイルのパスとファイル名（拡張子.pngまで） 変更前



			/*** テンプレートマッチング　ここから ***/

			CvPoint minLocation_1; //共通化可
			CvPoint minLocation_2;

			const char* cstr = NEW_IMG_PATH.c_str();

			IplImage *sourceImage = cvLoadImage(cstr, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
			IplImage *templateImage_1 = cvLoadImage("./Template/marker.jpg", CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR); //共通化可
			IplImage *templateImage_2 = cvLoadImage("./Template/marker_2.jpg", CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR); //共通化可

			IplImage *sourceGrayImage = cvCreateImage(cvGetSize(sourceImage), IPL_DEPTH_8U, 1);
			IplImage *templateGrayImage = cvCreateImage(cvGetSize(templateImage_1), IPL_DEPTH_8U, 1); //共通化可
			IplImage *templateGrayImage_2 = cvCreateImage(cvGetSize(templateImage_2), IPL_DEPTH_8U, 1); //共通化可
			IplImage *sourceBinaryImage = cvCreateImage(cvGetSize(sourceImage), IPL_DEPTH_8U, 1);
			IplImage *templateBinaryImage = cvCreateImage(cvGetSize(templateImage_1), IPL_DEPTH_8U, 1); //共通化可
			IplImage *templateBinaryImage_2 = cvCreateImage(cvGetSize(templateImage_2), IPL_DEPTH_8U, 1); //共通化可
			IplImage *differenceMapImage = cvCreateImage(cvSize(sourceImage->width - templateImage_1->width +1, sourceImage->height - templateImage_1->height +1), IPL_DEPTH_32F, 1); //共通化可？
			IplImage *differenceMapImage_2 = cvCreateImage(cvSize(sourceImage->width - templateImage_2->width + 1, sourceImage->height - templateImage_2->height + 1), IPL_DEPTH_32F, 1); //共通化可？

			cvCvtColor(sourceImage, sourceGrayImage, CV_BGR2GRAY);
			cvCvtColor(templateImage_1, templateGrayImage, CV_BGR2GRAY);
			cvCvtColor(templateImage_2, templateGrayImage_2, CV_BGR2GRAY);

			cvThreshold(sourceGrayImage, sourceBinaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY);
			cvThreshold(templateGrayImage, templateBinaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY);
			cvThreshold(templateGrayImage_2, templateBinaryImage_2, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY);

			cvMatchTemplate(sourceBinaryImage, templateBinaryImage, differenceMapImage, CV_TM_SQDIFF);
			cvMatchTemplate(sourceBinaryImage, templateBinaryImage_2, differenceMapImage_2, CV_TM_SQDIFF);

			cvMinMaxLoc(differenceMapImage, NULL, NULL, &minLocation_1, NULL, NULL );
			cvMinMaxLoc(differenceMapImage_2, NULL, NULL, &minLocation_2, NULL, NULL);

			cvRectangle(
				sourceImage,
				minLocation_1,
				cvPoint(minLocation_1.x + templateImage_1->width, minLocation_1.y + templateImage_1->height),
				CV_RGB(255,0,0),
				LINE_THICKNESS,
				LINE_TYPE,
				SHIFT
			);

			int y_1 = minLocation_1.y;
			int x_1 = minLocation_1.x;
			int y_2 = minLocation_2.y + templateImage_2->height;
			int x_2 = minLocation_2.x + templateImage_2->width;

			int ul_y = y_2 - y_1;
			int ul_x = x_2 - x_1;

			printf("%d\n", y_1);
			printf("%d\n", x_1);
			printf("%d\n", y_2);
			printf("%d\n", x_2);
						
			//cvWaitKey(0);

			std::string OUTPUT_PATH_2 = "./masked/" + num_str + "_2.jpg";
			const char* cstr2 = OUTPUT_PATH_2.c_str();

			cvSaveImage(cstr2, sourceImage);
			//Mat image_raw = imread(NEW_IMG_PATH);

			/*** テンプレートマッチング　ここまで ***/

			/*** 1,新規画像読み込み ***/
			Mat image_tmp = imread(NEW_IMG_PATH);

			/*** A4リサイズここから ***/
			//resize(image_raw, image_raw, cv::Size(), 2490.0 / image_raw.cols, 3536.0 / image_raw.rows, cv::INTER_CUBIC);
			/*** A4リサイズここまで ***/
			
			/*** 切り抜きここから ***/
			cv::Mat image(image_tmp, cv::Rect(x_1, y_1, ul_y, ul_x));
			/*** 切り抜きここまで ***/

			/*** 正方形リサイズここから ***/
			resize(image, image, cv::Size(), 1024.0 / image.cols, 1024.0 / image.rows, cv::INTER_CUBIC);
			/*** 正方形リサイズここまで ***/
			

			vector<Mat> channels;
			split(image, channels);   // 読み込んだimageをチャンネル(rgb)ごとに分離してvectorに入れる

			/*** 2,マスク処理 ***/
			Mat mask = imread(MASK_IMG_PATH, 0); // マスク画像読み込み 引数0はグレースケールで読み込み
			channels.push_back(mask); // vector最後尾にマスク画像追加
			Mat new_image;
			merge(channels, new_image); // mergeする 

			/*** 3,画像保存 ***/
			imwrite(OUTPUT_PATH, new_image);
			//imwrite(OUTPUT_PATH_2, new_image);

			num = num + 1;
			printf("%d\n", num);

		}
	} while (FindNextFile(hFind, &win32fd)); 

	FindClose(hFind);

	return 0;
}