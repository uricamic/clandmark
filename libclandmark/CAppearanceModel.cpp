#include "CAppearanceModel.h"

using namespace clandmark;

CAppearanceModel::CAppearanceModel(
		int nodeID,
		int search_space_minx,
		int search_space_miny,
		int search_space_maxx,
		int search_space_maxy,
		int window_size_w,
		int window_size_h,
		CLoss *loss_
		) : kNodeID(nodeID), kLength((search_space_maxx-search_space_minx+1)*(search_space_maxy-search_space_miny+1))
{
	size[0] = search_space_maxx-search_space_minx+1;
	size[1] = search_space_maxy-search_space_miny+1;

	searchSpace[0] = search_space_minx;
	searchSpace[1] = search_space_miny;
	searchSpace[2] = search_space_maxx;
	searchSpace[3] = search_space_maxy;

	windowSize[0] = window_size_w;
	windowSize[1] = window_size_h;

	windowPositions = new int[2*kLength];
	memset(windowPositions, 0, 2*kLength*sizeof(int));

	loss = loss_;

	imageData = 0x0;
	kFeatureDimension = 0;
	name = "";

	computeWindowPositions();
}

CAppearanceModel::~CAppearanceModel()
{
	if (windowPositions)
		delete [] windowPositions;

	if (loss)
		delete loss;

	NFfeatures = 0x0;
}

void CAppearanceModel::computeWindowPositions(void)
{
	int offset[2] = {searchSpace[0], searchSpace[1]};

	int halfWinSize[2];
	halfWinSize[0] = (windowSize[0] % 2) ? (windowSize[0]-1)/2 : windowSize[0]/2;
	halfWinSize[1] = (windowSize[1] % 2) ? (windowSize[1]-1)/2 : windowSize[1]/2;

	int index = 0;
	for (int x = 0; x < size[0]; ++x)
	{
		for (int y = 0; y < size[1]; ++y)
		{
			windowPositions[index]   = x + offset[0] - halfWinSize[0];		// x-coors
			windowPositions[index+1] = y + offset[1] - halfWinSize[1];		// y-coors
			index += 2;
		}
	}
}

void CAppearanceModel::update(cimg_library::CImg<unsigned char> *image_data, fl_double_t *const w, fl_double_t *q, int * const groundTruth)
{
	imageData = image_data;

	computeFeatures();

	// computeQ (= dot product of features and w)
	dotProductWithWq(w, q);

	if (loss)
	{
//		if (groundTruth)
		loss->addLoss(groundTruth, q, &searchSpace[0]);
//		else
//			loss->addLoss(q);
	}
}

void CAppearanceModel::update_optimized(CFeatures *const nffeatures, fl_double_t *const w, fl_double_t *q, int *const groundTruth)
{
	NFfeatures = nffeatures;

	computeFeatures_optimized();

	dotProductWithWq(w, q);

	if (loss)
	{
		loss->addLoss(groundTruth, q, &searchSpace[0]);
	}
}
