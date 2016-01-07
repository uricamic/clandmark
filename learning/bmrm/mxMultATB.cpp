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
	if (nlhs != 1 || nrhs != 7)
	{
		mexErrMsgTxt("mxMultATB: wrong I/O.\n"
					"Usage: \n"
					"c = mxMultATB(A, B, m, n, k, alpha, beta);\n"
                    "c := alpha * A' * B + beta * C \n"
                    "A [k x m (double)] \n"
                    "B [k x n (double)] \n");
	}

	char *chn = "N";
	char *cht = "T";

	double* A = (double*)mxGetData(prhs[0]);
	double* B = (double*)mxGetData(prhs[1]);
	double m = mxGetScalar(prhs[2]);
	long int M = (int)m;
	double n = mxGetScalar(prhs[3]);
	long int N = (int)n;
	double k = mxGetScalar(prhs[4]);
	long int K = (int)k;
	double alpha = mxGetScalar(prhs[5]);
	double beta = mxGetScalar(prhs[6]);

	long int lda = K;
	long int ldb = K;
	long int ldc = M;

	plhs[0] = mxCreateNumericMatrix(M, N, mxDOUBLE_CLASS, mxREAL);
	double* C = (double*)mxGetData(plhs[0]);

	dgemm(cht, chn, &M, &N, &K, &alpha, A, &lda, B, &ldb, &beta, C, &ldc);
}

//extern void dgemm(
//    char   *transa,
//    char   *transb,
//    ptrdiff_t *m,
//    ptrdiff_t *n,
//    ptrdiff_t *k,
//    double *alpha,
//    double *a,
//    ptrdiff_t *lda,
//    double *b,
//    ptrdiff_t *ldb,
//    double *beta,
//    double *c,
//    ptrdiff_t *ldc
//);

