//
//  CZeroLoss.cpp
//
//
//  Created by Kostia on 1/27/14.
//
//

#include "CZeroLoss.h"

using namespace clandmark;

CZeroLoss::CZeroLoss(int size_w, int size_h)
: CLoss(size_w, size_h)
{
	;
}

CZeroLoss::~CZeroLoss()
{
	;
}

fl_double_t CZeroLoss::computeLoss(fl_double_t * const ground_truth, fl_double_t * const estimate)
{
	return 0;
}

fl_double_t CZeroLoss::computeLoss(int * const ground_truth, int * const estimate)
{
	return 0;
}
