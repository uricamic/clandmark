#include "CMaxSumSolver.h"

using namespace clandmark;

CMaxSumSolver::CMaxSumSolver(std::vector< Vertex > * const vertices_, std::vector< CDeformationCost* > * const edges_, int bw_w, int bw_h)
{
	vertices = vertices_;
	edges = edges_;

	// DT
	bw[0] = bw_w;
	bw[1] = bw_h;
	tmpFsize = CLANDMARK_MAX(bw_w, bw_h);
	tmpA = new fl_double_t[tmpFsize];
	tmpB = new fl_double_t[tmpFsize];
	rectangle = new fl_double_t[bw[0]*bw[1]];
	tmpDT = new fl_double_t[bw[0]*bw[1]];
	tmpI = new int[tmpFsize];
	iX = new int[bw[0]*bw[1]];
	iY = new int[bw[0]*bw[1]];
}

CMaxSumSolver::~CMaxSumSolver()
{
	// DT
	if (tmpA)
	{
		delete [] tmpA;
	}
	if (tmpB)
	{
		delete [] tmpB;
	}
	if (tmpI)
	{
		delete [] tmpI;
	}
	if (rectangle)
	{
		delete [] rectangle;
	}
	if (tmpDT)
	{
		delete [] tmpDT;
	}
	if (iX)
	{
		delete [] iX;
	}
	if (iY)
	{
		delete [] iY;
	}
}
