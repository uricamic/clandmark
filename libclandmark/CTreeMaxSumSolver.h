/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2013 Michal Uricar
 * Copyright (C) 2013 Michal Uricar
 */

#ifndef _CTREEMAXSUMSOLVER_H__
#define _CTREEMAXSUMSOLVER_H__

#include "CMaxSumSolver.h"

#include <queue>

namespace clandmark {

/**
 *
 */
typedef struct node {
	int ID;
	std::vector< int > children;
	std::vector< int > ancestors;
	std::vector< int > incidentEdgeIDs;
} Node;

// NOTE: Specification of the root_id is not necessary since it will be found automatically based on provided oriented edges.

/**
 * @brief The CTreeMaxSumSolver class
 */
class CTreeMaxSumSolver : public CMaxSumSolver {

public:

	/** Default constructor */
	/**
	 * @brief CTreeMaxSumSolver
	 * @param vertices
	 * @param edges
	 * @param bw_w
	 * @param bw_h
	 */
	CTreeMaxSumSolver(std::vector< Vertex > * const vertices,  std::vector< CDeformationCost* > * const edges, int bw_w, int bw_h);

	/** Destructor */
	~CTreeMaxSumSolver();

	/**
	 * @brief solve
	 * @param w
	 * @param q
	 * @param g
	 * @param output
	 */
	void solve(const std::vector< std::vector< fl_double_t* > > &w, const std::vector< std::vector< fl_double_t* > > &q, fl_double_t **g, int *output);

	/**
	 * @brief getIntermediateResults
	 * @return
	 */
	std::vector<fl_double_t *> getIntermediateResults(void);

private:

	/**
	 * @brief maximizeSum
	 * @param q
	 * @param g
	 * @param length
	 * @param maximum
	 * @param index
	 */
	void maximizeSum(fl_double_t *const q, fl_double_t *const g, const int length, fl_double_t *maximum, int *index);

	/**
	 * @brief dt1D
	 * @param src
	 * @param dst
	 * @param ptr
	 * @param step
	 * @param len
	 * @param a
	 * @param b
	 * @param dshift
	 * @param dlen
	 * @param dstep
	 */
	void dt1D(fl_double_t *const src, fl_double_t *dst, int *ptr, int step, int len, double a, double b, double dshift, int dlen, double dstep);

	/**
	 * @brief dt
	 * @param qP
	 * @param qC
	 * @param w
	 * @param indices
	 * @param edge
	 */
	void dt(fl_double_t *qC, fl_double_t * const w, fl_double_t *qP, int *indices, CDeformationCost* edge);

private:

	std::queue< int > taskQueue;						/**< */
	std::vector< int > inDegrees;						/**< */
	std::vector< Node > graph;							/**< */
	std::queue< int > topsortQueue;						/**< */
	std::vector< int > order;							/**< */

	std::vector< fl_double_t* > intermediateResults;	/**< */
	std::vector< int* > intermediateIndices;			/**< */

	std::vector< int > argmaxIndices;					/**< */

	int kLandmarks;										/**< */
	int kEdges;											/**< */
};

}

#endif // _CTREEMAXSUMSOLVER_H__
