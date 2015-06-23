/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2014, 2015 Michal Uricar
 * Copyright (C) 2014, 2015 Michal Uricar
 */

#ifndef _CTYPES_H__
#define _CTYPES_H__

#include <cfloat>
#include <limits>

#include "CLandmarkConfig.h"

namespace clandmark {

// REDFINE DOUBLE_PRECISION AS FLOAT
#if DOUBLE_PRECISION==1
	typedef double  fl_double_t;
	#define FL_DBL_MAX DBL_MAX
#else
	typedef float  fl_double_t;
	#define FL_DBL_MAX FLT_MAX;
#endif

static const bool HAS_INF = std::numeric_limits<fl_double_t>::has_infinity;
static const fl_double_t FL_DBL_INF = (HAS_INF) ? std::numeric_limits<fl_double_t>::infinity() : DBL_MAX;

} /* namespace clandmark */

#endif // _CTYPES_H__
