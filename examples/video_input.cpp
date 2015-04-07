/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2013 Michal Uricar
 * Copyright (C) 2013 Michal Uricar
 */

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>

#include "Flandmark.h"
#include "helpers.h"

using namespace std;
using namespace cv;
using namespace clandmark;

/** Global variables */
String face_cascade_name = "haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
string window_name = "flandmark - video input";
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
void detectAndDisplay( Mat &frame, Flandmark *flandmark)
{
	std::vector<Rect> faces;
	Mat frame_gray;
//	int bbox[4];
	int bbox[8];
	fl_double_t *landmarks;

	cvtColor( frame, frame_gray, CV_BGR2GRAY );
//    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	//-- Detect faces
	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
//	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );

//    CImage *frm_gray = new CImage();
	cimg_library::CImg<unsigned char>* frm_gray = 0x0; //= new cimg_library::CImg<unsigned char>();

	for( uint32_t i = 0; i < faces.size(); i++ )
	{
		// Get detected face bounding box
		bbox[0] = faces[i].x;
		bbox[1] = faces[i].y;
		bbox[2] = faces[i].x + faces[i].width;
		bbox[3] = faces[i].y;
		bbox[4] = faces[i].x + faces[i].width;
		bbox[5] = faces[i].y + faces[i].height;
		bbox[6] = faces[i].x;
		bbox[7] = faces[i].y + faces[i].height;

		// Detect facial landmarks
		frm_gray = cvImgToCImg(frame_gray);
		flandmark->detect(frm_gray, bbox);

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

//    delete frm_gray;
}

/** @function main */
int main( int argc, const char** argv )
{
	if (argc < 5)
	{
		cerr << "Usage: video_input <path_to_haarcasade> <flandmark_model.xml> { cam | vid } [ filename | cam_id ] [ output_filename ]" << endl;
		return -1;
	}

	//CvCapture* capture = 0x0;
	//CvVideoWriter* writer = 0x0;
	VideoCapture capture;
	VideoWriter writer;

	Mat frame;

	double tic;
	int camID = -1;
	string str_type = argv[3];
	int type = -1;
	bool saveoutput = false;
	string out_fname;

	Flandmark *flandmark = Flandmark::getInstanceOf(argv[2]);

	if (!flandmark)
	{
		cerr << "Usage: video_input <path_to_haarcascade> <flandmark_model.xml> { cam | vid } [ filename | cam_id ] [ output_filename ]" << endl;
		return -1;
	}

	//-- 1. Load the cascades
	if( !face_cascade.load( argv[1]+face_cascade_name ) )
	{
		cerr << "Couldn't load the haar cascade. Exiting..." << endl;
		return -1;
	};

	if (str_type.compare("cam") == 0)
	{
		type = 0;
	} else if (str_type.compare("vid") == 0) {
		type = 1;
	} else {
		type = 2;
	}

	if (argc > 4)
	{
		switch(type)
		{
			case 0:
				camID = ::atoi(argv[4]);
				capture.open(camID);
			break;
			case 1:
				capture.open(argv[4]);
			break;
			default:
				cerr << "Usage: video_input <path_to_haarcascade> <flandmark_model.xml> { cam | vid } [ filename | cam_id ] [ output_filename ]" << endl;
				return -1;
			break;
		}

		if (!capture.isOpened())
		{
			cerr << "Could not open the video input. Exiting..." << endl;
			return -1;
		}

		capture >> frame;

		if (frame.empty())
		{
			cerr << "Unable to read frame. Exiting..." << endl;
			return -1;
		}

		if (argc == 6)
		{
			saveoutput = true;
			out_fname = argv[5];

			writer.open(out_fname,
						//CV_FOURCC('H', '2', '6', '4'),
						CV_FOURCC('X', 'V', 'I', 'D'),
//                        VideoWriter::fourcc('X', 'V', 'I', 'D'),
						//CV_FOURCC('I', 'Y', 'U', 'V'),
						//CV_FOURCC('M', 'J', 'P', 'G'),
						capture.get(CV_CAP_PROP_FPS),
//                        capture.get(CAP_PROP_FPS),
						Size(capture.get(CV_CAP_PROP_FRAME_WIDTH), capture.get(CV_CAP_PROP_FRAME_HEIGHT))
//                        Size(capture.get(CAP_PROP_FRAME_WIDTH), capture.get(CAP_PROP_FRAME_HEIGHT))
					);
		}
	}

	// window
	namedWindow(window_name, CV_WINDOW_KEEPRATIO);
//    namedWindow(window_name, WINDOW_KEEPRATIO);

	//-- 2. Read the video stream
	if( capture.isOpened() )
	{
		while( true )
		{
			tic = (double)getTickCount();

			capture >> frame;

			//-- 3. Apply the classifier to the frame
			if( !frame.empty() )
			{
				detectAndDisplay( frame, flandmark );

				tic = ((double)getTickCount() - tic)/getTickFrequency() * 1000;

				stringstream fps;
				fps << "fps: " << setprecision(4) << setw(4) << 1000.0 / tic << " ";
				putText(frame, fps.str(), Point(10, 25), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 255, 0));
//				putText(frame, fps.str(), Point(10, 25), FONT_HERSHEY_COMPLEX, 1, Scalar(255, 255, 0));

				imshow( window_name, frame );

			} else {
				cerr << "No frame --- break." << endl;
				break;
			}

			if (saveoutput)
			{
				writer << frame;
			}

			int c = waitKey(10);
			if( (char)c == 'c' )
			{
				break;
			}
		}
	}

	delete flandmark;

	return 0;
}
