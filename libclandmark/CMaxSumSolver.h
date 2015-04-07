/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2013, 2014, 2015 Michal Uricar
 * Copyright (C) 2013, 2014, 2015 Michal Uricar
 */

#ifndef _CMAXSUMSOLVER_H__
#define _CMAXSUMSOLVER_H__

#include "CAppearanceModel.h"
#include "CDeformationCost.h"

#include <vector>

namespace clandmark {

/**
 * @brief The CMaxSumSolver class
 */
class CMaxSumSolver {

public:

	/**
	 * @brief CMaxSumSolver
	 * @param vertices_
	 * @param edges_
	 * @param bw_w
	 * @param bw_h
	 */
	CMaxSumSolver(std::vector< Vertex > * const vertices_, std::vector< CDeformationCost* > * const edges_, int bw_w, int bw_h);

	/**
	 * @brief ~CMaxSumSolver
	 */
	virtual ~CMaxSumSolver();

	/**
	 * @brief solve
	 * @param w
	 * @param q
	 * @param g
	 * @param output
	 */
	virtual void solve(const std::vector< std::vector< fl_double_t* > > &w, const std::vector< std::vector< fl_double_t* > > &q, fl_double_t **g, int *output) = 0;

	// FOR VISUALIZATION IN MATLAB (and Python) INTERFACE
	/**
	 * @brief getIntermediateResults
	 * @return
	 */
	virtual std::vector<fl_double_t *> getIntermediateResults(void) = 0;

protected:

	std::vector< Vertex > *vertices;			/**< */
	std::vector< CDeformationCost* > *edges;	/**< */

	// DT
	int bw[2];									/**< */
	int tmpFsize;								/**< */
	fl_double_t * tmpA, * tmpB;					/**< */
	fl_double_t * rectangle;					/**< */
	fl_double_t * tmpDT;						/**< */
	int *tmpI;									/**< */
	int *iX, *iY;								/**< */
};

}

#endif // _CMAXSUMSOLVER_H__
