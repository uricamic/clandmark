/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2013 Michal Uricar
 * Copyright (C) 2013 Michal Uricar
 */

#ifndef _CDAGMAXSUMSOLVER_H__
#define _CDAGMAXSUMSOLVER_H__

#include "CMaxSumSolver.h"

namespace clandmark {

class CDAGMaxSumSolver : CMaxSumSolver {

public:

	/** Default constructor */
	CDAGMaxSumSolver();

	/** Destructor */
	~CDAGMaxSumSolver();

	void solve(std::vector<CAppearanceModel> *vertices, std::vector<CDeformationCost> *edges, double **q, double **g);

private:

	void sortTopologically(void);

private:

	// Degree of each vertex
	std::vector<uint32_t> inDegrees;

};

}

#endif // _CDAGMAXSUMSOLVER_H__
