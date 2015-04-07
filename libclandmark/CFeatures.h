/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2014, 2015 Michal Uricar
 * Copyright (C) 2014, 2015 Michal Uricar
 */

#ifndef _CFEATURES_H__
#define _CFEATURES_H__

//#include "CImg.h"

#include "msvc-compat.h"

namespace clandmark {

/**
 * @brief The CFeatures class
 */
class CFeatures {

public:

	/**
	 * @brief CFeatures
	 */
	CFeatures() : kWidth(-1), kHeight(-1), kLevels(-1)
	{  }

	/**
	 * @brief CFeatures
	 * @param width
	 * @param height
	 */
	CFeatures(int width, int height, int pyrLevels, int *cumwidths);

	/**
	 * @brief ~CFeatures
	 */
	virtual ~CFeatures();

	/**
	 * @brief compute
	 */
	virtual void compute(void) = 0;

	/**
	 * @brief getFeatures
	 * @return
	 */
	virtual void *const getFeatures(void) = 0;

	/**
	 * @brief setFeatures
	 * @param features
	 */
	virtual void setFeatures(CFeatures *features) = 0;

	/**
	 * @brief setFeaturesRaw
	 * @param data
	 */
	virtual void setFeaturesRaw(void *data) = 0;

	/**
	 * @brief setNFmipmap
	 * @param nfMipmap
	 */
	inline void setNFmipmap( unsigned char *nfMipmap) { NFmipmap = nfMipmap; }

public:

	unsigned char * NFmipmap;				/**< Normalized frame mipmap */
	const int kWidth;						/**< */
	const int kHeight;						/**< */
	const int kLevels;						/**< */
	int * cumWidths;						/**< */
};

}

#endif // _CFEATURES_H__
