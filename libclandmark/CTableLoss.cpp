#include "CTableLoss.h"

using namespace clandmark;

CTableLoss::CTableLoss(int size_w, int size_h) : CLoss(size_w, size_h)
{
	table = new fl_double_t[kLength];
	memset(table, 0, sizeof(fl_double_t)*kLength);
	this->size_h = size_h;
	this->size_w = size_w;
}

CTableLoss::~CTableLoss()
{
	if (table)
		delete [] table;
}

fl_double_t CTableLoss::computeLoss(fl_double_t * const ground_truth, fl_double_t * const estimate)
{
	throw int();
}

fl_double_t CTableLoss::computeLoss(int * const ground_truth, int * const estimate)
{
	return table[ INDEX(estimate[1], estimate[0], size_h) ];
}

fl_double_t CTableLoss::getLossAt(int position)
{
	return table[position];
}
