/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2013, 2014, 2015 Michal Uricar
 * Copyright (C) 2013, 2014, 2015 Michal Uricar
 */

#include "mex.h"
#define cimg_plugin "cimgmatlab.h"

#include "flandmark_handle.h"
#include "Flandmark.h"

using namespace clandmark;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	// Get the command string
	char cmd[64];
	if (nrhs < 1 || mxGetString(prhs[0], cmd, sizeof(cmd)))
		mexErrMsgTxt("First input should be a command string less than 64 characters long.");

	// New
	//---------------------------------------------------------------------------------------------
	if (!strcmp("new", cmd))
	{
		// Check parameters
		if (nlhs != 1)
		{
			mexErrMsgTxt("New: One output expected.");
		}

		if (nrhs < 2 || nrhs > 3)
		{
			mexErrMsgTxt("New: 1-2 input argument(s) expected.");
		}

		char * fname = mxArrayToString(prhs[1]);
		if (fname == NULL)
		{
			mexErrMsgTxt("New: string input expected.");
		}

		bool train = false;

		if (nrhs == 3)
		{
			//train = (bool)mxGetData(prhs[2]);
			int * p_int = (int*)mxGetData(prhs[2]);
//			mexPrintf("DEBUG: p_int[0] = %d\n", p_int[0]);
			train = (p_int[0] > 0) ? true : false;
		}
//		mexPrintf("train flag: %d\n", train);

		// Return a handle to a new C++ instance
		Flandmark *flandmark = Flandmark::getInstanceOf(fname, train);

		if (flandmark)
		{
			plhs[0] = convertPtr2Mat< Flandmark >(flandmark);
			mexPrintf("Flandmark - initialization \n");
			mexPrintf("========================== \n");
			mexPrintf("name: %s\n", flandmark->getName().c_str());
			mexPrintf("version: %s\n", flandmark->getVersion().c_str());
			mexPrintf("W dim: %d\n", flandmark->getWdimension());
#if DOUBLE_PRECISION==1
			mexPrintf("DOUBLE_PRECISION\n");
#else
			mexPrintf("SINGLE_PRECISION\n");
#endif
			mexPrintf("train flag: %d\n", train);
			mexPrintf("========================== \n");
			mexPrintf("\n");

		} else {
			mexErrMsgTxt("Couldn't create flandmark instance.");
		}

		return;
	}

	// Check there is a second input, which should be the class instance handle
	if (nrhs < 2)
		mexErrMsgTxt("Second input should be a class instance handle.");

	// Delete
	//---------------------------------------------------------------------------------------------
	if (!strcmp("delete", cmd))
	{
		// Destroy the C++ object
		destroyObject< Flandmark >(prhs[1]);

		// Warn if other commands were ignored
		if (nlhs != 0 || nrhs != 2)
			mexWarnMsgTxt("Delete: Unexpected arguments ignored.");
		return;
	}

	// Get the class instance pointer from the second input
	Flandmark *flandmark_instance = convertMat2Ptr< Flandmark >(prhs[1]);

	//---------------------------------------------------------------------------------------------
	//------------------------ Call the various class methods -------------------------------------
	//---------------------------------------------------------------------------------------------

	// Detect		TODO: input checks + nice error output
	//---------------------------------------------------------------------------------------------
	if (!strcmp("detect", cmd))
	{
		// Check parameters
		//if (nlhs < 0 || nrhs < 4 || nrhs > 5)
		if (nlhs < 0 || nrhs < 4 || nrhs > 6)
		{
			mexErrMsgTxt("Detect: unexpected arguments.");
		}

//		bool isVerbose = false;
//		if (nlhs == 6)
//		{
//			double *p_double = (double*)mxGetData(prhs[5]);
//			double tmp = p_double[0];
//			isVerbose = (tmp > 0) ? true : false;
//		}

		cimg_library::CImg<unsigned char> img(prhs[2]);

		fl_double_t *landmarks = 0;
		bool isFullBox = false;
		int bb[8];

		plhs[0] = mxCreateNumericMatrix(2, flandmark_instance->getLandmarksCount(), mxDOUBLE_CLASS, mxREAL);
		double *output = (double*)mxGetData(plhs[0]);

		const mwSize *mx_dim = mxGetDimensions(prhs[3]);
		if (mx_dim[0]*mx_dim[1] != 4 && mx_dim[0]*mx_dim[1] != 8)
		{
			mexErrMsgTxt("Detect: bbox must be [1 x 4 (int32)] or [1 x 8 (int32)] vector.");
		}

		int *bbox = (int*)mxGetData(prhs[3]);

		if (mx_dim[0]*mx_dim[1] == 4)
		{
			// MATLAB input 1-based => subtract 1
			//int bb[4] = {bbox[0]-1, bbox[1]-1, bbox[2]-1, bbox[3]-1};
			bb[0] = bbox[0]-1;
			bb[1] = bbox[1]-1;
			bb[2] = bbox[2]-1;
			bb[3] = bbox[3]-1;
			isFullBox = false;
		} else {
			// MATLAB input 1-based => subtract 1
			bb[0] = bbox[0]-1; bb[1] = bbox[1]-1;
			bb[2] = bbox[2]-1; bb[3] = bbox[3]-1;
			bb[4] = bbox[4]-1; bb[5] = bbox[5]-1;
			bb[6] = bbox[6]-1; bb[7] = bbox[7]-1;
			isFullBox = true;
		}

		if (!isFullBox)
		{
			bb[7] = bb[3];
			bb[6] = bb[0];
			bb[5] = bb[3];
			bb[4] = bb[2];
			bb[3] = bb[1];
		}

		if (nrhs == 5)
		{
			mx_dim = mxGetDimensions(prhs[4]);
#if DOUBLE_PRECISION==1
			if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount() || mxGetClassID(prhs[4]) != mxDOUBLE_CLASS)
			{
				mexErrMsgTxt("Detect: ground_truth must be [2 x LandmarksCount (double)] vector");
			}
#else
			if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount() || mxGetClassID(prhs[4]) != mxSINGLE_CLASS)
			{
				mexErrMsgTxt("Detect: ground_truth must be [2 x LandmarksCount (single)] vector");
			}
#endif

			//double *gt = (double*)mxGetData(prhs[4]);
			fl_double_t *gt = (fl_double_t*)mxGetData(prhs[4]);
			// MATLAB input 1-based => subtract 1
			fl_double_t *ground_truth = new fl_double_t[2*flandmark_instance->getLandmarksCount()];
			for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
				ground_truth[i] = gt[i] - 1.0;

			flandmark_instance->detect(&img, &bb[0], ground_truth);

		} else if (nrhs < 5){

			flandmark_instance->detect(&img, &bb[0]);

		} /* else {

			mx_dim = mxGetDimensions(prhs[5]);
			// TODO

			double *loss_tables = (double*)mxGetData(prhs[5]);

			// Add loss tables
			flandmark_instance->setLossTables(loss_tables);			// 2014-01-31 This functionality was disabled !!!

			flandmark_instance->detect(&img, &bb[0]);

		}
		*/

		landmarks = flandmark_instance->getLandmarks();

		// output to MATLAB (1-based) => add 1
		for (int i = 0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			output[i] = landmarks[i]+1;
		}

		if (nlhs > 1)
		{
			fl_double_t *dotProdVal = 0x0;
#if DOUBLE_PRECISION==1
			plhs[1] = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
#else
			plhs[1] = mxCreateNumericMatrix(1, 1, mxSINGLE_CLASS, mxREAL);
#endif
			dotProdVal = (fl_double_t*)mxGetData(plhs[1]);
			(*dotProdVal) = 0.0;

			fl_double_t *Q = flandmark_instance->getQvalues();
			fl_double_t *G = flandmark_instance->getGvalues();

			for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
			{
				(*dotProdVal) += Q[i];
			}

			for (int i=0; i < flandmark_instance->getEdgesCount(); ++i)
			{
				(*dotProdVal) += G[i];
			}

			// Get joint vector [Q's, G's]
			if (nlhs > 2)
			{
#if DOUBLE_PRECISION==1
				plhs[2] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount()+flandmark_instance->getEdgesCount(), 1, mxDOUBLE_CLASS, mxREAL);
#else
				plhs[2] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount()+flandmark_instance->getEdgesCount(), 1, mxSINGLE_CLASS, mxREAL);
#endif
				fl_double_t *jointVect = (fl_double_t*)mxGetData(plhs[2]);

				// Q's
				for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
				{
					jointVect[i] = Q[i];
				}

				// G's
				for (int i=0; i < flandmark_instance->getEdgesCount(); ++i)
				{
					jointVect[flandmark_instance->getLandmarksCount()+i] = G[i];
				}
			}
		}

		return;
	}

	// Detect		TODO: input checks + nice error output
	//---------------------------------------------------------------------------------------------
	if (!strcmp("detect_optimized", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs < 4 || nrhs > 6)
		{
			mexErrMsgTxt("Detect: unexpected arguments.");
		}

		cimg_library::CImg<unsigned char> img(prhs[2]);

		fl_double_t *landmarks = 0;
		bool isFullBox = false;
		int bb[8];

		plhs[0] = mxCreateNumericMatrix(2, flandmark_instance->getLandmarksCount(), mxDOUBLE_CLASS, mxREAL);
		double *output = (double*)mxGetData(plhs[0]);

		const mwSize *mx_dim = mxGetDimensions(prhs[3]);
		if (mx_dim[0]*mx_dim[1] != 4 && mx_dim[0]*mx_dim[1] != 8)
		{
			mexErrMsgTxt("Detect: bbox must be [1 x 4 (int32)] or [1 x 8 (int32)] vector.");
		}

		int *bbox = (int*)mxGetData(prhs[3]);

		if (mx_dim[0]*mx_dim[1] == 4)
		{
			// MATLAB input 1-based => subtract 1
			bb[0] = bbox[0]-1;
			bb[1] = bbox[1]-1;
			bb[2] = bbox[2]-1;
			bb[3] = bbox[3]-1;
			isFullBox = false;
		} else {
			// MATLAB input 1-based => subtract 1
			bb[0] = bbox[0]-1; bb[1] = bbox[1]-1;
			bb[2] = bbox[2]-1; bb[3] = bbox[3]-1;
			bb[4] = bbox[4]-1; bb[5] = bbox[5]-1;
			bb[6] = bbox[6]-1; bb[7] = bbox[7]-1;
			isFullBox = true;
		}

		if (!isFullBox)
		{
			bb[7] = bb[3];
			bb[6] = bb[0];
			bb[5] = bb[3];
			bb[4] = bb[2];
			bb[3] = bb[1];
		}

		if (nrhs == 5)
		{
			mx_dim = mxGetDimensions(prhs[4]);
#if DOUBLE_PRECISION==1
			if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount() || mxGetClassID(prhs[4]) != mxDOUBLE_CLASS)
			{
				mexErrMsgTxt("Detect: ground_truth must be [2 x LandmarksCount (double)] vector");
			}
#else
			if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount() || mxGetClassID(prhs[4]) != mxSINGLE_CLASS)
			{
				mexErrMsgTxt("Detect: ground_truth must be [2 x LandmarksCount (single)] vector");
			}
#endif

			//double *gt = (double*)mxGetData(prhs[4]);
			fl_double_t *gt = (fl_double_t*)mxGetData(prhs[4]);
			// MATLAB input 1-based => subtract 1
			fl_double_t *ground_truth = new fl_double_t[2*flandmark_instance->getLandmarksCount()];
			for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
				ground_truth[i] = gt[i] - 1.0;

			flandmark_instance->detect_optimized(&img, &bb[0], ground_truth);

		} else if (nrhs < 5){

			flandmark_instance->detect_optimized(&img, &bb[0]);

		}

		landmarks = flandmark_instance->getLandmarks();

		// output to MATLAB (1-based) => add 1
		for (int i = 0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			output[i] = landmarks[i]+1;
		}

		if (nlhs > 1)
		{
			fl_double_t *dotProdVal = 0x0;
#if DOUBLE_PRECISION==1
			plhs[1] = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
#else
			plhs[1] = mxCreateNumericMatrix(1, 1, mxSINGLE_CLASS, mxREAL);
#endif
			dotProdVal = (fl_double_t*)mxGetData(plhs[1]);
			(*dotProdVal) = 0.0;

			fl_double_t *Q = flandmark_instance->getQvalues();
			fl_double_t *G = flandmark_instance->getGvalues();

			for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
			{
				(*dotProdVal) += Q[i];
			}

			for (int i=0; i < flandmark_instance->getEdgesCount(); ++i)
			{
				(*dotProdVal) += G[i];
			}

			// Get joint vector [Q's, G's]
			if (nlhs > 2)
			{
#if DOUBLE_PRECISION==1
				plhs[2] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount()+flandmark_instance->getEdgesCount(), 1, mxDOUBLE_CLASS, mxREAL);
#else
				plhs[2] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount()+flandmark_instance->getEdgesCount(), 1, mxSINGLE_CLASS, mxREAL);
#endif
				fl_double_t *jointVect = (fl_double_t*)mxGetData(plhs[2]);

				// Q's
				for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
				{
					jointVect[i] = Q[i];
				}

				// G's
				for (int i=0; i < flandmark_instance->getEdgesCount(); ++i)
				{
					jointVect[flandmark_instance->getLandmarksCount()+i] = G[i];
				}
			}
		}

		return;
	}

	// Detect optimized from Pool
	//---------------------------------------------------------------------------------------------
	if (!strcmp("detectOptimizedFromPool", cmd))
	{
		// Check parameters
		//if (nlhs < 0 || nrhs < 4 || nrhs > 5)
		if (nlhs < 0 || nrhs < 3 || nrhs > 5)
		{
			mexErrMsgTxt("detectOptimizedFromPool: unexpected arguments.");
		}

		fl_double_t *landmarks = 0;
		bool isFullBox = false;
		int bb[8];

		plhs[0] = mxCreateNumericMatrix(2, flandmark_instance->getLandmarksCount(), mxDOUBLE_CLASS, mxREAL);
		double *output = (double*)mxGetData(plhs[0]);

		const mwSize *mx_dim = mxGetDimensions(prhs[2]);
		if (mx_dim[0]*mx_dim[1] != 4 && mx_dim[0]*mx_dim[1] != 8)
		{
			mexErrMsgTxt("detectOptimizedFromPool: bbox must be [1 x 4 (int32)] or [1 x 8 (int32)] vector.");
		}

		int *bbox = (int*)mxGetData(prhs[2]);

		if (mx_dim[0]*mx_dim[1] == 4)
		{
			// MATLAB input 1-based => subtract 1
			//int bb[4] = {bbox[0]-1, bbox[1]-1, bbox[2]-1, bbox[3]-1};
			bb[0] = bbox[0]-1;
			bb[1] = bbox[1]-1;
			bb[2] = bbox[2]-1;
			bb[3] = bbox[3]-1;
			isFullBox = false;
		} else {
			// MATLAB input 1-based => subtract 1
			bb[0] = bbox[0]-1; bb[1] = bbox[1]-1;
			bb[2] = bbox[2]-1; bb[3] = bbox[3]-1;
			bb[4] = bbox[4]-1; bb[5] = bbox[5]-1;
			bb[6] = bbox[6]-1; bb[7] = bbox[7]-1;
			isFullBox = true;
		}

		if (!isFullBox)
		{
			bb[7] = bb[3];
			bb[6] = bb[0];
			bb[5] = bb[3];
			bb[4] = bb[2];
			bb[3] = bb[1];
		}

		if (nrhs == 4)
		{
			mx_dim = mxGetDimensions(prhs[3]);
#if DOUBLE_PRECISION==1
			if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount() || mxGetClassID(prhs[3]) != mxDOUBLE_CLASS)
			{
				mexErrMsgTxt("detectOptimizedFromPool: ground_truth must be [2 x LandmarksCount (double)] vector");
			}
#else
			if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount() || mxGetClassID(prhs[3]) != mxSINGLE_CLASS)
			{
				mexErrMsgTxt("detectOptimizedFromPool: ground_truth must be [2 x LandmarksCount (single)] vector");
			}
#endif

			fl_double_t *gt = (fl_double_t*)mxGetData(prhs[3]);
			// MATLAB input 1-based => subtract 1
			fl_double_t *ground_truth = new fl_double_t[2*flandmark_instance->getLandmarksCount()];
			for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
				ground_truth[i] = gt[i] - 1.0;

			flandmark_instance->detect_optimizedFromPool(&bb[0], ground_truth);

		} else if (nrhs < 4){

			flandmark_instance->detect_optimizedFromPool(&bb[0]);

		}

		landmarks = flandmark_instance->getLandmarks();

		// output to MATLAB (1-based) => add 1
		for (int i = 0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			output[i] = landmarks[i]+1;
		}

		if (nlhs > 1)
		{
			fl_double_t *dotProdVal = 0x0;
#if DOUBLE_PRECISION==1
			plhs[1] = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
#else
			plhs[1] = mxCreateNumericMatrix(1, 1, mxSINGLE_CLASS, mxREAL);
#endif
			dotProdVal = (fl_double_t*)mxGetData(plhs[1]);
			(*dotProdVal) = 0.0;

			fl_double_t *Q = flandmark_instance->getQvalues();
			fl_double_t *G = flandmark_instance->getGvalues();

			for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
			{
				(*dotProdVal) += Q[i];
			}

			for (int i=0; i < flandmark_instance->getEdgesCount(); ++i)
			{
				(*dotProdVal) += G[i];
			}

			// Get joint vector [Q's, G's]
			if (nlhs > 2)
			{
#if DOUBLE_PRECISION==1
				plhs[2] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount()+flandmark_instance->getEdgesCount(), 1, mxDOUBLE_CLASS, mxREAL);
#else
				plhs[2] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount()+flandmark_instance->getEdgesCount(), 1, mxSINGLE_CLASS, mxREAL);
#endif
				fl_double_t *jointVect = (fl_double_t*)mxGetData(plhs[2]);

				// Q's
				for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
				{
					jointVect[i] = Q[i];
				}

				// G's
				for (int i=0; i < flandmark_instance->getEdgesCount(); ++i)
				{
					jointVect[flandmark_instance->getLandmarksCount()+i] = G[i];
				}
			}
		}

		return;
	}

	// Detect mirrored
	//---------------------------------------------------------------------------------------------
	if (!strcmp("detectMirrored", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs < 4 || nrhs > 6)
		{
			mexErrMsgTxt("Detect: unexpected arguments.");
		}

		cimg_library::CImg<unsigned char> img(prhs[2]);

		fl_double_t *landmarks = 0;
		bool isFullBox = false;
		int bb[8];

		plhs[0] = mxCreateNumericMatrix(2, flandmark_instance->getLandmarksCount(), mxDOUBLE_CLASS, mxREAL);
		double *output = (double*)mxGetData(plhs[0]);

		const mwSize *mx_dim = mxGetDimensions(prhs[3]);
		if (mx_dim[0]*mx_dim[1] != 4 && mx_dim[0]*mx_dim[1] != 8)
		{
			mexErrMsgTxt("Detect: bbox must be [1 x 4 (int32)] or [1 x 8 (int32)] vector.");
		}

		int *bbox = (int*)mxGetData(prhs[3]);

		if (mx_dim[0]*mx_dim[1] == 4)
		{
			// MATLAB input 1-based => subtract 1
			//int bb[4] = {bbox[0]-1, bbox[1]-1, bbox[2]-1, bbox[3]-1};
			bb[0] = bbox[0]-1;
			bb[1] = bbox[1]-1;
			bb[2] = bbox[2]-1;
			bb[3] = bbox[3]-1;
			isFullBox = false;
		} else {
			// MATLAB input 1-based => subtract 1
			bb[0] = bbox[0]-1; bb[1] = bbox[1]-1;
			bb[2] = bbox[2]-1; bb[3] = bbox[3]-1;
			bb[4] = bbox[4]-1; bb[5] = bbox[5]-1;
			bb[6] = bbox[6]-1; bb[7] = bbox[7]-1;
			isFullBox = true;
		}

		if (!isFullBox)
		{
			bb[7] = bb[3];
			bb[6] = bb[0];
			bb[5] = bb[3];
			bb[4] = bb[2];
			bb[3] = bb[1];
		}

		if (nrhs == 5)
		{
			mx_dim = mxGetDimensions(prhs[4]);
#if DOUBLE_PRECISION==1
			if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount() || mxGetClassID(prhs[4]) != mxDOUBLE_CLASS)
			{
				mexErrMsgTxt("Detect: ground_truth must be [2 x LandmarksCount (double)] vector");
			}
#else
			if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount() || mxGetClassID(prhs[4]) != mxSINGLE_CLASS)
			{
				mexErrMsgTxt("Detect: ground_truth must be [2 x LandmarksCount (single)] vector");
			}
#endif

			fl_double_t *gt = (fl_double_t*)mxGetData(prhs[4]);
			// MATLAB input 1-based => subtract 1
			fl_double_t *ground_truth = new fl_double_t[2*flandmark_instance->getLandmarksCount()];
			for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
				ground_truth[i] = gt[i] - 1.0;

			flandmark_instance->detect_mirrored(&img, &bb[0], ground_truth);
		} else if (nrhs < 5){
			flandmark_instance->detect_mirrored(&img, &bb[0]);
		}

		landmarks = flandmark_instance->getLandmarks();

		// output to MATLAB (1-based) => add 1
		for (int i = 0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			output[i] = landmarks[i]+1;
		}

		if (nlhs > 1)
		{
			fl_double_t *dotProdVal = 0x0;
#if DOUBLE_PRECISION==1
			plhs[1] = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
#else
			plhs[1] = mxCreateNumericMatrix(1, 1, mxSINGLE_CLASS, mxREAL);
#endif
			dotProdVal = (fl_double_t*)mxGetData(plhs[1]);
			(*dotProdVal) = 0.0;

			fl_double_t *Q = flandmark_instance->getQvalues();
			fl_double_t *G = flandmark_instance->getGvalues();

			for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
			{
				(*dotProdVal) += Q[i];
			}

			for (int i=0; i < flandmark_instance->getEdgesCount(); ++i)
			{
				(*dotProdVal) += G[i];
			}

			// Get joint vector [Q's, G's]
			if (nlhs > 2)
			{
#if DOUBLE_PRECISION==1
				plhs[2] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount()+flandmark_instance->getEdgesCount(), 1, mxDOUBLE_CLASS, mxREAL);
#else
				plhs[2] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount()+flandmark_instance->getEdgesCount(), 1, mxSINGLE_CLASS, mxREAL);
#endif
				fl_double_t *jointVect = (fl_double_t*)mxGetData(plhs[2]);

				// Q's
				for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
				{
					jointVect[i] = Q[i];
				}

				// G's
				for (int i=0; i < flandmark_instance->getEdgesCount(); ++i)
				{
					jointVect[flandmark_instance->getLandmarksCount()+i] = G[i];
				}
			}
		}

		return;
	}

	// Detect base		TODO: input checks + nice error output
	//---------------------------------------------------------------------------------------------
	if (!strcmp("detectBase", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs < 3 || nrhs > 4)
		{
			mexErrMsgTxt("detectBase: unexpected arguments.");
		}

		cimg_library::CImg<unsigned char> img(prhs[2]);

		int *landmarks = 0;

		plhs[0] = mxCreateNumericMatrix(2, flandmark_instance->getLandmarksCount(), mxINT32_CLASS, mxREAL);
		int *output = (int*)mxGetData(plhs[0]);

		if (nrhs == 4)
		{
			const mwSize *mx_dim = mxGetDimensions(prhs[3]);
			if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount())
			{
				mexErrMsgTxt("detectBase: ground_truth must be [2 x LandmarksCount (int32)] vector");
			}

			int *gt = (int*)mxGetData(prhs[3]);

			// MATLAB input 1-based => subtract 1
			int *ground_truth = new int[2*flandmark_instance->getLandmarksCount()];
			for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
				ground_truth[i] = gt[i]-1;

			flandmark_instance->detect_base(&img, ground_truth);

			delete [] ground_truth;
		} else {

			flandmark_instance->detect_base(&img);
		}

		landmarks = flandmark_instance->getLandmarksNF();

		// output to MATLAB (1-based) => add 1
		for (int i = 0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			output[i] = landmarks[i]+1;
		}

//		if (nlhs == 2)
		if (nlhs > 1)
		{
			fl_double_t *dotProdVal = 0x0;
#if DOUBLE_PRECISION==1
			plhs[1] = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
#else
			plhs[1] = mxCreateNumericMatrix(1, 1, mxSINGLE_CLASS, mxREAL);
#endif
			dotProdVal = (fl_double_t*)mxGetData(plhs[1]);
			(*dotProdVal) = 0.0;

			fl_double_t *Q = flandmark_instance->getQvalues();
			fl_double_t *G = flandmark_instance->getGvalues();

			for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
			{
				(*dotProdVal) += Q[i];
			}

			for (int i=0; i < flandmark_instance->getEdgesCount(); ++i)
			{
				(*dotProdVal) += G[i];
			}

			// Get joint vector [Q's, G's]
			if (nlhs > 2)
			{
#if DOUBLE_PRECISION==1
				plhs[2] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount()+flandmark_instance->getEdgesCount(), 1, mxDOUBLE_CLASS, mxREAL);
#else
				plhs[2] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount()+flandmark_instance->getEdgesCount(), 1, mxSINGLE_CLASS, mxREAL);
#endif
				fl_double_t *jointVect = (fl_double_t*)mxGetData(plhs[2]);

				// Q's
				for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
				{
					jointVect[i] = Q[i];
				}

				// G's
				for (int i=0; i < flandmark_instance->getEdgesCount(); ++i)
				{
					jointVect[flandmark_instance->getLandmarksCount()+i] = G[i];
				}
			}

			if (nlhs > 3)
			{
#if DOUBLE_PRECISION==1
				plhs[3] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount(), 1, mxDOUBLE_CLASS, mxREAL);
#else
				plhs[3] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount(), 1, mxSINGLE_CLASS, mxREAL);
#endif
				fl_double_t *losses = (fl_double_t*)mxGetData(plhs[3]);

				fl_double_t *L = flandmark_instance->getLossValues(flandmark_instance->getLandmarksNF());

				for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
				{
					losses[i] = L[i];
				}
			}
		}

		return;
	}

	// Get normalized frame		TODO: input checks + nice error output
	//---------------------------------------------------------------------------------------------
	if (!strcmp("getNF", cmd))
	{
		// Check parameters
		//if (nlhs < 0 || nrhs < 4 || nrhs > 5)
		if (nlhs < 0 || nrhs < 2 || nrhs > 5)
		{
			//mexErrMsgTxt("getNF: unexpected arguments.");
			mexErrMsgTxt("getNF: chujlo.");
		}

		cimg_library::CImg<unsigned char> *nf = 0x0;

		if (nrhs == 2)
		{
			nf = flandmark_instance->getNF();
			plhs[0] = nf->toMatlab(mxUINT8_CLASS);
			return;
		}

		cimg_library::CImg<unsigned char> img(prhs[2]);

		int *bbox = (int*)mxGetData(prhs[3]);

		int bb[8];
		bool isFullBox = false;

		const mwSize *mx_dim = mxGetDimensions(prhs[3]);
		if (mx_dim[0]*mx_dim[1] == 4)
		{
			// MATLAB input 1-based => subtract 1
			bb[0] = bbox[0]-1;
			bb[1] = bbox[1]-1;
			bb[2] = bbox[2]-1;
			bb[3] = bbox[3]-1;
			isFullBox = false;
		} else {
			// MATLAB input 1-based => subtract 1
			bb[0] = bbox[0]-1; bb[1] = bbox[1]-1;
			bb[2] = bbox[2]-1; bb[3] = bbox[3]-1;
			bb[4] = bbox[4]-1; bb[5] = bbox[5]-1;
			bb[6] = bbox[6]-1; bb[7] = bbox[7]-1;
			isFullBox = true;
		}

		if (!isFullBox)
		{
			bb[7] = bb[3];
			bb[6] = bb[0];
			bb[5] = bb[3];
			bb[4] = bb[2];
			bb[3] = bb[1];
		}

		if ( nrhs == 5)
		{
			const mwSize *mx_dim = mxGetDimensions(prhs[4]);
#if DOUBLE_PRECISION==1
			//if (mx_dim[0] != 2 && mx_dim[1] != flandmark_instance->getLandmarksCount() && mxGetClassID(prhs[4]) != mxDOUBLE_CLASS)
			if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount() || mxGetClassID(prhs[4]) != mxDOUBLE_CLASS)
			{
				mexErrMsgTxt("getNF: ground truth must be [2 x numLandmarks (double)] matrix.");
			}
#else
			//if (mx_dim[0] != 2 && mx_dim[1] != flandmark_instance->getLandmarksCount() && mxGetClassID(prhs[4]) != mxSINGLE_CLASS)
			if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount() || mxGetClassID(prhs[4]) != mxSINGLE_CLASS)
			{
				mexErrMsgTxt("getNF: ground truth must be [2 x numLandmarks (single)] matrix.");
			}
#endif

			fl_double_t *gt = (fl_double_t*)mxGetData(prhs[4]);

			// MATLAB input 1-based => subtract 1
			fl_double_t *ground_truth = new fl_double_t[2*flandmark_instance->getLandmarksCount()];
			for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
				ground_truth[i] = gt[i]-1.0;

			nf = flandmark_instance->getNF(&img, &bb[0], ground_truth);

			int *outgt = flandmark_instance->getGroundTruthNF();

			plhs[1] = mxCreateNumericMatrix(2, flandmark_instance->getLandmarksCount(), mxINT32_CLASS, mxREAL);
			int *out = (int*)mxGetData(plhs[1]);

			// output to MATLAB (1-based) => add 1
			for (int i = 0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
			{
				out[i] = outgt[i]+1;
			}

			plhs[2] = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
			double *p_double = (double*)mxGetData(plhs[2]);
			p_double[0] = flandmark_instance->getNormalizationFactor();

			delete [] ground_truth;
		} else {

			if (nlhs > 1)
			{
				mexErrMsgTxt("getNF: Cannot return ground truth in normalized frame, without providing ground truths in image.");
			}

			nf = flandmark_instance->getNF(&img, &bb[0]);
		}

		plhs[0] = nf->toMatlab(mxUINT8_CLASS);

		return;
	}

	// Set normalization factor
	//---------------------------------------------------------------------------------------------
	if (!strcmp("setNormalizationFactor", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 3)
		{
			mexErrMsgTxt("setNormalizationFactor: unexpected arguments.\n"
						 "Usage: \n"
						 "\t setNormalizationFactor(kappa); \n"
						 "Input: \n"
						 "\t kappa \t [1 x 1 (double)] number\n");
		}

		const mwSize *mx_dim = mxGetDimensions(prhs[2]);
#if DOUBLE_PRECISION==1
		if (mx_dim[0] != 1 || mx_dim[1] != 1 || mxGetClassID(prhs[2]) != mxDOUBLE_CLASS)
		{
			mexErrMsgTxt("setNormalizationFactor: unexpected arguments.\n"
						 "Usage: \n"
						 "\t setNormalizationFactor(kappa); \n"
						 "Input: \n"
						 "\t kappa \t [1 x 1 (double)] number\n");
		}
#else
		if (mx_dim[0] != 1 || mx_dim[1] != 1 || mxGetClassID(prhs[2]) != mxSINGLE_CLASS)
		{
			mexErrMsgTxt("setNormalizationFactor: unexpected arguments.\n"
						 "Usage: \n"
						 "\t setNormalizationFactor(kappa); \n"
						 "Input: \n"
						 "\t kappa \t [1 x 1 (single)] number\n");
		}
#endif
		fl_double_t *input = (fl_double_t*)mxGetData(prhs[2]);
		flandmark_instance->setNormalizationFactor(input[0]);

		return;
	}

	// Get W dim
	//---------------------------------------------------------------------------------------------
	if (!strcmp("getWdim", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("getWdim: unexpected arguments.\n"
						 "Usage: \n"
						 "\t getWdim(); \n");
		}

		int wdim = flandmark_instance->getWdimension();

		plhs[0] = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
		double *out = (double*)mxGetData(plhs[0]);
		out[0] = (double)wdim;

		return;
	}

	// Set W
	//---------------------------------------------------------------------------------------------
	if (!strcmp("setW", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 3)
		{
			mexErrMsgTxt("setW: unexpected arguments.\n"
						 "Usage: \n"
						 "\t setW(w); \n"
						 "Input: \n"
						 "\t w \t [N x 1 (double)] vector\n");
		}

		// Check input format
		const mwSize *mx_dim = mxGetDimensions(prhs[2]);
#if DOUBLE_PRECISION==1
		if (mx_dim[0] != flandmark_instance->getWdimension() || mx_dim[1] != 1 || mxGetClassID(prhs[2]) != mxDOUBLE_CLASS)
		{
			mexErrMsgTxt("setW: unexpected arguments.\n"
						 "Usage: \n"
						 "\t setW(w); \n"
						 "Input: \n"
						 "\t w \t [N x 1 (double)] vector\n");
		}
#else
		if (mx_dim[0] != flandmark_instance->getWdimension() || mx_dim[1] != 1 || mxGetClassID(prhs[2]) != mxSINGLE_CLASS)
		{
			mexErrMsgTxt("setW: unexpected arguments.\n"
						 "Usage: \n"
						 "\t setW(w); \n"
						 "Input: \n"
						 "\t w \t [N x 1 (single)] vector\n");
		}
#endif

		fl_double_t *input = (fl_double_t*)mxGetData(prhs[2]);
		flandmark_instance->setW(input);

		return;
	}

	// Get W
	//---------------------------------------------------------------------------------------------
	if (!strcmp("getW", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("getW: unexpected arguments.\n"
						 "Usage: \n"
						 "\t w = getW(); \n"
						 "Output: \n"
						 "\t w \t [N x 1 (double)] vector\n");
		}

		fl_double_t *W = flandmark_instance->getW();
#if DOUBLE_PRECISION==1
		plhs[0] = mxCreateNumericMatrix(flandmark_instance->getWdimension(), 1, mxDOUBLE_CLASS, mxREAL);
#else
		plhs[0] = mxCreateNumericMatrix(flandmark_instance->getWdimension(), 1, mxSINGLE_CLASS, mxREAL);
#endif
		fl_double_t *out_w = (fl_double_t*)mxGetData(plhs[0]);
		memcpy(out_w, W, flandmark_instance->getWdimension()*sizeof(fl_double_t));
		delete [] W;

		return;
	}

	// Get Psi base
	//---------------------------------------------------------------------------------------------
	if (!strcmp("getPsi_base", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 4)
		{
			mexErrMsgTxt("getPsi_base: unexpected arguments.\n"
						 "Usage: \n"
						 "\t psi = getPsi_base(img, configuration); \n"
						 "Input: \n"
						 "\t img \t [BW_H x BW_W (uint8)] matrix \n"
						 "\t configuration \t [2 X M (int32)] matrix \n"
						 "Output: \n"
						 "\t psi \t [N x 1 (double)] vector \n");
		}

		const mwSize *mx_dim = mxGetDimensions(prhs[3]);
		if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount() || mxGetClassID(prhs[3]) != mxINT32_CLASS)
		{
			mexErrMsgTxt("getPsi_base: wrong format of configuration!\n"
						 "Usage: \n"
						 "\t psi = getPsi_base(img, configuration); \n"
						 "Input: \n"
						 "\t img \t [BW_H x BW_W (uint8)] matrix \n"
						 "\t configuration \t [2 X M (int32)] matrix \n"
						 "Output: \n"
						 "\t psi \t [N x 1 (double)] vector \n");
		}

		int *input = (int*)mxGetData(prhs[3]);
		int *configuration = new int[2*flandmark_instance->getLandmarksCount()];

		// MATLAB input 1-based => subtract 1
		for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			configuration[i] = input[i]-1;
		}

		// get normalized frame from input
		cimg_library::CImg<unsigned char> img(prhs[2]);

		// check input
		cimg_library::CImg<unsigned char> *tmpNF = flandmark_instance->getNF();
		if (img.height() != tmpNF->height() || img.width() != tmpNF->width() || mxGetClassID(prhs[2]) != mxUINT8_CLASS)
		{
			mexErrMsgTxt("getPsi_base: wrong format of img!\n"
						 "Usage: \n"
						 "\t psi = getPsi_base(img, configuration); \n"
						 "Input: \n"
						 "\t img \t [BW_H x BW_W (uint8)] matrix \n"
						 "\t configuration \t [2 X M (int32)] matrix \n"
						 "Output: \n"
						 "\t psi \t [N x 1 (double)] vector \n");
		}

		// compute joint vector Psi dimension
		flandmark_instance->computeWdimension();

#if DOUBLE_PRECISION==1
		plhs[0] = mxCreateNumericMatrix(flandmark_instance->getWdimension(), 1, mxDOUBLE_CLASS, mxREAL);
#else
		plhs[0] = mxCreateNumericMatrix(flandmark_instance->getWdimension(), 1, mxSINGLE_CLASS, mxREAL);
#endif
		fl_double_t *out_psi = (fl_double_t*)mxGetData(plhs[0]);
		fl_double_t *psi;

		psi = flandmark_instance->getFeatures_base(&img, configuration);
		memcpy(out_psi, psi, flandmark_instance->getWdimension()*sizeof(fl_double_t));

		delete [] configuration;
//        delete img;

		return;
	}

	// Get Psi base optimized
	//---------------------------------------------------------------------------------------------
	if (!strcmp("getPsi_base_optimized", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 3)
		{
			mexErrMsgTxt("getPsi_base_optimized: unexpected arguments.\n"
						 "Usage: \n"
						 "\t psi = getPsi_base_optimized(configuration); \n"
						 "Input: \n"
						 "\t configuration \t [2 X M (int32)] matrix \n"
						 "Output: \n"
						 "\t psi \t [N x 1 (double)] vector \n");
		}

		const mwSize *mx_dim = mxGetDimensions(prhs[2]);
		if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount() || mxGetClassID(prhs[2]) != mxINT32_CLASS)
		{
			mexErrMsgTxt("getPsi_base_optimized: wrong format of configuration!\n"
						 "Usage: \n"
						 "\t psi = getPsi_base_optimized(configuration); \n"
						 "Input: \n"
						 "\t configuration \t [2 X M (int32)] matrix \n"
						 "Output: \n"
						 "\t psi \t [N x 1 (double)] vector \n");
		}

		int *input = (int*)mxGetData(prhs[2]);
		int *configuration = new int[2*flandmark_instance->getLandmarksCount()];

		// MATLAB input 1-based => subtract 1
		for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			configuration[i] = input[i]-1;
		}

		// compute joint vector Psi dimension
		flandmark_instance->computeWdimension();

#if DOUBLE_PRECISION==1
		plhs[0] = mxCreateNumericMatrix(flandmark_instance->getWdimension(), 1, mxDOUBLE_CLASS, mxREAL);
#else
		plhs[0] = mxCreateNumericMatrix(flandmark_instance->getWdimension(), 1, mxSINGLE_CLASS, mxREAL);
#endif
		fl_double_t *out_psi = (fl_double_t*)mxGetData(plhs[0]);
		fl_double_t *psi;

		psi = flandmark_instance->getFeatures_base_optimized(configuration);
		memcpy(out_psi, psi, flandmark_instance->getWdimension()*sizeof(fl_double_t));

		delete [] configuration;

		return;
	}

	// Get Psi
	//---------------------------------------------------------------------------------------------
	if (!strcmp("getPsi", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs < 3 || nrhs > 5)
			mexErrMsgTxt("getPsi: unexpected arguments.\n"
				"Usage: \n"
				"\t psi = getPsi(configuration, [img, bbox]);\n"
				"Input:\n"
				"\t configuration \t [2 x M (int32)] matrix\n"
				"\t img \t [H x W (uint8)] matrix\n"
				"\t bbox \t [1 x 4 (int32)] vector\n"
				"Output:\n"
				"\t psi \t [N x 1 (double)] vector\n");

		const mwSize *mx_dim = mxGetDimensions(prhs[2]);
		if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount() || mxGetClassID(prhs[2]) != mxINT32_CLASS)
		{
			mexErrMsgTxt("getPsi: wrong format of configuration!\n"
				"Usage: \n"
				"\t psi = getPsi(configuration, [img, bbox]);\n"
				"Input:\n"
				"\t configuration \t [2 x M (int32)] matrix\n"
				"\t img \t [H x W (uint8)] matrix\n"
				"\t bbox \t [1 x 4 (int32)] vector\n"
				"Output:\n"
				"\t psi \t [N x 1 (double)] vector\n");
		}

		// compute joint vector Psi dimension
		flandmark_instance->computeWdimension();

#if DOUBLE_PRECISION==1
		plhs[0] = mxCreateNumericMatrix(flandmark_instance->getWdimension(), 1, mxDOUBLE_CLASS, mxREAL);
#else
		plhs[0] = mxCreateNumericMatrix(flandmark_instance->getWdimension(), 1, mxSINGLE_CLASS, mxREAL);
#endif
		fl_double_t *out_psi = (fl_double_t*)mxGetData(plhs[0]);
		fl_double_t *psi;

		int *input = (int*)mxGetData(prhs[2]);
		int *configuration = new int[2*flandmark_instance->getLandmarksCount()];

		// MATLAB input 1-based => subtract 1
		for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			configuration[i] = input[i]-1;
		}

		if (nrhs == 5)
		{
			cimg_library::CImg<unsigned char> img(prhs[3]);

			const mwSize *mx_dim = mxGetDimensions(prhs[4]);
			if ((mx_dim[0]*mx_dim[1] != 4 && mx_dim[0]*mx_dim[1] != 8) || mxGetClassID(prhs[4]) != mxINT32_CLASS)
			{
				mexErrMsgTxt("getPsi: wrong format of bbox.\n"
					"Usage: \n"
					"\t psi = getPsi(configuration, [img, bbox]);\n"
					"Input:\n"
					"\t configuration \t [2 x M (int32)] matrix\n"
					"\t img \t [H x W (uint8)] matrix\n"
					 "\t bbox \t [1 x 4 (int32)] or [1 x 8 (int32)]	vector\n"
					"Output:\n"
					"\t psi \t [N x 1 (double)] vector\n");
			}

			int *bbox = (int*)mxGetData(prhs[4]);

			bool isFullBox = false;
			int bb[8];

			if (mx_dim[0]*mx_dim[1] == 4)
			{
				// MATLAB input 1-based => subtract 1
				//int bb[4] = {bbox[0]-1, bbox[1]-1, bbox[2]-1, bbox[3]-1};
				bb[0] = bbox[0]-1;
				bb[1] = bbox[1]-1;
				bb[2] = bbox[2]-1;
				bb[3] = bbox[3]-1;
				isFullBox = false;
			} else {
				// MATLAB input 1-based => subtract 1
				bb[0] = bbox[0]-1; bb[1] = bbox[1]-1;
				bb[2] = bbox[2]-1; bb[3] = bbox[3]-1;
				bb[4] = bbox[4]-1; bb[5] = bbox[5]-1;
				bb[6] = bbox[6]-1; bb[7] = bbox[7]-1;
				isFullBox = true;
			}

			if (!isFullBox)
			{
				bb[7] = bb[3];
				bb[6] = bb[0];
				bb[5] = bb[3];
				bb[4] = bb[2];
				bb[3] = bb[1];
			}

			psi = flandmark_instance->getFeatures(&img, &bb[0], configuration);

//            delete img;
		} else {
			psi = flandmark_instance->getFeatures(configuration);
		}

		delete [] configuration;

		memcpy(out_psi, psi, flandmark_instance->getWdimension()*sizeof(fl_double_t));

		return;
	}

	// Write
	//---------------------------------------------------------------------------------------------
	if (!strcmp("write", cmd))
	{
		// Check parameters
		//if (nlhs < 0 || nrhs != 3)
		if (nlhs < 0 || nrhs < 3 || nrhs > 4)
		{
			mexErrMsgTxt("write: unexpected arguments.");
		}

		char * filename = mxArrayToString(prhs[2]);
		if (filename == NULL)
		{
			mexErrMsgTxt("write: string input expected.\n");
		}

		bool writeW = true;

		if (nrhs == 4)
		{
			writeW = (bool)mxGetData(prhs[3]);
		}

		flandmark_instance->write(filename, writeW);

		return;
	}

	// SetTableLoss
	//---------------------------------------------------------------------------------------------
	if (!strcmp("setTableLoss", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs  < 3 || nrhs > 4)
		{
			mexErrMsgTxt("setTableLoss: unexpected arguments.\n"
				"Usage: \n"
				"\t setTableLoss(loss_tables);\n"
				"Input:\n"
				"\t loss_tables \t [1 x M (cell)] cell array\n");
		}

		mxArray *loss_cell_ptr;
		const mxArray *cell_ptr = prhs[2];

		// Check input
		if (!mxIsCell(cell_ptr))
		{
			mexErrMsgTxt("setTableLoss: wrong input.\n"
				"Usage: \n"
				"\t setTableLoss(loss_tables);\n"
				"Input:\n"
				"\t loss_tables \t [1 x M (cell)] cell array\n");
		}

		// Check dimensions
		const mwSize *dims = mxGetDimensions(cell_ptr);
		const int * bw = flandmark_instance->getBaseWindowSize();
		fl_double_t *loss = 0x0;

		if (dims[0]*dims[1] != flandmark_instance->getLandmarksCount())
		{
			mexErrMsgTxt("setTableLoss: wrong input (2).\n"
				"Usage: \n"
				"\t setTableLoss(loss_tables);\n"
				"Input:\n"
				"\t loss_tables \t [1 x M (cell)] cell array\n");
		}

		// Parse matlab input and call setLossTable method from Flandmark.cpp
		for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
		{
			loss_cell_ptr = mxGetCell(cell_ptr, i);
			dims = mxGetDimensions(loss_cell_ptr);

			if (dims[0] != bw[1] || dims[1] != bw[0])
			{
				mexPrintf("DEBUG: dims[0] (%d) != bw[1] (%d) || dims[1] (%d) != bw[0] (%d) \n", dims[0], bw[1], dims[1], bw[1]);
				mexErrMsgTxt("setTableLoss: wrong input (3).\n"
					"Usage: \n"
					"\t setTableLoss(loss_tables);\n"
					"Input:\n"
					"\t loss_tables \t [1 x M (cell)] cell array\n");
			}

			loss = (fl_double_t*)mxGetData(loss_cell_ptr);

			// Check if applicable
			if (flandmark_instance->nodeHasLoss(i))
			{
				flandmark_instance->setLossTable(loss, i);
			} else {
				// TODO: decide what will be better here (error or ignore?)!!!
				mexPrintf("setTableLoss: node ID %d cannot have loss table applied!\n", i);
				mexErrMsgTxt("setTableLoss: not applicable!\n");
			}
		}

		return;
	}

	// getNodesSearchSpaces
	//---------------------------------------------------------------------------------------------
	if (!strcmp("getNodesSearchSpaces", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("getNodesSearchSpaces: unexpected arguments.\n"
						 "Usage: \n"
						 "\t ss = getNodesSearchSpaces(); \n"
						 "Output: \n"
						 "\t ss \t [4 x N (double)] vector\n");
		}

		// Get nodes search spaces
		plhs[0] = mxCreateNumericMatrix(4, flandmark_instance->getLandmarksCount(), mxDOUBLE_CLASS, mxREAL);
		double *output = (double*)mxGetData(plhs[0]);

		int *ss = flandmark_instance->getNodesSearchSpaces();

		for (unsigned int i=0; i < 4*flandmark_instance->getLandmarksCount(); ++i)
		{
			output[i] = (double)ss[i];
		}

		delete [] ss;

		return;
	}

	// getNodesWindowSizes
	//---------------------------------------------------------------------------------------------
	if (!strcmp("getNodesWindowSizes", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("getNodesWindowSizes: unexpected arguments.\n"
						 "Usage: \n"
						 "\t wins = getNodesWindowSizes(); \n"
						 "Output: \n"
						 "\t wins \t [2 x N (double)] vector\n");
		}

		// Get nodes search spaces
		plhs[0] = mxCreateNumericMatrix(2, flandmark_instance->getLandmarksCount(), mxDOUBLE_CLASS, mxREAL);
		double *output = (double*)mxGetData(plhs[0]);

		int *wins = flandmark_instance->getWindowSizes();

		for (unsigned int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			output[i] = (double)wins[i];
		}

		delete [] wins;

		return;
	}

	// getBWsize
	//---------------------------------------------------------------------------------------------
	if (!strcmp("getBWsize", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("getBWsize: unexpected arguments.\n"
						 "Usage: \n"
						 "\t ss = getBWsize(); \n"
						 "Output: \n"
						 "\t bw \t [2 x 1 (double)] vector\n");
		}

		// Get nodes search spaces
		plhs[0] = mxCreateNumericMatrix(2, 1, mxDOUBLE_CLASS, mxREAL);
		double *output = (double*)mxGetData(plhs[0]);

		const int *bw = flandmark_instance->getBaseWindowSize();
		output[0] = bw[0];
		output[1] = bw[1];

		return;
	}

	// Get Landmarks Count dim
	//---------------------------------------------------------------------------------------------
	if (!strcmp("getLandmarksCount", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("getLandmarksCount: unexpected arguments.\n"
						 "Usage: \n"
						 "\t getLandmarksCount(); \n");
		}

		int landmarksCnt = flandmark_instance->getLandmarksCount();

		plhs[0] = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
		double *out = (double*)mxGetData(plhs[0]);
		out[0] = (double)landmarksCnt;

		return;
	}

	// Get Has Loss info
	if (!strcmp("hasLoss", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("hasLoss: unexpected arguments.\n"
						 "Usage: \n"
						 "losses = hasLoss(); \n"
						 "Output: \n"
						 "\t losses [(n x 1) logical] array with flags whether the node has loss or not.\n");
		}

		plhs[0] = mxCreateLogicalMatrix(flandmark_instance->getLandmarksCount(), 1);
		bool *out = (bool*)mxGetData(plhs[0]);

		for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
		{
			out[i] = flandmark_instance->nodeHasLoss(i);
		}

		return;
	}

	//=============================================================================================
	// Two stage learning functions
	//---------------------------------------------------------------------------------------------

	// ArgmaxNodes
	//---------------------------------------------------------------------------------------------
	if (!strcmp("argmaxNodes_base", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs < 3 || nrhs > 4)
		{
			mexErrMsgTxt("argmaxNodes_base: unexpected arguments.");
		}

		cimg_library::CImg<unsigned char> img(prhs[2]);

		int *nodes = 0;

		plhs[0] = mxCreateNumericMatrix(2, flandmark_instance->getLandmarksCount(), mxINT32_CLASS, mxREAL);
		int *output = (int*)mxGetData(plhs[0]);

		if (nrhs == 4)
		{
			const mwSize *mx_dim = mxGetDimensions(prhs[3]);
			if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount())
			{
				mexErrMsgTxt("argmaxNodes_base: ground_truth must be [2 x LandmarksCount (int32)] vector");
			}

			int *gt = (int*)mxGetData(prhs[3]);

			// MATLAB input 1-based => subtract 1
			int *ground_truth = new int[2*flandmark_instance->getLandmarksCount()];
			for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
				ground_truth[i] = gt[i]-1;

			flandmark_instance->nodemax_base(&img, ground_truth);

			delete [] ground_truth;
		} else {
			flandmark_instance->nodemax_base(&img);
		}

		nodes = flandmark_instance->getLandmarksNF();

		// output to MATLAB (1-based) => add 1
		for (int i = 0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			output[i] = nodes[i]+1;
		}

		return;
	}

	// Get Nodes Dimendions
	//---------------------------------------------------------------------------------------------
	if (!strcmp("getNodesDimensions", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("getNodesDimensions: unexpected arguments.\n"
						 "Usage: \n"
						 "\t getPsiNodesDimension(); \n");
		}

//		int psiNodesDimension = flandmark_instance->getPsiNodesDimension();

		plhs[0] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount(), 1, mxDOUBLE_CLASS, mxREAL);
		double *out = (double*)mxGetData(plhs[0]);
//		out[0] = (double)psiNodesDimension;

		int *dimensions = flandmark_instance->getNodesDimensions();

		for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
		{
			out[i] = (double)dimensions[i];
		}

		delete [] dimensions;

		return;
	}

	// Get Edges Dimendions
	//---------------------------------------------------------------------------------------------
	if (!strcmp("getEdgesDimensions", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("getEdgesDimensions: unexpected arguments.\n"
						 "Usage: \n"
						 "\t edges_dim = getEdgesDimensions(); \n"
						 "Output: \n"
						 "\t edges_dim \t [n x 1 (double)] \n");
		}

		plhs[0] = mxCreateNumericMatrix(flandmark_instance->getEdgesCount(), 1, mxDOUBLE_CLASS, mxREAL);
		double *out = (double*)mxGetData(plhs[0]);

		int *dimensions = flandmark_instance->getEdgesDimensions();

		for (int i=0; i < flandmark_instance->getEdgesCount(); ++i)
		{
			out[i] = (double)dimensions[i];
		}

		delete [] dimensions;

		return;
	}

	// Get PsiNodes dim
	//---------------------------------------------------------------------------------------------
	if (!strcmp("getPsiNodesDimension", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("getPsiNodesDimension: unexpected arguments.\n"
						 "Usage: \n"
						 "\t getPsiNodesDimension(); \n");
		}

		int psiNodesDimension = flandmark_instance->getPsiNodesDimension();

		plhs[0] = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
		double *out = (double*)mxGetData(plhs[0]);
		out[0] = (double)psiNodesDimension;

		return;
	}

	// Get PsiEdges dim
	//---------------------------------------------------------------------------------------------
	if (!strcmp("getPsiEdgesDimension", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("getPsiEdgesDimension: unexpected arguments.\n"
						 "Usage: \n"
						 "\t getPsiEdgesDimension(); \n");
		}

		int psiEdgesDimension = flandmark_instance->getPsiEdgesDimension();

		plhs[0] = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
		double *out = (double*)mxGetData(plhs[0]);
		out[0] = (double)psiEdgesDimension;

		return;
	}

	// Set Nodes W
	//---------------------------------------------------------------------------------------------
	if (!strcmp("setWnodes", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 3)
		{
			mexErrMsgTxt("setWnodes: unexpected arguments.\n"
						 "Usage: \n"
						 "\t setWnodes(w); \n"
						 "Input: \n"
						 "\t w \t [N x 1 (double)] vector\n");
		}

		// Check input format
		const mwSize *mx_dim = mxGetDimensions(prhs[2]);
#if DOUBLE_PRECISION==1
		if (mx_dim[0] != flandmark_instance->getPsiNodesDimension() || mx_dim[1] != 1 || mxGetClassID(prhs[2]) != mxDOUBLE_CLASS)
		{
			mexErrMsgTxt("setWnodes: unexpected arguments.\n"
						 "Usage: \n"
						 "\t setWnodes(w); \n"
						 "Input: \n"
						 "\t w \t [N x 1 (double)] vector\n");
		}
#else
		if (mx_dim[0] != flandmark_instance->getPsiNodesDimension() || mx_dim[1] != 1 || mxGetClassID(prhs[2]) != mxSINGLE_CLASS)
		{
			mexErrMsgTxt("setWnodes: unexpected arguments.\n"
						 "Usage: \n"
						 "\t setWnodes(w); \n"
						 "Input: \n"
						 "\t w \t [N x 1 (single)] vector\n");
		}
#endif

		fl_double_t *input = (fl_double_t*)mxGetData(prhs[2]);
		flandmark_instance->setNodesW(input);

		return;
	}

	// Get PsiNodes base
	//---------------------------------------------------------------------------------------------
	if (!strcmp("getPsiNodes_base", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 4)
		{
			mexErrMsgTxt("getPsiNodes_base: unexpected arguments.\n"
						 "Usage: \n"
						 "\t psiNodes = getPsiNodes_base(img, configuration); \n"
						 "Input: \n"
						 "\t img \t [BW_H x BW_W (uint8)] matrix \n"
						 "\t configuration \t [2 X M (int32)] matrix \n"
						 "Output: \n"
						 "\t psiNodes \t [N x 1 (double)] vector \n");
		}

		const mwSize *mx_dim = mxGetDimensions(prhs[3]);
		if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount() || mxGetClassID(prhs[3]) != mxINT32_CLASS)
		{
			mexErrMsgTxt("getPsiNodes_base: wrong format of configuration!\n"
						 "Usage: \n"
						 "\t psiNodes = getPsiNodes_base(img, configuration); \n"
						 "Input: \n"
						 "\t img \t [BW_H x BW_W (uint8)] matrix \n"
						 "\t configuration \t [2 X M (int32)] matrix \n"
						 "Output: \n"
						 "\t psiNodes \t [N x 1 (double)] vector \n");
		}

		int *input = (int*)mxGetData(prhs[3]);
		int *configuration = new int[2*flandmark_instance->getLandmarksCount()];

		// MATLAB input 1-based => subtract 1
		for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			configuration[i] = input[i]-1;
		}

		// get normalized frame from input
		cimg_library::CImg<unsigned char> img(prhs[2]);

		// check input
		cimg_library::CImg<unsigned char> *tmpNF = flandmark_instance->getNF();
		if (img.height() != tmpNF->height() || img.width() != tmpNF->width() || mxGetClassID(prhs[2]) != mxUINT8_CLASS)
		{
			mexErrMsgTxt("getPsiNodes_base: wrong format of img!\n"
						 "Usage: \n"
						 "\t psiNodes = getPsiNodes_base(img, configuration); \n"
						 "Input: \n"
						 "\t img \t [BW_H x BW_W (uint8)] matrix \n"
						 "\t configuration \t [2 X M (int32)] matrix \n"
						 "Output: \n"
						 "\t psiNodes \t [N x 1 (double)] vector \n");
		}

		// compute joint vector Psi dimension
//		flandmark_instance->computeWdimension();	// <= this is not necessary, since wDimension is computed in constructor

#if DOUBLE_PRECISION==1
		plhs[0] = mxCreateNumericMatrix(flandmark_instance->getPsiNodesDimension(), 1, mxDOUBLE_CLASS, mxREAL);
#else
		plhs[0] = mxCreateNumericMatrix(flandmark_instance->getPsiNodesDimension(), 1, mxSINGLE_CLASS, mxREAL);
#endif
		fl_double_t *out_psi = (fl_double_t*)mxGetData(plhs[0]);
		fl_double_t *psi;

		psi = flandmark_instance->getFeatures_base(&img, configuration);
		memcpy(out_psi, psi, flandmark_instance->getPsiNodesDimension()*sizeof(fl_double_t));

		delete [] configuration;

		return;
	}

	// Get PsiNodes
	//---------------------------------------------------------------------------------------------
	if (!strcmp("getPsiNodes", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs < 3 || nrhs > 5)
			mexErrMsgTxt("getPsiNodes: unexpected arguments.\n"
				"Usage: \n"
				"\t psiNodes = getPsiNodes(configuration, [img, bbox]);\n"
				"Input:\n"
				"\t configuration \t [2 x M (int32)] matrix\n"
				"\t img \t [H x W (uint8)] matrix\n"
				"\t bbox \t [1 x 4 (int32)] vector\n"
				"Output:\n"
				"\t psiNodes \t [N x 1 (double)] vector\n");

		const mwSize *mx_dim = mxGetDimensions(prhs[2]);
		if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount() || mxGetClassID(prhs[2]) != mxINT32_CLASS)
		{
			mexErrMsgTxt("getPsiNodes: wrong format of configuration!\n"
				"Usage: \n"
				"\t psiNodes = getPsiNodes(configuration, [img, bbox]);\n"
				"Input:\n"
				"\t configuration \t [2 x M (int32)] matrix\n"
				"\t img \t [H x W (uint8)] matrix\n"
				"\t bbox \t [1 x 4 (int32)] vector\n"
				"Output:\n"
				"\t psiNodes \t [N x 1 (double)] vector\n");
		}

		// compute joint vector Psi dimension
//		flandmark_instance->computeWdimension();	// <= not needed, done in constructor

#if DOUBLE_PRECISION==1
		plhs[0] = mxCreateNumericMatrix(flandmark_instance->getPsiNodesDimension(), 1, mxDOUBLE_CLASS, mxREAL);
#else
		plhs[0] = mxCreateNumericMatrix(flandmark_instance->getPsiNodesDimension(), 1, mxSINGLE_CLASS, mxREAL);
#endif
		fl_double_t *out_psi = (fl_double_t*)mxGetData(plhs[0]);
		fl_double_t *psi;

		int *input = (int*)mxGetData(prhs[2]);
		int *configuration = new int[2*flandmark_instance->getLandmarksCount()];

		// MATLAB input 1-based => subtract 1
		for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			configuration[i] = input[i]-1;
		}

		if (nrhs == 5)
		{
			cimg_library::CImg<unsigned char> img(prhs[3]);

			const mwSize *mx_dim = mxGetDimensions(prhs[4]);
			if (mx_dim[0]*mx_dim[1] != 4 || mxGetClassID(prhs[4]) != mxINT32_CLASS)
			{
				mexErrMsgTxt("getPsiNodes: wrong format of bbox.\n"
					"Usage: \n"
					"\t psiNodes = getPsiNodes(configuration, [img, bbox]);\n"
					"Input:\n"
					"\t configuration \t [2 x M (int32)] matrix\n"
					"\t img \t [H x W (uint8)] matrix\n"
					"\t bbox \t [1 x 4 (int32)]	vector\n"
					"Output:\n"
					"\t psiNodes \t [N x 1 (double)] vector\n");
			}

			int *bbox = (int*)mxGetData(prhs[4]);

			// MATLAB input 1-based => subtract 1
			int bb[4] = {bbox[0]-1, bbox[1]-1, bbox[2]-1, bbox[3]-1};

			psi = flandmark_instance->getFeatures(&img, &bb[0], configuration);

		} else {
			psi = flandmark_instance->getFeatures(configuration);
		}

		delete [] configuration;

		memcpy(out_psi, psi, flandmark_instance->getPsiNodesDimension()*sizeof(fl_double_t));

		return;
	}

	// GetLossAt
	//==================================================================================================================
	if (!strcmp("getLossAt", cmd))
	{
		// TODO: input output check

		int *p = (int*)mxGetData(prhs[2]);

		//DEBUG
		for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			mexPrintf("D: P[%d] = %d \n", i, p[i]);
		}
		//=====

		// MATLAB input 1-based => subtract 1
		int *position = new int[2*flandmark_instance->getLandmarksCount()];
		for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			position[i] = p[i]-1;
		}

		//DEBUG
		for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			mexPrintf("D: P[%d] = %d \n", i, position[i]);
		}
		//=====

#if DOUBLE_PRECISION==1
		plhs[0] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount(), 1, mxDOUBLE_CLASS, mxREAL);
#else
		plhs[0] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount(), 1, mxSINGLE_CLASS, mxREAL);
#endif
		fl_double_t *losses = (fl_double_t*)mxGetData(plhs[0]);

		fl_double_t *L = flandmark_instance->getLossValues(position);

		for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
		{
			losses[i] = L[i];

			// DEBUG
			mexPrintf("L[%d] = %f \n", i, L[i]);
			//-=====
		}

		return;
	}

	// Get Timings Stats
	//=============================================================================================
	if (!strcmp("getTimingsStats", cmd))
	{
		const char *field_names[] = {"overall", "normalizedFrame", "features", "maxsum"};

		Timings timings = flandmark_instance->timings;

		mwSize dims[2] = {1, 1};

		plhs[0] = mxCreateStructArray(1, dims, 4, field_names);

		mxSetField(plhs[0], 0, "overall", mxCreateDoubleScalar((double)timings.overall));
		mxSetField(plhs[0], 0, "normalizedFrame", mxCreateDoubleScalar((double)timings.normalizedFrame));
		mxSetField(plhs[0], 0, "features", mxCreateDoubleScalar((double)timings.features));
		mxSetField(plhs[0], 0, "maxsum", mxCreateDoubleScalar((double)timings.maxsum));

		return;
	}

	/// SPEED UP functions ========================================================================

	// --------------------------------------------------------------------------------------------

	if (!strcmp("setFeaturesPool", cmd))
	{
		// TODO: implement

		// Check parameters
		if (nlhs < 0 || nrhs != 3)
		{
			mexErrMsgTxt("setFeaturesPool: unexpected arguments.");
		}

		CFeaturePool *featurePool = convertMat2Ptr< CFeaturePool >(prhs[2]);

		flandmark_instance->setNFfeaturesPool(featurePool);

		return;
	}

	///--------------------------------------------------------------------------------------------

	if (!strcmp("detectBaseOptimized", cmd))
	{
		// TODO: Implement

		// Check parameters
		if (nlhs < 0 || nrhs < 3 || nrhs > 4)
		{
			mexErrMsgTxt("detectBaseOptimized: unexpected arguments.");
		}

		cimg_library::CImg<unsigned char> img(prhs[2]);

		int *landmarks = 0;

		plhs[0] = mxCreateNumericMatrix(2, flandmark_instance->getLandmarksCount(), mxINT32_CLASS, mxREAL);
		int *output = (int*)mxGetData(plhs[0]);

		if (nrhs == 4)
		{
			const mwSize *mx_dim = mxGetDimensions(prhs[3]);
			if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount())
			{
				mexErrMsgTxt("detectBaseOptimized: ground_truth must be [2 x LandmarksCount (int32)] vector");
			}

			int *gt = (int*)mxGetData(prhs[3]);

			// MATLAB input 1-based => subtract 1
			int *ground_truth = new int[2*flandmark_instance->getLandmarksCount()];
			for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
				ground_truth[i] = gt[i]-1;

			flandmark_instance->detect_base_optimized(&img, ground_truth);

			delete [] ground_truth;
		} else {

			flandmark_instance->detect_base_optimized(&img);
		}

		landmarks = flandmark_instance->getLandmarksNF();

		// output to MATLAB (1-based) => add 1
		for (int i = 0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			output[i] = landmarks[i]+1;
		}

		if (nlhs > 1)
		{
			fl_double_t *dotProdVal = 0x0;
#if DOUBLE_PRECISION==1
			plhs[1] = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
#else
			plhs[1] = mxCreateNumericMatrix(1, 1, mxSINGLE_CLASS, mxREAL);
#endif
			dotProdVal = (fl_double_t*)mxGetData(plhs[1]);
			(*dotProdVal) = 0.0;

			fl_double_t *Q = flandmark_instance->getQvalues();
			fl_double_t *G = flandmark_instance->getGvalues();

			for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
			{
				(*dotProdVal) += Q[i];
			}

			for (int i=0; i < flandmark_instance->getEdgesCount(); ++i)
			{
				(*dotProdVal) += G[i];
			}

			// Get joint vector [Q's, G's]
			if (nlhs > 2)
			{
#if DOUBLE_PRECISION==1
				plhs[2] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount()+flandmark_instance->getEdgesCount(), 1, mxDOUBLE_CLASS, mxREAL);
#else
				plhs[2] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount()+flandmark_instance->getEdgesCount(), 1, mxSINGLE_CLASS, mxREAL);
#endif
				fl_double_t *jointVect = (fl_double_t*)mxGetData(plhs[2]);

				// Q's
				for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
				{
					jointVect[i] = Q[i];
				}

				// G's
				for (int i=0; i < flandmark_instance->getEdgesCount(); ++i)
				{
					jointVect[flandmark_instance->getLandmarksCount()+i] = G[i];
				}
			}

			if (nlhs > 3)
			{
#if DOUBLE_PRECISION==1
				plhs[3] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount(), 1, mxDOUBLE_CLASS, mxREAL);
#else
				plhs[3] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount(), 1, mxSINGLE_CLASS, mxREAL);
#endif
				fl_double_t *losses = (fl_double_t*)mxGetData(plhs[3]);

				fl_double_t *L = flandmark_instance->getLossValues(flandmark_instance->getLandmarksNF());

				for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
				{
					losses[i] = L[i];
				}
			}
		}

		return;
	}

	// --------------------------------------------------------------------------------------------------------------

	if (!strcmp("detectBaseOptimizedFromPool", cmd))
	{
		// TODO: implement

		// Check parameters
		if (nlhs < 0 || nrhs < 2 || nrhs > 3)
		{
			mexErrMsgTxt("detectBaseOptimizedFromPool: unexpected arguments.");
		}

		int *landmarks = 0;

		plhs[0] = mxCreateNumericMatrix(2, flandmark_instance->getLandmarksCount(), mxINT32_CLASS, mxREAL);
		int *output = (int*)mxGetData(plhs[0]);

		if (nrhs == 3)
		{
			const mwSize *mx_dim = mxGetDimensions(prhs[2]);
			if (mx_dim[0] != 2 || mx_dim[1] != flandmark_instance->getLandmarksCount())
			{
				mexErrMsgTxt("detectBaseOptimizedFromPool: ground_truth must be [2 x LandmarksCount (int32)] vector");
			}

			int *gt = (int*)mxGetData(prhs[2]);

			// MATLAB input 1-based => subtract 1
			int *ground_truth = new int[2*flandmark_instance->getLandmarksCount()];
			for (int i=0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
				ground_truth[i] = gt[i]-1;

			flandmark_instance->detect_base_optimized(ground_truth);

			delete [] ground_truth;
		} else {
			flandmark_instance->detect_base_optimized();
		}

		landmarks = flandmark_instance->getLandmarksNF();

		// output to MATLAB (1-based) => add 1
		for (int i = 0; i < 2*flandmark_instance->getLandmarksCount(); ++i)
		{
			output[i] = landmarks[i]+1;
		}

		if (nlhs > 1)
		{
			fl_double_t *dotProdVal = 0x0;
#if DOUBLE_PRECISION==1
			plhs[1] = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
#else
			plhs[1] = mxCreateNumericMatrix(1, 1, mxSINGLE_CLASS, mxREAL);
#endif
			dotProdVal = (fl_double_t*)mxGetData(plhs[1]);
			(*dotProdVal) = 0.0;

			fl_double_t *Q = flandmark_instance->getQvalues();
			fl_double_t *G = flandmark_instance->getGvalues();

			for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
			{
				(*dotProdVal) += Q[i];
			}

			for (int i=0; i < flandmark_instance->getEdgesCount(); ++i)
			{
				(*dotProdVal) += G[i];
			}

			// Get joint vector [Q's, G's]
			if (nlhs > 2)
			{
#if DOUBLE_PRECISION==1
				plhs[2] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount()+flandmark_instance->getEdgesCount(), 1, mxDOUBLE_CLASS, mxREAL);
#else
				plhs[2] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount()+flandmark_instance->getEdgesCount(), 1, mxSINGLE_CLASS, mxREAL);
#endif
				fl_double_t *jointVect = (fl_double_t*)mxGetData(plhs[2]);

				// Q's
				for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
				{
					jointVect[i] = Q[i];
				}

				// G's
				for (int i=0; i < flandmark_instance->getEdgesCount(); ++i)
				{
					jointVect[flandmark_instance->getLandmarksCount()+i] = G[i];
				}
			}

			if (nlhs > 3)
			{
#if DOUBLE_PRECISION==1
				plhs[3] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount(), 1, mxDOUBLE_CLASS, mxREAL);
#else
				plhs[3] = mxCreateNumericMatrix(flandmark_instance->getLandmarksCount(), 1, mxSINGLE_CLASS, mxREAL);
#endif
				fl_double_t *losses = (fl_double_t*)mxGetData(plhs[3]);

				fl_double_t *L = flandmark_instance->getLossValues(flandmark_instance->getLandmarksNF());

				for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
				{
					losses[i] = L[i];
				}
			}
		}

		return;
	}

	/// DEBUG & HELPER functions ========================================================================

	if (!strcmp("getIntermediateResults", cmd))
	{
		//TODO!!

		// Check the parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("getIntermediateResults: unexpected arguments.\n"
						 "Usage: \n"
						 "\t A = getIntermediateResults();\n"
					);
		}

		int M = flandmark_instance->getLandmarksCount();
		std::vector< fl_double_t* > intermediateResults = flandmark_instance->getIntermediateResults();

		mxArray *arr;
		fl_double_t *p_data;

		plhs[0] = mxCreateCellMatrix(M, 1);

		int ss[4];
		int siz[2];
		int length;

		for (int i=0; i < M; ++i)
		{
			const int *tmp = flandmark_instance->getSearchSpace(i);
			memcpy(&ss[0], tmp, 4*sizeof(int));
			siz[0] = ss[2]-ss[0]+1;
			siz[1] = ss[3]-ss[1]+1;
			length = siz[0]*siz[1];

#if DOUBLE_PRECISION==1
			arr = mxCreateNumericMatrix(siz[1], siz[0], mxDOUBLE_CLASS, mxREAL);
#else
			arr = mxCreateNumericMatrix(siz[1], siz[0], mxSINGLE_CLASS, mxREAL);
#endif
			p_data = (fl_double_t*)mxGetData(arr);
			memcpy(p_data, intermediateResults.at(i), (length)*sizeof(fl_double_t));
			mxSetCell(plhs[0], i, arr);
		}

		return;
	}

	if (!strcmp("getH", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("getH: unexpected arguments.\n"
						 "Usage:\n"
						 "\t H = getH(); \n"
					);
		}

		fl_double_t * H = flandmark_instance->getH();

		// we do not need to consider fl_double_t here (it might cause troubles in MATLAB - incompatible types operations, etc. )
		plhs[0] = mxCreateNumericMatrix(3, 3, mxDOUBLE_CLASS, mxREAL);

		double *out = (double*)mxGetData(plhs[0]);

		for (int i=0; i < 9; ++i)
		{
			out[i] = (double)H[i];
		}

		return;
	}

	//-------------------------------------------------------------------------------------------------------

	if (!strcmp("getHinv", cmd))
	{
		// Check parameters
		if (nlhs <0 || nrhs != 2)
		{
			mexErrMsgTxt("getH: unexpected arguments.\n"
						 "Usage:\n"
						 "\t Hinv = getHinv(); \n"
					);
		}

		fl_double_t * Hinv = flandmark_instance->getHinv();

		// we do not need to consider fl_double_t here (it might cause troubles in MATLAB - incompatible types operations, etc. )
		plhs[0] = mxCreateNumericMatrix(3, 3, mxDOUBLE_CLASS, mxREAL);

		double *out = (double*)mxGetData(plhs[0]);

		for (int i=0; i < 9; ++i)
		{
			out[i] = (double)Hinv[i];
		}

		return;
	}

	//-------------------------------------------------------------------------------------------------------

	if (!strcmp("getEdgesCount", cmd))
	{
		// Check parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("getEdgesCount: unexpected arguments.\n"
						 "Usage: \n"
						 "\t count = getEdgesCount(); \n");
		}

		int edgesCnt = flandmark_instance->getEdgesCount();

		plhs[0] = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
		double *out = (double*)mxGetData(plhs[0]);
		out[0] = (double)edgesCnt;

		return;
	}

	//---------------------------------------------------------------------------------------------------
	if (!strcmp("getWnodes", cmd))
	{
		// Create cell array with Wq for individual nodes

		// Check parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("getWnodes: unexpected arguments.\n"
						 "Usage: \n"
						 "\t w_q = getWnodes(); \n"
						 "Output: \n"
						 "\t w_q \t [1 x num_nodes (cell)] cell array with w_q^i\n");
		}

		int N = flandmark_instance->getLandmarksCount();

		plhs[0] = mxCreateCellMatrix(N, 1);

		int *dimensions = flandmark_instance->getNodesDimensions();

		fl_double_t *W = flandmark_instance->getW();

		int start = 0;
		int end = 0;

		mxArray *arr;
		fl_double_t *p_data;

		for (int i=0; i < N; ++i)
		{
			start = end;
			end += dimensions[i];

#if DOUBLE_PRECISION==1
			arr = mxCreateNumericMatrix(end-start, 1, mxDOUBLE_CLASS, mxREAL);
#else
			arr = mxCreateNumericMatrix(end-start, 1, mxSINGLE_CLASS, mxREAL);
#endif
			p_data = (fl_double_t*)mxGetData(arr);
			memcpy(p_data, &W[start], (end-start)*sizeof(fl_double_t));
			mxSetCell(plhs[0], i, arr);
		}

		delete [] dimensions;
		delete [] W;

		return;
	}

	//---------------------------------------------------------------------------------------------------
	if (!strcmp("getWedges", cmd))
	{
		// Create cell array with Wg for individual edges

		// Check parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("getWedges: unexpected arguments.\n"
						 "Usage: \n"
						 "\t w_g = getWedges(); \n"
						 "Output: \n"
						 "\t w_g \t [1 x num_edges (cell)] cell array with w_g^i\n");
		}

		int N = flandmark_instance->getEdgesCount();

		plhs[0] = mxCreateCellMatrix(N, 1);

		int *dimensions = flandmark_instance->getNodesDimensions();
		int WedgeOffset = 0;
		for (int i=0; i < flandmark_instance->getLandmarksCount(); ++i)
		{
			WedgeOffset += dimensions[i];
		}
		delete [] dimensions;

		dimensions = flandmark_instance->getEdgesDimensions();
		fl_double_t *W = flandmark_instance->getW();

		int start = WedgeOffset;
		int end = WedgeOffset;

		mxArray *arr;
		fl_double_t *p_data;

		for (int i=0; i < N; ++i)
		{
			start = end;
			end += dimensions[i];

#if DOUBLE_PRECISION==1
			arr = mxCreateNumericMatrix(end-start, 1, mxDOUBLE_CLASS, mxREAL);
#else
			arr = mxCreateNumericMatrix(end-start, 1, mxSINGLE_CLASS, mxREAL);
#endif
			p_data = (fl_double_t*)mxGetData(arr);
			memcpy(p_data, &W[start], (end-start)*sizeof(fl_double_t));
			mxSetCell(plhs[0], i, arr);
		}

		delete [] dimensions;
		delete [] W;

		return;
	}

	//---------------------------------------------------------------------------------------------------
	if (!strcmp("getWoffsets", cmd))
	{
		// Create 1x2 cell with arrays of W offset values for both nodes & edges
		// Check parameters
		if (nlhs < 0 || nrhs != 2)
		{
			mexErrMsgTxt("getWedges: unexpected arguments.\n"
						 "Usage: \n"
						 "\t offsets = getWoffsets(); \n"
						 "Output: \n"
						 "\t offsets \t [1 x 2 (cell)] cell array with W offsets for nodes & edges.\n");
		}

		int V = flandmark_instance->getLandmarksCount();
		int E = flandmark_instance->getEdgesCount();

		plhs[0] = mxCreateCellMatrix(1, 2);

		mxArray *arr = 0x0;
		fl_double_t *p_data = 0x0;
		int *dimensions = 0x0;
		int Woffset = 0;

		// Nodes
#if DOUBLE_PRECISION==1
		arr = mxCreateNumericMatrix(V, 1, mxDOUBLE_CLASS, mxREAL);
#else
		arr = mxCreateNumericMatrix(V, 1, mxSINGLE_CLASS, mxREAL);
#endif
		mxSetCell(plhs[0], 0, arr);
		p_data = (fl_double_t*)mxGetData(arr);

		dimensions = flandmark_instance->getNodesDimensions();

		for (int i=0; i < V; ++i)
		{
			p_data[i] = Woffset+1;		// MATLAB 1-based indexing (+1)
			Woffset += dimensions[i];
		}

		delete [] dimensions;

		// Edges
#if DOUBLE_PRECISION==1
		arr = mxCreateNumericMatrix(E, 1, mxDOUBLE_CLASS, mxREAL);
#else
		arr = mxCreateNumericMatrix(E, 1, mxSINGLE_CLASS, mxREAL);
#endif
		mxSetCell(plhs[0], 1, arr);
		p_data = (fl_double_t*)mxGetData(arr);

		dimensions = flandmark_instance->getEdgesDimensions();

		for (int i=0; i < E; ++i)
		{
			p_data[i] = Woffset+1;		// MATLAB 1-based indexing (+1)
			Woffset += dimensions[i];
		}

		delete [] dimensions;

		return;
	}

	// DEBUG functions ============================================================================
	//---------------------------------------------------------------------------------------------
//	if (!strcmp("getNodeFeatures", cmd))
//	{
//		// Check parameters
//		if (nlhs < 0 || nrhs != 4)
//			mexErrMsgTxt("getFeatures: unexpected arguments.\n"
//				"Usage: \n"
//				"\t F = getFeatures(NF, node_id);\n"
//				"Input:\n"
//				"\t NF \t [BW_H x BW_W (uint8)] matrix\n"
//				"\t node_id \t (double) number\n"
//				"Output:\n"
//				"\t F \t [N x L (double)] vector\n");

//		// get normalized frame from input
//		cimg_library::CImg<unsigned char> img(prhs[2]);

//		// check input
//		cimg_library::CImg<unsigned char> *tmpNF = flandmark_instance->getNF();
//		if (img.height() != tmpNF->height() || img.width() != tmpNF->width() || mxGetClassID(prhs[2]) != mxUINT8_CLASS)
//		{
//			mexErrMsgTxt("getFeatures: wrong format of NF! \n"
//				"Usage: \n"
//				"\t F = getFeatures(NF, node_id);\n"
//				"Input:\n"
//				"\t NF \t [BW_H x BW_W (uint8)] matrix\n"
//				"\t node_id \t (double) number\n"
//				"Output:\n"
//				"\t F \t [N x L (double)] vector\n");
//		}

//		// Copy NF to flandmark
//		flandmark_instance->detect_base(&img);

//		// Check NF (save NF)
//		flandmark_instance->getNF()->save_ascii("D_NF.dat");

//		// get nodeID
//		double *p_double = (double*)mxGetData(prhs[3]);
//		int nodeID = (int)p_double[0] - 1;

//		mexPrintf("nodeID = %d\n", nodeID);

//		int dim = 0, length = 0;
//		int * LBPSparse = flandmark_instance->getNodeFeatures(nodeID, &dim, &length);

//		mexPrintf("dim = %d\nlength= %d\n", dim, length);

//		plhs[0] = mxCreateNumericMatrix(dim, length, mxINT32_CLASS, mxREAL);
//		int *out = (int*)mxGetData(plhs[0]);

//		// copy LBPsparse on output
//		for (int i = 0; i < dim*length; ++i)
//		{
//			out[i] = LBPSparse[i];
//		}

//		return;
//	}

	//------------------------------------------------------------------------------------------------------------------------

	// Got here, so command not recognized
	mexErrMsgTxt("Command not recognized.");
}

