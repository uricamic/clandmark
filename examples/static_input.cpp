/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2013 Michal Uricar
 * Copyright (C) 2013 Michal Uricar
 */

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <iomanip>
#include <stdio.h>

#include "Flandmark.h"
#include "CFeaturePool.h"
#include "CSparseLBPFeatures.h"
#include "helpers.h"

#define SHOW_WINDOWS

using namespace std;
using namespace cv;
using namespace clandmark;

/** Global variables */
//String face_cascade_name = "lbpcascade_frontalface.xml";
String face_cascade_name = "haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
string window_name = "flandmark - static input demo";
RNG rng(12345);

cimg_library::CImg<unsigned char> * cvImgToCImg(cv::Mat &cvImg)
{
	cimg_library::CImg<unsigned char> * result = new cimg_library::CImg<unsigned char>(cvImg.cols, cvImg.rows);

	for (int x = 0; x < cvImg.cols; ++x)
		for (int y = 0; y < cvImg.rows; ++y)
			(*result)(x, y) = cvImg.at<uchar>(y, x);

	return result;
}

cv::Mat & CImgtoCvImg(cv::Mat &result, cimg_library::CImg<unsigned char> *img)
{
	result = cv::Mat(img->height(), img->width(), CV_8U);

	for (int x=0; x < result.cols; ++x)
		for (int y=0; y < result.rows; ++y)
			result.at<uchar>(y, x) = (*img)(x, y);

	return result;
}

/** @function detectAndDisplay */
void detectAndDisplay( Mat &frame, Flandmark *flandmark, CFeaturePool *featurePool)
{
	std::vector<Rect> faces;
	Mat frame_gray;
	int bbox[8];
	fl_double_t *landmarks;

	cvtColor( frame, frame_gray, CV_BGR2GRAY );
//    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );  // <- OpenCV 3.0
	//equalizeHist( frame_gray, frame_gray );

	//-- Detect faces
	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
//    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );

	for( uint32_t i = 0; i < faces.size(); i++ )
	{
		// Get detected face bounding box
		bbox[0] = faces[i].x;
		bbox[1] = faces[i].y;
		bbox[2] = faces[i].x+faces[i].width;
		bbox[3] = faces[i].y;
		bbox[4] = faces[i].x+faces[i].width;
		bbox[5] = faces[i].y+faces[i].height;
		bbox[6] = faces[i].x;
		bbox[7] = faces[i].y+faces[i].height;

		// Detect facial landmarks
		cimg_library::CImg<unsigned char>* frm_gray = cvImgToCImg(frame_gray);
//		flandmark->detect_from_nf(frm_gray, bbox);
//		flandmark->detect( frm_gray, bbox );
		flandmark->detect_optimized(frm_gray, bbox);

		delete frm_gray;

		// Get detected landmarks
		landmarks = flandmark->getLandmarks();

		// Draw bounding box and detected landmarks
//		rectangle(frame, Point(bbox[0], bbox[1]), Point(bbox[2], bbox[3]), Scalar(255, 0, 0));
		circle(frame, Point(int(landmarks[0]), int(landmarks[1])), 2, Scalar(255, 0, 0), -1);
		for (int i=2; i < 2*flandmark->getLandmarksCount(); i+=2)
		{
			circle(frame, Point(int(landmarks[i]), int(landmarks[i+1])), 2, Scalar(0, 0, 255), -1);
		}

		// Textual output
		printTimingStats(flandmark->timings);
		printLandmarks(landmarks, flandmark->getLandmarksCount());
		printLandmarks(flandmark->getLandmarksNF(), flandmark->getLandmarksCount());
	}

	//-- Show what you got
#ifdef SHOW_WINDOWS
	imshow( window_name, frame );
#endif
}

/** @function main */
int main( int argc, const char** argv )
{
	if (argc < 4)
	{
		cerr << "Usage: static_input <path_to_haarcascade> <flandmark_model.xml> <input_image> [<output_image>]" << endl;
		return -1;
	}

	double tim;
	Mat image;

	//Flandmark *flandmark = new Flandmark();

	tim = (double)getTickCount();

	Flandmark *flandmark = Flandmark::getInstanceOf(argv[2]);
	if (!flandmark)
	{
		cerr << "Usage: static_input <flandmark_model.xml> <input_image> [<output_image>]" << endl;
		return -1;
	}

	CFeaturePool *featurePool = new CFeaturePool(flandmark->getBaseWindowSize()[0], flandmark->getBaseWindowSize()[1]);
	featurePool->addFeaturesToPool(
				new CSparseLBPFeatures(
					featurePool->getWidth(),
					featurePool->getHeight(),
					featurePool->getPyramidLevels(),
					featurePool->getCumulativeWidths()
					)
				);

	flandmark->setNFfeaturesPool(featurePool);

	tim = ((double)getTickCount() - tim)/getTickFrequency() * 1000;

	cout << "Flandmark model loaded in " << tim << " ms" << endl;

	//-- 1. Load the cascades
	if( !face_cascade.load( argv[1]+face_cascade_name ) )
	{
		printf("--(!)Error loading\n");
		return -1;
	};

#ifdef SHOW_WINDOWS
	namedWindow(window_name, CV_WINDOW_KEEPRATIO);
//  //namedWindow(window_name, WINDOW_KEEPRATIO);
#endif

	// Read input image
	image = imread(argv[3]);

	if (!image.empty())
	{
		detectAndDisplay( image, flandmark, featurePool );
	} else {
		cout << "Wrong input." << endl;
	}

	if (argc > 4)
	{
		imwrite(argv[4], image);
	}

#ifdef SHOW_WINDOWS
	waitKey();
#endif

	delete featurePool;
	delete flandmark;

	return 0;
}
