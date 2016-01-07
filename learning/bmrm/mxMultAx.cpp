/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2016 Michal Uricar
 * Copyright (C) 2016 Michal Uricar
 */

#if !defined(_WIN32)
#define dgemm dgemm_
#endif

#include "mex.h"
#include "blas.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if (nlhs != 1 || nrhs != 6)
	{
		mexErrMsgTxt("mxMultAx: wrong I/O.\n"
					"Usage: \n"
					"y = mxMultAx(alpha, A, m, n, x, beta); \n"
                    "y := alpha * A * x + beta * y \n"
                    "A [m x n (double)] \n"
                    "x [m x 1 (double)] \n");
	}

	char *chn = "N";
	char *cht = "T";

	double* A = (double*)mxGetData(prhs[1]);
	double alpha = mxGetScalar(prhs[0]);
	double m = mxGetScalar(prhs[2]);
	long int M = (int)m;
	double n = mxGetScalar(prhs[3]);
	long int N = (int)n;
	double* x = (double*)mxGetData(prhs[4]);
	double beta = mxGetScalar(prhs[5]);

	long int lda = M;
	long int incx = 1;
	long int incy = 1;

	plhs[0] = mxCreateNumericMatrix(M, 1, mxDOUBLE_CLASS, mxREAL);
	double* y = (double*)mxGetData(plhs[0]);

//	cblas_dgemv(CblasColMajor, CblasNoTrans, M, N, alpha, A, lda, x, incx, beta, y, incy);
	dgemv(chn, &M, &N, &alpha, A, &lda, x, &incx, &beta, y, &incy);
}

///* Source: dgemv.f */
//#define dgemv FORTRAN_WRAPPER(dgemv)
//extern void dgemv(
//    char   *trans,
//    ptrdiff_t *m,
//    ptrdiff_t *n,
//    double *alpha,
//    double *a,
//    ptrdiff_t *lda,
//    double *x,
//    ptrdiff_t *incx,
//    double *beta,
//    double *y,
//    ptrdiff_t *incy
//);

