/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2013, 2014, 2015 Michal Uricar
 * Copyright (C) 2013, 2014, 2015 Michal Uricar
 */

#ifndef _CSPARSELBPAPPEARANCEMODEL_H__
#define _CSPARSELBPAPPEARANCEMODEL_H__

#include "CAppearanceModel.h"
#include "CSparseLBPFeatures.h"

namespace clandmark {

/**
 * @brief The CSparseLBPAppearanceModel class
 */
class CSparseLBPAppearanceModel : public CAppearanceModel {

public:

	/**
	 * @brief CSparseLBPAppearanceModel
	 * @param nodeID
	 * @param search_space_min_x
	 * @param search_space_min_y
	 * @param search_space_max_x
	 * @param search_space_max_y
	 * @param window_size_w
	 * @param window_size_h
	 * @param height_of_pyramid
	 * @param loss_
	 */
	CSparseLBPAppearanceModel(
		int nodeID,
		int search_space_min_x,
		int search_space_min_y,
		int search_space_max_x,
		int search_space_max_y,
		int window_size_w,
		int window_size_h,
		int height_of_pyramid,
		CLoss *loss_=0
	);

	/**
	 * Destructor
	 */
	~CSparseLBPAppearanceModel();

	/**
	 * @brief getFeatureAt
	 * @param index
	 * @param feature_vector
	 */
	void getFeatureAt(const int index, fl_double_t *feature_vector);

	/**
	 * @brief getQvalue
	 * @param s_i
	 * @param w
	 * @return
	 */
	fl_double_t getQvalue(int *const s_i, fl_double_t *const w);

	/**
	 * @brief write
	 * @param fs
	 */
	void write(XmlStorage &fs, fl_double_t * const w, bool writeW = true) const;

protected:

	/**
	 * @brief computeFeatures
	 */
	void computeFeatures(void);

	/**
	 * @brief computeFeatures_optimized
	 */
	void computeFeatures_optimized(void);

	/**
	 * @brief dotProductWithWq
	 * @param w
	 * @param q
	 */
	void dotProductWithWq(fl_double_t * const w, fl_double_t *q);

	/**
	 * @brief dotProductWithWq_optimized
	 * @param w
	 * @param q
	 * @param containerSizes
	 */
//	void dotProductWithWq_optimized(fl_double_t *const w, fl_double_t *q, int *containerSizes);

private:

	/**
	 * @brief getDim
	 * @param img_nRows
	 * @param img_nCols
	 * @param nPyramids
	 * @return
	 */
	int getDim(int img_nRows, int img_nCols, int nPyramids);

	/**
	 * @brief featuresSparse
	 * @param vec
	 */
	void featuresSparse(int* vec);

private:

	const int kHeightOfPyramid;			/**< */

	int *LBPFeatures;					/**< */
	int kSparseFeatureDimension;		/**< */
	int *window;						/**< */

};

}

#endif // _CSPARSELBPAPPEARANCEMODEL_H__
