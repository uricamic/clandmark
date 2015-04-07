#include "CFeaturePool.h"

#include <cstring>

using namespace clandmark;

CFeaturePool::CFeaturePool() : kWidth(-1), kHeight(-1)
{
	NF_mipmap = 0x0;
	cumWidths = 0x0;
}

CFeaturePool::CFeaturePool(int width, int height, unsigned char *nf)
	: kWidth(width), kHeight(height)
{
	NF_mipmap = new unsigned char[2*kWidth*kHeight];
	memset(NF_mipmap, 0, 2*kWidth*kHeight*sizeof(unsigned char));

	tmpNF = new unsigned char[width*height];
	memset(NF_mipmap, 0, width*height*sizeof(unsigned char));

	kPyrLevels = (int)floor(log2((double)height)+1+0.5);	// floor(x+0.5) is equivalent to round(x)
	cumWidths = new int[kPyrLevels];
	cumWidths[0] = 0;
	int w = width;
	maxPyrLBP = 0;
	for (int i=1; i < kPyrLevels; ++i)
	{
		cumWidths[i] = cumWidths[i-1] + w;
		w /= 2;
		if (w <= 3)
		{
			maxPyrLBP = i;
		}
	}

	if (nf)
	{
		createNFmipmap(width, height, nf);
	}
}

CFeaturePool::~CFeaturePool()
{
	if (NF_mipmap)
		delete [] NF_mipmap;

	if (tmpNF)
		delete [] tmpNF;

	if (cumWidths)
		delete [] cumWidths;

	if (!featurePool.empty())
	{
		for (unsigned int i=0; i < featurePool.size(); ++i)
		{
			delete featurePool[i];
		}
		featurePool.clear();
	}
}

void CFeaturePool::createNFmipmap(int width, int height, unsigned char *const nf)
{
	// Watch out, nf is row-wise ordered (comes from cimg->data()) !!!

	memcpy(tmpNF, nf, (width*height)*sizeof(unsigned char));

	for (int i=0; i < kPyrLevels; ++i)
	{
		for (int y=0; y < height; ++y)
		{
			for (int x=cumWidths[i], x2=0; x < cumWidths[i]+width; ++x, ++x2)
			{
				NF_mipmap[INDEX(y, x, kHeight)] = tmpNF[INDEX(x2, y, kHeight)];
			}
		}

		// Downsample the NF for the next scale-level in the pyramid
		width /= 2;

		for(int x=0; x < width; x++)
		{
			for(int j=0; j < height; j++)
			{
				tmpNF[INDEX(j, x, kHeight)] = (unsigned char) ((tmpNF[INDEX(j, 2*x, kHeight)] + tmpNF[INDEX(j, 2*x+1, kHeight)])/2.0);
			}
		}

		height /= 2;

		for(int y=0; y < height; y++)
		{
			for(int j=0; j < width; j++)
			{
				tmpNF[INDEX(y, j, kHeight)] = (unsigned char) ((tmpNF[INDEX(2*y, j, kHeight)] + tmpNF[INDEX(2*y+1, j, kHeight)])/2.0);
			}
		}
	}
}

void CFeaturePool::updateNFmipmap(int width, int height, unsigned char *const nf)
{
	createNFmipmap(width, height, nf);

	computeFeatures();
}

void CFeaturePool::computeFeatures(void)
{
	for (unsigned int i=0; i < featurePool.size(); ++i)
	{
		featurePool[i]->setNFmipmap(NF_mipmap);
		featurePool[i]->compute();
	}
}

void CFeaturePool::updateFeaturesRaw(int index, void *data)
{
	featurePool.at(index)->setFeaturesRaw(data);
}
