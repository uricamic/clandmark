cimport c_flandmark
cimport c_cimg
cimport c_featurePool

from hgext.largefiles import featuresetup
from libc.stdlib cimport calloc, malloc, free

from cpython.version cimport PY_MAJOR_VERSION

cimport numpy
import numpy

from py_featurePool cimport PyFeaturePool

# cdef c_featurePool.CFeaturePool *fp

def convert_text(text):
    if isinstance(text, unicode): # most common case first
        utf8_data = text.encode('UTF-8')
    elif (PY_MAJOR_VERSION < 3) and isinstance(text, str):
        text.decode('ASCII') # trial decoding, or however you want to check for plain ASCII data
        utf8_data = text
    else:
        raise ValueError("requires text input, got %s" % type(text))
    return utf8_data


cdef class PyFlandmark:

    def __cinit__(self, file_name, learning_mode):
        """
        :param file_name:
        :param learning_mode:
        :return:
        """
        utf8_data = convert_text(file_name)
        cdef char *str = utf8_data

        if learning_mode:
            self.thisptr = new c_flandmark.Flandmark(str, True)
        else:
            self.thisptr = new c_flandmark.Flandmark(str, False)

    def __dealloc__(self):
        """
        :return:
        """
        del self.thisptr

    def detect(self, numpy.ndarray[double, ndim = 2] img, box):
        """
        :param img:
        :param box:
        :return:
        """

        if not self._is_2xN_array(box, numpy.dtype('int32')): raise Exception('configuration be 2xN numpy.int32 array')

        if box.size == 4:
            box = numpy.array([[ box[0, 0], box[0, 1], box[0, 1], box[0, 0] ],
                               [ box[1,0], box[1,0], box[1,1], box[1,1] ]], box.dtype)

        cdef int i,j
        cdef int height = img.shape[0]
        cdef int width = img.shape[1]
        cdef c_cimg.CImg[unsigned char]* c_img = new c_cimg.CImg[unsigned char](width, height, 1, 1)

        #convert img to CImg object
        cdef unsigned char *ptr
        cdef double tmp

        for i in xrange(height):
            for j in xrange(width):
                ptr = c_img.data(j,i,0,0)
                tmp = max(0, img[i,j])
                tmp = min(255, tmp)
                ptr[0] = <unsigned char>(tmp)

        # convert box to needed format
        box = numpy.ascontiguousarray(box.flatten('F'), dtype = numpy.int32)
        #call native function
        self.thisptr.detect(c_img, <int *const>numpy.PyArray_DATA(box), <double*> 0)

        #extract result
        cdef double *res_ptr = self.thisptr.getLandmarks()
        cdef int landmarks_num = self.thisptr.getLandmarksCount()
        cdef numpy.ndarray landmarks = numpy.zeros((2,landmarks_num))

        for i in xrange(landmarks_num):
            landmarks[0,i] = res_ptr[2*i]
            landmarks[1,i] = res_ptr[2*i+1]

        del c_img

        return landmarks

    def detect_base(self, numpy.ndarray[double, ndim = 2] img, ground_truth = None):
        """
        :param img:
        :param ground_truth:
        :return:
        """

        if not ground_truth is None and not self._is_2xN_array(ground_truth, numpy.dtype('int32')):
            raise Exception("ground_truth must be 2-dim numpy.int32")

        cdef int i,j
        cdef int height = img.shape[0]
        cdef int width = img.shape[1]
        cdef c_cimg.CImg[unsigned char]* c_img = new c_cimg.CImg[unsigned char](width, height, 1, 1)

        #convert img to CImg object
        cdef unsigned char *ptr
        cdef double tmp

        for i in xrange(height):
            for j in xrange(width):
                ptr = c_img.data(j,i,0,0)
                tmp = max(0, img[i,j])
                tmp = min(255, tmp)
                ptr[0] = <unsigned char>(tmp)

        #call native function
        if ground_truth is None:
            self.thisptr.detect_base(c_img, <int*> 0)
        else:
            ground_truth = numpy.ascontiguousarray(ground_truth.flatten('F'), dtype = numpy.int32)
            self.thisptr.detect_base(c_img, <int*> numpy.PyArray_DATA(ground_truth))

        cdef int *res_ptr = self.thisptr.getLandmarksNF()
        cdef int landmarks_num = self.thisptr.getLandmarksCount()
        cdef numpy.ndarray landmarks = numpy.zeros((2,landmarks_num), dtype=numpy.int32)

        for i in xrange(landmarks_num):
            landmarks[0,i] = res_ptr[2*i]
            landmarks[1,i] = res_ptr[2*i+1]

        del c_img
        return landmarks

    def get_normalized_frame(self, numpy.ndarray[double, ndim = 2] img, box, numpy.ndarray ground_truth = None):
        """
        :param img:
        :param box:
        :param ground_truth:
        :return:
        """

        if not self._is_2xN_array(box, numpy.dtype('int32')): raise Exception('configuration be 2xN numpy.int32 array')

        if box.size == 4:
            box = numpy.array([[ box[0, 0], box[0, 1], box[0, 1], box[0, 0] ],
                               [ box[1,0], box[1,0], box[1,1], box[1,1] ]], box.dtype)

        if not ground_truth is None:
            if not self._is_2xN_array(ground_truth, numpy.dtype('double')): raise Exception('configuration be 2xN numpy.double array')

        cdef int i,j
        cdef int height = img.shape[0]
        cdef int width = img.shape[1]
        cdef c_cimg.CImg[unsigned char]* c_img = new c_cimg.CImg[unsigned char](width, height, 1, 1)

        #convert img to CImg object
        cdef unsigned char *ptr
        cdef double tmp

        for i in xrange(height):
            for j in xrange(width):
                ptr = c_img.data(j,i,0,0)
                tmp = max(0, img[i,j])
                tmp = min(255, tmp)
                ptr[0] = <unsigned char>(tmp)

        # convert box to needed format
        box = numpy.ascontiguousarray(box.flatten('F'), dtype = numpy.int32)

        cdef int landmark_count = self.thisptr.getLandmarksCount()
        cdef c_cimg.CImg[unsigned char]* nf
        cdef int *outgt
        cdef numpy.ndarray out = numpy.zeros((2,landmark_count), dtype=numpy.int32)
        #scale_factor = None

        #call native function

        if not ground_truth is None:
            ground_truth = numpy.ascontiguousarray(ground_truth.flatten('F'), dtype=numpy.double)
            nf = self.thisptr.getNF(c_img, <int *const>numpy.PyArray_DATA(box), <double*> numpy.PyArray_DATA(ground_truth))
            outgt = self.thisptr.getGroundTruthNF()
            for i in xrange(landmark_count):
                out[0,i] = outgt[2*i]
                out[1,i] = outgt[2*i+1]
            #scale_factor = self.thisptr.getNormalizationFactor()
        else:
            nf = self.thisptr.getNF(c_img, <int *const>numpy.PyArray_DATA(box), <double*> 0)

        #free memory
        del c_img

        cdef int height_nf = nf.height()
        cdef int width_nf = nf.width()
        # cdef numpy.ndarray nf_out = numpy.zeros((height_nf, width_nf), dtype=numpy.double)
        cdef numpy.ndarray nf_out = numpy.zeros((height_nf, width_nf), dtype=numpy.uint8)

        for i in xrange(height_nf):
            for j in xrange(width_nf):
                ptr = nf.data(j,i,0,0)
                nf_out[i,j] = ptr[0]

        return nf_out, out

    def set_normalization_factor(self, double factor):
        """
        :param factor:
        :return:
        """
        self.thisptr.setNormalizationFactor(factor)

    def get_model_parameters_dimension(self):
        """
        :return:
        """
        self.thisptr.computeWdimension()
        return self.thisptr.getWdimension()

    def set_weights_parameters(self, numpy.ndarray params):
        """
        wrapper for setW
        :param params:
        :return:
        """

        if not self._raise_exception_if_not_array_one_dimensional_array_or_two_dimensional_with_second_dim_equal_one(params):
            raise Exception("params must be 1-dim array or Nx1(1xN) 2-dim array")

        params = numpy.ascontiguousarray(params, dtype = numpy.double)

        # needed to be called before function getWdimension call
        self.thisptr.computeWdimension()

        if self.thisptr.getWdimension() != params.size:
            raise Exception('dimmension of input vector parameters is different than dimmension of true model parameters')

        self.thisptr.setW(<double*> numpy.PyArray_DATA(params))

    def write(self, file_path, write_weights):
        """
        """
        utf8_data = convert_text(file_path)
        cdef char *str = utf8_data
        self.thisptr.write(utf8_data, write_weights)

    def get_psi_base(self, configuration, numpy.ndarray[unsigned char, ndim = 2] img):
        """
        :param configuration:
        :param img:
        :return:
        """

        if not self._is_2xN_array(configuration, numpy.dtype('int32')):
            raise Exception('configuration be 2xN numpy.int32 array')

        self.thisptr.computeWdimension()
        cdef int landmarks_count = self.thisptr.getWdimension()

        if configuration.size != self.thisptr.getLandmarksCount() * 2:
            raise Exception("...")

        configuration = numpy.ascontiguousarray(configuration.flatten('F'), dtype = numpy.int32)

        cdef int i,j
        cdef int height = img.shape[0]
        cdef int width = img.shape[1]
        cdef c_cimg.CImg[unsigned char]* c_img = new c_cimg.CImg[unsigned char](width, height, 1, 1)

        #convert img to CImg object
        cdef unsigned char *ptr
        cdef double tmp

        for i in xrange(height):
            for j in xrange(width):
                ptr = c_img.data(j,i,0,0)
                tmp = max(0, img[i,j])
                tmp = min(255, tmp)
                ptr[0] = <unsigned char>(tmp)

        cdef c_cimg.CImg[unsigned char]* nf = self.thisptr.getNF()
        cdef double * psi_ptr = self.thisptr.getFeatures_base(c_img, <int*>numpy.PyArray_DATA(configuration))

        self.thisptr.computeWdimension()

        cdef int weghts_dim = self.thisptr.getWdimension()
        cdef numpy.ndarray psi = numpy.zeros((weghts_dim,1))

        for i in xrange(weghts_dim): psi[i,0] = psi_ptr[i]

        return psi

    def get_psi(self, configuration, numpy.ndarray[double, ndim = 2] img = None, box = None):
        """
        :param configuration:
        :param img:
        :param box:
        :return:
        """

        if configuration is None:
            raise Exception('configuration can not be None')

        if not configuration is None and not self._is_2xN_array(configuration, numpy.dtype('int32')):
            raise Exception('configuration be 2xN numpy.int32 array')

        if not box is None and not self._is_2xN_array(box, numpy.dtype('int32')):
            raise Exception('configuration be 2xN numpy.int32 array')

        if configuration.size != self.thisptr.getLandmarksCount() * 2:
            raise Exception("number of landmarks does not coincide with object number of landmarks")

        configuration = numpy.ascontiguousarray(configuration.flatten('F'), dtype = numpy.int32)

        cdef double * psi_ptr
        cdef int i,j,height,width
        cdef c_cimg.CImg[unsigned char]* c_img
        cdef unsigned char *ptr
        cdef double tmp

        if img is None and box is None:
            psi_ptr = self.thisptr.getFeatures(<int*>numpy.PyArray_DATA(configuration))
        elif not img is None and not box is None:
            if box.size == 4:
                box = numpy.array([[ box[0, 0], box[0, 1], box[0, 1], box[0, 0] ], [ box[1,0], box[1,0], box[1,1], box[1,1] ]], box.dtype)
            height = img.shape[0]
            width = img.shape[1]
            c_img = new c_cimg.CImg[unsigned char](width, height, 1, 1)

            #convert img to CImg object
            for i in xrange(height):
                for j in xrange(width):
                    ptr = c_img.data(j,i,0,0)
                    tmp = max(0, img[i,j])
                    tmp = min(255, tmp)
                    ptr[0] = <unsigned char>(tmp)

            box = numpy.ascontiguousarray(box.flatten('F'), dtype = numpy.int32)
            psi_ptr = self.thisptr.getFeatures(c_img, <int*>numpy.PyArray_DATA(box), <int*>numpy.PyArray_DATA(configuration))

            del c_img
        else:
            raise Exception('both img and box have to be either noNone or valid arguments')

        self.thisptr.computeWdimension()
        cdef int weghts_dim = self.thisptr.getWdimension()

        cdef numpy.ndarray psi = numpy.zeros((weghts_dim,1))
        for i in xrange(weghts_dim): psi[i,0] = psi_ptr[i]

        return psi

    def setLossTable(self, loss_table, landmark_id):
        """
        :param loss_table:
        :param landmark_id:
        :return:
        """
        loss_table = numpy.ascontiguousarray(loss_table.flatten('C'), dtype = numpy.double)
        self.thisptr.setLossTable(<double*>numpy.PyArray_DATA(loss_table), landmark_id)

    def getSearchSpace(self, landmark_id):
        """
        :param landmark_id:
        :return:
        """
        cdef const int *ss = self.thisptr.getSearchSpace(landmark_id)
        return ss[0], ss[1], ss[2], ss[3]

    def getBaseWindowSize(self):
        cdef const int * ss = self.thisptr.getBaseWindowSize()
        return ss[0], ss[1]

    # def setFeaturePool(self, *featurePool):
    def setFeaturePool(self, featurePool):
        """
        :param featurePool:
        :return:
        """
        if isinstance(featurePool, PyFeaturePool):
            self.thisptr.setNFfeaturesPool((<PyFeaturePool>featurePool).thisptr)
        else:
            raise Exception("featurePool must be of type PyFeaturePool")

    def detect_optimized(self, numpy.ndarray[double, ndim = 2] img, box):
        """
        :param img:
        :param box:
        :return:
        """

        if not self._is_2xN_array(box, numpy.dtype('int32')): raise Exception('configuration be 2xN numpy.int32 array')

        if box.size == 4:
            box = numpy.array([[ box[0, 0], box[0, 1], box[0, 1], box[0, 0] ],
                               [ box[1,0], box[1,0], box[1,1], box[1,1] ]], box.dtype)

        cdef int i,j
        cdef int height = img.shape[0]
        cdef int width = img.shape[1]
        cdef c_cimg.CImg[unsigned char]* c_img = new c_cimg.CImg[unsigned char](width, height, 1, 1)

        #convert img to CImg object
        cdef unsigned char *ptr
        cdef double tmp

        for i in xrange(height):
            for j in xrange(width):
                ptr = c_img.data(j,i,0,0)
                tmp = max(0, img[i,j])
                tmp = min(255, tmp)
                ptr[0] = <unsigned char>(tmp)

        # convert box to needed format
        box = numpy.ascontiguousarray(box.flatten('F'), dtype = numpy.int32)
        #call native function
        self.thisptr.detect_optimized(c_img, <int *const>numpy.PyArray_DATA(box), <double*> 0)

        #extract result
        cdef double *res_ptr = self.thisptr.getLandmarks()
        cdef int landmarks_num = self.thisptr.getLandmarksCount()
        cdef numpy.ndarray landmarks = numpy.zeros((2,landmarks_num))

        for i in xrange(landmarks_num):
            landmarks[0,i] = res_ptr[2*i]
            landmarks[1,i] = res_ptr[2*i+1]

        del c_img

        return landmarks


    ###################################################################################
    # aulilary functions
    ###################################################################################

    def _raise_exception_if_not_array_one_dimensional_array_or_two_dimensional_with_second_dim_equal_one(self, arr):
        if arr.ndim > 2: return False
        if arr.ndim == 2 and arr.shape[1] != 1: return False
        return True

    def _is_2xN_array(self, arr, dtype):
        if arr.ndim != 2 or arr.shape[0] != 2: return False
        if arr.dtype.kind != dtype.kind: return False
        return True