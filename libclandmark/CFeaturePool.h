/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2014, 2015 Michal Uricar
 * Copyright (C) 2014, 2015 Michal Uricar
 */

#include "CFeatures.h"

#include <vector>

#ifndef _CFEATUREPOOL_H__
#define _CFEATUREPOOL_H__

namespace clandmark {

/**
 * @brief The CFeaturePool class
 */
class CFeaturePool {

public:

	/**
	 * @brief CFeaturePool
	 */
	CFeaturePool();

	/**
	 * @brief CFeaturePool
	 * @param width
	 * @param height
	 */
	CFeaturePool(int width, int height, unsigned char *nf=0x0);

	/**
	 *
	 */
	~CFeaturePool();

	/**
	 * @brief getFeaturesFromPool
	 * @param index
	 * @return
	 */
	inline CFeatures * getFeaturesFromPool(unsigned int index) { return featurePool.at(index); }

	/**
	 * @brief addFeaturesToPool
	 * @param features
	 */
	inline void addFeaturesToPool(CFeatures *features) { featurePool.push_back(features); }

	/**
	 * @brief updateNFmipmap
	 * @param nf
	 */
	void updateNFmipmap(int width, int height, unsigned char * const nf);

	/**
	 * @brief setNFmipmap
	 * @param mipmap
	 */
	void setNFmipmap(unsigned char * const mipmap);

	/**
	 * @brief getCumulativeWidths
	 * @return
	 */
	inline int * getCumulativeWidths(void) { return cumWidths; }

	/**
	 * @brief getPyramidLevels
	 * @return
	 */
	inline int getPyramidLevels(void) { return maxPyrLBP; }

	/**
	 * @brief getWidth
	 * @return
	 */
	inline int getWidth(void) { return kWidth; }

	/**
	 * @brief getHeight
	 * @return
	 */
	inline int getHeight(void) { return kHeight; }

	/**
	 * @brief computeFeatures
	 */
	void computeFeatures(void);

	/**
	 * @brief updateFeaturesRaw
	 * @param data
	 */
	void updateFeaturesRaw(int index, void *data);

private:

	void createNFmipmap(int width, int height, unsigned char * const nf);

private:

	const int kWidth;						/**< width of NF mipmap */
	const int kHeight;						/**< height of NF mipmap */

	std::vector<CFeatures*> featurePool;	/**< */
	unsigned char *NF_mipmap;				/**< Normalized Frame mipmap */
	unsigned char *tmpNF;					/**< */
	int *cumWidths;							/**< */
	int maxPyrLBP;							/**< */
	int kPyrLevels;							/**< */

};

}

#endif // _CFEATUREPOOL_H__
