/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2013, 2014, 2015 Michal Uricar
 * Copyright (C) 2013, 2014, 2015 Michal Uricar
 */

#ifndef _CDISPLACEMENTDEFORMATIONCOST_H__
#define _CDISPLACEMENTDEFORMATIONCOST_H__

#include "CDeformationCost.h"

namespace clandmark {

/**
 * @brief The CDisplacementDeformationCost class
 */
class CDisplacementDeformationCost : public CDeformationCost {

public:

	// Constructor
	/**
	 * @brief CDisplacementDeformationCost
	 * @param parent
	 * @param child
	 * @param dimension
	 */
	CDisplacementDeformationCost(CAppearanceModel * const parent, CAppearanceModel * const child, int dimension=4);

	/** Destructor */
	~CDisplacementDeformationCost();

	/**
	 * @brief dotProductWithWg
	 * @param w
	 * @param g
	 * @param index
	 */
	void dotProductWithWg(fl_double_t * const w, fl_double_t *g, const int index);

	/**
	 * @brief update
	 * @param w
	 * @param g
	 * @param image_data
	 * @param ground_truth
	 */
	void update(fl_double_t * const w, fl_double_t *g, cimg_library::CImg<unsigned char> *image_data, int * const ground_truth=0);

	/**
	 * @brief getDeformationCostAt
	 * @param s_i
	 * @param s_j
	 * @param deformation_cost
	 */
	void getDeformationCostAt(int * const s_i, int * const s_j, int *deformation_cost);

	/**
	 * @brief getGvalue
	 * @param s_i
	 * @param s_j
	 * @param w
	 * @return
	 */
	fl_double_t getGvalue(int *const s_i, int *const s_j, fl_double_t *const w);

	/**
	 * @brief write
	 * @param fs
	 */
	void write(XmlStorage &fs, fl_double_t * const w, bool writeW=true);

private:

	/**
	 * @brief computeDeformationCosts
	 */
	void computeDeformationCosts();

	/**
	 * @brief distanceTransform1D
	 * @param f
	 * @param n
	 * @return
	 */
//	void distanceTransform1D(fl_double_t w1, fl_double_t w2, fl_double_t *f, const int coor);

	/**
	 * @brief distanceTransform2D
	 * @param rectangle
	 * @param width
	 * @param height
	 */
//	void distanceTransform2D(fl_double_t *const w, fl_double_t *rectangle, int *indices);

private:

//	int **deformationCosts;
	int parentLength;
	int childLength;
};

}

#endif // _CDISPLACEMENTDEFORMATIONCOST_H__
