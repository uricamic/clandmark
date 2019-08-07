# -*- coding: utf-8 -*-
"""
Created on Sun Feb 05 16:26:55 2017

@author: uricar.michal
"""

import os
import sys
import time
import numpy as np
import cv2

sys.path.append('D:/GitHub/clandmark/build_win10/install/share/clandmark/python/')

from py_flandmark import PyFlandmark
from py_featurePool import PyFeaturePool


# webcam ID
CAM_ID = 3

# Models
MODEL_PATH = 'D:/GitHub/clandmark/build_win10/install/share/clandmark/models/coarse2fine/'

MODEL = os.path.join(MODEL_PATH, 'CDPM.xml')

CV_CASCADE_PATH = "D:/GitHub/clandmark/data/haarcascade_frontalface_alt.xml"


def rgb2gray(rgb):
	"""
	converts rgb array to grey scale variant
	accordingly to fomula taken from wiki
	(this function is missing in python)
	"""
	return np.dot(rgb[...,:3], [0.299, 0.587, 0.144])


def flandmark_init(model=MODEL):
    flandmark = PyFlandmark(model, False)

    # Initialize featurePool
    bw = flandmark.getBaseWindowSize()
    featurePool = PyFeaturePool(bw[0], bw[1], None)
    featurePool.addFeatuaddSparseLBPfeatures()

    flandmark.setFeaturePool(featurePool)

    return flandmark


if __name__ == '__main__':

    # flandmark = flandmark_init()
    flandmark = PyFlandmark(MODEL, False)
    # Initialize featurePool
    bw = flandmark.getBaseWindowSize()
    featurePool = PyFeaturePool(bw[0], bw[1], None)
    featurePool.addFeatuaddSparseLBPfeatures()
    flandmark.setFeaturePool(featurePool)

    faceCascade = cv2.CascadeClassifier(CV_CASCADE_PATH)

    video_capture = cv2.VideoCapture(CAM_ID)

    while True:
        # Capture frame-by-frame
        ret, frame = video_capture.read()

        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        arr = rgb2gray(frame)

        faces = faceCascade.detectMultiScale(
            gray,
            scaleFactor=1.1,
            minNeighbors=5,
            minSize=(30, 30),
            flags=cv2.CASCADE_SCALE_IMAGE
        )

        # Draw a rectangle around the faces
        for (x, y, w, h) in faces:

            bbox = np.array([x, y, x+w, y+h], dtype=np.int32)
            bbox = bbox.reshape((2,2), order='F')
            start_time = time.time()

            P = flandmark.detect_optimized(arr, bbox)

            print('Elapsed time: {} ms'.format((time.time() - start_time) * 1000))

            for i in range(len(P[0,:])-1):
                cv2.circle(frame, (int(round(P[0,i])), int(round(P[1,i]))), 1, (0, 0, 255), 2)
            cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)

        # Display the resulting frame
        cv2.imshow('CLandmark - webcam input', frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # When everything is done, release the capture
    video_capture.release()
    cv2.destroyAllWindows()
