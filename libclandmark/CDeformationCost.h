/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2013, 2014, 2015 Michal Uricar
 * Copyright (C) 2013, 2014, 2015 Michal Uricar
 */

#ifndef _CDEFORMATIONCOST_H__
#define _CDEFORMATIONCOST_H__

#include "CAppearanceModel.h"

#define cimg_verbosity 1        // we don't need window output capabilities of CImg
#define cimg_display 0          // we don't need window output capabilities of CImg
#include "CImg.h"

#include "CXMLInOut.h"

namespace clandmark {

/**
 *
 */
typedef enum {
	DISPLACEMENT_VECTOR=1
} EDeformationCostType;


/**
 * @brief The CDeformationCost class
 */
class CDeformationCost {

public:

	// constructor
	/**
	 * @brief CDeformationCost
	 * @param parent_
	 * @param child_
	 * @param loss_
	 */
	CDeformationCost(CAppearanceModel * const parent_, CAppearanceModel * const child_, CLoss * const loss_=0);

	/** Destructor */
	virtual ~CDeformationCost();

	/**
	 * @brief update
	 * @param w
	 * @param g
	 * @param image_data
	 * @param ground_truth
	 */
	virtual void update(fl_double_t * const w, fl_double_t *g, cimg_library::CImg<unsigned char> *image_data, int * const ground_truth=0);

	/**
	 * @brief getParent
	 * @return
	 */
	inline CAppearanceModel * getParent(void) { return parent; }

	/**
	 * @brief getChild
	 * @return
	 */
	inline CAppearanceModel * getChild(void) { return child; }

	/**
	 * @brief getDimension
	 * @return
	 */
	inline const int getDimension(void) { return kDimension; }

	// compute dot product <W_g_{i,j}, deformation_costs_map>
	/**
	 * @brief dotProductWithWg
	 * @param w
	 * @param g
	 * @param index
	 */
	virtual void dotProductWithWg(fl_double_t * const w, fl_double_t *g, const int index) = 0;

	/**
	 * @brief getDeformationCostAt
	 * @param s_i
	 * @param s_j
	 * @param deformation_cost
	 */
	virtual void getDeformationCostAt(int * const s_i, int * const s_j, int * deformation_cost) = 0;

	/**
	 * @brief setLossNormalizationFactor
	 * @param factor
	 */
	inline void setLossNormalizationFactor(fl_double_t factor)
	{
		if (loss)
			loss->setNormalizationFactor(factor);
	}

	/**
	 * @brief getNormalizationFactor
	 * @return
	 */
	inline fl_double_t getNormalizationFactor(void)
	{
		if (loss)
			return loss->getNormalizationFactor();
		return 1.0;
	}

	/**
	 * @brief getGvalue
	 * @param s_i
	 * @param s_j
	 * @param w
	 * @return
	 */
	virtual fl_double_t getGvalue(int * const s_i, int * const s_j, fl_double_t * const w) = 0;

	/**
	 * @brief write
	 * @param fs
	 */
	virtual void write(XmlStorage &fs, fl_double_t * const w, bool writeW=true) = 0;

protected:

	// compute deformation cost (table) for given pair s_i, s_j (edges from graph constraints)
	/**
	 * @brief computeDeformationCosts
	 */
	virtual void computeDeformationCosts() = 0;

private:

	CDeformationCost(const CDeformationCost&)
	{}

protected:

	CAppearanceModel *parent;   // <- contains also information about search space for given node
	CAppearanceModel *child;    // <- contains also information about search space for given node

	cimg_library::CImg<unsigned char> *imageData;

	// Loss function
	CLoss *loss;

	int kDimension;

	// internal representation of features goes to specialized classes (e.g. CDisplacementDeformationCost, etc.)
	// convention on order for edge (parent, child)
};

}

#endif // _CDEFORMATIONCOST_H__
