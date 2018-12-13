/////////////////////////////////////
// opencv 3.4.3 vc14,vc15
// for x64
/////////////////////////////////////

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <memory.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <time.h>
#include <string>
#include <Windows.h>

//#include <winsock2.h>
//#include <WS2tcpip.h>
//#pragma comment(lib, "ws2_32.lib")

#define setting_num 13
#define detect_num 4
#define std_width 320
#define std_height 240

using namespace cv;
using namespace std;

typedef struct PIXEL {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char gray;
} PIXEL;

typedef struct pt {
	int x;
	int y;
} location;

typedef struct {
	int group_num;
	int size;
	pt avg_location;
	pt end_point[4]; // 0 up 1 right 2 down 3 left
} keypoint_group;

uint8_t **ptr;

vector<location> corner;
vector<int> group_index;

typedef struct {
	Mat img;
	IplImage *trans;
	PIXEL **Maps;
	vector<keypoint_group> keypg;
} background_info;

vector<background_info> bgi;
background_info proc_BnS[2];
int bgFlag;

Mat rotate_pi(Mat img, int width, int height); // upside down
void IppHarrisCorner(int w, int h, double th, int img_index); // extract key points
void mallocMaps(int width, int height, int img_index);
void mallocByte(int width, int height);
void assignPtr(int width, int height, int img_index);
void transImgToArray(IplImage* img, int img_index);
void grayscaling(int width, int height, int img_index);
void binarization(int width, int height, int img_index);
void setRedPoint(IplImage* img); // for debuging & visuallization
int find(int a, vector<int> & v); // for grouping
void join(int a, int b, vector<int> & vt); // for grouping
void grouping(); // for grouping
void decision(vector<int> & vt, int n1, int n2); // for grouping
int abs(int a);
void alloc_keyPoint(int img_index); // for grouping
void push_keyPoint(int n, int img_index); // for grouping
void setRedBox(IplImage* img, int height, int width, int img_index); // for debuging & visuallization
Mat findCigar(Mat img);
Mat subBackground(); // compare with background image
Mat check_nearby(int a, int b); // to attach picture (check status)
Mat composite_by_point(int a, int b, int i, int j); // to attach picture (composition)
void test_avg_loc(int x, int y, int img_index); // for debuging & visuallization
Mat resize_comp(int img_index); // to compare with attached picture

bool compare(const keypoint_group &a, const keypoint_group &b) { // to sort
	return a.avg_location.x < b.avg_location.x;
}

int main() {

	Mat img[setting_num + detect_num];
	IplImage *trans[setting_num + detect_num];
	string inputString;
	string outputString;
	vector<int> picture_rank;

	for (int i = 0; i < setting_num; i++) {
		inputString = "C:\\Users\\caucse\\Downloads\\bg\\"; // ★★★★ background image directory setting ★★★★
		inputString.append(to_string(i));
		inputString.append(".jpg");
		img[i] = imread(inputString, IMREAD_COLOR);
		if (!img[i].data) {
			cout << "BG Image Waiting....." << i << endl;
			i--;
			Sleep(5000);
			continue;
		}
	}

	for (int i = 0; i < setting_num; i++) {
		trans[i] = &IplImage(img[i]);
		outputString = "bgr_";
		outputString.append(to_string(i));
		outputString.append(".jpg");
		img[i] = rotate_pi(img[i], trans[i]->width, trans[i]->height);
		imwrite(outputString, img[i]);
	}

	background_info tbg; // initialize bgi
	bgi.assign(setting_num + detect_num + (detect_num * 2), tbg);
	for (int i = 0; i < setting_num; i++) {
		bgi[i].img = img[i];
	}
	clock_t begin, end;
	begin = clock();
	for (int i = 0; i < setting_num; i++) {

		bgi[i].trans = &IplImage(bgi[i].img);
		int width = bgi[i].trans->width;
		int height = bgi[i].trans->height;
		int threshold = 100;

		mallocMaps(width, height, i);
		transImgToArray(bgi[i].trans, i);

		grayscaling(width, height, i);
		//binarization(width, height, i);

		IppHarrisCorner(width, height, threshold, i);
		// setRedPoint(bgi[i].trans); //
		grouping();
		alloc_keyPoint(i);

		// setRedBox(bgi[i].trans, height, width, i); //
		group_index.clear();

		outputString = to_string(i);
		outputString.append("_bg");
		/*
		for (int k = 0; k < bgi[i].keypg.size(); k++) {
			test_avg_loc(bgi[i].keypg[k].avg_location.x, bgi[i].keypg[k].avg_location.y, i);
		}
		*/
		cvShowImage(outputString.c_str(), bgi[i].trans);
	}

	end = clock();

	for (int i = setting_num; i < setting_num + detect_num; i++) {
		inputString = "C:\\Users\\caucse\\Downloads\\bg\\"; // ★★★★ smoke detect image directory setting ★★★★
		inputString.append(to_string(i));
		inputString.append(".jpg");
		img[i] = imread(inputString, IMREAD_COLOR);
		if (!img[i].data) {
			cout << "SD Image Waiting....." << i - setting_num << endl;
			i--;
			Sleep(5000);
			continue;
		}
	}

	for (int i = setting_num; i < setting_num + detect_num; i++) {
		trans[i] = &IplImage(img[i]);
		outputString = "sdr_";
		outputString.append(to_string(i - setting_num));
		outputString.append(".jpg");
		img[i] = rotate_pi(img[i], trans[i]->width, trans[i]->height);
		imwrite(outputString, img[i]);
	}

	// sdi.assign(detect_num, tbg);
	for (int i = setting_num; i < setting_num + detect_num; i++) {
		bgi[i].img = img[i];
	}

	for (int i = setting_num; i < setting_num + detect_num; i++) {

		bgi[i].trans = &IplImage(bgi[i].img);
		int width = bgi[i].trans->width;
		int height = bgi[i].trans->height;
		int threshold = 100;

		mallocMaps(width, height, i);
		transImgToArray(bgi[i].trans, i);

		grayscaling(width, height, i);
		//binarization(width, height, i);

		IppHarrisCorner(width, height, threshold, i);
		// if (i == 11)
		// setRedPoint(bgi[i].trans); //
		grouping();
		alloc_keyPoint(i);
		// if (i == 11)
		// setRedBox(bgi[i].trans, height, width, i); //
		group_index.clear();

		outputString = to_string(i - setting_num);
		outputString.append("_sd");
		
		//for (int k = 0; k < bgi[i].keypg.size(); k++) {
		//	test_avg_loc(bgi[i].keypg[k].avg_location.x, bgi[i].keypg[k].avg_location.y, i);
		//}
		
		cvShowImage(outputString.c_str(), bgi[i].trans);
	}

	// debug ****************************************
	//
	//for (int i = 0; i < setting_num - 1; i++) {
	//	imshow("test_comp", check_nearby(i, i + 1));
	//	waitKey(0);
	//}
	//imshow("test_comp", check_nearby(4, 5));
	//waitKey(0);
	//imshow("test_comp", check_nearby(6, 12));
	//waitKey(0);
	//
	// debug ****************************************

	Mat temp[9];
	
	for (int i = 1; i < detect_num; i++) {
		proc_BnS[0].img = check_nearby(0, setting_num + i);
		bgi[setting_num + detect_num] = proc_BnS[0];
		
		if (bgFlag % setting_num == 0) {
			proc_BnS[1].img = check_nearby((bgFlag / setting_num) - 1, bgFlag / setting_num);
			bgi[setting_num + detect_num + 1] = proc_BnS[1];
		}
		else {
			proc_BnS[1].img = check_nearby(bgFlag / setting_num, (bgFlag / setting_num) + 1);
			bgi[setting_num + detect_num + 1] = proc_BnS[1];
		}
		// proc_BnS[1].img = check_nearby(4, 5); //
		// bgi[setting_num + detect_num + 1] = proc_BnS[1]; //

		for (int j = setting_num + detect_num + 1; j >= setting_num + detect_num; j--) {

			bgi[j].trans = &IplImage(bgi[j].img);

			int threshold = 100;

			mallocMaps(bgi[j].img.size().width, bgi[j].img.size().height, j);
			transImgToArray(bgi[j].trans, j);

			grayscaling(bgi[j].img.size().width, bgi[j].img.size().height, j);
			//binarization(width, height, i);

			IppHarrisCorner(bgi[j].img.size().width, bgi[j].img.size().height, threshold, j);

			// setRedPoint(bgi[j].trans); //
			grouping();
			alloc_keyPoint(j);

			// setRedBox(bgi[j].trans, bgi[j].img.size().height, bgi[j].img.size().width, j); //
			group_index.clear();
			// trans[0] = &IplImage(bgi[setting_num + detect_num].img);
			// trans[1] = &IplImage(bgi[setting_num + detect_num + 1].img);
		}

		for (int j = 0; j < 2; j++) {
			imshow("test_comp", proc_BnS[j].img);
			waitKey(0);
		}
		if (proc_BnS[1].img.size().width <= std_width + 5 || proc_BnS[0].img.size().width <= std_width + 5) { // exception handling
			break;
		}
		proc_BnS[1].img = resize_comp(1);
		imshow("test_comp", proc_BnS[1].img);
		waitKey(0);
		Mat sub_bg = subBackground();
		imshow("test_sub_bg", subBackground());
		waitKey(0);
		imshow("test_find", findCigar(sub_bg));
		waitKey(0);
		picture_rank.push_back(i + setting_num);
	}		

	/*
	for (int i = 0; i < 9; i++) { // composite test
		temp[i] = check_nearby(i, i + 1);
		imshow("test_comp", temp[i]);
		waitKey(0);
	}*/
	
	cout << ((end - begin)) << "ms" << endl;

	if (picture_rank.size() == 0) { // exception handling
		for (int i = 0; i < 4; i++) {
			picture_rank.push_back(setting_num + i);
		}
	}
	else if (picture_rank.size() == 1) {
		for (int i = setting_num; i < setting_num + 4; i++) {
			if (i == picture_rank[0]) {
				continue;
			}
			picture_rank.push_back(i);
		}
	}
	else if (picture_rank.size() == 2) {
		for (int i = setting_num; i < setting_num + 4; i++) {
			if (i == picture_rank[0] || i == picture_rank[1]) {
				continue;
			}
			picture_rank.push_back(i);
		}
	}
	else if (picture_rank.size() == 3) {
		for (int i = setting_num; i < setting_num + 4; i++) {
			if (i == picture_rank[0] || i == picture_rank[1] || i == picture_rank[2]) {
				continue;
			}
			picture_rank.push_back(i);
		}
	}

	ofstream out;
	out.open("output.txt");
	int result = 0;
	for (int i = detect_num - 1; i >= 0; i--) {
		result += picture_rank[detect_num - i - 1];
		result *= 100;
	}
	out << result / 100;
	out.close();

	return 0;
}

Mat rotate_pi(Mat img, int width, int height) { // rotate 180 degree
	Mat ret = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			for (int k = 0; k < 3; k++) {
				ret.at<Vec3b>(i, j)[k] = img.at<Vec3b>(height - i - 1, width - j - 1)[k];
			}
		}
	}
	return ret;
}

void IppHarrisCorner(int width, int height, double th, int img_index) {
	register int i, j, x, y;

	int w = width;
	int h = height;

	mallocByte(w, h);
	assignPtr(w, h, img_index);
	
	// 1. (fx)*(fx), (fx)*(fy), (fy)*(fy)
	
	float **dx2, **dy2, **dxy;

	dx2 = new float*[height];
	dy2 = new float*[height];
	dxy = new float*[height];

	for (int i = 0; i < height; i++) {
		dx2[i] = new float[width];
		dy2[i] = new float[width];
		dxy[i] = new float[width];
	}

	float tx, ty;
	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			if (j == 0 || i == 0 || j == h - 1 || i == w - 1) {
				dx2[j][i] = 0;
				dy2[j][i] = 0;
				dxy[j][i] = 0;
				continue;
			}

			tx = (ptr[j - 1][i + 1] + ptr[j][i + 1] + ptr[j + 1][i + 1]
				- ptr[j - 1][i - 1] - ptr[j][i - 1] - ptr[j + 1][i - 1]) / 6.f;
			ty = (ptr[j + 1][i - 1] + ptr[j + 1][i] + ptr[j + 1][i + 1]
				- ptr[j - 1][i - 1] - ptr[j - 1][i] - ptr[j - 1][i + 1]) / 6.f;

			dx2[j][i] = tx * tx;
			dy2[j][i] = ty * ty;
			dxy[j][i] = tx * ty;
		}
	}

	// 2. filtering
	
	float **gdx2, **gdy2, **gdxy;
	gdx2 = new float*[height];
	gdy2 = new float*[height];
	gdxy = new float*[height];

	for (int i = 0; i < height; i++) {
		gdx2[i] = new float[width];
		gdy2[i] = new float[width];
		gdxy[i] = new float[width];
	}

	float g[5][5] = { { 1, 4, 6, 4, 1 },{ 4, 16, 24, 16, 4 },
	{ 6, 24, 36, 24, 6 },{ 4, 16, 24, 16, 4 },{ 1, 4, 6, 4, 1 } };

	for (y = 0; y < 5; y++) {
		for (x = 0; x < 5; x++) {
			g[y][x] /= 256.f;
		}
	}

	float tx2, ty2, txy;
	for (j = 2; j < h - 2; j++) {
		for (i = 2; i < w - 2; i++) {
			tx2 = ty2 = txy = 0;
			for (y = 0; y < 5; y++) {
				for (x = 0; x < 5; x++) {
					tx2 += (dx2[j + y - 2][i + x - 2] * g[y][x]);
					ty2 += (dy2[j + y - 2][i + x - 2] * g[y][x]);
					txy += (dxy[j + y - 2][i + x - 2] * g[y][x]);
				}
			}
			gdx2[j][i] = tx2;
			gdy2[j][i] = ty2;
			gdxy[j][i] = txy;
		}
	}

	// 3. check corner

	float **crf;
	crf = new float*[height];

	for (int i = 0; i < height; i++) {
		crf[i] = new float[width];
	}

	float k = 0.04f;
	for (j = 2; j < h - 2; j++) {
		for (i = 2; i < w - 2; i++) {
			crf[j][i] = (gdx2[j][i] * gdy2[j][i] - gdxy[j][i] * gdxy[j][i])
				- k * (gdx2[j][i] + gdy2[j][i])*(gdx2[j][i] + gdy2[j][i]);
		}
	}

	// 4. decision by th

	corner.clear();

	float cvf_value;

	for (j = 2; j < h - 2; j++) {
		for (i = 2; i < w - 2; i++) {
			cvf_value = crf[j][i];
			/*
			if (cvf_value != 0) {
				printf(" hit \n");
			}
			*/
			if (cvf_value > th) {
				if (cvf_value > crf[j - 1][i] && cvf_value > crf[j - 1][i + 1] &&
					cvf_value > crf[j][i + 1] && cvf_value > crf[j + 1][i + 1] &&
					cvf_value > crf[j + 1][i] && cvf_value > crf[j + 1][i - 1] &&
					cvf_value > crf[j][i - 1] && cvf_value > crf[j - 1][i - 1]) {
					location l;
					l.x = i;
					l.y = j;
					group_index.push_back(corner.size());
					corner.push_back(l);
				}
			}
		}
	}
}

void mallocMaps(int width, int height, int img_index) { // malloc Maps array (size : width * height)
	bgi[img_index].Maps = new PIXEL*[height];
	for (int i = 0; i < height; i++) {
		bgi[img_index].Maps[i] = new PIXEL[width];
	}
}

void mallocByte(int width, int height) { // malloc Maps array (size : width * height)
	ptr = new uint8_t*[height];
	for (int i = 0; i < height; i++) {
		ptr[i] = new uint8_t[width];
	}
}

void assignPtr(int width, int height, int img_index) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			ptr[i][j] = bgi[img_index].Maps[i][j].gray;
		}
	}
}

void transImgToArray(IplImage* img, int img_index) { // take image's info to Maps array
	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			bgi[img_index].Maps[i][j].red = img->imageData[i*img->widthStep + j * img->nChannels + 2];
			bgi[img_index].Maps[i][j].green = img->imageData[i*img->widthStep + j * img->nChannels + 1];
			bgi[img_index].Maps[i][j].blue = img->imageData[i*img->widthStep + j * img->nChannels + 0];
			bgi[img_index].Maps[i][j].gray = (bgi[img_index].Maps[i][j].red + bgi[img_index].Maps[i][j].green + bgi[img_index].Maps[i][j].blue) / 3;
		}
	}
}

void setRedPoint(IplImage* img) {
	int dx, dy;
	for (int i = 0; i < corner.size(); i++) {
		for (dx = -2; dx < 2; dx++) {
			for (dy = -2; dy < 2; dy++) {
				if (corner[i].y + dy < 0 || corner[i].y + dy >= std_height || corner[i].x + dx < 0 || corner[i].x + dx > img->width) {
					continue;
				}
				img->imageData[(corner[i].y + dy) * img->widthStep + (corner[i].x + dx)* img->nChannels + 2] = 255; // red
				img->imageData[(corner[i].y + dy) * img->widthStep + (corner[i].x + dx) * img->nChannels + 1] = 0; // green
				img->imageData[(corner[i].y + dy) * img->widthStep + (corner[i].x + dx)* img->nChannels + 0] = 0; // blue
			}
		}
	}
}

void setRedBox(IplImage* img, int height, int width, int img_index) {
	for (int i = 0; i < bgi[img_index].keypg.size(); i++) {
		for (int YStep = 0; YStep < height; YStep++) {
			for (int XStep = 0; XStep < width; XStep++) {
				if ((XStep > bgi[img_index].keypg[i].end_point[3].x - 2 && XStep < bgi[img_index].keypg[i].end_point[3].x) || (XStep > bgi[img_index].keypg[i].end_point[1].x && XStep < bgi[img_index].keypg[i].end_point[1].x + 2)) {
					if (YStep > bgi[img_index].keypg[i].end_point[0].y - 2 && YStep < bgi[img_index].keypg[i].end_point[2].y + 2) {
						img->imageData[(YStep)* img->widthStep + (XStep)* img->nChannels + 2] = 255; // red
						img->imageData[(YStep)* img->widthStep + (XStep)* img->nChannels + 1] = 0; // red
						img->imageData[(YStep)* img->widthStep + (XStep)* img->nChannels + 0] = 0; // red
					}
				}
				if ((YStep > bgi[img_index].keypg[i].end_point[0].y - 2 && YStep < bgi[img_index].keypg[i].end_point[0].y) || (YStep > bgi[img_index].keypg[i].end_point[2].y && YStep < bgi[img_index].keypg[i].end_point[2].y + 2)) {
					if (XStep > bgi[img_index].keypg[i].end_point[3].x - 2 && XStep < bgi[img_index].keypg[i].end_point[1].x + 2) {
						img->imageData[(YStep)* img->widthStep + (XStep)* img->nChannels + 2] = 255; // red
						img->imageData[(YStep)* img->widthStep + (XStep)* img->nChannels + 1] = 0; // red
						img->imageData[(YStep)* img->widthStep + (XStep)* img->nChannels + 0] = 0; // red
					}
				}
			}
		}
	}
}

void grayscaling(int width, int height, int img_index) { // image processing (to grayscale)
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			bgi[img_index].Maps[i][j].red = bgi[img_index].Maps[i][j].gray;
			bgi[img_index].Maps[i][j].green = bgi[img_index].Maps[i][j].gray;
			bgi[img_index].Maps[i][j].blue = bgi[img_index].Maps[i][j].gray;
		}
	}
}

void binarization(int width, int height, int img_index) { // image processing (to binary)
	int threshold = 127;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (bgi[img_index].Maps[i][j].gray > threshold) {
				bgi[img_index].Maps[i][j].gray = 255;
				bgi[img_index].Maps[i][j].red = 255;
				bgi[img_index].Maps[i][j].green = 255;
				bgi[img_index].Maps[i][j].blue = 255;
			}
			else {
				bgi[img_index].Maps[i][j].gray = 0;
				bgi[img_index].Maps[i][j].red = 0;
				bgi[img_index].Maps[i][j].green = 0;
				bgi[img_index].Maps[i][j].blue = 0;
			}
		}
	}
}

int find(int a, vector<int> & v) {
	if (v[a] == a) {
		return a;
	}
	else {
		v[a] = find(v[a], v);
		return v[a];
	}
}

void join(int a, int b, vector<int> & vt) {
	int aroot = find(a, vt);
	int broot = find(b, vt);
	vt[aroot] = broot;
}

void grouping() {
	for (int i = 1; i < corner.size(); i++) {
		for (int j = 0; j < i; j++) {
			decision(group_index, i, j);
		}
	}
}

void decision(vector<int> & vt, int n1, int n2) {

	int tx = abs(corner[n1].x - corner[n2].x);
	int ty = abs(corner[n1].y - corner[n2].y);

	int distance = (tx * tx) + (ty * ty);

	if (distance < 500) {
		join(n1, n2, vt);
	}
}

int abs(int a) {
	if (a < 0)
		return (-a);
	else
		return a;
}

void alloc_keyPoint(int img_index) {

	int size = group_index.size();
	int flag;

	push_keyPoint(0, img_index);

	for (int i = 1; i < size; i++) {
		flag = 0;
		for (int j = 0; j < bgi[img_index].keypg.size(); j++) {
			if (bgi[img_index].keypg[j].group_num == group_index[i]) {
				// 해당 keypg.size  ++
				bgi[img_index].keypg[j].size++;
				bgi[img_index].keypg[j].avg_location.x += corner[i].x;
				bgi[img_index].keypg[j].avg_location.y += corner[i].y;

				//비교 0 하 우 
				if (corner[i].y < bgi[img_index].keypg[j].end_point[0].y) {
					bgi[img_index].keypg[j].end_point[0].x = corner[i].x;
					bgi[img_index].keypg[j].end_point[0].y = corner[i].y;
				}
				if (corner[i].x > bgi[img_index].keypg[j].end_point[1].x) {
					bgi[img_index].keypg[j].end_point[1].x = corner[i].x;
					bgi[img_index].keypg[j].end_point[1].y = corner[i].y;
				}
				if (corner[i].y > bgi[img_index].keypg[j].end_point[2].y) {
					bgi[img_index].keypg[j].end_point[2].x = corner[i].x;
					bgi[img_index].keypg[j].end_point[2].y = corner[i].y;
				}
				if (corner[i].x < bgi[img_index].keypg[j].end_point[3].x) {
					bgi[img_index].keypg[j].end_point[3].x = corner[i].x;
					bgi[img_index].keypg[j].end_point[3].y = corner[i].y;
				}
				flag = 1;
				break;
			}
		}
		if (flag == 0) {
			push_keyPoint(i, img_index);
		}
	}
	for (int i = 0; i < bgi[img_index].keypg.size(); i++) {
		bgi[img_index].keypg[i].avg_location.x /= bgi[img_index].keypg[i].size;
		bgi[img_index].keypg[i].avg_location.y /= bgi[img_index].keypg[i].size;
	}
}

void push_keyPoint(int n, int img_index) {

	keypoint_group tmp;
	tmp.group_num = group_index[n];
	tmp.size = 1;
	tmp.avg_location.x = corner[n].x;
	tmp.avg_location.y = corner[n].y;

	tmp.end_point[0].x = corner[n].x;
	tmp.end_point[1].x = corner[n].x;
	tmp.end_point[2].x = corner[n].x;
	tmp.end_point[3].x = corner[n].x;
	tmp.end_point[0].y = corner[n].y;
	tmp.end_point[1].y = corner[n].y;
	tmp.end_point[2].y = corner[n].y;
	tmp.end_point[3].y = corner[n].y;

	bgi[img_index].keypg.push_back(tmp);
}

Mat check_nearby(int a, int b) {
	int i, j;
	int x, y;
	float rgb[2][3];
	float darkness;
	Point pt[2];
	for (i = bgi[a].keypg.size() - 1; i > 0; i--) {
		for (j = 0; j < bgi[b].keypg.size(); j++) {
			if (bgi[a].keypg[i].size > 3 && abs(bgi[a].keypg[i].avg_location.y - bgi[b].keypg[j].avg_location.y) < 5) { // height compare
				if (bgi[a].keypg[i].size > 5 && bgi[b].keypg[j].size > 5 && bgi[a].keypg[i].size > (bgi[b].keypg[j].size / 2) && bgi[a].keypg[i].size < (bgi[b].keypg[j].size * 2)) { // group size compare
					pt[0].x = bgi[a].keypg[i].avg_location.x;
					pt[0].y = bgi[a].keypg[i].avg_location.y;
					pt[1].x = bgi[b].keypg[j].avg_location.x;
					pt[1].y = bgi[b].keypg[j].avg_location.y;
					rgb[0][0] = 0;
					rgb[0][1] = 0;
					rgb[0][2] = 0;
					rgb[1][0] = 0;
					rgb[1][1] = 0;
					rgb[1][2] = 0;
					for (x = -2; x < 2; x++) {
						for (y = -2; y < 2; y++) {
							rgb[0][0] += bgi[a].img.at<Vec3b>(pt[0].y + y, pt[0].x + x)[0];
							rgb[0][1] += bgi[a].img.at<Vec3b>(pt[0].y + y, pt[0].x + x)[1];
							rgb[0][2] += bgi[a].img.at<Vec3b>(pt[0].y + y, pt[0].x + x)[2];
							rgb[1][0] += bgi[b].img.at<Vec3b>(pt[1].y + y, pt[1].x + x)[0];
							rgb[1][1] += bgi[b].img.at<Vec3b>(pt[1].y + y, pt[1].x + x)[1];
							rgb[1][2] += bgi[b].img.at<Vec3b>(pt[1].y + y, pt[1].x + x)[2];
						}
					}
					rgb[0][0] /= 25;
					rgb[0][1] /= 25;
					rgb[0][2] /= 25;
					rgb[1][0] /= 25;
					rgb[1][1] /= 25;
					rgb[1][2] /= 25;
					darkness = (rgb[0][0] + rgb[0][1] + rgb[0][2]) / (rgb[1][0] + rgb[1][1] + rgb[1][2]);
					rgb[1][0] *= darkness;
					rgb[1][1] *= darkness;
					rgb[1][2] *= darkness;
					if (abs(rgb[0][0] - rgb[1][0]) < 20 && abs(rgb[0][1] - rgb[1][1]) < 20 && abs(rgb[0][2] - rgb[1][2]) < 20) { // pixel compare
						return composite_by_point(a, b, i, j);
					}					
				}
			}
		}
	}
	if (a + 1 == setting_num || a + 1 == b) {
		return bgi[b].img;
	}
	return check_nearby(a + 1, b);
}

Mat composite_by_point(int a, int b, int i, int j) {

	int widthdiff = bgi[a].keypg[i].avg_location.x - bgi[b].keypg[j].avg_location.x;
	int max_width;
	int tempToSwap;
	int newSize;
	if (widthdiff < 0) {
		widthdiff *= -1;
		tempToSwap = a;
		a = b;
		b = tempToSwap;
		tempToSwap = i;
		i = j;
		j = tempToSwap;
	}
	max_width = std_width + widthdiff;
	Mat img_Result(std_height, max_width, CV_8UC3);
	if (a > b) {
		newSize = std_width;
		bgFlag = b * setting_num; // left sd, right bg
	}
	else {
		newSize = std_width;//widthdiff;
		bgFlag = a * setting_num + 1; // right sd, left bg
	}

	for (int y = 0; y < std_height; y++) {
		for (int x = 0; x < std_width + widthdiff; x++) {
			for (int k = 0; k < 3; k++) {
				if (x < newSize) {
					img_Result.at<Vec3b>(y, x)[k] = bgi[a].img.at<Vec3b>(y, x)[k];
				}
				else {
					img_Result.at<Vec3b>(y, x)[k] = bgi[b].img.at<Vec3b>(y, x - widthdiff)[k];
				}
			}
		}
	}
		
	
	for (int y = -2; y < 2; y++) { // key group test
		for (int x = -2; x < 2; x++) {
			img_Result.at<Vec3b>(bgi[a].keypg[i].avg_location.y + y, bgi[a].keypg[i].avg_location.x + x)[0] = 0;
			img_Result.at<Vec3b>(bgi[a].keypg[i].avg_location.y + y, bgi[a].keypg[i].avg_location.x + x)[1] = 255;
			img_Result.at<Vec3b>(bgi[a].keypg[i].avg_location.y + y, bgi[a].keypg[i].avg_location.x + x)[2] = 255;
		}
	}

	
	return img_Result;
}

void test_avg_loc(int x, int y, int img_index) {
	int index;
	for (int i = -4; i < 4; i++) {
		for (int j = -4; j < 4; j++) {
			if (x + i >= 0 && x + i < std_width && y + j >= 0 && y + j < std_height) {
				index = 3 * ((x + i) + (y + j) * std_width);
				bgi[img_index].trans->imageData[index] = 0;
				bgi[img_index].trans->imageData[index + 1] = 255;
				bgi[img_index].trans->imageData[index + 2] = 255;
			}
		}
	}
}

Mat findCigar(Mat img) {

	int XStep;
	int YStep;
	int x, y;
	int i, j;
	int color_diff[3];
	int brightness;
	int compare = 0;
	int diff[4][3];
	int diffx[4] = { 0, 0, 1, -1 };
	int diffy[4] = { 1, -1, 0, 0 };
	unsigned long long CIx = 0;
	unsigned long long CIy = 0;
	unsigned long long CIp = 0;
	int xIndex, yIndex;
	int realSize;
	int cigarOrNot;

	int tempRed, tempGreen, tempBlue;

	int height = img.size().height;
	int width = img.size().width;
	Mat ret = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));

	//CxImage * buffer = new CxImage;
	//buffer->Create(width, height, 24, CXIMAGE_FORMAT_BMP);

	for (YStep = 0; YStep < height; YStep++) {
		for (XStep = 0; XStep < width; XStep++) {
			// bgi[img_index].img = bgi[img_index].Maps[XStep][YStep]; // get**********
			cigarOrNot = 0;
			color_diff[0] = img.at<Vec3b>(YStep, XStep)[0] - img.at<Vec3b>(YStep, XStep)[1];
			if (color_diff[0] < 0) {
				color_diff[0] *= -1;
			}
			color_diff[1] = img.at<Vec3b>(YStep, XStep)[1] - img.at<Vec3b>(YStep, XStep)[2];
			if (color_diff[1] < 0) {
				color_diff[1] *= -1;
			}
			color_diff[2] = img.at<Vec3b>(YStep, XStep)[2] - img.at<Vec3b>(YStep, XStep)[0];
			if (color_diff[2] < 0) {
				color_diff[2] *= -1;
			}
			brightness = (img.at<Vec3b>(YStep, XStep)[0] + img.at<Vec3b>(YStep, XStep)[1] + img.at<Vec3b>(YStep, XStep)[2]) / 3;
			if (XStep == width - 20 && YStep == height / 2) {
				i = 1;
			}

			if (color_diff[0] + color_diff[1] + color_diff[2] < 20 && brightness > 200) { // white cigar only
				cigarOrNot = 1;
			}

			if (cigarOrNot == 1) {
				for (int diff_len = -2; diff_len < 3; diff_len++) { // check thickness
					if (YStep + diff_len >= std_height || YStep + diff_len < 0) {
						continue;
					}
					color_diff[0] = img.at<Vec3b>(YStep + diff_len, XStep)[0] + img.at<Vec3b>(YStep + diff_len, XStep)[1] + img.at<Vec3b>(YStep, XStep)[2];
					if (color_diff[0] > 700) {
						cigarOrNot = 0;						
					}
					else {
						cigarOrNot = 1;
						break;
					}
				}
			}

			if (cigarOrNot == 1) {
				ret.at<Vec3b>(YStep, XStep)[0] = img.at<Vec3b>(YStep, XStep)[0];
				ret.at<Vec3b>(YStep, XStep)[1] = img.at<Vec3b>(YStep, XStep)[1];
				ret.at<Vec3b>(YStep, XStep)[2] = img.at<Vec3b>(YStep, XStep)[2];
				// NewImg[XStep][YStep] = newcolor; // set**********
				CIx += XStep;
				CIy += YStep;
				CIp++;
			}				
			
			else {
				ret.at<Vec3b>(YStep, XStep)[0] = 0; // img.at<Vec3b>(YStep, XStep)[0];
				ret.at<Vec3b>(YStep, XStep)[1] = 0; // img.at<Vec3b>(YStep, XStep)[1];
				ret.at<Vec3b>(YStep, XStep)[2] = 0; // img.at<Vec3b>(YStep, XStep)[2];
				// NewImg[XStep][YStep] = newcolor; // set**********
			}
		}
	}
	CIx /= CIp;
	CIy /= CIp;

	return ret;
	// draw redbox
	/*
	for (YStep = 0; YStep < height; YStep++) {
		for (XStep = 0; XStep < width; XStep++) {
			if ((XStep > CIx - 55 && XStep < CIx - 50) || (XStep > CIx + 50 && XStep < CIx + 55)) {
				if (YStep > CIy - 55 && YStep < CIy + 55) {
					newcolor.blue = 0;
					newcolor.green = 0;
					newcolor.red = 255;
					// NewImg[XStep][YStep] = newcolor; // set**********
				}
			}
			if ((YStep > CIy - 55 && YStep < CIy - 50) || (YStep > CIy + 50 && YStep < CIy + 55)) {
				if (XStep > CIx - 55 && XStep < CIx + 55) {
					newcolor.blue = 0;
					newcolor.green = 0;
					newcolor.red = 255;
					// NewImg[XStep][YStep] = newcolor; // set**********
				}
			}
		}
	}
	*/
}

Mat subBackground() {

	float tempBlue, tempGreen, tempRed, tempGray;

	int height = bgi[setting_num + detect_num].img.size().height;
	int width = bgi[setting_num + detect_num].img.size().width;

	int x, y;
	int bx, by;
	int tempSize;
	int diff;
	float brightScale = 1;
	float firstBright;
	float secondBright;
	int isCigar = 0;
	int boundary[3];
	Mat ret = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {

			diff = 0;
			// bgi[setting_num + detect_num + 1] = bgi[img_index_bg].Maps[x][y]; // get**********
			tempBlue = bgi[setting_num + detect_num + 1].img.at<Vec3b>(y, x)[0];
			tempGreen = bgi[setting_num + detect_num + 1].img.at<Vec3b>(y, x)[1];
			tempRed = bgi[setting_num + detect_num + 1].img.at<Vec3b>(y, x)[2];

			// bgi[setting_num + detect_num] = [x][y]; // get**********
			firstBright = tempBlue + tempGreen + tempRed;
			secondBright = bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[0] + bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[1] + bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[2];

			if ((abs(bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[0] - bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[1]) + abs(bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[1] - bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[2]) + abs(bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[2] - bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[0])) > 20) {
				brightScale = firstBright / secondBright;
			}
			
			else { // white pixel (may cigar)
				brightScale = 1;
				/*
				for (bx = -5; bx <= 5; bx++) {
					if (bx + x < 0 || bx + x >= width) {
						continue;
					}
					// bgi[setting_num + detect_num + 1] = [x][y]; // get**********
					boundary[0] = abs(bgi[setting_num + detect_num + 1].img.at<Vec3b>(y, x)[0] - bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[0]);
					boundary[1] = abs(bgi[setting_num + detect_num + 1].img.at<Vec3b>(y, x)[1] - bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[1]);
					boundary[2] = abs(bgi[setting_num + detect_num + 1].img.at<Vec3b>(y, x)[2] - bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[2]);
					if (boundary[0] + boundary[1] + boundary[2] > 20) {
						diff = 1;
					}
				}
				for (by = -5; by <= 5; by++) {

				}*/
			}

			tempBlue = abs(bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[0] * brightScale - tempBlue);
			tempGreen = abs(bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[1] * brightScale - tempGreen);
			tempRed = abs(bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[2] * brightScale - tempRed);
			tempGray = tempBlue + tempGreen + tempRed;

			if (tempBlue > 10) {
				diff = 1;
			}
			else if (tempGreen > 10) {
				diff = 1;
			}
			else if (tempRed > 10) {
				diff = 1;
			}
			else if (tempGray > 60) {
				diff = 1;
			}
			/*
			if (diff == 1) { // sub little change of location (by camera env.)
				for (bx = -3; bx <= 3; bx++) {
					for (by = -3; by <= 3; by++) {
						if (x + bx < 0 || x + bx >= width || y + by < 0 || y + by >= height) {
							continue;
						}
						tempBlue = bgi[setting_num + detect_num + 1].img.at<Vec3b>(by + y, bx + x)[0] - bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[0];
						tempGreen = bgi[setting_num + detect_num + 1].img.at<Vec3b>(by + y, bx + x)[1] - bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[1];
						tempRed = bgi[setting_num + detect_num + 1].img.at<Vec3b>(by + y, bx + x)[2] - bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[2];
						if (abs(tempBlue - tempGreen) + abs(tempGreen - tempRed) + abs(tempRed - tempBlue) > 30) {
							continue;
						}
						tempGray = abs(tempBlue) + abs(tempGreen) + abs(tempRed);
						if (abs(tempBlue) < 20 && abs(tempGreen) < 20 && abs(tempRed) < 20 && tempGray < 60) {
							diff = 0;
							bx = 6;
							break;
						}
					}
				}
			}*/
			if (y < 36) {
				diff = 0;
			}
			
			if (diff == 1) {
				diff = 0;
				for (int k_index = 0; k_index < corner.size(); k_index++) {
					if (y > corner[k_index].y - 5 && y < corner[k_index].y + 5) { // up, down
						if (x > corner[k_index].x - 5 && x < corner[k_index].x + 5) { // left, right 
							diff = 1;
						}
					}
				}		
			}

			if (diff == 1) {
				ret.at<Vec3b>(y, x)[0] = bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[0];
				ret.at<Vec3b>(y, x)[1] = bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[1];
				ret.at<Vec3b>(y, x)[2] = bgi[setting_num + detect_num].img.at<Vec3b>(y, x)[2];
			}

			else {
				ret.at<Vec3b>(y, x)[0] = 0;
				ret.at<Vec3b>(y, x)[1] = 0;
				ret.at<Vec3b>(y, x)[2] = 0;
			}
		}
	}
	return ret;
}

Mat resize_comp(int img_index) {
	int compIndex = 1 - img_index;
	int height = proc_BnS[compIndex].img.size().height;
	int width = proc_BnS[compIndex].img.size().width;
	Mat ret = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			for (int k = 0; k < 3; k++) {
				ret.at<Vec3b>(i, j)[k] = proc_BnS[img_index].img.at<Vec3b>(i, j)[k];
			}
		}
	}
	return ret;
}
