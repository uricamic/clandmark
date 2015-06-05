#include "CLandmark.h"
#include "msvc-compat.h"
#include "CTimer.h"

#include <cfloat>
#include <cstring>
#include <cmath>
#include <math.h>

#ifdef _OPENMP
	#include <omp.h>

	#include <iostream>
	#include <iomanip>
#else
	#define omp_get_thread_num() 0
#endif

//#define DEBUG_MSG

using namespace clandmark;

CLandmark::CLandmark(
		int landmarksCount,
		int edgesCount,
		int base_window_width,
		int base_window_height,
		fl_double_t base_window_margin_x,
		fl_double_t base_window_margin_y)
{
	init(landmarksCount, edgesCount, base_window_width, base_window_height, base_window_margin_x, base_window_margin_y);
}

CLandmark::CLandmark()
{
	kLandmarksCount = 0;
	kEdgesCount = 0;

	// convention for size [width x height]
	baseWindow[0] = 0;
	baseWindow[1] = 0;
	baseWindowMargin[0] = 0;
	baseWindowMargin[1] = 0;

	// normalized image frame related variables
	memset(H, 0, sizeof(H));
	memset(Hinv, 0, sizeof(Hinv));
	memset(BB, 0, sizeof(BB));

	g = 0x0;

	wDimension = 0;

	landmarksPositions = 0x0;
	landmarksPositionsNF = 0x0;

	solver = 0x0;

	// GroundTruth (for training only)
	groundTruthPositions = 0x0;
	groundTruthPositionsNF = 0x0;

	normalizationFactor = 1.0;

	psi = 0x0;

	normalizedFrame = 0x0;

	Q = 0x0;
	G = 0x0;
	L = 0x0;

	psiNodesDimension = 0;
}

void CLandmark::init(
		int landmarksCount,
		int edgesCount,
		int base_window_width,
		int base_window_height,
		fl_double_t base_window_margin_x,
		fl_double_t base_window_margin_y)
{
	kLandmarksCount = landmarksCount;
	kEdgesCount = edgesCount;

	baseWindow[0] = base_window_width;
	baseWindow[1] = base_window_height;
	baseWindowMargin[0] = base_window_margin_x;
	baseWindowMargin[1] = base_window_margin_y;

	groundTruthPositions = new fl_double_t[2*kLandmarksCount];
	groundTruthPositionsNF = new int[2*kLandmarksCount];

	landmarksPositions = new fl_double_t[2*kLandmarksCount];
	landmarksPositionsNF = new int[2*kLandmarksCount];

	memset(groundTruthPositions, 0, sizeof(fl_double_t)*2*kLandmarksCount);
	memset(groundTruthPositionsNF, 0, sizeof(int)*2*kLandmarksCount);
	memset(landmarksPositions, 0, sizeof(fl_double_t)*2*kLandmarksCount);
	memset(landmarksPositionsNF, 0, sizeof(int)*2*kLandmarksCount);

	// normalized image frame related variables
	memset(H, 0, sizeof(H));
	memset(Hinv, 0, sizeof(Hinv));
	memset(BB, 0, sizeof(BB));

	for (int i=0; i < kLandmarksCount; ++i)
	{
		q.push_back(std::vector< fl_double_t* >());
	}

	g = new fl_double_t*[kEdgesCount];

	for (int i=0; i < kLandmarksCount+kEdgesCount; ++i)
	{
		w.push_back(std::vector< fl_double_t* >());
	}

	normalizedFrame =  new cimg_library::CImg<unsigned char>(base_window_width, base_window_height);

	normalizationFactor = 1.0;

	Q = new fl_double_t[kLandmarksCount];
	G = new fl_double_t[kEdgesCount];
	L = new fl_double_t[kLandmarksCount];
}

CLandmark::~CLandmark()
{
	if (!q.empty())
	{
		for (unsigned int i=0; i < q.size(); ++i)
		{
			for (unsigned int j=0; j < q[i].size(); ++j)
				delete [] q[i][j];
			q[i].clear();
		}
		q.clear();
	}

	if (g)
	{
		for (int i=0; i < kEdgesCount; ++i)
		{
			if (g[i])
				delete [] g[i];
		}
		delete [] g;
	}

	if (!w.empty())
	{
		for (unsigned int i=0; i < w.size(); ++i)
		{
			for (unsigned int j=0; j < w[i].size(); ++j)
				delete [] w[i][j];
			w[i].clear();
		}
		w.clear();
	}

	if (groundTruthPositions)
		delete [] groundTruthPositions;

	if (groundTruthPositionsNF)
		delete [] groundTruthPositionsNF;

	if (landmarksPositions)
		delete [] landmarksPositions;

	if (landmarksPositionsNF)
		delete [] landmarksPositionsNF;

	if (psi)
		delete [] psi;

	delete normalizedFrame;

	if (Q)
		delete [] Q;

	if (G)
		delete [] G;

	if (L)
		delete [] L;
}

void CLandmark::setGroundTruth(fl_double_t *ground_truth)
{
	memcpy(groundTruthPositions, ground_truth, 2*kLandmarksCount*sizeof(fl_double_t));

	// transform GT to normalized frame
	transformCoordinatesImage2NF(groundTruthPositions, groundTruthPositionsNF);
}

void CLandmark::setGroundTruth(int *ground_truth)
{
	memcpy(groundTruthPositionsNF, ground_truth, 2*kLandmarksCount*sizeof(int));
}

void CLandmark::getQG(void)
{
	for (unsigned int i=0; i < vertices.size(); ++i)
	{
		int best = 0;
		fl_double_t maximum = -FL_DBL_MAX;

		for (unsigned int j=0; j < q[i].size(); ++j)
		{
			vertices[i].appearances[j]->update(normalizedFrame, w[i][j], q[i][j], &groundTruthPositionsNF[INDEX(0, i, 2)]);

			fl_double_t sum = 0.0;
			//for (int k=0; k < vertices[i].appearances[j]->getFeatureDimension(); ++k)
			for (int k=0; k < vertices[i].appearances[j]->getLength(); ++k)
				sum += q[i][j][k];

			if (sum > maximum)
			{
				maximum = sum;
				best = j;
			}
		}

		vertices[i].best = best;
	}

	for (uint32_t i = 0; i < edges.size(); ++i)
		edges[i]->update(w[kLandmarksCount+i][0], g[i], normalizedFrame, groundTruthPositionsNF);
}

void CLandmark::getQG_optimized()
{
#ifdef _OPENMP
//	omp_set_num_threads(omp_get_max_threads());
	omp_set_num_threads(4);
	#pragma omp parallel for
#endif
	for (unsigned int i=0; i < vertices.size(); ++i)
	{
#ifdef DEBUG_MSG
		CTimer timer;
		double tim;
		timer.tic();
#endif
		int best = 0;
		fl_double_t maximum = -FL_DBL_MAX;

		for (unsigned int j=0; j < q[i].size(); ++j)
		{
			//std::string appearance_type = vertices[i].appearances[j]->getName();
			std::string appearance_type = vertices[i].appearances[j]->getType();
			EAppearanceModelType type = SPARSE_LBP;

			if (appearance_type.compare("SPARSE_LBP")==0)
			{
				type = SPARSE_LBP;
			}

			switch (type)
			{
				case SPARSE_LBP:
					vertices[i].appearances[j]->update_optimized(NFfeaturesPool->getFeaturesFromPool(0), w[i][j], q[i][j], &groundTruthPositionsNF[INDEX(0, i, 2)]);
				break;
				case EXTENDED_SPARSE_LBP:
					// TODO: Implementation needed
				break;
				case HOG:
					// TODO: Implementation needed
				break;
			}

			fl_double_t sum = 0.0;
			//for (int k=0; k < vertices[i].appearances[j]->getFeatureDimension(); ++k)
			for (int k=0; k < vertices[i].appearances[j]->getLength(); ++k)
				sum += q[i][j][k];

			if (sum > maximum)
			{
				maximum = sum;
				best = j;
			}
		}

		vertices[i].best = best;
#ifdef DEBUG_MSG
		tim = timer.toc();
		std::cout << "GetQG: Node " << i << " took " << tim << " ms" << std::endl;
#endif
	}

	for (uint32_t i = 0; i < edges.size(); ++i)
	{
		edges[i]->update(w[kLandmarksCount+i][0], g[i], normalizedFrame, groundTruthPositionsNF);
	}
}

void CLandmark::getQGTableLoss()
{
	for (unsigned int i=0; i < vertices.size(); ++i)
	{
		int best = 0;

		for (unsigned int j=0; j < q[i].size(); ++j)
		{
			vertices[i].appearances[j]->update(normalizedFrame, w[i][j], q[i][j], 0x0);

			fl_double_t sum = 0.0;
			fl_double_t maximum = -DBL_MAX;
			for (int k=0; k < vertices[i].appearances[j]->getFeatureDimension(); ++k)
				sum += q[i][j][k];

			if (sum > maximum)
			{
				maximum = sum;
				best = j;
			}
		}

		vertices[i].best = best;
	}

	for (uint32_t i = 0; i < edges.size(); ++i)
		edges[i]->update(w[kLandmarksCount+i][0], g[i], normalizedFrame, groundTruthPositionsNF);
}

fl_double_t *CLandmark::getLossValues(int *position)
{
	for (int i=0; i < kLandmarksCount; ++i)
	{
		const int * ss = vertices[i].appearances[vertices[i].best]->getSearchSpace();
		const int * size = vertices[i].appearances[vertices[i].best]->getSize();

		int estimate[2] = {position[INDEX(0, i, 2)]-ss[0], position[INDEX(1, i, 2)]-ss[1]};

		int position = INDEX(estimate[1], estimate[0], size[1]);
		if (vertices[i].appearances[vertices[i].best]->hasLoss())
		{
			L[i] = vertices[i].appearances[vertices[i].best]->getLoss()->getLossAt(position);
		} else {
			L[i] = 0.0;
		}
	}

	return L;
}

void CLandmark::detect(cimg_library::CImg<unsigned char> *inputImage, int *boundingBox, fl_double_t * const ground_truth)
{
	CTimer timer, timerPart;

	timer.tic();
	timerPart.tic();

	// obtain normalized frame
	getNormalizedFrame(inputImage, boundingBox);

	timings.normalizedFrame = timerPart.toc();

	// if ground_truth provided, store it
	if (ground_truth)
	{
		setGroundTruth(ground_truth);
	}

	//
	applyNormalizationFactor();

	timerPart.tic();

	// update values of unary and pair-wise potentials
//	if (ground_truth)
	getQG();
//	else
//		getQGTableLoss();

	timings.features = timerPart.toc();

	timerPart.tic();

	// solve (Max,+) problem
	solver->solve(w, q, g, landmarksPositionsNF);

	// Get Q response
	for (int i=0; i < kLandmarksCount; ++i)
	{
		Q[i] = vertices[i].appearances[vertices[i].best]->getQvalue(&landmarksPositionsNF[INDEX(0, i, 2)], w[i][vertices[i].best]);
	}

	// Get G response
	for (int i=0; i < kEdgesCount; ++i)
	{
		G[i] = edges[i]->getGvalue(&landmarksPositionsNF[INDEX(0, edges[i]->getParent()->getNodeID(), 2)], &landmarksPositionsNF[INDEX(0, edges[i]->getChild()->getNodeID(), 2)], w[kLandmarksCount+i][0]);
	}

	timings.maxsum = timerPart.toc();

	// tranform coordinates from normalized image frame to original image
	transformCoordinatesNF2Image(landmarksPositionsNF, landmarksPositions);

	timings.overall = timer.toc();
}

void CLandmark::detect_optimized(cimg_library::CImg<unsigned char> *inputImage, int *boundingBox, fl_double_t * const ground_truth)
{
	CTimer timer, timerPart;

	timer.tic();
	timerPart.tic();

	// obtain normalized frame
	getNormalizedFrame(inputImage, boundingBox);

	timings.normalizedFrame = timerPart.toc();

	// if ground_truth provided, store it
	if (ground_truth)
	{
		setGroundTruth(ground_truth);
	}

	//
	applyNormalizationFactor();

	timerPart.tic();

	// update values of unary and pair-wise potentials
	NFfeaturesPool->updateNFmipmap(normalizedFrame->width(), normalizedFrame->height(), normalizedFrame->data()); // update NF mipmap + compute features
	getQG_optimized();

	timings.features = timerPart.toc();

	timerPart.tic();

	// solve (Max,+) problem
	solver->solve(w, q, g, landmarksPositionsNF);

	// Get Q response
	for (int i=0; i < kLandmarksCount; ++i)
	{
		Q[i] = vertices[i].appearances[vertices[i].best]->getQvalue(&landmarksPositionsNF[INDEX(0, i, 2)], w[i][vertices[i].best]);
	}

	// Get G response
	for (int i=0; i < kEdgesCount; ++i)
	{
		G[i] = edges[i]->getGvalue(&landmarksPositionsNF[INDEX(0, edges[i]->getParent()->getNodeID(), 2)], &landmarksPositionsNF[INDEX(0, edges[i]->getChild()->getNodeID(), 2)], w[kLandmarksCount+i][0]);
	}

	timings.maxsum = timerPart.toc();

	// tranform coordinates from normalized image frame to original image
	transformCoordinatesNF2Image(landmarksPositionsNF, landmarksPositions);

	timings.overall = timer.toc();
}

void CLandmark::detect_optimizedFromPool(int *boundingBox, fl_double_t *const ground_truth)
{
	CTimer timer, timerPart;

	timer.tic();
	timerPart.tic();

	// Update H and Hinv (to be able to compute landmark positions in original image)
	buildHandHinv(boundingBox);

	timings.normalizedFrame = timerPart.toc();

	// if ground_truth provided, store it
	if (ground_truth)
	{
		setGroundTruth(ground_truth);
	}

	//
	applyNormalizationFactor();

	timerPart.tic();

	// update values of unary and pair-wise potentials
	getQG_optimized();

	timings.features = timerPart.toc();

	timerPart.tic();

	// solve (Max,+) problem
	solver->solve(w, q, g, landmarksPositionsNF);

	// Get Q response
	for (int i=0; i < kLandmarksCount; ++i)
	{
		Q[i] = vertices[i].appearances[vertices[i].best]->getQvalue(&landmarksPositionsNF[INDEX(0, i, 2)], w[i][vertices[i].best]);
	}

	// Get G response
	for (int i=0; i < kEdgesCount; ++i)
	{
		G[i] = edges[i]->getGvalue(&landmarksPositionsNF[INDEX(0, edges[i]->getParent()->getNodeID(), 2)],
				&landmarksPositionsNF[INDEX(0, edges[i]->getChild()->getNodeID(), 2)], w[kLandmarksCount+i][0]);
	}

	timings.maxsum = timerPart.toc();

	// tranform coordinates from normalized image frame to original image
	transformCoordinatesNF2Image(landmarksPositionsNF, landmarksPositions);

	timings.overall = timer.toc();
}

void CLandmark::detect_mirrored(cimg_library::CImg<unsigned char> *inputImage, int *boundingBox, fl_double_t * const ground_truth)
{
	CTimer timer, timerPart;

	timer.tic();
	timerPart.tic();

	// obtain normalized frame
	getNormalizedFrame(inputImage, boundingBox);

	(*this->normalizedFrame) = this->normalizedFrame->get_mirror('x');

	timings.normalizedFrame = timerPart.toc();

	// if ground_truth provided, store it
	if (ground_truth)
	{
		setGroundTruth(ground_truth);
	}

	//
	applyNormalizationFactor();

	timerPart.tic();

	// update values of unary and pair-wise potentials
	getQG();

	timings.features = timerPart.toc();

	timerPart.tic();

	// solve (Max,+) problem
	solver->solve(w, q, g, landmarksPositionsNF);

	// Get Q response
	for (int i=0; i < kLandmarksCount; ++i)
	{
		Q[i] = vertices[i].appearances[vertices[i].best]->getQvalue(&landmarksPositionsNF[INDEX(0, i, 2)], w[i][vertices[i].best]);
	}

	// Get G response
	for (int i=0; i < kEdgesCount; ++i)
	{
		G[i] = edges[i]->getGvalue(&landmarksPositionsNF[INDEX(0, edges[i]->getParent()->getNodeID(), 2)],
				&landmarksPositionsNF[INDEX(0, edges[i]->getChild()->getNodeID(), 2)], w[kLandmarksCount+i][0]);
	}

	timings.maxsum = timerPart.toc();

	// tranform coordinates from normalized image frame to original image
	for (int i=0; i < 2*kLandmarksCount; i+=2)
	{
		landmarksPositionsNF[i] = baseWindow[0] - landmarksPositionsNF[i];
	}
	transformCoordinatesNF2Image(landmarksPositionsNF, landmarksPositions);

	timings.overall = timer.toc();
}

void CLandmark::applyNormalizationFactor()
{
	// vertices
	for (int i=0; i < kLandmarksCount; ++i)
	{
		for (unsigned int k=0; k < vertices[i].appearances.size(); ++k)
		{
			vertices[i].appearances[k]->setLossNormalizationFactor(normalizationFactor);
		}
	}

	// edges
	for (int i=0; i < kEdgesCount; ++i)
	{
		edges[i]->setLossNormalizationFactor(normalizationFactor);
	}
}

void CLandmark::detect_base(cimg_library::CImg<unsigned char> *inputImage, int * const ground_truth)
{
	CTimer timer, timerPart;

	timer.tic();
	timerPart.tic();

	// if ground_truth provided, store it
	if (ground_truth)
	{
		setGroundTruth(ground_truth);
	}

	// update normalization factor in appearance model and deformation cost
	applyNormalizationFactor();

	// copy normalized frame
	for (int x = 0; x < normalizedFrame->width(); ++x)
		for (int y = 0; y < normalizedFrame->height(); ++y)
			(*normalizedFrame)(x, y) = (*inputImage)(x, y);

	timings.normalizedFrame = timerPart.toc();

	timerPart.tic();

	// update values of unary and pair-wise potentials
//	if (ground_truth)
	getQG();
//	else
//		getQGTableLoss();

	timings.features = timerPart.toc();

	timerPart.tic();

	// solve (Max,+) problem
	solver->solve(w, q, g, landmarksPositionsNF);

	// Get Q response
	for (int i=0; i < kLandmarksCount; ++i)
	{
		Q[i] = vertices[i].appearances[vertices[i].best]->getQvalue(&landmarksPositionsNF[INDEX(0, i, 2)], w[i][vertices[i].best]);
	}

	// Get G response
	for (int i=0; i < kEdgesCount; ++i)
	{
		G[i] = edges[i]->getGvalue(&landmarksPositionsNF[INDEX(0, edges[i]->getParent()->getNodeID(), 2)], &landmarksPositionsNF[INDEX(0, edges[i]->getChild()->getNodeID(), 2)], w[kLandmarksCount+i][0]);
	}

	timings.maxsum = timerPart.toc();

	timings.overall = timer.toc();
}

void CLandmark::detect_base_optimized(cimg_library::CImg<unsigned char> *inputImage, int *const ground_truth)
{
	CTimer timer, timerPart;

	timer.tic();
	timerPart.tic();

	// if ground_truth provided, store it
	if (ground_truth)
	{
		setGroundTruth(ground_truth);
	}

	// update normalization factor in appearance model and deformation cost
	applyNormalizationFactor();

	timings.normalizedFrame = timerPart.toc();

	timerPart.tic();

	NFfeaturesPool->updateNFmipmap(normalizedFrame->width(), normalizedFrame->height(), inputImage->data()); // update NF mipmap + compute features
	getQG_optimized();

	timings.features = timerPart.toc();

	timerPart.tic();

	// solve (Max,+) problem
	solver->solve(w, q, g, landmarksPositionsNF);

	// Get Q response
	for (int i=0; i < kLandmarksCount; ++i)
	{
		Q[i] = vertices[i].appearances[vertices[i].best]->getQvalue(&landmarksPositionsNF[INDEX(0, i, 2)], w[i][vertices[i].best]);
	}

	// Get G response
	for (int i=0; i < kEdgesCount; ++i)
	{
		G[i] = edges[i]->getGvalue(&landmarksPositionsNF[INDEX(0, edges[i]->getParent()->getNodeID(), 2)],
				&landmarksPositionsNF[INDEX(0, edges[i]->getChild()->getNodeID(), 2)], w[kLandmarksCount+i][0]);
	}

	timings.maxsum = timerPart.toc();

	timings.overall = timer.toc();
}

void CLandmark::detect_base_optimized(int *const ground_truth)
{
	CTimer timer, timerPart;

	timer.tic();
	timerPart.tic();

	// if ground_truth provided, store it
	if (ground_truth)
	{
		setGroundTruth(ground_truth);
	}

	// update normalization factor in appearance model and deformation cost
	applyNormalizationFactor();

	timings.normalizedFrame = timerPart.toc();

	timerPart.tic();

	getQG_optimized();

	timings.features = timerPart.toc();

	timerPart.tic();

	// solve (Max,+) problem
	solver->solve(w, q, g, landmarksPositionsNF);

	// Get Q response
	for (int i=0; i < kLandmarksCount; ++i)
	{
		Q[i] = vertices[i].appearances[vertices[i].best]->getQvalue(&landmarksPositionsNF[INDEX(0, i, 2)], w[i][vertices[i].best]);
	}

	// Get G response
	for (int i=0; i < kEdgesCount; ++i)
	{
		G[i] = edges[i]->getGvalue(&landmarksPositionsNF[INDEX(0, edges[i]->getParent()->getNodeID(), 2)],
				&landmarksPositionsNF[INDEX(0, edges[i]->getChild()->getNodeID(), 2)], w[kLandmarksCount+i][0]);
	}

	timings.maxsum = timerPart.toc();

	timings.overall = timer.toc();
}

void CLandmark::nodemax_base(cimg_library::CImg<unsigned char> *inputImage, int *const ground_truth)
{
	setGroundTruth(ground_truth);

	// update normalization factor in appearance model and deformation cost
	applyNormalizationFactor();

	// copy normalized frame
	for (int x = 0; x < normalizedFrame->width(); ++x)
		for (int y = 0; y < normalizedFrame->height(); ++y)
			(*normalizedFrame)(x, y) = (*inputImage)(x, y);

	// get Qs
	getQG();

	// Solve the maximization	\hat{s} = \max_{s \in S} [ \ell(s^i, s) + < w, \Psi(I^i, s) > ]
	for (int i=0; i < kLandmarksCount; ++i)
	{
		fl_double_t maximum = -DBL_MAX;
		int argmax = 0;
		for (int j=0; j < vertices.at(i).appearances[vertices.at(i).best]->getLength(); ++j)
		{
			if (q[i][vertices.at(i).best][j] > maximum)
			{
				maximum = q[i][vertices.at(i).best][j];
				argmax = j;
			}
		}

		//
		const int *size = vertices.at(i).appearances[vertices.at(i).best]->getSize();
		int offset[2] = { vertices.at(i).appearances[vertices.at(i).best]->getSearchSpace()[0],
						  vertices.at(i).appearances[vertices.at(i).best]->getSearchSpace()[1] };

		landmarksPositionsNF[INDEX(0, i, 2)] = argmax / size[1] + offset[0];	// x-coordinate
		landmarksPositionsNF[INDEX(1, i, 2)] = argmax % size[1] + offset[1];	// y-coordinate
	}
}

fl_double_t* CLandmark::getFeatures(cimg_library::CImg<unsigned char> *inputImage, int * const boundingBox, int * const configuration)
{
	// obtain normalized frame
	getNormalizedFrame(inputImage, boundingBox);

	// update values of unary and pair-wise potentials
	getQG();

	// build vector psi
	getFeatures(configuration);

	return psi;
}

fl_double_t *CLandmark::getFeatures_base(cimg_library::CImg<unsigned char> *nf, int * const configuration)
{
	for (int x = 0; x < normalizedFrame->width(); ++x)
		for (int y = 0; y < normalizedFrame->height(); ++y)
			(*normalizedFrame)(x, y) = (*nf)(x, y);

	// update values of unary and pair-wise potentials
	getQG();

	// build vector psi
	getFeatures(configuration);

	return psi;
}

fl_double_t *CLandmark::getFeatures_base_optimized(int * const configuration)
{
	// update values of unary and pair-wise potentials
	getQG_optimized();

	// build vector psi
	getFeatures(configuration);

	return psi;
}

fl_double_t* CLandmark::getFeatures(int * const configuration)
{
	int psi_index = 0;
	int index = 0;
	int Si[2];
	int Sj[2];
	int P[2] = {0, 0};
	int parentID = 0;
	int childID = 0;

	// build vector psi

	// vertices
	for (int i=0; i < kLandmarksCount; ++i)
	{
		int best = vertices[i].best;

		P[0] = configuration[INDEX(0, i, 2)] - vertices[i].appearances[best]->getSearchSpace()[0];
		P[1] = configuration[INDEX(1, i, 2)] - vertices[i].appearances[best]->getSearchSpace()[1];
		index = INDEX(P[1], P[0], vertices.at(i).appearances[best]->getSize()[1]);

		vertices[i].appearances[best]->getFeatureAt(index, &psi[psi_index]);

		psi_index += vertices[i].appearances[best]->getFeatureDimension();
	}

	// edges
	for (int i=0; i < kEdgesCount; ++i)
	{
		int *tmp = new int[edges[i]->getDimension()];

		parentID = edges.at(i)->getParent()->getNodeID();
		childID = edges.at(i)->getChild()->getNodeID();

		Si[0] = configuration[INDEX(0, parentID, 2)];
		Si[1] = configuration[INDEX(1, parentID, 2)];
		Sj[0] = configuration[INDEX(0, childID, 2)];
		Sj[1] = configuration[INDEX(1, childID, 2)];

		//edges.at(i)->getDeformationCostAt(&Si[0], &Sj[0], (int*)&psi[psi_index]);
		edges.at(i)->getDeformationCostAt(&Si[0], &Sj[0], tmp);
		for (int j=0; j < edges[i]->getDimension(); ++j)
			psi[psi_index+j] = (fl_double_t)tmp[j];

		psi_index += edges.at(i)->getDimension();

		delete [] tmp;
	}

	return psi;
}

void CLandmark::setW(fl_double_t * const input_w)
{
	int start = 0;
	int end = 0;

	// vertices
	for (int i=0; i < kLandmarksCount; ++i)
	{
		int best = vertices[i].best;

		start = end;
		end += vertices[i].appearances[best]->getFeatureDimension();

		memcpy(w[i][best], &input_w[start], (end-start)*sizeof(fl_double_t));
	}

	// edges
	for (int i=0; i < kEdgesCount; ++i)
	{
		start = end;
		end += edges[i]->getDimension();

		memcpy(w[kLandmarksCount+i][0], &input_w[start], (end-start)*sizeof(fl_double_t));
	}
}

fl_double_t * CLandmark::getW(void)
{
	fl_double_t * W = new fl_double_t[wDimension];

	int start = 0;
	int end = 0;

	// vertices
	for (int i=0; i < kLandmarksCount; ++i)
	{
		int best = vertices[i].best;

		start = end;
		end += vertices[i].appearances[best]->getFeatureDimension();

		memcpy(&W[start], w[i][best], (end-start)*sizeof(fl_double_t));
	}

	// edges
	for (int i=0; i < kEdgesCount; ++i)
	{
		start = end;
		end += edges[i]->getDimension();

		memcpy(&W[start], w[kLandmarksCount+i][0], (end-start)*sizeof(fl_double_t));
	}

	return W;
}

void CLandmark::setNodesW(fl_double_t *const input_w)
{
	int start = 0;
	int end = 0;

	// vertices
	for (int i=0; i < kLandmarksCount; ++i)
	{
		int best = vertices[i].best;

		start = end;
		end += vertices[i].appearances[best]->getFeatureDimension();

		memcpy(w[i][best], &input_w[start], (end-start)*sizeof(fl_double_t));
	}
}

void CLandmark::transformCoordinatesImage2NF(fl_double_t *const input, int *output)
{
	for (int i=0; i < 2*kLandmarksCount; i+=2)
	{
		output[i] = floor(Hinv[INDEX(0, 0, 3)]*input[i] + Hinv[INDEX(0, 1, 3)]*input[i+1] + Hinv[INDEX(0, 2, 3)] + 0.5);
		output[i+1] = floor(Hinv[INDEX(1, 0, 3)]*input[i] + Hinv[INDEX(1, 1, 3)]*input[i+1] + Hinv[INDEX(1, 2, 3)] + 0.5);;
	}
}

void CLandmark::transformCoordinatesNF2Image(int *const input, fl_double_t *output)
{
	for (int i=0; i < 2*kLandmarksCount; i+=2)
	{
		output[i] = H[INDEX(0, 0, 3)]*input[i] + H[INDEX(0, 1, 3)]*input[i+1] + H[INDEX(0, 2, 3)];
		output[i+1] = H[INDEX(1, 0, 3)]*input[i] + H[INDEX(1, 1, 3)]*input[i+1] + H[INDEX(1, 2, 3)];
	}
}

void CLandmark::buildHandHinv(int *boundingBox)
{
	double scalefac[2];
	double C[] = { (boundingBox[INDEX(0, 0, 2)]+boundingBox[INDEX(0, 2, 2)])/2.0,
				   (boundingBox[INDEX(1, 0, 2)]+boundingBox[INDEX(1, 2, 2)])/2.0};

	for (int i=0; i < 4; ++i)
	{
		BB[INDEX(0, i, 2)] = C[0] + baseWindowMargin[0]*(boundingBox[INDEX(0, i, 2)]-C[0]);
		BB[INDEX(1, i, 2)] = C[1] + baseWindowMargin[1]*(boundingBox[INDEX(1, i, 2)]-C[1]);
	}

	// get scale factor
	scalefac[0] = sqrt( (float)(square( BB[INDEX(0, 0, 2)]-BB[INDEX(0, 3, 2)] )
				  + square(BB[INDEX(1, 0, 2)]-BB[INDEX(1, 3, 2)]) ) ) / baseWindow[0];
	scalefac[1] = sqrt( (float)(square( BB[INDEX(0, 2, 2)]-BB[INDEX(0, 3, 2)] )
				  + square(BB[INDEX(1, 2, 2)]-BB[INDEX(1, 3, 2)]) ) ) / baseWindow[1];

	// get angle of the in-plane rotation
	double angle = (double)std::asin(
					   float(fabs(BB[INDEX(1, 0, 2)]-BB[INDEX(1, 1, 2)])/sqrt( (float)(square( BB[INDEX(0, 0, 2)]-BB[INDEX(0, 1, 2)] )
				   + square( BB[INDEX(1, 0, 2)]-BB[INDEX(1, 1, 2)] )) ) ));

	if (BB[INDEX(1, 1, 2)] > BB[INDEX(1, 0, 2)])
	{
		angle = -angle;
	}

	H[INDEX(0, 0, 3)] = scalefac[0]*cos(angle);
	H[INDEX(0, 1, 3)] = scalefac[0]*sin(angle);
	H[INDEX(0, 2, 3)] = BB[INDEX(0, 0, 2)];
	H[INDEX(1, 0, 3)] = -scalefac[1]*sin(angle);
	H[INDEX(1, 1, 3)] = scalefac[1]*cos(angle);
	H[INDEX(1, 2, 3)] = BB[INDEX(1, 0, 2)];
	H[INDEX(2, 0, 3)] = 0;
	H[INDEX(2, 1, 3)] = 0;
	H[INDEX(2, 2, 3)] = 1;

	// prepare the inverse matrix as well (needed for the back-transformation of landmarks to the original image)
	double sfinv[2] = {1.0/scalefac[0], 1.0/scalefac[1]};
	Hinv[INDEX(0, 0, 3)] = sfinv[0]*cos(-angle);
	Hinv[INDEX(0, 1, 3)] = sfinv[1]*sin(-angle);
	Hinv[INDEX(0, 2, 3)] = cos(-angle)*(-BB[INDEX(0, 0, 2)]*sfinv[0]) + sin(-angle)*(-BB[INDEX(1, 0, 2)]*sfinv[1]);
	Hinv[INDEX(1, 0, 3)] = -sfinv[0]*sin(-angle);
	Hinv[INDEX(1, 1, 3)] = sfinv[1]*cos(-angle);
	Hinv[INDEX(1, 2, 3)] = (-BB[INDEX(0, 0, 2)]*sfinv[0])*-sin(-angle) + (-BB[INDEX(1, 0, 2)]*sfinv[1])*cos(-angle);
	Hinv[INDEX(2, 0, 3)] = 0;
	Hinv[INDEX(2, 1, 3)] = 0;
	Hinv[INDEX(2, 2, 3)] = 1;
}


void CLandmark::getNormalizedFrame(cimg_library::CImg<unsigned char> *inputImage, int *boundingBox)
{
	// Update H and Hinv needed for affine transformation (NF <-> Image)
	buildHandHinv(boundingBox);

	// Fill in NF
	float u, v;
	for (int x=0; x < baseWindow[0]; ++x)
	{
		for (int y=0; y < baseWindow[1]; ++y)
		{
			u = H[INDEX(0, 0, 3)]*x + H[INDEX(0, 1, 3)]*y + H[INDEX(0, 2, 3)];
			v = H[INDEX(1, 0, 3)]*x + H[INDEX(1, 1, 3)]*y + H[INDEX(1, 2, 3)];
			(*normalizedFrame)(x, y) = inputImage->linear_atXY(u, v);
		}
	}
}

void CLandmark::computeWdimension(void)
{
	wDimension = 0;

	// Nodes
	for (unsigned int i=0; i < vertices.size(); ++i)
	{
		int best = vertices[i].best;
		wDimension += vertices[i].appearances[best]->getFeatureDimension();
	}

	psiNodesDimension = wDimension;

	// Edges
	for (unsigned int i=0; i < edges.size(); ++i)
	{
		wDimension += edges[i]->getDimension();
	}
}

void CLandmark::write(const char *filename, bool writeW)
{
	XmlStorage fs(filename, FILE_WRITE);

	time_t rawtime;
	time(&rawtime);

	fs
	<< "name" << this->name
	<< "version" << asctime(localtime(&rawtime))
	<< "num_nodes" << kLandmarksCount
	<< "num_edges" << kEdgesCount
	<< "graph_type" << TREE
	<< "bw_width" << baseWindow[0]
	<< "bw_height" << baseWindow[1]
	<< "bw_margin_x" << baseWindowMargin[0]
	<< "bw_margin_y" << baseWindowMargin[1];

	fs << "Nodes" << "[";
	for (int i=0; i < kLandmarksCount; ++i)
	{
		fs << "Node";
		fs << "{";

		vertices[i].write(fs);

		fs << "Appearances" << "[";
		for (unsigned int j=0; j < vertices[i].appearances.size(); ++j)
		{
			vertices[i].appearances[j]->write(fs, w[i][j], writeW);
		}
		fs << "]";

		fs << "}";
	}
	fs << "]";

	fs << "Edges" << "[";
	for (int i=0; i < kEdgesCount; ++i)
	{
		edges.at(i)->write(fs, w[kLandmarksCount+i][0], writeW);
	}
	fs << "]";

	fs.release();
}

int *CLandmark::getEdges(void)
{
	int *edgeList = new int[2*kEdgesCount];

	int edgeId = 0;
	for (unsigned int i=0; i < edges.size(); ++i)
	{
		edgeList[edgeId++] = edges[i]->getParent()->getNodeID();
		edgeList[edgeId++] = edges[i]->getChild()->getNodeID();
	}

	return edgeList;
}

int *CLandmark::getNodesDimensions(void)
{
	int *dimensions = new int[kLandmarksCount];

	for (int i=0; i < kLandmarksCount; ++i)
	{
		dimensions[i] = vertices[i].appearances[vertices[i].best]->getFeatureDimension();
	}

	return dimensions;
}

int *CLandmark::getEdgesDimensions(void)
{
	int *dimensions = new int[kEdgesCount];

	for (int i=0; i < kEdgesCount; ++i)
	{
		dimensions[i] = edges[i]->getDimension();
	}

	return dimensions;
}

int *CLandmark::getNodesSearchSpaces(void)
{
	int *searchSpaces = new int[4*kLandmarksCount];

	int idx=0;
	for (unsigned int i=0; i < vertices.size(); ++i)
	{
		int best = vertices[i].best;
		const int *ss = vertices[i].appearances[best]->getSearchSpace();
		memcpy(&searchSpaces[idx], ss, sizeof(int)*4);
		idx += 4;
	}

	return searchSpaces;
}

int *CLandmark::getWindowSizes(void)
{
	int *winSizes = new int[2*kLandmarksCount];

	int idx=0;
	for (unsigned int i=0; i < vertices.size(); ++i)
	{
		int best = vertices[i].best;
		const int *wins = vertices[i].appearances[best]->getWindowSize();
		memcpy(&winSizes[idx], wins, sizeof(int)*2);
		idx += 2;
	}

	return winSizes;
}
