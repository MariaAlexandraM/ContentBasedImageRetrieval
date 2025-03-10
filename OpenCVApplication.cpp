// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <opencv2/core/utils/logger.hpp>
#include <random>
#include <queue> 
#define EPSILON 0.0000000000000000000001
using namespace std;

wchar_t* projectPath;

default_random_engine gen;
uniform_int_distribution<int> d(0, 255);


char folderName[MAX_PATH];

void testOpenImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("image", src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName) == 0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName, "bmp");
	while (fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(), src);
		if (waitKey() == 27) //ESC pressed
			break;
		waitKey();
	}
}

void testImageOpenAndSave()
{
	_wchdir(projectPath);

	Mat src, dst;

	src = imread("Images/Lena_24bits.bmp", IMREAD_COLOR);	// Read the image

	if (!src.data)	// Check for invalid input
	{
		printf("Could not open or find the image\n");
		return;
	}

	// Get the image resolution
	Size src_size = Size(src.cols, src.rows);

	// Display window
	const char* WIN_SRC = "Src"; //window for the source image
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Dst"; //window for the destination (processed) image
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, src_size.width + 10, 0);

	cvtColor(src, dst, COLOR_BGR2GRAY); //converts the source image to a grayscale one

	imwrite("Images/Lena_24bits_gray.bmp", dst); //writes the destination to file

	imshow(WIN_SRC, src);
	imshow(WIN_DST, dst);

	waitKey(0);
}

void testBGR2HSV()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);
		int height = src.rows;
		int width = src.cols;

		// HSV components
		Mat H = Mat(height, width, CV_8UC1);
		Mat S = Mat(height, width, CV_8UC1);
		Mat V = Mat(height, width, CV_8UC1);

		// Defining pointers to each matrix (8 bits/pixels) of the individual components H, S, V 
		uchar* lpH = H.data;
		uchar* lpS = S.data;
		uchar* lpV = V.data;

		Mat hsvImg;
		cvtColor(src, hsvImg, COLOR_BGR2HSV);

		// Defining the pointer to the HSV image matrix (24 bits/pixel)
		uchar* hsvDataPtr = hsvImg.data;

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int hi = i * width * 3 + j * 3;
				int gi = i * width + j;

				lpH[gi] = hsvDataPtr[hi] * 510 / 360;	// lpH = 0 .. 255
				lpS[gi] = hsvDataPtr[hi + 1];			// lpS = 0 .. 255
				lpV[gi] = hsvDataPtr[hi + 2];			// lpV = 0 .. 255
			}
		}

		imshow("input image", src);
		imshow("H", H);
		imshow("S", S);
		imshow("V", V);

		waitKey();
	}
}

void testResize()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		Mat dst1, dst2;
		//without interpolation
		resizeImg(src, dst1, 320, false);
		//with interpolation
		resizeImg(src, dst2, 320, true);
		imshow("input image", src);
		imshow("resized image (without interpolation)", dst1);
		imshow("resized image (with interpolation)", dst2);
		waitKey();
	}
}

void showHistogram(const std::string& name, int* hist, const int  hist_cols, const int hist_height)
{
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i < hist_cols; i++)
		if (hist[i] > max_hist)
			max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
}

bool isInside(Mat img, int i, int j) {
	return (i < img.rows && i >= 0
		&& j < img.cols && j >= 0);
}

void showHistogram(const std::string& name, double* hist, const int  hist_cols, const int hist_height)
{
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	double max_hist = 0;
	for (int i = 0; i < hist_cols; i++)
		if (hist[i] > max_hist)
			max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
}

Mat convolutieFTJ(Mat src, double H[][5], int w) { // w = 5
	Mat dst = src.clone();
	int height = src.rows;
	int width = src.cols;

	int k = (w - 1) / 2; // = 1; k ii pt contur

	double sumaConvolutie = 0.0; // ca sa fac gen la medie: 1/9 sau la gaussian: 1/16

	for (int u = 0; u < w; u++) {
		for (int v = 0; v < w; v++) {
			sumaConvolutie += H[u][v];
		}
	}

	for (int i = k; i < height - k; i++) {
		for (int j = k; j < height - k; j++) {
			int val_dst = 0;

			for (int u = 0; u < w; u++) {
				for (int v = 0; v < w; v++) {
					int val_src = (int)src.at<uchar>(i + u - k, j + v - k);
					val_dst += H[u][v] * val_src * 1.0;
				}
			}
			val_dst /= sumaConvolutie * 1.0;

			if (val_dst < 0) {
				val_dst = 0;
			}
			else if (val_dst > 255) {
				val_dst = 255;
			}

			dst.at<uchar>(i, j) = (uchar)val_dst;
		}
	}
	return dst;
}

Mat FTJ(Mat src) {
	int height = src.rows;
	int width = src.cols;

	double H_Gauss[5][5] = { 0.0005, 0.0050, 0.0109, 0.0050, 0.0005,
						 0.0050, 0.0521, 0.1139, 0.0521, 0.0050,
						 0.0109, 0.1139, 0.2487, 0.1139, 0.0109,
						 0.0050, 0.0521, 0.1139, 0.0521, 0.0050,
						 0.0005, 0.0050, 0.0109, 0.0050, 0.0005 };

	Mat ftj = Mat(height, width, CV_8UC1);

	int w = 5;
	ftj = convolutieFTJ(src, H_Gauss, w);
	return ftj;

}

Mat ftjPeCanaleRGB(Mat src) {

	Mat newMat = Mat(src.rows, src.cols, CV_8UC3);

	int height = src.rows;
	int width = src.cols;

	Mat r = Mat(height, width, CV_8UC1);
	Mat g = Mat(height, width, CV_8UC1);
	Mat b = Mat(height, width, CV_8UC1);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Vec3b aux = src.at<Vec3b>(i, j);
			b.at<uchar>(i, j) = aux[0];
			g.at<uchar>(i, j) = aux[1];
			r.at<uchar>(i, j) = aux[2];
		}
	}

	Mat ftj_R = FTJ(r);
	Mat ftj_G = FTJ(g);
	Mat ftj_B = FTJ(b);

	/*vectorFiltre.push_back(ftj_R);
	vectorFiltre.push_back(ftj_G);
	vectorFiltre.push_back(ftj_B);*/

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Vec3b culori;
			culori[0] = ftj_B.at<uchar>(i, j);
			culori[1] = ftj_G.at<uchar>(i, j);
			culori[2] = ftj_R.at<uchar>(i, j);
			newMat.at<Vec3b>(i, j) = culori;
		}
	}

	return newMat;

}

int* generareHistogramaCanalHue(Mat src) {
	int height = src.rows;
	int width = src.cols;

	int hist[256] = { 0 };
	

	vector<double> hues;
	Mat hsi = Mat(height, width, CV_8UC3);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Vec3b rgb = src.at<Vec3b>(i, j);
			uchar b = rgb[0]; // v3[0]
			uchar g = rgb[1]; // v3[1]
			uchar r = rgb[2]; // v3[2]

			float R = (float)r / 255.0;
			float G = (float)g / 255.0;
			float B = (float)b / 255.0;

			float I = ((float)R + (float)G + (float)B) / 3.0;

			float S = 0.0, H = 0.0;

			if (I != 0) {
				float m = min((float)R, min((float)G, (float)B));
				S = 1 - (m / I * 1.0);
			}
			else {
				S = 0;
			}

			if (S != 0) {
				float termen1 = (R - G) + (R - B);
				float termen21 = (R - G) * (R - G) + (R - B) * (G - B);
				float termen22 = 2.0 * sqrt(termen21);
				float termen2 = termen1 / termen22 * 1.0;
				H = acos(termen2);

				if (B > G) {
					H = (360 * (PI / 180.0)) - H;
				}
				else {
					H = H;
				}
				hues.push_back(H);
			}
			else {
				H = 0.0;
				if (B > G) {
					H = (360 * (PI / 180.0)) - H;
				}
				else {
					H = H;
				}
				hues.push_back(H);
			}
			hsi.at<Vec3b>(i, j)[0] = (uchar)((H / PI / 2) * 255);
			hsi.at<Vec3b>(i, j)[1] = (uchar)S;
			hsi.at<Vec3b>(i, j)[2] = (uchar)I;
		}
	}
//	hist[256] = { 0 };
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int hue = hsi.at<Vec3b>(i, j)[0];
			hist[hue]++;
		}
	}
	showHistogram("hist222", hist, 256, 200);
	return hist;
}

vector<Mat> divideIntoGrids(Mat src, int M) {
	vector<Mat> gridsMat;

	int height = src.rows;
	int width = src.cols;

	int tileHeight = height / M;
	int tileWidth = width / M;

	for (int i = 0; i < M; i++) {
		for (int j = 0; j < M; j++) {
			int startX = j * tileWidth;
			int endX = (j + 1) * tileWidth;
			int startY = i * tileHeight;
			int endY = (i + 1) * tileHeight;

			endX = min(endX, width);
			endY = min(endY, height);

			Mat tile = src(Range(startY, endY), Range(startX, endX));
			//imshow("", tile);
			gridsMat.push_back(tile);
		}
	}

	return gridsMat;
}

double computeAverageHueInTile(Mat tile) { // calculez H pt fiecare pixel in tile

	vector<double> huesInTile;
	int tileHeight = tile.rows;
	int tileWidth = tile.cols;

	int ok = 0; // sa vad daca toate S-urile mi-s 0

	for (int i = 0; i < tileHeight; i++) {
		for (int j = 0; j < tileWidth; j++) {
			Vec3b rgb = tile.at<Vec3b>(i, j);
			uchar b = rgb[0]; // v3[0]
			uchar g = rgb[1]; // v3[1]
			uchar r = rgb[2]; // v3[2]

			float R = (float)r / 255.0;
			float G = (float)g / 255.0;
			float B = (float)b / 255.0;

			float I = ((float)R + (float)G + (float)B) / 3.0;

			float S = 0.0, H = 0.0;

			if (I != 0) {
				float m = min((float)R, min((float)G, (float)B));
				S = 1 - (m / I * 1.0);
			}
			else {
				S = 0;
			}

			if (S != 0) {
				ok = 1; // am cel putin un pixel cu S != 0

				float termen1 = (R - G) + (R - B);
				float termen21 = (R - G) * (R - G) + (R - B) * (G - B);
				float termen22 = 2.0 * sqrt(termen21);
				float termen2 = termen1 / termen22 * 1.0;
				H = acos(termen2);

				if (B > G) {
					H = (360 * (PI / 180.0)) - H;
				}
				else {
					H = H;
				}
				huesInTile.push_back(H);
			}
			else {
				H = 0.0;
				if (B > G) {
					H = (360 * (PI / 180.0)) - H;
				}
				else {
					H = H;
				}
			}
		}
	}
	if (ok == 0) { // toti pixelii au S = 0
		return -1; // pe asta o sa il pun in vectorul de reprezentare
	}
	double sum = 0.0;
	int size = huesInTile.size();
	for (int i = 0; i < size; i++) {
		sum += huesInTile.at(i);
	}
	double averageHue = sum / (double)size;
	return averageHue;
}

vector<double> vectorDeReprezentare(vector<Mat> grid, int M) {
	vector<double> V(M * M);
	V.clear(); // ca altfel imi pune by default primu element initializat cu ceva si daca de ex am M = 1, imi face size 2
	for (Mat tile : grid) {
		double avgHue = computeAverageHueInTile(tile);
		V.push_back(avgHue);
	}
	return V;
}

double computeSimilarity(vector<double> V1, vector<double> V2) {
	int N = 0;
	int vectorSize = V1.size();
	for (int i = 0; i < vectorSize; i++) {
		if (V1.at(i) >= 0 && V2.at(i) >= 0) {
			N++;
		}
	}
	if (N == 0) {
		return 0.0;
	}
	double C = sqrt(N) * PI;

	double sum = 0.0;
	for (int i = 0; i < vectorSize; i++) {
		if (V1.at(i) >= 0 && V2.at(i) >= 0) {
			double factor1 = abs(V1.at(i) - V2.at(i));
			double factor2 = 2 * PI - abs(V1.at(i) - V2.at(i));
			double minim = min(factor1, factor2);
			sum += minim * minim;
		}
	}
	double totalSum = sqrt(sum);
	double D = (1 / C * 1.0) * totalSum;
	double S = 1 - D;
	return S;
}

vector<Mat> getImagesFromFolder()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName) == 0)
		return vector<Mat>();

	char fname[MAX_PATH];
	FileGetter fg(folderName, "bmp"); // cauta fisiere cu .bmp
	vector<Mat> images; // imaginile in sine
	vector<string> imageNames; // numele lor gen Alps.bmp

	while (fg.getNextAbsFile(fname)) {
		Mat src = imread(fname);
		if (!src.empty()) {
			images.push_back(src);
			imageNames.push_back(fg.getFoundFileName());
		}
	}

	/*for (int i = 0; i < images.size(); i++) {
		imshow(imageNames[i], images[i]);
	}*/

	waitKey(0);
	return images;
}

bool egal(double a, double b)
{
	return fabs(a - b) < EPSILON;
}

double computePearsonsCorrelation(int* hist1, int* hist2) {
	double medie1 = 0.0, medie2 = 0.0;

	for (int i = 0; i < 256; i++) {
		medie1 += hist1[i]; // x cu bara
		medie2 += hist2[i]; // y cu bara
	}
	medie1 /= 256;
	medie2 /= 256;

	double scadere1 = 0.0, scadere2 = 0.0;
	double numarator = 0.0, numitor = 0.0;
	for (int i = 0; i < 256; i++) {
		numarator += (hist1[i] - medie1) * (hist2[i] - medie2);
		scadere1 += (hist1[i] - medie1) * (hist1[i] - medie1);
		scadere2 += (hist2[i] - medie2) * (hist2[i] - medie2);

	}

	scadere1 = sqrt(scadere1);
	scadere2 = sqrt(scadere2);
	numitor = scadere1 * scadere2;

	double correlation = 0.0;
	correlation = numarator / numitor;
	return correlation;
}

void start() {
	cout << "Alege folder\n";
	vector<Mat> images = getImagesFromFolder();
	if (images.empty()) {
		cout << "Nu s-a putut deschide\n";
		return;
	}

	int M;
	do {
		cout << "M = ";
		cin >> M;
		if (M <= 0) {
			cout << "M trebuie sa fie o valoare strict pozitiva!\n";
		}
	} while (M <= 0);

	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_COLOR);
		imshow("Selectie", src);
		int height = src.rows;
		int width = src.cols;

		vector<Mat> srcGrids = divideIntoGrids(src, M);
		vector<double> srcVectorDeReprezentare = vectorDeReprezentare(srcGrids, M);

		Mat similaritate1;
		Mat similaritate2;
		Mat similaritate3;

		double max1 = -1, max2 = -1;
		double sim1 = -1, sim2 = -1, sim3 = -1;
		for (int i = 0; i < images.size(); i++) {
			Mat aux;
			images[i].copyTo(aux);
			vector<Mat> auxGrids = divideIntoGrids(aux, M);
			vector<double> auxVector = vectorDeReprezentare(auxGrids, M);
			double auxSimilitudine = computeSimilarity(srcVectorDeReprezentare, auxVector);
			if (auxSimilitudine == 1) { // poza in sine
				aux.copyTo(similaritate1);
				sim1 = auxSimilitudine;
			}
			else {
				if (auxSimilitudine > max1) {
					max2 = max1;
					max1 = auxSimilitudine;
				}
				else {
					if (auxSimilitudine < max1 && auxSimilitudine > max2) {
						max2 = auxSimilitudine;
					}
				}
			}
		}

		sim2 = max1;
		sim3 = max2;

		for (int i = 0; i < images.size(); i++) {
			Mat aux;
			images[i].copyTo(aux);
			vector<Mat> auxGrids = divideIntoGrids(aux, M);
			vector<double> auxVector = vectorDeReprezentare(auxGrids, M);
			double auxSimilitudine = computeSimilarity(srcVectorDeReprezentare, auxVector);
			if (egal(auxSimilitudine, sim2)) {
				aux.copyTo(similaritate2);
			}
			else {
				if (egal(auxSimilitudine, sim3)) {
					aux.copyTo(similaritate3);
				}
			}

		}

		cout << "\n\nSimilitudine: " << sim1 << ", " << sim2 << ", " << sim3 << "\n";

		imshow("Similaritate 1", similaritate1);
		imshow("Similaritate 2", similaritate2);
		imshow("Similaritate 3", similaritate3);


		// ftj 
		Mat ftj_og = ftjPeCanaleRGB(src);
		imshow("ftj", ftj_og);
		Mat ftj_1 = ftjPeCanaleRGB(similaritate1);
		Mat ftj_2 = ftjPeCanaleRGB(similaritate2);
		Mat ftj_3 = ftjPeCanaleRGB(similaritate3);

		int* hist = generareHistogramaCanalHue(src);
		int* hist1 = generareHistogramaCanalHue(ftj_1);
		int* hist2 = generareHistogramaCanalHue(ftj_2);
		int* hist3 = generareHistogramaCanalHue(ftj_3);

		showHistogram("Histograma", hist, 256, 200);
		showHistogram("Histograma1", hist1, 256, 200);
		showHistogram("Histograma2", hist2, 256, 200);
		showHistogram("Histograma3", hist3, 256, 200);

		double p1 = computePearsonsCorrelation(hist, hist1);
		double p2 = computePearsonsCorrelation(hist, hist2);
		double p3 = computePearsonsCorrelation(hist, hist3);

		cout << "Coeficient Pearson: " << p1 << ", " << p2 << ", " << p3 << "\n";

		waitKey();
	}
}

int main()
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_FATAL);
	projectPath = _wgetcwd(0, 0);

	start();
	return 0;

}