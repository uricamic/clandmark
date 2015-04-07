#include "CFeatures.h"

using namespace clandmark;

CFeatures::CFeatures(int width, int height, int pyrLevels, int *cumwidths) : kWidth(width), kHeight(height), kLevels(pyrLevels)
{
	cumWidths = cumwidths;
}

CFeatures::~CFeatures()
{
	;
}
