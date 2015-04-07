#include "CNormalizedEuclideanLoss.h"

#include <cmath>

using namespace clandmark;

CNormalizedEuclideanLoss::CNormalizedEuclideanLoss(int size_w, int size_h)
	: CLoss(size_w, size_h)
{
	;
}

CNormalizedEuclideanLoss::~CNormalizedEuclideanLoss()
{
	;
}

fl_double_t CNormalizedEuclideanLoss::computeLoss(fl_double_t * const ground_truth, fl_double_t * const estimate)
{
	return sqrt((ground_truth[0]-estimate[0])*(ground_truth[0]-estimate[0])	+ (ground_truth[1]-estimate[1])*(ground_truth[1]-estimate[1]))*normalizationFactor;
}

fl_double_t CNormalizedEuclideanLoss::computeLoss(int * const ground_truth, int * const estimate)
{
	return sqrt(fl_double_t(ground_truth[0]-estimate[0])*(ground_truth[0]-estimate[0]) + (ground_truth[1]-estimate[1])*(ground_truth[1]-estimate[1]))*normalizationFactor;
}
