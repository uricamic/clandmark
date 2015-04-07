/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2014, 2015 Michal Uricar
 * Copyright (C) 2014, 2015 Michal Uricar
 */

#include "mex.h"
#define cimg_plugin "cimgmatlab.h"

#include "flandmark_handle.h"
#include "CFeaturePool.h"
#include "CSparseLBPFeatures.h"

using namespace clandmark;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	// Get the command string
	char cmd[64];
	if (nrhs < 1 || mxGetString(prhs[0], cmd, sizeof(cmd)))
		mexErrMsgTxt("First input should be a command string less than 64 characters long.");

	// New
	//-----------------------------------------------------------------------------------------------------------------------------------------------
	if (!strcmp("new", cmd)) //----------------------------------------------------------------------------------------------------------------------
	{
		if (nrhs != 3)
		{
			mexErrMsgTxt("FeaturesPool: new(width, height) - Unexpected number of arguments.\n");
		}

		double *p_double;
		int width=0, height=0;

		p_double = (double*)mxGetData(prhs[1]);
		width = (int)p_double[0];

		p_double = (double*)mxGetData(prhs[2]);
		height = (int)p_double[0];

		CFeaturePool *featurePool = new CFeaturePool(width, height);

		if (featurePool)
		{
			plhs[0] = convertPtr2Mat< CFeaturePool >(featurePool);
		}

		return;
	}

	// Delete
	//----------------------------------------------------------------------------------------------------------------------------------------------
	if (!strcmp("delete", cmd)) //------------------------------------------------------------------------------------------------------------------
	{
		// Destroy the C++ object
		destroyObject< CFeaturePool >(prhs[1]);

		// Warn if other commands were ignored
		if (nlhs != 0 || nrhs != 2)
			mexWarnMsgTxt("FeaturesPool: Delete: Unexpected arguments ignored.\n");
		return;
	}

	CFeaturePool *featurePool_instance = convertMat2Ptr< CFeaturePool >(prhs[1]);

	//---------------------------------------------------------------------------------------------
	//------------------------ Call the various class methods -------------------------------------
	//---------------------------------------------------------------------------------------------

	// Add sparse LBP features
	//----------------------------------------------------------------------------------------------------------------------------------------------
	if (!strcmp("addSparseLBPfeatures", cmd)) //----------------------------------------------------------------------------------------------------
	{
		//TODO: MATLAB input/output check

		featurePool_instance->addFeaturesToPool(
					new CSparseLBPFeatures(featurePool_instance->getWidth(),
										featurePool_instance->getHeight(),
										featurePool_instance->getPyramidLevels(),
										featurePool_instance->getCumulativeWidths()
						)
					);

		return;
	}

	// TODO: Add other methods here (e.g. like updating the NFmipmap directly, etc.)

	// Set features raw
	//----------------------------------------------------------------------------------------------------------------------------------------------
	if (!strcmp("setFeaturesRaw", cmd)) //----------------------------------------------------------------------------------------------------------
	{
		//TODO: MATLAB input check
		if (nrhs != 4)
		{
			mexErrMsgTxt("setFeaturesRaw: unexpected number of arguments.\n"
						 "Usage: \n"
						 "\t setFeaturesRaw(index, features)\n\n"
						 "Input:\n"
						 "\t index \t [1 x 1 (int32)] index representing type of features to be set.\n"
						 "\t features \t [H x 2W (uint32)] \t features computed on the whole NFmipmap.\n");
		}

		double *tmp = (double*)mxGetData(prhs[2]);
		int index = int(tmp[0])-1;		// MATLAB is 1based
		unsigned char *featuresRaw = (unsigned char*)mxGetData(prhs[3]);

		featurePool_instance->updateFeaturesRaw(index, (void *)featuresRaw);

		return;
	}

	// Get features raw
	//----------------------------------------------------------------------------------------------------------------------------------------------
	if (!strcmp("getFeaturesRaw", cmd)) //----------------------------------------------------------------------------------------------------------
	{
		//TODO: MATLAB input & output check!
		if (nrhs != 3 || nlhs != 1)
		{
			mexErrMsgTxt("getFeaturesRaw: unexpected number of arguments.\n"
						 "Usage: \n"
						 "\t features = getFeaturesRaw(index) \n"
						 "Input: \n"
						 "\t index \t [1 x 1 (int32)] \t index respresenting the type of features to be returned. \n"
						 "Output: \n"
						 "\t features \t [H x 2W (uint32)] \t NFmipmap features. \n");
		}

		const mwSize *mx_dim = mxGetDimensions(prhs[2]);
		if (mx_dim[0] != 1 && mx_dim[1] != 1)
		{
			mexErrMsgTxt("getFeaturesRaw: index must be an integer number.");
		}

		double *tmp = (double*)mxGetData(prhs[2]);
		int index = int(tmp[0])-1;		// MATLAB is 1based
		CFeatures *features;

		features = featurePool_instance->getFeaturesFromPool(index);
		unsigned char *rawLBPfeatures = (unsigned char*)features->getFeatures();

		// MATLAB output
//		plhs[0] = mxCreateNumericMatrix(featurePool_instance->getHeight(), 2*featurePool_instance->getWidth(), mxUINT32_CLASS, mxREAL);
		plhs[0] = mxCreateNumericMatrix(featurePool_instance->getHeight(), 2*featurePool_instance->getWidth(), mxUINT8_CLASS, mxREAL);
//		unsigned int *output = (unsigned int*)mxGetData(plhs[0]);
		unsigned char *output = (unsigned char*)mxGetData(plhs[0]);

		memcpy(output, rawLBPfeatures, featurePool_instance->getHeight()*featurePool_instance->getWidth()*2*sizeof(unsigned char));

		return;
	}

	// Compute from NF
	//----------------------------------------------------------------------------------------------------------------------------------------------
	if (!strcmp("computeFromNF", cmd)) //------------------------------------------------------------------------------------------------------------
	{
		if (nrhs != 3)
		{
			mexErrMsgTxt("computeFromNF: unexpected number of arguments.\n"
						 "Usage: \n"
						 "\t computeFromNF(NF) \n"
						 "Input: \n"
						 "NF \t [BW_W x BW_H (uint8)] \t Normalized Frame (NOTE: row-wise ordered).\n");
		}

		const mwSize *mx_dim = mxGetDimensions(prhs[2]);
		if (mx_dim[0] != featurePool_instance->getWidth() && mx_dim[1] != featurePool_instance->getHeight())
		{
			mexErrMsgTxt("computeFromNF: NF must be [BW_H x BW_W (uint8)] matrix.");
		}

		if (mxGetClassID(prhs[2]) != mxUINT8_CLASS)
		{
			mexErrMsgTxt("computeFromNF: NF must be [BW_H x BW_W (uint8)] matrix.");
		}

		unsigned char *nf = (unsigned char*)mxGetData(prhs[2]);

		featurePool_instance->updateNFmipmap(featurePool_instance->getWidth(), featurePool_instance->getHeight(), nf);

		return;
	}

	// Got here, so command not recognized
	mexErrMsgTxt("Command not recognized."); //------------------------------------------------------------------------------------------------------
}
