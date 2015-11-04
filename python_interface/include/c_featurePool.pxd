cdef extern from "CFeatures.h" namespace "clandmark":
    cdef cppclass CFeatures:
        CFeatures(int, int, int, int*)
        inline void setNFmipmap( unsigned char *)
        void * const getFeatures()

cdef extern from "CSparseLBPFeatures.h" namespace "clandmark":
    cdef cppclass CSparseLBPFeatures:
        CSparseLBPFeatures(int, int, int, int*) except +
        void compute()
        void *getFeatures()
        void setFeatures(CFeatures * const)
        void setFeaturesRaw(void *)

cdef extern from "CFeaturePool.h"  namespace "clandmark":
    cdef cppclass CFeaturePool:
        CFeaturePool(int, int, unsigned char *) except +
        CFeaturePool(int, int) except +
        void addFeaturesToPool(CFeatures *)
        void updateFeaturesRaw(int, void*)
        CFeatures *getFeaturesFromPool(unsigned int)
        void *getFeatures()
        void setNFmipmap(unsigned char * const)
        void updateNFmipmap(int, int, unsigned char *const)
        inline int getWidth()
        inline int getHeight()
        void computeFeatures()
        inline int getPyramidLevels()
        inline int *getCumulativeWidths()
