cimport c_featurePool
from c_featurePool cimport CFeaturePool

from cpython.version cimport PY_MAJOR_VERSION

cimport numpy
import numpy

cdef class PyFeaturePool:
    """
    """

    def __cinit__(self, width, height, nf):
        """
        :param width:
        :param height:
        :param nf:
        :return:
        """
        if nf is not None:
            self.thisptr = new c_featurePool.CFeaturePool(width, height, nf)
        else:
            self.thisptr = new c_featurePool.CFeaturePool(width, height)

    def __dealloc__(self):
        """
        :return:
        """
        del self.thisptr

    def addFeatuaddSparseLBPfeatures(self):
        """
        :return:
        """
        self.thisptr.addFeaturesToPool(
            <c_featurePool.CFeatures*>
            new c_featurePool.CSparseLBPFeatures(
                self.thisptr.getWidth(),
                self.thisptr.getHeight(),
                self.thisptr.getPyramidLevels(),
                self.thisptr.getCumulativeWidths()
            )
        )

    def getWidth(self):
        """
        :return:
        """
        return self.thisptr.getWidth()

    def getHeight(self):
        """
        :return:
        """
        return self.thisptr.getHeight()

    def getPyramidLevels(self):
        """
        :return:
        """
        return self.thisptr.getPyramidLevels()

    def getCumulativeWidths(self):
        """
        :return:
        """
        cdef int* pyr = self.thisptr.getCumulativeWidths()
        cdef numpy.ndarray out = numpy.zeros((self.thisptr.getPyramidLevels(), 1), dtype=numpy.int32)
        for i in xrange(self.thisptr.getPyramidLevels()): out[i,0] = pyr[i]
        return out

    def setFeaturesRaw(self, index, features):
        """
        :param index:
        :param features:
        :return:
        """
        # TODO: check inputs
        cdef unsigned char* featuresRaw = <unsigned char*>features
        self.thisptr.updateFeaturesRaw(index, <void*>featuresRaw)
        return

    def getFeaturesRaw(self, index):
        """
        :param index:
        :return:
        """
        # TODO: check input
        width = 2*self.thisptr.getWidth()
        height = self.thisptr.getHeight()
        cdef numpy.ndarray out = numpy.zeros((height, width), dtype=numpy.uint8)
        cdef unsigned char* rawLBPfeatures = <unsigned char*>self.thisptr.getFeaturesFromPool(index).getFeatures()
        for i in xrange(height):
            for j in xrange(width):
                out[i, j] = rawLBPfeatures[j*height+i]
        return out

    def computeFromNF(self, numpy.ndarray[unsigned char, ndim = 2] nf):
        """
        :param NF:
        :return:
        """
        # TODO: Check input
        # cdef unsigned char* nf = <unsigned char*>NF
        self.thisptr.updateNFmipmap(self.thisptr.getWidth(), self.thisptr.getHeight(), <unsigned char*>numpy.PyArray_DATA(nf))