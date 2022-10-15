/*
 * This code is provided as part of "A Practical Introduction to Computer Vision with OpenCV"
 * by Kenneth Dawson-Howe © Wiley & Sons Inc. 2014.  All rights reserved.
 */
#include "Utilities.h"
#include "opencv2/video.hpp"
#include <algorithm> // for std::find
//#include <iterator> // for std::begin, std::end
RNG rng(12345);

//namespace std;
#define FOCAL_LENGTH_ESTIMATE 1770
#define PLATE_WIDTH_IN_MM 465
#define PLATE_HEIGHT_IN_MM 100
#define FRAMES_PER_SECOND 29.97
#define REQUIRED_DICE 0.8

int LICENCE_PLATE_LOCATIONS[][5] = { {1, 67, 88, 26, 6}, {2, 67, 88, 26, 6}, {3, 68, 88, 26, 6},
	{4, 69, 88, 26, 6}, {5, 70, 89, 26, 6}, {6, 70, 89, 27, 6}, {7, 71, 89, 27, 6}, {8, 73, 89, 27, 6},
	{9, 73, 90, 27, 6}, {10, 74, 90, 27, 6}, {11, 75, 90, 27, 6}, {12, 76, 90, 27, 6}, {13, 77, 91, 27, 6},
	{14, 78, 91, 27, 6}, {15, 78, 91, 27, 6}, {16, 79, 91, 27, 6}, {17, 80, 92, 27, 6}, {18, 81, 92, 27, 6},
	{19, 81, 92, 28, 6}, {20, 82, 93, 28, 6}, {21, 83, 93, 28, 6}, {22, 83, 93, 28, 6}, {23, 84, 93, 28, 6},
	{24, 85, 94, 28, 6}, {25, 85, 94, 28, 6}, {26, 86, 94, 28, 6}, {27, 86, 94, 28, 6}, {28, 86, 95, 29, 6},
	{29, 87, 95, 29, 6}, {30, 87, 95, 29, 6}, {31, 88, 95, 29, 6}, {32, 88, 96, 29, 6}, {33, 89, 96, 29, 6},
	{34, 89, 96, 29, 6}, {35, 89, 97, 29, 6}, {36, 90, 97, 29, 6}, {37, 90, 97, 30, 6}, {38, 91, 98, 30, 6},
	{39, 91, 98, 30, 6}, {40, 92, 98, 30, 7}, {41, 92, 99, 30, 7}, {42, 93, 99, 30, 7}, {43, 93, 99, 30, 7},
	{44, 94, 100, 30, 7}, {45, 95, 100, 30, 7}, {46, 95, 101, 30, 7}, {47, 96, 101, 30, 7}, {48, 97, 102, 30, 7},
	{49, 97, 102, 31, 7}, {50, 98, 102, 31, 7}, {51, 99, 103, 31, 7}, {52, 99, 103, 32, 7}, {53, 100, 104, 32, 7},
	{54, 101, 104, 32, 7}, {55, 102, 105, 32, 7}, {56, 103, 105, 32, 7}, {57, 104, 106, 32, 7}, {58, 105, 106, 32, 7},
	{59, 106, 107, 32, 7}, {60, 107, 107, 32, 7}, {61, 108, 108, 32, 7}, {62, 109, 108, 33, 7}, {63, 110, 109, 33, 7},
	{64, 111, 109, 33, 7}, {65, 112, 110, 34, 7}, {66, 113, 111, 34, 7}, {67, 114, 111, 34, 7}, {68, 116, 112, 34, 7},
	{69, 117, 112, 34, 8}, {70, 118, 113, 35, 8}, {71, 119, 113, 35, 8}, {72, 121, 114, 35, 8}, {73, 122, 114, 35, 8},
	{74, 124, 115, 35, 8}, {75, 125, 116, 36, 8}, {76, 127, 116, 36, 8}, {77, 128, 117, 36, 8}, {78, 130, 118, 36, 8},
	{79, 132, 118, 36, 9}, {80, 133, 119, 37, 9}, {81, 135, 120, 37, 9}, {82, 137, 121, 37, 9}, {83, 138, 122, 38, 9},
	{84, 140, 122, 38, 9}, {85, 142, 123, 38, 9}, {86, 144, 124, 38, 9}, {87, 146, 125, 38, 9}, {88, 148, 126, 39, 9},
	{89, 150, 127, 39, 9}, {90, 152, 128, 39, 9}, {91, 154, 129, 40, 9}, {92, 156, 129, 40, 10}, {93, 158, 130, 40, 10},
	{94, 160, 131, 41, 10}, {95, 163, 133, 41, 10}, {96, 165, 133, 41, 10}, {97, 167, 135, 42, 10}, {98, 170, 135, 42, 10},
	{99, 172, 137, 43, 10}, {100, 175, 138, 43, 10}, {101, 178, 139, 43, 10}, {102, 180, 140, 44, 10}, {103, 183, 141, 44, 10},
	{104, 186, 142, 44, 11}, {105, 188, 143, 45, 11}, {106, 192, 145, 45, 11}, {107, 195, 146, 45, 11}, {108, 198, 147, 45, 11},
	{109, 201, 149, 46, 11}, {110, 204, 150, 47, 11}, {111, 207, 151, 47, 11}, {112, 211, 152, 47, 11}, {113, 214, 154, 48, 11},
	{114, 218, 155, 48, 12}, {115, 221, 157, 49, 12}, {116, 225, 158, 50, 12}, {117, 229, 160, 50, 12}, {118, 234, 162, 50, 12},
	{119, 237, 163, 51, 12}, {120, 241, 164, 52, 12}, {121, 245, 166, 52, 12}, {122, 250, 168, 52, 12}, {123, 254, 169, 53, 12},
	{124, 258, 171, 54, 12}, {125, 263, 173, 55, 12}, {126, 268, 175, 55, 12}, {127, 273, 177, 55, 12}, {128, 278, 179, 56, 13},
	{129, 283, 181, 57, 13}, {130, 288, 183, 57, 13}, {131, 294, 185, 58, 13}, {132, 299, 187, 59, 13}, {133, 305, 190, 59, 13},
	{134, 311, 192, 60, 13}, {135, 317, 194, 60, 14}, {136, 324, 196, 60, 14}, {137, 330, 198, 61, 14}, {138, 336, 201, 63, 14},
	{139, 342, 203, 64, 14}, {140, 349, 206, 65, 14}, {141, 357, 208, 65, 15}, {142, 364, 211, 66, 15}, {143, 372, 214, 67, 15},
	{144, 379, 217, 68, 15}, {145, 387, 220, 69, 15}, {146, 396, 223, 70, 15}, {147, 404, 226, 71, 16}, {148, 412, 229, 72, 16},
	{149, 422, 232, 73, 17}, {150, 432, 236, 74, 17}, {151, 440, 239, 75, 18}, {152, 450, 243, 76, 18}, {153, 460, 247, 77, 18},
	{154, 470, 250, 78, 19}, {155, 482, 254, 78, 19}, {156, 492, 259, 81, 19}, {157, 504, 263, 82, 20}, {158, 516, 268, 83, 20},
	{159, 528, 272, 85, 21}, {160, 542, 277, 85, 21}, {161, 554, 282, 88, 21}, {162, 569, 287, 88, 22}, {163, 584, 292, 89, 22},
	{164, 598, 297, 91, 23}, {165, 614, 302, 92, 24}, {166, 630, 308, 94, 24}, {167, 646, 314, 96, 25}, {168, 664, 320, 97, 26},
	{169, 681, 327, 100, 26}, {170, 700, 334, 101, 27}, {171, 719, 341, 103, 28}, {172, 740, 349, 105, 29}, {173, 762, 357, 107, 29},
	{174, 784, 365, 109, 30}, { 175, 808, 374, 110, 31 }, { 176, 832, 383, 113, 32 } };
const int NUMBER_OF_PLATES = sizeof(LICENCE_PLATE_LOCATIONS) / (sizeof(LICENCE_PLATE_LOCATIONS[0]));
const int FRAMES_FOR_DISTANCES[] = { 54,   70,   86,  101,  115,  129,  143,  158,  172 };
const int DISTANCES_TRAVELLED_IN_MM[] = { 2380, 2380, 2400, 2380, 2395, 2380, 2385, 2380 };
const double SPEEDS_IN_KMPH[] = { 16.0, 16.0, 17.3, 18.3, 18.5, 18.3, 17.2, 18.3 };


void MeanShiftDemo(VideoCapture& video, Rect& starting_position, int starting_frame_number, int end_frame)
{
	if (video.isOpened())
	{
		bool half_size = true;
		video.set(cv::CAP_PROP_POS_FRAMES, starting_frame_number);
		Mat current_frame, hls_image;
		std::vector<cv::Mat> hls_planes(3);
		video >> current_frame;
		Rect current_position(starting_position);
		if (half_size)
		{
			resize(current_frame, current_frame, Size(current_frame.cols / 2, current_frame.rows / 2));
			current_position.height /= 2;
			current_position.width /= 2;
			current_position.x /= 2;
			current_position.y /= 2;
		}
		cvtColor(current_frame, hls_image, COLOR_BGR2HLS);
		split(hls_image, hls_planes);
		int chosen_channel = 0;  // Hue channel
		Mat image1ROI = hls_planes[chosen_channel](current_position);

		float channel_range[2] = { 0.0, 255.0 };
		int channel_numbers[1] = { 0 };
		int number_bins[1] = { 32 };
		MatND histogram[1];
		const float* channel_ranges = channel_range;
		calcHist(&(image1ROI), 1, channel_numbers, Mat(), histogram[0], 1, number_bins, &channel_ranges);
		normalize(histogram[0], histogram[0], 1.0);
		rectangle(current_frame, current_position, Scalar(0, 255, 0), 2);
		Mat starting_frame = current_frame.clone();
		int frame_number = starting_frame_number;
		while (!current_frame.empty() && (frame_number < end_frame))
		{
			// Calculate back projection
			Mat back_projection_probabilities;
			calcBackProject(&(hls_planes[chosen_channel]), 1, channel_numbers, *histogram, back_projection_probabilities, &channel_ranges, 255.0);
			// Remove low saturation points from consideration
			Mat saturation_mask;
			inRange(hls_image, Scalar(0, 10, 50, 0), Scalar(180, 256, 256, 0), saturation_mask);
			bitwise_and(back_projection_probabilities, back_projection_probabilities, back_projection_probabilities, saturation_mask);
			// Mean shift
			TermCriteria criteria(cv::TermCriteria::MAX_ITER, 5, 0.01);
			meanShift(back_projection_probabilities, current_position, criteria);
			// Output to screen
			rectangle(current_frame, current_position, Scalar(0, 255, 0), 2);
			Mat chosen_channel_image, back_projection_image;
			cvtColor(hls_planes[chosen_channel], chosen_channel_image, COLOR_GRAY2BGR);
			cvtColor(back_projection_probabilities, back_projection_image, COLOR_GRAY2BGR);
			Mat row1_output = JoinImagesHorizontally(starting_frame, "Starting position", chosen_channel_image, "Chosen channel (Hue)", 4);
			Mat row2_output = JoinImagesHorizontally(back_projection_image, "Back projection", current_frame, "Current position", 4);
			Mat mean_shift_output = JoinImagesVertically(row1_output, "", row2_output, "", 4);
			imshow("Mean Shift Tracking", mean_shift_output);
			// Advance to next frame
			video >> current_frame;
			if (half_size)
				resize(current_frame, current_frame, Size(current_frame.cols / 2, current_frame.rows / 2));
			cvtColor(current_frame, hls_image, COLOR_BGR2HLS);
			split(hls_image, hls_planes);
			frame_number++;
			cv::waitKey(1000);
		}
		char c = cv::waitKey();
		cv::destroyAllWindows();
	}
}

void drawOpticalFlow(Mat& optical_flow, Mat& display, int spacing, Scalar passed_line_colour = -1.0, Scalar passed_point_colour = -1.0)
{
	Scalar colour(rand() & 0xFF, rand() & 0xFF, rand() & 0xFF);
	for (int row = spacing / 2; row < display.rows; row += spacing)
		for (int column = spacing / 2; column < display.cols; column += spacing)
		{
			const Point2f& fxy = optical_flow.at<Point2f>(row, column);
			circle(display, Point(column, row), 1, (passed_point_colour.val[0] == -1.0) ? colour : passed_point_colour, -1);
			line(display, Point(column, row), Point(cvRound(column + fxy.x), cvRound(row + fxy.y)),
				(passed_line_colour.val[0] == -1.0) ? colour : passed_line_colour);
		}
}

#define MAX_FEATURES 400
void LucasKanadeOpticalFlow(Mat& previous_gray_frame, Mat& gray_frame, Mat& display_image)
{
	Size img_sz = previous_gray_frame.size();
	int win_size = 10;
	cvtColor(previous_gray_frame, display_image, COLOR_GRAY2BGR);
	vector<Point2f> previous_features, current_features;
	const int MAX_CORNERS = 500;
	goodFeaturesToTrack(previous_gray_frame, previous_features, MAX_CORNERS, 0.05, 5, noArray(), 3, false, 0.04);
	cornerSubPix(previous_gray_frame, previous_features, Size(win_size, win_size), Size(-1, -1),
		TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 20, 0.03));
	vector<uchar> features_found;
	calcOpticalFlowPyrLK(previous_gray_frame, gray_frame, previous_features, current_features, features_found, noArray(),
		Size(win_size * 4 + 1, win_size * 4 + 1), 5,
		TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 20, .3));
	for (int i = 0; i < (int)previous_features.size(); i++)
	{
		if (!features_found[i])
			continue;
		circle(display_image, previous_features[i], 1, Scalar(0, 0, 255));
		line(display_image, previous_features[i], current_features[i], Scalar(0, 255, 0));
	}
}

class MedianBackground
{
private:
	Mat mMedianBackground;
	float**** mHistogram;
	float*** mLessThanMedian;
	float mAgingRate;
	float mCurrentAge;
	float mTotalAges;
	int mValuesPerBin;
	int mNumberOfBins;
public:
	MedianBackground(Mat initial_image, float aging_rate, int values_per_bin);
	Mat GetBackgroundImage();
	void UpdateBackground(Mat current_frame);
	float getAgingRate()
	{
		return mAgingRate;
	}
};

MedianBackground::MedianBackground(Mat initial_image, float aging_rate, int values_per_bin)
{
	mCurrentAge = 1.0;
	mAgingRate = aging_rate;
	mTotalAges = 0.0;
	mValuesPerBin = values_per_bin;
	mNumberOfBins = 256 / mValuesPerBin;
	mMedianBackground = Mat::zeros(initial_image.size(), initial_image.type());
	mLessThanMedian = (float***) new float** [mMedianBackground.rows];
	mHistogram = (float****) new float*** [mMedianBackground.rows];
	for (int row = 0; (row < mMedianBackground.rows); row++)
	{
		mHistogram[row] = (float***) new float** [mMedianBackground.cols];
		mLessThanMedian[row] = (float**) new float* [mMedianBackground.cols];
		for (int col = 0; (col < mMedianBackground.cols); col++)
		{
			mHistogram[row][col] = (float**) new float* [mMedianBackground.channels()];
			mLessThanMedian[row][col] = new float[mMedianBackground.channels()];
			for (int ch = 0; (ch < mMedianBackground.channels()); ch++)
			{
				mHistogram[row][col][ch] = new float[mNumberOfBins];
				mLessThanMedian[row][col][ch] = 0.0;
				for (int bin = 0; (bin < mNumberOfBins); bin++)
				{
					mHistogram[row][col][ch][bin] = (float)0.0;
				}
			}
		}
	}
}

Mat MedianBackground::GetBackgroundImage()
{
	return mMedianBackground;
}

void MedianBackground::UpdateBackground(Mat current_frame)
{
	mTotalAges += mCurrentAge;
	float total_divided_by_2 = mTotalAges / ((float)2.0);
	for (int row = 0; (row < mMedianBackground.rows); row++)
	{
		for (int col = 0; (col < mMedianBackground.cols); col++)
		{
			for (int ch = 0; (ch < mMedianBackground.channels()); ch++)
			{
				int new_value = (mMedianBackground.channels() == 3) ? current_frame.at<Vec3b>(row, col)[ch] : current_frame.at<uchar>(row, col);
				int median = (mMedianBackground.channels() == 3) ? mMedianBackground.at<Vec3b>(row, col)[ch] : mMedianBackground.at<uchar>(row, col);
				int bin = new_value / mValuesPerBin;
				mHistogram[row][col][ch][bin] += mCurrentAge;
				if (new_value < median)
					mLessThanMedian[row][col][ch] += mCurrentAge;
				int median_bin = median / mValuesPerBin;
				while ((mLessThanMedian[row][col][ch] + mHistogram[row][col][ch][median_bin] < total_divided_by_2) && (median_bin < 255))
				{
					mLessThanMedian[row][col][ch] += mHistogram[row][col][ch][median_bin];
					median_bin++;
				}
				while ((mLessThanMedian[row][col][ch] > total_divided_by_2) && (median_bin > 0))
				{
					median_bin--;
					mLessThanMedian[row][col][ch] -= mHistogram[row][col][ch][median_bin];
				}
				if (mMedianBackground.channels() == 3)
					mMedianBackground.at<Vec3b>(row, col)[ch] = median_bin * mValuesPerBin;
				else mMedianBackground.at<uchar>(row, col) = median_bin * mValuesPerBin;
			}
		}
	}
	mCurrentAge *= mAgingRate;
}

