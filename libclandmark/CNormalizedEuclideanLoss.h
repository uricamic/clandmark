/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2013 Michal Uricar
 * Copyright (C) 2013 Michal Uricar
 */

#ifndef _CNORMALIZEDEUCLIDEANLOSS_H__
#define _CNORMALIZEDEUCLIDEANLOSS_H__

#include "CLoss.h"

namespace clandmark {

/**
 * @brief The CNormalizedEuclideanLoss class
 */
class CNormalizedEuclideanLoss : public CLoss {

public:

	// Constructor
	/**
	 * @brief CNormalizedEuclideanLoss
	 * @param size_w
	 * @param size_h
	 */
	CNormalizedEuclideanLoss(int size_w, int size_h);

	/** Destructor */
	~CNormalizedEuclideanLoss();

	/**
	 * @brief computeLoss
	 * @param ground_truth
	 * @param estimate
	 * @return
	 */
	fl_double_t computeLoss(fl_double_t * const ground_truth, fl_double_t * const estimate);

	/**
	 * @brief computeLoss
	 * @param ground_truth
	 * @param estimate
	 * @return
	 */
	fl_double_t computeLoss(int * const ground_truth, int * const estimate);

	/**
	 * @brief getLossAt
	 * @param position
	 * @return
	 */
	fl_double_t getLossAt(int position)
	{
		return 0;
	}

	/**
	 * @brief getLossType
	 * @return
	 */
	ELossType getType(void) { return NORMALIZED_EUCLIDEAN; }

	/**
	 * @brief getName
	 * @return
	 */
	std::string getName(void) { return "NORMALIZED_EUCLIDEAN"; }

};

}

#endif // _CNORMALIZEDEUCLIDEANLOSS_H__
