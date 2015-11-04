import numpy as np
import os
from fnmatch import fnmatch
from py_flandmark import PyFlandmark
from PIL import Image
import ImageDraw
import matplotlib.pyplot as plt


def rgb2gray(rgb):
    """
    converts rgb array to grey scale variant
    accordingly to fomula taken from wiki
    (this function is missing in python)
    """
    return np.dot(rgb[...,:3], [0.299, 0.587, 0.144])

def read_bbox_from_txt(file_name):
    """
        returns 2x2 matrix coordinates of
        left upper and right lower corners
        of rectangle that contains face stored
        in columns of matrix
    """
    f = open(file_name)
    str = f.read().replace(',', ' ')
    f.close()
    ret = np.array(map(int,str.split()) ,dtype=np.int32)
    ret = ret.reshape((2,2), order='F')
    return ret


DIR = '../../../data/Images/'
JPGS = [f for f in os.listdir(DIR) if fnmatch(f, '*.jpg')]
flmrk = PyFlandmark("../../../data/flandmark_model.xml", False)


for jpg_name in JPGS:

    file_name = jpg_name[:-4]
    img = Image.open(DIR + jpg_name)
    arr = rgb2gray(np.asarray(img))
    bbox = read_bbox_from_txt(DIR + jpg_name[:-4] + '.det')

    d_landmarks = flmrk.detect(arr, bbox)
    n = d_landmarks.shape[1]

    print "test detect method"

    im = Image.fromarray(arr)
    img_dr = ImageDraw.Draw(im)
    img_dr.rectangle([tuple(bbox[:,0]), tuple(bbox[:,1])], outline="#FF00FF")
    r = 2.
    for i in xrange(n):
        x = d_landmarks[0,i]
        y = d_landmarks[1,i]
        img_dr.ellipse((x-r, y-r, x+r, y+r), fill=0.)

    plt.imshow(np.asarray(im), cmap = plt.get_cmap('gray'))
    plt.show()

    print "test get_normalized_frame method"

    frame = flmrk.get_normalized_frame(arr, bbox)[0]
    frame = frame.astype(np.double)
    im = Image.fromarray(frame)
    plt.imshow(np.asarray(im), cmap = plt.get_cmap('gray'))
    plt.show()

    print "test detect_base method"

    landmarks = flmrk.detect_base(frame)

    im = Image.fromarray(frame)
    img_dr = ImageDraw.Draw(im)

    r = 2.
    for i in xrange(n):
        x = landmarks[0, i]
        y = landmarks[1, i]
        img_dr.ellipse((x-r, y-r, x+r, y+r), fill=0.)

    plt.imshow(np.asarray(im), cmap = plt.get_cmap('gray'))
    plt.show()

    print "test psi method"
    # psi = flmrk.get_psi(frame, landmarks.astype(np.int32), bbox)
    psi = flmrk.get_psi(landmarks, frame, bbox)

#flmrk.get_psi(d_landmarks, arr, bbox)

    break