from c_featurePool cimport CFeaturePool

cdef class PyFeaturePool:
    cdef CFeaturePool* thisptr   # hold a C++ instance which we're wrapping