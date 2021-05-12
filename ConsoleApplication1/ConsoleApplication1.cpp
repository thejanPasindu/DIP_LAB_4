#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{

	Mat img = imread(argv[1], IMREAD_GRAYSCALE); // Load the color image as Gray Image
	if (!img.data) // check image is available
	{
		cout << "Could not load image file" << endl;
		return -1;
	}

	int height = img.rows;
	int width = img.cols;
	int widthstep = img.step;
	int ch = img.channels();
	printf("height: %d, widht: %d, width step: %d, chanels: %d\n", height, width, widthstep, ch);

	//allocate memory for gray level count
	int histogram[256];
	//allocate memory for gray level count
	int e_histogram[256];

	//allocate memory for probability
	double prob[256];
	//allocate memory for cumulative_probability
	double cprob[256];
	//allocate memory for scaled_cumulative_probability
	double s_cprob[256];

	for (int x = 0; x < 256; x++){
		prob[x] = 0;
		cprob[x] = 0;
		s_cprob[x] = 0;
		histogram[x] = 0;
		e_histogram[x] = 0;
	}

	// calculate # of pixels for each gray level
	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			histogram[(int)img.at<uchar>(i, j)]++;
		}
	}

	//total # of pixels
	int N = height * width;
	
	//calculate probability of each gray(intensity) level
	for (int i = 0; i < 256; i++){
		prob[i] = (double)histogram[i] / N;
	}

	// Generate cumulative probability
	cprob[0] = prob[0];
	for (int i = 1; i < 256; i++) {
		cprob[i] = cprob[i-1] + prob[i];
	}

	/*for (int i = 0; i < 256; i++) {
		printf("%f\n", cprob[i]);
	}*/

	// Scale cumulative probability
	for (int i = 0; i < 256; i++) {
		s_cprob[i] = cvRound(cprob[i] * 255);
	}

	Mat enhance_image = img.clone(); //get the copy of the original image

	// Generate the equalized image
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			 enhance_image.at<uchar>(i,j) = (uchar)s_cprob[(int)img.at<uchar>(i, j)];

			 // for histogram
			 //count equalized image intencity
			 e_histogram[(int)enhance_image.at<uchar>(i, j)]++;
		}
	}

	// draw histogram
	
	//create image to display the histograme
	int hist_w = 512;
	int hist_h = 400;
	int bin_w = cvRound((double)hist_w / 255);

	Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(255));
	Mat equHistImage(hist_h, hist_w, CV_8UC1, Scalar(255));

	//Find the maximum number of pixels from histogram array
	int max_hist = histogram[0];
	int max_equ_hist = e_histogram[0];
	for (int i = 1; i < 256; i++) {
		if (max_hist < histogram[i]) {
			max_hist = histogram[i];
		}
		if (max_equ_hist < e_histogram[i]) {
			max_equ_hist = e_histogram[i];
		}
	}

	//Normalize the histogram between 0 and histogram height
	for (int i = 0; i < 256; i++) {
		histogram[i] = ((double)histogram[i] / max_hist) * hist_h;
		e_histogram[i] = ((double)e_histogram[i] / max_equ_hist) * hist_h;
	}

	//draw histrograme lines
	for (int i = 0; i < 256; i++) {
		line(histImage, Point(bin_w * (i), hist_h), Point(bin_w * (i), (hist_h - histogram[i])), Scalar(0), 1, 8, 0); //line ( histImage, Point pt1, Point pt2,color , thickness, lineType, shift)
		line(equHistImage, Point(bin_w * (i), hist_h), Point(bin_w * (i), (hist_h - e_histogram[i])), Scalar(0), 1, 8, 0); //line ( histImage, Point pt1, Point pt2,color , thickness, lineType, shift)
	}

	//original image
	namedWindow("Gray"); // Create the window
	imshow("Gray", img); // load and show image

	//enhanced image
	namedWindow("Enhanced"); // Create the window
	imshow("Enhanced", enhance_image); // load and show image

	//histogram image
	namedWindow("Histogram"); // Create the window
	imshow("Histogram", histImage); // load and show image

	//equalized histogram image
	namedWindow("Equalized_Histogram"); // Create the window
	imshow("Equalized_Histogram", equHistImage); // load and show image

	waitKey(0); // wait to close

	return 0;

}