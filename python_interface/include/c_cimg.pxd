cdef extern from "CLandmark.h" namespace "cimg_library":
    cdef cppclass CImg[T]:             
        CImg(const unsigned int, const unsigned int, const unsigned int, const T)
        T* data(const unsigned int, const unsigned int, const unsigned int, const unsigned int)
        int width()
        int height()