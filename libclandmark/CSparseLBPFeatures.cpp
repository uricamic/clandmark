#include "CSparseLBPFeatures.h"

#include <cstring>

using namespace clandmark;

CSparseLBPFeatures::CSparseLBPFeatures(int width, int height, int pyrLevels, int *cumwidths)
	: CFeatures(width, height, pyrLevels, cumwidths)
{
	Wtimes2 = 2*kWidth;

	// Allocate "mipmap" structure for LBP features (on whole NF)
	LBP_NF = new unsigned char[Wtimes2*height];	// 1 + 1/2 + 1/4 + ... + 1/2^n = 2 => (2xW) x H = 2 x W x H

	memset(LBP_NF, 0, Wtimes2*height*sizeof(unsigned char));
}

CSparseLBPFeatures::~CSparseLBPFeatures()
{
	if (LBP_NF) // && !sharedFlag)
	{
		delete [] LBP_NF;
	}
}

void CSparseLBPFeatures::compute()
{
	// compute LBPs
	int width;
	int height;

	for (int i=0; i < kLevels; ++i)
	{
		width = kWidth >> i;
		height= kHeight >> i;

		unsigned char pattern;
		unsigned char center;

		for (int y=1; y < height-1; ++y)
		{
			for (int x=cumWidths[i]+1; x < cumWidths[i]+width-1; ++x)
			{
				pattern = 0;
				center = NFmipmap[INDEX(y, x, kHeight)];

				if (NFmipmap[INDEX(y-1, x-1, kHeight)] < center) pattern = pattern | 0x01;
				if (NFmipmap[INDEX(y-1,   x, kHeight)] < center) pattern = pattern | 0x02;
				if (NFmipmap[INDEX(y-1, x+1, kHeight)] < center) pattern = pattern | 0x04;
				if (NFmipmap[INDEX(  y, x-1, kHeight)] < center) pattern = pattern | 0x08;
				if (NFmipmap[INDEX(  y, x+1, kHeight)] < center) pattern = pattern | 0x10;
				if (NFmipmap[INDEX(y+1, x-1, kHeight)] < center) pattern = pattern | 0x20;
				if (NFmipmap[INDEX(y+1,	  x, kHeight)] < center) pattern = pattern | 0x40;
				if (NFmipmap[INDEX(y+1, x+1, kHeight)] < center) pattern = pattern | 0x80;

				LBP_NF[INDEX(y, x, kHeight)] = pattern;
			}
		}
	}
}

void CSparseLBPFeatures::setFeatures(CFeatures *const features)
{
	if (LBP_NF) // && !sharedFlag)
	{
		delete [] LBP_NF;
	}

	LBP_NF = (unsigned char*)features->getFeatures();
}

void CSparseLBPFeatures::setFeaturesRaw(void *data)
{
	if (!LBP_NF)
	{
		LBP_NF = new unsigned char[Wtimes2*kHeight];
	}

	memcpy(LBP_NF, data, Wtimes2*kHeight*sizeof(unsigned char));
}
