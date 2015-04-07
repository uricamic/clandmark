#include "CLoss.h"

// DEBUG
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
//------

using namespace clandmark;

CLoss::CLoss(int size_w, int size_h) : kLength(size_w*size_h)
{
	admissiblePositions = new int[2*kLength];

	int index = 0;
	for (int x = 0; x < size_w; ++x)
	{
		for (int y = 0; y < size_h; ++y)
		{
			admissiblePositions[index] = x;
			admissiblePositions[index+1] = y;
			index += 2;
		}
	}
}

CLoss::~CLoss()
{
	if (admissiblePositions)
		delete [] admissiblePositions;
}

void CLoss::addLoss(int * const ground_truth, fl_double_t *array, int * const offset)
{
	ELossType type = this->getType();

	if(type == NORMALIZED_EUCLIDEAN)
	{
		int estimate[2] = {0, 0};
		for (int i = 0; i < kLength; ++i)
		{
			estimate[0] = admissiblePositions[INDEX(0, i, 2)] + offset[0];
			estimate[1] = admissiblePositions[INDEX(1, i, 2)] + offset[1];

			array[i] += computeLoss(ground_truth, &estimate[0]);
		}
	} else if(type == TABLE_LOSS) {

		int estimate[2] = {0, 0};
		for (int i = 0; i < kLength; ++i)
		{
			estimate[0] = admissiblePositions[INDEX(0, i, 2)];
			estimate[1] = admissiblePositions[INDEX(1, i, 2)];

			array[i] += computeLoss(ground_truth, &estimate[0]);
		}
	} else {
		throw int();
	}
}

void CLoss::addLoss(fl_double_t *array)
{
	for (int i = 0; i < kLength; ++i)
	{
		array[i] += getLossAt(i);
	}
}
