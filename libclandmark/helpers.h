/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2014, 2015 Michal Uricar
 * Copyright (C) 2014, 2015 Michal Uricar
 */

#ifndef __HELPERS_H__
#define __HELPERS_H__

namespace clandmark {

// DOUBLE_PRECISION
#if DOUBLE_PRECISION==1
	typedef double DOUBLE;
#else
	typedef float DOUBLE;
#endif

/**
 * @brief printQG
 * @param Q
 * @param G
 * @param M
 * @param N
 */
void printQG(DOUBLE *Q, DOUBLE *G, int M, int N)
{
	std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(2); // << setw(5) << endl;
	std::cout << "Q: " << std::endl;
	std::cout << "[ ";
	for (int i=0; i < M; ++i)
	{
		std::cout << Q[i] << " ";
	}
	std::cout << "]" << std::endl;
	std::cout << "G: " << std::endl;
	std::cout << "[ ";
	for (int i=0; i < N; ++i)
	{
		std::cout << G[i] << " ";
	}
	std::cout << "]" << std::endl;
}

/**
 * @brief printLandmarks
 * @param landmarks
 * @param M
 */
void printLandmarks(DOUBLE *landmarks, int M)
{
	std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(2) << std::setw(5) << std::endl << "Landmarks:" << std::endl;
	std::cout << "[ ";
	for (int i=0; i < 2*M; i+=2)
	{
		std::cout << landmarks[i] << " ";
	}
	std::cout << "]" << std::endl << "[ ";
	for (int i=1; i < 2*M; i+=2)
	{
		std::cout << landmarks[i] << " ";
	}
	std::cout << "]" << std::endl;
}

/**
 * @brief printLandmarks
 * @param landmarks
 * @param M
 */
void printLandmarks(int *landmarks, int M)
{
	std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(4) << std::setw(5) << std::endl << "Landmarks:" << std::endl;
	std::cout << "[ ";
	for (int i=0; i < 2*M; i+=2)
	{
		std::cout << landmarks[i] << " ";
	}
	std::cout << "]" << std::endl << "[ ";
	for (int i=1; i < 2*M; i+=2)
	{
		std::cout << landmarks[i] << " ";
	}
	std::cout << "]" << std::endl;
}

/**
 * @brief printTimingStats
 * @param timings
 */
void printTimingStats(Timings &timings)
{
	std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(4) << std::setw(5) << std::endl
		 << "FLANDMARK Time statistics:" << std::endl
		 << "--------------------------" << std::endl
		 << "NF extraction:	\t\t" << timings.normalizedFrame << " ms" << std::endl
		 << "Features comptation:	\t\t" << timings.features << " ms" << std::endl
		 << "Maxsum solver:	\t\t" << timings.maxsum << " ms" << std::endl
		 << "Overall time:	\t\t" << timings.overall << " ms" << std::endl << std::endl;
}

}

#endif // __HELPERS_H__
