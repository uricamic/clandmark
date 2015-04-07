/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2013, 2014, 2015 Michal Uricar
 * Copyright (C) 2013, 2014, 2015 Michal Uricar
 */

#ifndef __CTIMER__H_
#define __CTIMER__H_

#include "msvc-compat.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

namespace clandmark {

class CTimer {

public:

	void tic()
	{
		//gettimeofday(&start, 0x0);
#ifdef _MSC_VER
		time(&start);
#else
		gettimeofday(&start, 0x0);
#endif // _MSC_VER
	}

	double toc()
	{
#ifdef _MSC_VER
		time(&end);
		duration = difftime(start, end)*1000.0;
#else

		gettimeofday(&end, 0x0);

		sec = end.tv_sec - start.tv_sec;
		usec = end.tv_usec - start.tv_usec;

		duration = (sec + usec/1000000.0) * 1000.0;
#endif // _MSC_VER
		return duration;
	}

private:

#ifdef _MSC_VER
	time_t start, end;
#else
	timeval start, end;
#endif // _MSC_VER
	tm* local;
	long sec, usec;
	double duration;

};

}

#endif // __CTIMER__H_
