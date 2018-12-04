#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <memory.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>
#include <string>
#include <Windows.h>

#define setting_num 10
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
	pt end_point[4]; // 0 상 1 우 2 하 3 좌
} keypoint_group;

// PIXEL **Maps;
PIXEL **Bgs;
PIXEL **NewImg;

uint8_t **ptr;

vector<location> corner;
vector<int> group_index;
// vector<keypoint_group> keypg;

typedef struct {
	Mat img;
	IplImage *trans;
	PIXEL **Maps;
	vector<keypoint_group> keypg;
} background_info ;

vector<background_info> bgi;
Mat rotate_pi(Mat img, int width, int height);
void IppHarrisCorner(int w, int h, double th, int img_index);
void mallocMaps(int width, int height, int img_index);
void mallocByte(int width, int height);
void assignPtr(int width, int height, int img_index);
void transImgToArray(IplImage* img, int img_index);
void grayscaling(int width, int height, int img_index);
void binarization(int width, int height, int img_index);
void setRedPoint(IplImage* img);
int find(int a, vector<int> & v);
void join(int a, int b, vector<int> & vt);
void grouping();
void decision(vector<int> & vt, int n1, int n2);
int abs(int a);
void alloc_keyPoint(int img_index);
void push_keyPoint(int n, int img_index);
void setRedBox(IplImage* img, int height, int width, int img_index);
void findCigar(int width, int height, int img_index);
void subBackground(int width, int height, int img_index);
Mat check_nearby(int a, int b);
Mat composite_by_point(int a, int b, int i, int j);

bool compare(const keypoint_group &a, const keypoint_group &b) {
	return a.avg_location.x < b.avg_location.x;
}

int main() {

	Mat img[setting_num];
	IplImage *trans[setting_num];
	string inputString;
	string outputString;

	for (int i = 0; i < setting_num; i++) {
		inputString = "C:\\"; // ★★★★★★ image directory setting ★★★★★★
		inputString.append(to_string(i));
		inputString.append(".jpg");
		img[i] = imread(inputString, IMREAD_COLOR);
		if (!img[i].data) {
			cout << "Waiting....." << i << endl;
			i--;
			Sleep(5000);
			continue;
		}
	}
	
	for (int i = 0; i < setting_num; i++) {
		trans[i] = &IplImage(img[i]);
		outputString = "r_";
		outputString.append(to_string(i));
		outputString.append(".jpg");
		img[i] = rotate_pi(img[i], trans[i]->width, trans[i]->height);
		imwrite(outputString, img[i]);
	}
	
	background_info tbg;
	bgi.assign(setting_num, tbg);
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
		//binarization(width, height);

		IppHarrisCorner(width, height, threshold, i);
		setRedPoint(bgi[i].trans);
		//cvResizeWindow("test.jpg", 320, 240);
		grouping();
		alloc_keyPoint(i);

		setRedBox(bgi[i].trans, height, width, i);
		group_index.clear();
		// keypg.clear();
		outputString = to_string(i);
		outputString.append(".jpg");
		cvShowImage(outputString.c_str(), bgi[i].trans);
		
	}

	Mat temp;
	temp = check_nearby(0, 1);
	imshow("test_comp", temp);
	waitKey(0);
	end = clock();
	cout << ((end - begin)) << "ms" << endl;
	
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

void IppHarrisCorner(int width, int height, double th, int img_index)
{
	register int i, j, x, y;

	int w = width;
	int h = height;

	mallocByte(w, h);
	assignPtr(w, h, img_index);
	//-------------------------------------------------------------------------
	// 1. (fx)*(fx), (fx)*(fy), (fy)*(fy) 계산
	//-------------------------------------------------------------------------

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

	//-------------------------------------------------------------------------
	// 2. 가우시안 필터링
	//-------------------------------------------------------------------------


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

	//-------------------------------------------------------------------------
	// 3. 코너 응답 함수 생성
	//-------------------------------------------------------------------------

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

	//-------------------------------------------------------------------------
	// 4. 임계값보다 큰 국지적 최댓값을 찾아 코너 포인트로 결정
	//-------------------------------------------------------------------------

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

void findCigar(int width, int height, int img_index) {

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

	PIXEL color;
	PIXEL newcolor;

	int tempRed, tempGreen, tempBlue;

	//CxImage * buffer = new CxImage;
	//buffer->Create(width, height, 24, CXIMAGE_FORMAT_BMP);

	for (YStep = 0; YStep < height; YStep++) {
		for (XStep = 0; XStep < width; XStep++) {
			color = bgi[img_index].Maps[XStep][YStep]; // get**********
			color_diff[0] = color.blue - color.green;
			if (color_diff[0] < 0) {
				color_diff[0] *= -1;
			}
			color_diff[1] = color.green - color.red;
			if (color_diff[1] < 0) {
				color_diff[1] *= -1;
			}
			color_diff[2] = color.red - color.blue;
			if (color_diff[2] < 0) {
				color_diff[2] *= -1;
			}
			brightness = (color.blue + color.green + color.red) / 3;
			if (XStep == width - 20 && YStep == height / 2) {
				i = 1;
			}

			if (color_diff[0] + color_diff[1] + color_diff[2] < 20 && brightness > 150) {
				newcolor.blue = color.blue;
				newcolor.green = color.green;
				newcolor.red = color.red;
				NewImg[XStep][YStep] = newcolor; // set**********
				CIx += XStep;
				CIy += YStep;
				CIp++;
			}
			else {
				newcolor.blue = color.blue;
				newcolor.green = color.green;
				newcolor.red = color.red;
				NewImg[XStep][YStep] = newcolor; // set**********
			}
		}
	}
	CIx /= CIp;
	CIy /= CIp;
	for (YStep = 0; YStep < height; YStep++) {
		for (XStep = 0; XStep < width; XStep++) {
			if ((XStep > CIx - 55 && XStep < CIx - 50) || (XStep > CIx + 50 && XStep < CIx + 55)) {
				if (YStep > CIy - 55 && YStep < CIy + 55) {
					newcolor.blue = 0;
					newcolor.green = 0;
					newcolor.red = 255;
					NewImg[XStep][YStep] = newcolor; // set**********
				}
			}
			if ((YStep > CIy - 55 && YStep < CIy - 50) || (YStep > CIy + 50 && YStep < CIy + 55)) {
				if (XStep > CIx - 55 && XStep < CIx + 55) {
					newcolor.blue = 0;
					newcolor.green = 0;
					newcolor.red = 255;
					NewImg[XStep][YStep] = newcolor; // set**********
				}
			}
		}
	}
}

Mat check_nearby(int a, int b) {
	int i, j;
	for (i = 0; i < bgi[a].keypg.size(); i++) {
		for (j = 0; j < bgi[b].keypg.size(); j++) {
			if (bgi[a].keypg[i].size > 3 && abs(bgi[a].keypg[i].avg_location.y - bgi[b].keypg[j].avg_location.y) < 20) {
				if (bgi[a].keypg[i].size > (bgi[b].keypg[j].size / 2) && bgi[a].keypg[i].size < (bgi[b].keypg[j].size * 2)) {
					return composite_by_point(a, b, i, j);
				}
			}
		}
	}
	return bgi[a].img;
}

Mat composite_by_point (int a, int b, int i, int j) {
	
	int widthdiff = bgi[a].keypg[i].avg_location.x - bgi[b].keypg[j].avg_location.x;
	int max_width;
	if (widthdiff > 0) {
		max_width = std_width + widthdiff;
	}
	else {
		max_width = std_width - widthdiff;
	}
	Mat img_Result(std_height, max_width, CV_8UC3);
	if (widthdiff > 0) {
		for (int y = 0; y < std_height; y++) {
			for (int x = 0; x < std_width + widthdiff; x++) {
				for (int i = 0; i < 3; i++) {
					if (x < std_width) {
						img_Result.at<Vec3b>(y, x)[i] = bgi[a].img.at<Vec3b>(y, x)[i];
					}
					else {
						img_Result.at<Vec3b>(y, x)[i] = bgi[b].img.at<Vec3b>(y, x - widthdiff)[i];
					}
				}
			}
		}
	}
	else {
		for (int y = 0; y < std_height; y++) {
			for (int x = 0; x < std_width + widthdiff; x++) {
				for (int i = 0; i < 3; i++) {
					if (x < std_width) {
						img_Result.at<Vec3b>(y, x)[i] = bgi[b].img.at<Vec3b>(y, x)[i];
					}
					else {
						img_Result.at<Vec3b>(y, x)[i] = bgi[a].img.at<Vec3b>(y, x - widthdiff)[i];
					}
				}
			}
		}
	}	

	return img_Result;
}

void subBackground(int width, int height, int img_index) {

	PIXEL firstColor;
	PIXEL secondColor;
	PIXEL newColor;

	float tempBlue, tempGreen, tempRed, tempGray;
	int bx, by;
	int tempSize;
	int diff;
	float brightScale = 1;
	float firstBright;
	float secondBright;
	int isCigar = 0;
	int boundary[3];
	int x, y;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {

			diff = 0;
			firstColor = bgi[img_index].Maps[x][y]; // get**********
			tempBlue = firstColor.blue;
			tempGreen = firstColor.green;
			tempRed = firstColor.red;

			secondColor = Bgs[x][y]; // get**********
			firstBright = tempBlue + tempGreen + tempRed;
			secondBright = secondColor.blue + secondColor.green + secondColor.red;

			if ((abs(secondColor.blue - secondColor.green) + abs(secondColor.green - secondColor.red) + abs(secondColor.red - secondColor.blue)) > 20) {
				brightScale = firstBright / secondBright;
			}
			else {
				for (bx = -5; bx <= 5; bx++) {
					if (bx + x < 0 || bx + x >= width) {
						continue;
					}
					firstColor = Bgs[x][y]; // get**********
					boundary[0] = abs(firstColor.blue - secondColor.blue);
					boundary[1] = abs(firstColor.green - secondColor.green);
					boundary[2] = abs(firstColor.red - secondColor.red);
					if (boundary[0] + boundary[1] + boundary[2] > 20) {
						diff = 1;
					}
				}
				for (by = -5; by <= 5; by++) {

				}
			}

			tempBlue = abs(secondColor.blue * brightScale - tempBlue);
			tempGreen = abs(secondColor.green * brightScale - tempGreen);
			tempRed = abs(secondColor.red * brightScale - tempRed);
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
			if (diff == 1) {
				newColor.blue = secondColor.blue;
				newColor.green = secondColor.green;
				newColor.red = secondColor.red;
			}
			else {
				newColor.blue = 0;
				newColor.green = 0;
				newColor.red = 0;
			}

			NewImg[x][y] = newColor; // set**********
		}
	}
}