/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2013, 2014, 2015 Michal Uricar
 * Copyright (C) 2013, 2014, 2015 Michal Uricar
 */

#ifndef _CLOSS_H__
#define _CLOSS_H__

#include "msvc-compat.h"
#include "CTypes.h"

#include <string>

namespace clandmark {

/**
 *
 */
typedef enum {
	NORMALIZED_EUCLIDEAN = 1,
	TABLE_LOSS = 2,
	ZERO_LOSS = 3
} ELossType;

/**
 * @brief The CLoss class
 */
class CLoss {

public:

	/**
	 * @brief CLoss
	 * @param size_
	 */
	//CLoss(int * const size_);

	/**
	 * @brief CLoss
	 * @param size_w
	 * @param size_h
	 */
	CLoss(int size_w, int size_h);

	/**
	 * @brief ~CLoss
	 */
	virtual ~CLoss();

	// virtual function, computes the loss given the ground truth position and the estimated position
	/**
	 * @brief computeLoss
	 * @param ground_truth
	 * @param estimate
	 * @return
	 */
	virtual fl_double_t computeLoss(fl_double_t * const ground_truth, fl_double_t * const estimate) = 0;

	/**
	 * @brief computeLoss
	 * @param ground_truth
	 * @param estimate
	 * @return
	 */
	virtual fl_double_t computeLoss(int * const ground_truth, int * const estimate) = 0;

	/**
	 * @brief getLossAt
	 * @param position
	 * @return
	 */
	virtual fl_double_t getLossAt(int position) = 0;

	/**
	 * @brief addLoss
	 * @param ground_truth
	 * @param array
	 * @param offset
	 */
	void addLoss(int * const ground_truth, fl_double_t *array, int * const offset);

	/**
	 * @brief addLoss
	 * @param array
	 */
	void addLoss(fl_double_t *array);

	/**
	 * @brief setNormalizationFactor
	 * @param normalization_factor
	 */
	inline void setNormalizationFactor(fl_double_t normalization_factor)
	{ normalizationFactor = normalization_factor; }

	/**
	 * @brief getNormalizationFactor
	 * @return
	 */
	inline fl_double_t getNormalizationFactor(void)
	{ return normalizationFactor; }

	/**
	 * @brief getLossType
	 * @return
	 */
	virtual ELossType getType(void) = 0;

	/**
	 * @brief getName
	 * @return
	 */
	virtual std::string getName(void) = 0;

private:

	/**
	 * @brief CLoss
	 */
	CLoss(const CLoss&) : kLength(-1)
	{}

protected:

	const int kLength;			/**< */
	fl_double_t normalizationFactor;	/**< */
	int *admissiblePositions;	/**< */

};

} /* namespace clandmark */

#endif // _CLOSS_H__
