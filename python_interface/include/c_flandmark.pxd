from c_cimg cimport CImg

cdef extern from "Flandmark.h"  namespace "clandmark":
    cdef cppclass Flandmark:             
        Flandmark(const char*, int) except +
        void detect(CImg[unsigned char]*, int *, double * const)
        void detect_base(CImg[unsigned char]*, int * const)
        int getLandmarksCount()
        double* getLandmarks()        
        int* getLandmarksNF()
        CImg[unsigned char]* getNF(CImg[unsigned char]*, int *, double * const)
        CImg[unsigned char]* getNF()
        int* getGroundTruthNF()
        double getNormalizationFactor()
        void setNormalizationFactor(double)
        int getWdimension()
        void computeWdimension()
        void setW(double * const)
        void write(const char *, int)
        double* getFeatures_base(CImg[unsigned char]*, int * const)
        double *getFeatures(CImg[unsigned char]*, int * const, int * const)
        double *getFeatures(int * const)
        void setLossTable(double *loss_data, const int landmark_id)
        const int * getSearchSpace(const int landmark_id)
        const int * getBaseWindowSize()