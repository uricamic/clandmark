#include "CSparseLBPAppearanceModel.h"

#include <cstring>

using namespace clandmark;

CSparseLBPAppearanceModel::CSparseLBPAppearanceModel(
		int nodeID,
		int search_space_min_x,	int search_space_min_y,	int search_space_max_x,	int search_space_max_y,
		int window_size_w,	int window_size_h,
		int height_of_pyramid,
		CLoss *loss_
		) : CAppearanceModel(
				nodeID,
				search_space_min_x,	search_space_min_y,	search_space_max_x,	search_space_max_y,
				window_size_w, window_size_h,
				loss_
			), kHeightOfPyramid(height_of_pyramid)
{
	kFeatureDimension = getDim(window_size_w, window_size_h, height_of_pyramid);
	kSparseFeatureDimension = kFeatureDimension/256;

	// allocate features
	LBPFeatures = new int[kLength*kSparseFeatureDimension];
	memset(LBPFeatures, 0, sizeof(int)*kLength*kSparseFeatureDimension);

	// prepare window
	window = new int[window_size_w*window_size_h];
	memset(window, 0, window_size_w*window_size_h*sizeof(int));

	// set feature type
	setType("SPARSE_LBP");
}

CSparseLBPAppearanceModel::~CSparseLBPAppearanceModel()
{
	if (LBPFeatures)
		delete [] LBPFeatures;
	if (window)
		delete [] window;
}

void CSparseLBPAppearanceModel::getFeatureAt(const int index, fl_double_t *feature_vector)
{
	// zero-out feature vector
	memset(feature_vector, 0, kFeatureDimension*sizeof(fl_double_t));

	for (int i=0; i < kSparseFeatureDimension; ++i)
	{
		feature_vector[LBPFeatures[kSparseFeatureDimension*index+i]] = 1.0;
	}
}

void CSparseLBPAppearanceModel::computeFeatures(void)
{
	int subWin[4] = {0, 0, windowSize[0], windowSize[1]};

	for (int i=0; i < kLength; ++i)
	{
		subWin[0] = windowPositions[INDEX(0, i, 2)];
		subWin[1] = windowPositions[INDEX(1, i, 2)];

		int index = 0;
		for (int x=subWin[0]; x < subWin[0]+subWin[2]; ++x)
		{
			for (int y=subWin[1]; y < subWin[1]+subWin[3]; ++y)
			{
				window[index++] = (int)(*imageData)(x, y);
			}
		}

		featuresSparse(&LBPFeatures[kSparseFeatureDimension*i]);
	}
}

void CSparseLBPAppearanceModel::computeFeatures_optimized(void)
{
	// Get pre-computed NF features mipmap
	unsigned char * NF_LBPs = (unsigned char*)NFfeatures->getFeatures();
	int * LBPvec = &LBPFeatures[0];

	int winW, winH;
	int idx = 0;
	int kHeight = NFfeatures->kHeight;
	int offset, x_init, y_init, x_limit, y_limit;

	for (int i=0; i < kLength; ++i)
	{
		winW = windowSize[0];
		winH = windowSize[1];

		x_init = windowPositions[INDEX(0, i, 2)];
		y_init = windowPositions[INDEX(1, i, 2)];

		offset = 0;

		// compile feature vector for this position
		for (int l=0; l < kHeightOfPyramid; ++l)
		{
			x_init = windowPositions[INDEX(0, i, 2)] >> l;
			y_init = windowPositions[INDEX(1, i, 2)] >> l;
			winW = windowSize[0] >> l;
			winH = windowSize[1] >> l;

			x_limit = x_init + winW;
			y_limit = y_init + winH;

			for (int y=y_init+1; y < y_limit-1; ++y)
			{
				for (int x=x_init+NFfeatures->cumWidths[l]+1; x < x_limit+NFfeatures->cumWidths[l]-1; ++x)
				{
					LBPvec[idx++] = offset + NF_LBPs[INDEX(y, x, kHeight)];
					offset += 256;
					//TODO: Rewrite these ^ 2 lines to enable parallelization!!!
				}
			}
		}
	}

	NF_LBPs = 0x0;
}

int CSparseLBPAppearanceModel::getDim(int img_nRows, int img_nCols, int nPyramids)
{
	int w = img_nCols;
	int h = img_nRows;
	int N=0;

	for(int i=0; (i < nPyramids) && (CLANDMARK_MIN(w,h) >= 3); ++i)
	{
		N += (w-2)*(h-2);

		if (w % 2)
			w--;
		if (h % 2)
			h--;
		w >>= 1;
		h >>= 1;
	}

	return N << 8;
}

void CSparseLBPAppearanceModel::featuresSparse(int *vec)
{
	int offset, ww, hh, x, y, idx, j;
	unsigned char pattern;
	int center;

	idx = 0;
	offset = 0;
	ww = windowSize[0];
	hh = windowSize[1];

	while (1)
	{
		for (x = 1; x < ww-1; ++x)
		{
			for (y = 1; y < hh-1; ++y)
			{
				pattern = 0;
				center = window[INDEX(y,x,windowSize[1])];
				if(window[INDEX(y-1,x-1,windowSize[1])] < center) pattern = pattern | 0x01;
				if(window[INDEX(y-1,x  ,windowSize[1])] < center) pattern = pattern | 0x02;
				if(window[INDEX(y-1,x+1,windowSize[1])] < center) pattern = pattern | 0x04;
				if(window[INDEX(y  ,x-1,windowSize[1])] < center) pattern = pattern | 0x08;
				if(window[INDEX(y  ,x+1,windowSize[1])] < center) pattern = pattern | 0x10;
				if(window[INDEX(y+1,x-1,windowSize[1])] < center) pattern = pattern | 0x20;
				if(window[INDEX(y+1,x  ,windowSize[1])] < center) pattern = pattern | 0x40;
				if(window[INDEX(y+1,x+1,windowSize[1])] < center) pattern = pattern | 0x80;

				vec[idx++] = offset + pattern;
				offset += 256;
			}
		}

		if (kSparseFeatureDimension <= idx)
			return;

		if (ww % 2 == 1)
			--ww;
		if (hh % 2 == 1)
			--hh;

		ww >>= 1;

		for (x=0; x < ww; ++x)
			for (j=0; j < hh; ++j)
				window[INDEX(j,x,windowSize[1])] = window[INDEX(j,2*x,windowSize[1])] + window[INDEX(j,2*x+1,windowSize[1])];

		hh >>= 1;

		for (y=0; y < hh; ++y)
			for (j=0; j < ww; ++j)
				window[INDEX(y,j,windowSize[1])] = window[INDEX(2*y,j,windowSize[1])] + window[INDEX(2*y+1,j,windowSize[1])];
	}

	return;
}

void CSparseLBPAppearanceModel::dotProductWithWq(fl_double_t *const w, fl_double_t *q)
{
	int temp_index = 0;
	fl_double_t dotProduct = 0.0;

	// sparse dot product of LBP features and vector w
	for (int i=0; i < kLength; ++i)
	{
		dotProduct = 0.0;
		for (int j=0; j < kSparseFeatureDimension; ++j)
		{
			temp_index = LBPFeatures[INDEX(j, i, kSparseFeatureDimension)];
			dotProduct += w[temp_index];
		}
		q[i] = dotProduct;
	}
}

fl_double_t CSparseLBPAppearanceModel::getQvalue(int *const s_i, fl_double_t *const w)
{
	int temp_index = 0;
	fl_double_t dotProduct = 0.0;

	int offset[2] = {searchSpace[0], searchSpace[1]};
	int i = INDEX(s_i[1]-offset[1], s_i[0]-offset[0], size[1]);

	for (int j=0; j < kSparseFeatureDimension; ++j)
	{
		temp_index = LBPFeatures[INDEX(j, i, kSparseFeatureDimension)];
		dotProduct += w[temp_index];
	}

	return dotProduct;
}

void CSparseLBPAppearanceModel::write(XmlStorage &fs, fl_double_t * const w, bool writeW) const
{
	fs << "Appearance";
	fs << "{"
//	  << "Type" << "SPARSE_LBP"
	   << "Type" << this->type
	   << "hop" << kHeightOfPyramid;

	if (writeW)
	{
		fs << "w";
		fs.writeRaw(w, sizeof(fl_double_t)*kFeatureDimension);
	}

	fs << "}";
}
