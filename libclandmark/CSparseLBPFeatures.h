/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2014, 2015 Michal Uricar
 * Copyright (C) 2014, 2015 Michal Uricar
 */

#ifndef _CSPARSELBPFEATURES_H__
#define _CSPARSELBPFEATURES_H__

#include "CFeatures.h"

#include <vector>

namespace clandmark {

class CSparseLBPFeatures : public CFeatures {

public:

	/**
	 * @brief CSparseLBPFeatures
	 */
	CSparseLBPFeatures()
	{  }

	/**
	 * @brief CSparseLBPFeatures
	 * @param width
	 * @param height
	 */
	CSparseLBPFeatures(int width, int height, int pyrLevels, int *cumwidths);

	/**
	 * @brief destructor
	 */
	~CSparseLBPFeatures();

	/**
	 * @brief compute
	 */
	void compute(void);

	/**
	 * @brief getFeatures
	 * @return
	 */
	void * const getFeatures(void) { return LBP_NF; }

	/**
	 * @brief setFeatures
	 * @param features
	 * @return
	 */
	void setFeatures(CFeatures * const features);

	/**
	 * @brief setFeaturesRaw
	 * @param data
	 */
	void setFeaturesRaw(void *data);

private:

	unsigned char * LBP_NF;		/**< "Mipmap like structure holding LBP features computed on the whole NF" */
	int Wtimes2;				/**< */

};

}

#endif // _CSPARSELBPFEATURES_H__
