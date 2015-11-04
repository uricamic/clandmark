from c_cimg cimport CImg

from c_featurePool cimport CFeaturePool
from py_featurePool import PyFeaturePool

# cdef extern from "CTypes.h":
#     cdef int _DOUBLE_PRECISION
# DOUBLE_PRECISION = _DOUBLE_PRECISION
# if DOUBLE_PRECISION==1:

# TODO: check and update this
from wx.lib import flashwin_old

# TODO: propagate definition from CMake and set here either double or float
ctypedef double fl_double_t

cdef extern from "Flandmark.h"  namespace "clandmark":
    cdef cppclass Flandmark:
        Flandmark(const char*, int) except +
        void detect(CImg[unsigned char]*, int *, fl_double_t * const)
        void detect_base(CImg[unsigned char]*, int * const)
        int getLandmarksCount()
        fl_double_t* getLandmarks()
        int* getLandmarksNF()
        CImg[unsigned char]* getNF(CImg[unsigned char]*, int *, fl_double_t * const)
        CImg[unsigned char]* getNF()
        int* getGroundTruthNF()
        fl_double_t getNormalizationFactor()
        void setNormalizationFactor(fl_double_t)
        int getWdimension()
        void computeWdimension()
        void setW(fl_double_t * const)
        void write(const char *, int)
        fl_double_t* getFeatures_base(CImg[unsigned char]*, int * const)
        fl_double_t* getFeatures(CImg[unsigned char]*, int * const, int * const)
        fl_double_t* getFeatures(int * const)
        void setLossTable(fl_double_t* loss_data, const int landmark_id)
        const int * getSearchSpace(const int landmark_id)
        const int * getBaseWindowSize()
        void setNFfeaturesPool(CFeaturePool * const)
        void detect_optimized(CImg[unsigned char]*, int *, fl_double_t * const)
        void detect_optimizedFromPool(int *, fl_double_t * const)
        void detect_base_optimized(int * const)
        void detect_base_optimized(CImg[unsigned char]*, int * const)

