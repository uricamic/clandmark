cimport c_flandmark

cdef class PyFlandmark:
    cdef c_flandmark.Flandmark *thisptr      # hold a C++ instance which we're wrapping