#include "Utilities.h"

using namespace std;
using namespace cv;

int spoonInspection(Mat& image) {
	int saturation = 0;
	Mat hsl_image;
	Mat satImage;	
	Mat test = image;
	cvtColor(image, hsl_image, COLOR_BGR2HLS);
	vector<Mat> hls_planes(3);
	split(hsl_image, hls_planes);
	cvtColor(hls_planes[2], satImage, COLOR_GRAY2BGR);
	for (int row = 0; row < satImage.rows; row++)
		for (int col = 0; col < satImage.cols; col++)
			saturation += satImage.at<Vec3b>(row, col)[2];

	return saturation;
}
