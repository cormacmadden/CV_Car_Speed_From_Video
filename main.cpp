#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <opencv2/core/core.hpp>
#include "Utilities.h"
#include <iostream>
#include <fstream>
#include <list>
#include <experimental/filesystem> // C++-standard header file name
#include <filesystem> // Microsoft-specific implementation header file name
using namespace std::experimental::filesystem::v1;
using namespace std;
using namespace cv;
#define FOCAL_LENGTH_ESTIMATE 1770
#define PLATE_WIDTH_IN_MM 465
#define PLATE_HEIGHT_IN_MM 100
#define FRAMES_PER_SECOND 29.97
#define REQUIRED_DICE 0.8

const int LICENCE_PLATE_LOCATIONS[][5] = { {1, 67, 88, 26, 6}, {2, 67, 88, 26, 6}, {3, 68, 88, 26, 6},
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

void frameDistances(int frames[], float distances[], float positions[7][2], VideoCapture& surveillance_video) {

	if (surveillance_video.isOpened())
	{
		int starting_frame = 0;
		Mat current_frame, thresholded_image, closed_image, first_frame;
		Mat current_frame_gray, running_average_background;
		Mat selective_running_average_background;
		double running_average_learning_rate = 0.01;
		surveillance_video.set(cv::CAP_PROP_POS_FRAMES, starting_frame);
		surveillance_video >> current_frame;
		first_frame = current_frame.clone();
		cvtColor(current_frame, current_frame_gray, COLOR_BGR2GRAY);
		current_frame.convertTo(running_average_background, CV_32F);
		selective_running_average_background = running_average_background.clone();
		int rad = running_average_background.depth();

		int codec = static_cast<int>(surveillance_video.get(cv::CAP_PROP_FOURCC));
		// V3.0.0 update on next line.  OLD CODE was    BackgroundSubtractorMOG2 gmm; //(50,16,true);
		Ptr<BackgroundSubtractorMOG2> gmm = createBackgroundSubtractorMOG2();
		Mat foreground_mask, foreground_image = Mat::zeros(current_frame.size(), CV_8UC3);

		double frame_rate = surveillance_video.get(cv::CAP_PROP_FPS);
		double time_between_frames = 10.0 / 29.97;
		Timestamper* timer = new Timestamper();
		int frame_count = 0;
		int x = 0;
		int trueP = 0;
		int falseP = 0;
		int falseN = 0;
		while ((!current_frame.empty()) && (frame_count++ < 1000))//1800))
		{
			double duration = static_cast<double>(getTickCount());

			// Update the Gaussian Mixture Model
			// V3.0.0 update on next line.  OLD CODE was  gmm(current_frame, foreground_mask);
			gmm->apply(current_frame, foreground_mask);

			// Clean the resultant binary (moving pixel) mask using an opening.
			threshold(foreground_mask, thresholded_image, 30, 255, THRESH_BINARY);
			Mat cleaned_foreground_mask;

			cleaned_foreground_mask = thresholded_image.clone();
			foreground_image.setTo(Scalar(0, 0, 0));
			current_frame.copyTo(foreground_image, cleaned_foreground_mask);

			timer->recordTime("Gaussian Mixture Model");
			// Create an average background image (just for information)
			Mat mean_background_image;
			timer->ignoreTimeSinceLastRecorded();
			// V3.0.0 update on next line.  OLD CODE was   gmm.getBackgroundImage(mean_background_image);
			gmm->getBackgroundImage(mean_background_image);

			duration = static_cast<double>(getTickCount()) - duration;
			duration /= getTickFrequency() / 1000.0;
			int delay = (time_between_frames > duration) ? ((int)(time_between_frames - duration)) : 1;
			char c = cv::waitKey(delay);
			char frame_str[100];
			sprintf(frame_str, "Frame = %d", frame_count);
			Mat grey_foreground, binary_im;
			cvtColor(foreground_image, grey_foreground, COLOR_BGR2GRAY);
			threshold(grey_foreground, binary_im, 30, 255, THRESH_BINARY);

			vector<vector<Point>> contours;
			vector<Vec4i> hierarchy;
			findContours(binary_im, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
			Mat contours_image = Mat::zeros(binary_im.size(), CV_8UC3);
			Mat boxes_image = Mat::zeros(binary_im.size(), CV_8UC3);
			vector<Point> points;
			Scalar green = Scalar(0, 225, 0);
			Rect trueRect(LICENCE_PLATE_LOCATIONS[frame_count - 2][1], LICENCE_PLATE_LOCATIONS[frame_count - 2][2], LICENCE_PLATE_LOCATIONS[frame_count - 2][3], LICENCE_PLATE_LOCATIONS[frame_count - 2][4]);
			//rectangle(foreground_image, trueRect.tl(), trueRect.br(), green, 1, 8, 0);
			Rect bounding_rect;
			for (int contour_number = 0; (contour_number < (int)contours.size()); contour_number++)
			{
				Scalar colour(rand() & 0xFF, rand() & 0xFF, rand() & 0xFF);
				RotatedRect min_bounding_rectangle = minAreaRect(contours[contour_number]);

				Point_<float> boxCenter = min_bounding_rectangle.center;
				Size_<float> boxSize = min_bounding_rectangle.size;
				float boxAngle = min_bounding_rectangle.angle;
				Point2f vertices[4];
				if (boxSize.width > 8.0f && boxSize.height < 40.0f) {
					if (((boxSize.width / boxSize.height) > 2) && (boxAngle < 25.0f))
					{
						if ((contourArea(contours[contour_number]) / (boxSize.width * boxSize.height)) > 0.7f)
						{
							if (hierarchy[contour_number][2] != -1) {
								std::cout << "found for frame: " << frame_count << '\n';
								drawContours(foreground_image, contours, contour_number, colour, cv::FILLED, 8, hierarchy);
								if (frame_count == frames[x]) {
									drawContours(contours_image, contours, contour_number, colour, cv::FILLED, 8, hierarchy);
									//std::cout.precision(2);
									float pixelRatio = 520 / boxSize.width;
									float pixelRatio1 = 100 / boxSize.height;
									float calc_distance_mm = 1770 * pixelRatio;
									distances[x] = calc_distance_mm;
									positions[x][0] = boxCenter.x;
									positions[x][1] = boxCenter.y;
									positions[x][2] = pixelRatio;
									x++;
								}
								//min_bounding_rectangle.points(vertices);
								//for (int i = 0; i < 4; i++)
								//	line(foreground_image, vertices[i], vertices[(i + 1) % 4], colour, 2);
								//vector<Point2f> intersecting_region;
								bounding_rect = boundingRect(contours[contour_number]);
							}
						}
					}
				}
			}
			Rect int_rect = trueRect & bounding_rect;
			float TRsize = trueRect.width * trueRect.height;
			float INsize = int_rect.width * int_rect.height;
			float BOsize = bounding_rect.width * bounding_rect.height;
			float dice = (INsize * 2) / (TRsize + BOsize);
			//cout << "dice: " << dice << '\n';
			if (frame_count < 176 && frame_count > 50)
				if (bounding_rect.empty()) {
					falseN++;
				}
				else if (dice < 0.76) {
					falseP++;
				}
				else
				{
					trueP++;
				}
			if (frame_count == frames[x]) {
				std::cout << x << '\n';
				x++;
			}
			Scalar red = Scalar(0, 0, 255);
			//rectangle(foreground_image, bounding_rect.tl(), bounding_rect.br(), red, 1, 8, 0);

			cout << "true positives : " << trueP << '\n';
			cout << "false positives : " << falseP << '\n';
			cout << "false negatives : " << falseN << '\n';
			timer->putTimes(current_frame);
			imshow("Boxes", foreground_image);
			surveillance_video >> current_frame;
		}
		cv::destroyAllWindows();
	}
}

int main(int argc, const char** argv)
{
	char* file_location = "Media/";
	int frames[] = {54, 70, 86, 101, 115, 129, 143, 158, 172};
	float distances[7] = {};
	float positions[7][2];

	char* video_files[] = {
		"CarSpeedTest1.mp4"};

	int number_of_videos = sizeof(video_files) / sizeof(video_files[0]);
	VideoCapture* video = new VideoCapture[number_of_videos];
	for (int video_file_no = 0; (video_file_no < number_of_videos); video_file_no++)
	{
		string filename(file_location);
		filename.append(video_files[video_file_no]);
		video[video_file_no].open(filename);
		if (!video[video_file_no].isOpened())
		{
			cout << "Cannot open video file: " << filename << endl;
			//			return -1;
		}
	}
	video[0].set(cv::CAP_PROP_POS_FRAMES, 150);

	char c = cv::waitKey();
	cv::destroyAllWindows();

	frameDistances(frames, distances,positions, video[0]);
	float speeds[7];
	for (int i = 1; i <= 7; i++) {
		//cout << "Pos x: " << positions[i][0] << '\n';
		//cout << "Pos y: " << positions[i][1] << '\n';
		float dx = positions[i-1][0] - positions[i][0];
		float dy = positions[i-1][1] - positions[i][1];
		float dist_px = sqrt((dx*dx) + (dy*dy));
		//cout << "dsit_px :" << dist_px << '\n';
		float dist_mm = dist_px * positions[i][2];
		//cout << "dist_mm :" << dist_mm << '\n';
		float dz = distances[i - 1] - distances[i];
		//cout << "dz :" << dz << '\n';
		cout << "Distance :" << distances[i-1] - distances [i]<< '\n';
		cout << "Change in Frames :" << frames[i - 1] - frames[i] << '\n';
		int df = (frames[i - 1] - frames[i]);
		//speeds[i] = ((sqrt((dz*dz)+(dist_mm*dist_mm))) / 10 / 100 / 1000) / (((frames[i - 1] - frames[i] / FRAMES_PER_SECOND) / 60 / 60);

		speeds[i] = ((dz) / 10 / 100 / 1000) / ((df/ FRAMES_PER_SECOND) / 60 / 60);
		cout << "Speed :" << speeds[i] << '\n';
	}
	c = cv::waitKey();
	cv::destroyAllWindows();
}

