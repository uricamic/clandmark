#include "CDisplacementDeformationCost.h"

#include <cstring>
#include <limits>

using namespace clandmark;

CDisplacementDeformationCost::CDisplacementDeformationCost(CAppearanceModel * const parent, CAppearanceModel * const child, int dimension)
	: CDeformationCost(parent, child)
{
	kDimension = dimension;

	parentLength = parent->getLength();
	childLength = child->getLength();
}

CDisplacementDeformationCost::~CDisplacementDeformationCost()
{
}

void CDisplacementDeformationCost::getDeformationCostAt(int * const s_i, int * const s_j, int *deformation_cost)
{
	deformation_cost[0]	= s_j[0] - s_i[0];							// dx (= Sj_x - Si_x)
	deformation_cost[1] = s_j[1] - s_i[1];							// dy
	deformation_cost[2] = deformation_cost[0]*deformation_cost[0];	// dx^2
	deformation_cost[3] = deformation_cost[1]*deformation_cost[1];	// dy^2
	if (kDimension == 5)
		deformation_cost[4] = 1;
}

void CDisplacementDeformationCost::computeDeformationCosts()
{
	/*
	const int *i_size = parent->getSize();
	const int *j_size = child->getSize();
	int SiOffset[2] = { parent->getSearchSpace()[0], parent->getSearchSpace()[1] };
	int SjOffset[2] = { child->getSearchSpace()[0], child->getSearchSpace()[1] };
	int Si[2] = {0, 0};
	int Sj[2] = {0, 0};
	int index = 0;

	for (int i = 0; i < parentLength; ++i)
	{
		Si[0] = i / i_size[1] + SiOffset[0];		// x-coordinate
		Si[1] = i % i_size[1] + SiOffset[1];		// y-coordinate

		index = 0;

		for (int j = 0; j < childLength; ++j)
		{
			Sj[0]=j / j_size[1] + SjOffset[0];		// x-coordinate
			Sj[1]=j % j_size[1] + SjOffset[1];		// y-coordinate
			getDeformationCostAt(Si, Sj, &deformationCosts[i][index]);
			index+=kDimension;
		}
	}
	*/
}

void CDisplacementDeformationCost::dotProductWithWg(fl_double_t *const w, fl_double_t *g, const int index)
{
	fl_double_t dotProduct = 0.0;

	int defCost[4] = {0, 0, 0, 0};
	const int *i_size = parent->getSize();
	const int *j_size = child->getSize();
	int SiOffset[2] = { parent->getSearchSpace()[0], parent->getSearchSpace()[1] };
	int SjOffset[2] = { child->getSearchSpace()[0], child->getSearchSpace()[1] };
	int Si[2] = {index / i_size[1] + SiOffset[0], index % i_size[1] + SiOffset[1]};
	int Sj[2] = {0, 0};

	for (int i=0; i < childLength; ++i)
	{
		dotProduct = 0.0;

		Sj[0]=i / j_size[1] + SjOffset[0];		// x-coordinate
		Sj[1]=i % j_size[1] + SjOffset[1];		// y-coordinate
		getDeformationCostAt(Si, Sj, &defCost[0]);

		for (int j=0; j < kDimension; ++j)
		{
			dotProduct += w[j]*defCost[j];
		}

		g[i] = dotProduct;
	}
}

void CDisplacementDeformationCost::update(fl_double_t * const w, fl_double_t *g, cimg_library::CImg<unsigned char> *image_data, int * const ground_truth)
{
	// Not needed for this type of deformation cost!!!
}

fl_double_t CDisplacementDeformationCost::getGvalue(int *const s_i, int *const s_j, fl_double_t *const w)
{
	fl_double_t dotProduct = 0.0;
	int * defCost = new int[kDimension];
//	int defCost[4];

	getDeformationCostAt(s_i, s_j, &defCost[0]);

	for (int j=0; j < kDimension; ++j)
	{
		dotProduct += w[j]*defCost[j];
	}

	delete [] defCost;

	return dotProduct;
}

void CDisplacementDeformationCost::write(XmlStorage &fs, fl_double_t * const w, bool writeW)
{
	fs << "Edge";
	fs << "{"
	   << "ParentID" << parent->getNodeID()
	   << "ChildID" << child->getNodeID()
	   << "Type" << DISPLACEMENT_VECTOR
	   << "Dims" << kDimension;

	if (loss)
		fs << "LossType" << loss->getName();

	if (writeW)
	{
		fs << "w";
		fs.writeRaw(w, sizeof(fl_double_t)*kDimension);
	}

	fs << "}";
}

