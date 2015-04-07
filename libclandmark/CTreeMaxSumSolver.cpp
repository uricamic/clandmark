#include "CTreeMaxSumSolver.h"

#include <cstring>
#include <cfloat>
#include <stdexcept>


//=== DEBUG (SPEED UP) ===
//#define DEBUG_SPEED
#ifdef DEBUG_SPEED
	#include "CTimer.h"
#endif

//#define DEBUG
#ifdef DEBUG
	#include <iostream>
	#include <fstream>
	#include <sstream>
	#include <string>
	#include <iomanip>
	using namespace std;
#endif
//========================

using namespace clandmark;

CTreeMaxSumSolver::CTreeMaxSumSolver(std::vector< Vertex > * const vertices, std::vector< CDeformationCost* > * const edges, int bw_w, int bw_h)
	: CMaxSumSolver(vertices, edges, bw_w, bw_h)
{
	// initialization
	Node tempNode;
	std::vector< int > tempInDegrees;
	int id;
	fl_double_t *p_double = 0;
	int *p_int = 0;

	kLandmarks = (int)vertices->size();
	kEdges = (int)edges->size();

	for (int i=0; i < kLandmarks; ++i)
	{
		int best = vertices->at(i).best;
		int length = vertices->at(i).appearances[best]->getLength();

		p_double = new fl_double_t[length];
		memset(p_double, 0, sizeof(fl_double_t)*length);
		intermediateResults.push_back(p_double);

		argmaxIndices.push_back(int(0));
		inDegrees.push_back(int(0));
		tempNode.ID = i;
		graph.push_back(tempNode);
	}

	// fill in in-degrees
	for (int i=0; i < kEdges; ++i)
	{
		inDegrees[edges->at(i)->getParent()->getNodeID()]++;
		graph[edges->at(i)->getParent()->getNodeID()].incidentEdgeIDs.push_back(i);
		graph[edges->at(i)->getParent()->getNodeID()].children.push_back(edges->at(i)->getChild()->getNodeID());
		graph[edges->at(i)->getChild()->getNodeID()].ancestors.push_back(edges->at(i)->getParent()->getNodeID());
	}

	// Topological sort
	// 1. Find leaves and put them to queue
	for (unsigned int i=0; i < inDegrees.size(); ++i)
	{
		tempInDegrees.push_back(inDegrees[i]);

		if (inDegrees[i]==0)
		{
			topsortQueue.push(i);
		}
	}

	// 2. Go through queue and update it
	while (!topsortQueue.empty())
	{
		int top = topsortQueue.front();
		topsortQueue.pop();
		order.push_back(top);

		for (unsigned int i=0; i < graph[top].ancestors.size(); ++i)
		{
			id = graph[top].ancestors[i];
			if (--tempInDegrees[id] == 0)
			{
				topsortQueue.push(id);
			}
		}
	}

	// prepare intermediate arrays for indices
	for (int i=0; i < kLandmarks; ++i)
	{
		int best = 0, length = 0;
		if (graph[i].ancestors.size() > 0)
		{
			best = vertices->at(graph[i].ancestors.at(0)).best;
			length = vertices->at(graph[i].ancestors.at(0)).appearances[best]->getLength();
		} else {
			best = vertices->at(i).best;
			length = vertices->at(i).appearances[best]->getLength();
		}

		p_int = new int[length];
		memset(p_int, 0, sizeof(int)*length);
		intermediateIndices.push_back(p_int);
	}
}

CTreeMaxSumSolver::~CTreeMaxSumSolver()
{
	inDegrees.clear();

	for (unsigned int i=0; i < intermediateResults.size(); ++i)
	{
		delete [] intermediateResults[i];
		delete [] intermediateIndices[i];
	}

	intermediateResults.clear();
	intermediateIndices.clear();
	argmaxIndices.clear();
	order.clear();
	graph.clear();
}

std::vector<fl_double_t *> CTreeMaxSumSolver::getIntermediateResults(void)
{
	return intermediateResults;
}

void CTreeMaxSumSolver::maximizeSum(fl_double_t * const q, fl_double_t * const g, const int length, fl_double_t * maximum, int *index)
{
#if DOUBLE_PRECISION==1
	fl_double_t tmp_maximum = -DBL_MAX;
#else
	fl_double_t tmp_maximum = -FLT_MAX;
#endif
	int tmp_index = 0;
	fl_double_t sum = 0;

	for (int i=0; i < length; ++i)
	{
		sum = q[i]+g[i];
		if (sum > tmp_maximum)
		{
			tmp_maximum = sum;
			tmp_index = i;
		}
	}

	*maximum += tmp_maximum;
	*index = tmp_index;
}

void CTreeMaxSumSolver::dt1D(fl_double_t *const src, fl_double_t *dst, int *ptr, int step, int len, double a, double b, double dshift, int dlen, double dstep)
{
	int *v = new int[len];
	memset(v, 0, len*sizeof(int));
	fl_double_t *z = new fl_double_t[len+1];
	memset(z, 0, (len+1)*sizeof(fl_double_t));

	int k = 0;
	int q = 0;
	v[0] = 0;
	z[0] = -FL_DBL_INF;
	z[1] = FL_DBL_INF;

	for (q = 1; q <= len-1; q++)
	{
		fl_double_t s = ((src[q*step] - src[v[k]*step]) - b*(q - v[k]) + a*(square(q) - square(v[k]))) / (2*a*(q-v[k]));
		while (s <= z[k])
		{
			k--;
			s = ((src[q*step] - src[v[k]*step]) - b*(q - v[k]) + a*(square(q) - square(v[k]))) / (2*a*(q-v[k]));
		}
		k++;
		v[k] = q;
		z[k] = s;
		z[k+1] = FL_DBL_INF;
	}

	k = 0;
	fl_double_t q2 = dshift;

	for (int i=0; i <= dlen-1; i++)
	{
		while (z[k+1] < q2)
		{
			k++;
		}
		dst[i*step] = a*square(q2-fl_double_t(v[k])) + b*(q2-fl_double_t(v[k])) + src[v[k]*step];
		ptr[i*step] = v[k];
		q2 += dstep;
	}

	delete [] v;
	delete [] z;
}

void CTreeMaxSumSolver::dt(fl_double_t *qC, fl_double_t *const w, fl_double_t *qP, int *indices, CDeformationCost *edge)
{
	fl_double_t W[4] = {-w[0], -w[1], -w[2], -w[3]};
	CAppearanceModel *parent = edge->getParent();
	CAppearanceModel *child = edge->getChild();
	const int * parentSS = parent->getSearchSpace();
	const int * childSS = child->getSearchSpace();
	int sizeP[2] = {parent->getSize()[0], parent->getSize()[1]};
	int sizeC[2] = {child->getSize()[0], child->getSize()[1]};

	fl_double_t offx = parentSS[0]-childSS[0]-(W[0]/W[2]);
	fl_double_t offy = parentSS[1]-childSS[1]-(W[1]/W[3]);

	fl_double_t *tmpM = new fl_double_t[sizeP[1]*sizeC[0]];
	memset(tmpM, 0, sizeP[1]*sizeC[0]*sizeof(fl_double_t));
	int *tmpIy = new int[sizeP[1]*sizeC[0]];
	memset(tmpIy, 0, sizeP[1]*sizeC[0]*sizeof(int));
	int *Ix = new int[sizeP[0]*sizeP[1]];
	memset(Ix, 0, sizeP[0]*sizeP[1]*sizeof(int));
	fl_double_t *M = new fl_double_t[sizeP[0]*sizeP[1]];
	memset(M, 0, sizeP[0]*sizeP[1]*sizeof(fl_double_t));

	// copy qC and transform it to -qC (we are looking for a maximum, not a minimum)
	fl_double_t *negQC = new fl_double_t[sizeC[0]*sizeC[1]];
	for (int i=0; i < sizeC[0]*sizeC[1]; ++i)
	{
		negQC[i] = -qC[i];
	}

	for (int x=0; x < sizeC[0]; ++x)
	{
		dt1D(&negQC[INDEX(0, x, sizeC[1])], &tmpM[INDEX(0, x, sizeP[1])], &tmpIy[INDEX(0, x, sizeP[1])], 1, sizeC[1], W[3], W[1], offy, sizeP[1], 1);
	}

	for (int y=0; y < sizeP[1]; ++y)
	{
		dt1D(tmpM+y, M+y, Ix+y, sizeP[1], sizeC[0], W[2], W[0], offx, sizeP[0], 1);
	}

	for (int x=0; x < sizeP[0]; ++x)
	{
		for (int y=0; y < sizeP[1]; ++y)
		{
			int p = INDEX(y, x, sizeP[1]);
			int tIy = tmpIy[INDEX(y, Ix[p], sizeP[1])];
			int tIx = Ix[p];
			indices[INDEX(y, x, sizeP[1])] = INDEX(tIy, tIx, sizeC[1]);
			qP[INDEX(y, x, sizeP[1])] += -M[INDEX(y, x, sizeP[1])];
		}
	}

	delete [] tmpM;
	delete [] tmpIy;
	delete [] Ix;
	delete [] M;
	delete [] negQC;
}

void CTreeMaxSumSolver::solve(const std::vector<std::vector<fl_double_t *> > &w, const std::vector<std::vector<fl_double_t *> > &q, fl_double_t **g, int *output)
{
#ifdef DEBUG_SPEED
	CTimer timer, timerPart;
	fl_double_t tim_overall, tim_part;
	timer.tic();
#endif

	int parentID = 0;
	int childID = 0;
	int edgeID = 0;
#if DOUBLE_PRECISION==1
	fl_double_t maximum = -DBL_MAX;
#else
	fl_double_t maximum = -FLT_MAX;
#endif

	// fill in taskQueue based on fixed order of Nodes (created in the constructor)
	for (unsigned int i=0; i < order.size(); ++i)
	{
		taskQueue.push(order[i]);
	}

	// While TaskQueue is not empty
	while (!taskQueue.empty())
	{
		parentID = taskQueue.front();
		taskQueue.pop();

		// B_i[x_i] = D_i(x_i) + \sum_{v_j \in C_i} max_{x_j} (B_j[x_j] + V_{ij}(x_i, x_j)
		memcpy(intermediateResults[parentID],
			q[parentID][vertices->at(parentID).best],
			vertices->at(parentID).appearances[vertices->at(parentID).best]->getLength()*sizeof(fl_double_t));

		// B_i[x_i] += \sum_{v_j \in C_i} max_{x_j} (B_j[x_j] + V_{ij}(x_i, x_j)
		if (graph[parentID].children.size())
		{
#ifdef DEBUG_SPEED
			timerPart.tic();
#endif

			int length = vertices->at(parentID).appearances[vertices->at(parentID).best]->getLength();

			for (unsigned int child=0; child < graph.at(parentID).children.size(); ++child)
			{
				childID = graph.at(parentID).children.at(child);
				edgeID = graph.at(parentID).incidentEdgeIDs.at(child);

				// Check the weights of w[kLandmarks+edgeID][0] if concave => DT otherwise WITHOUT DT
				bool canUseDT = true;
				fl_double_t *W = w[kLandmarks+edgeID][0];

				// Check for 0 weight (then DT is not working)
				if (fabs(W[0]) <= FLT_EPSILON || fabs(W[1]) <= FLT_EPSILON || fabs(W[2]) <= FLT_EPSILON || fabs(W[3]) <= FLT_EPSILON)
				{
					// if any of the weight W is 0, DT cannot be used
					canUseDT = false;
				}

				// Check for concavity (principal minors of hessian)
				if (!(W[2] < 0 && -(W[2]*W[3]) < 0))
				{
					canUseDT = false;
//					std::cout << "Cannot use DT for edgeID " << edgeID << " | W[2] = " << W[2] << " | W[3] = " << W[3] << " | D1<0: " << (W[2] < 0) << " | D2>0: " << (W[2]*W[3] > 0) << std::endl;
				}

				if (canUseDT)
				{
					// // WITH DT
					dt(intermediateResults.at(childID),
						w[kLandmarks+edgeID][0],
						intermediateResults.at(parentID),
						intermediateIndices.at(childID),
						edges->at(edgeID)
					);
				} else {

					// WITHOUT DT
					for (int i=0; i < length; ++i)
					{

						// get g[edgeID]
						edges->at(edgeID)->dotProductWithWg(w[kLandmarks+edgeID][0], g[edgeID], i);

						// max+argmax (q_childID + g_edgeID)
						maximizeSum(intermediateResults.at(childID),
									g[edgeID],
									vertices->at(childID).appearances[vertices->at(parentID).best]->getLength(),
									&intermediateResults.at(parentID)[i],
									&intermediateIndices.at(childID)[i]);
					}
				}
			}

#ifdef DEBUG
			//
			ostringstream fname;
			fname << "D_Q_EDGEID_" << edgeID << "_CHILDID_" << childID << ".txt";
			ofstream out(fname.str().c_str(), ios::out);
			for (int a=0; a < vertices->at(childID).appearances[vertices->at(childID).best]->getLength(); ++a)
			{
				out << intermediateResults[childID][a] << " ";
			}
			out << endl;
			out.close();

			fname.str("");
			fname << "D_Q_EDGEID_" << edgeID << "_PARENTID_" << parentID << ".txt";
			out.open(fname.str().c_str(), ios::out);
			for (int a=0; a < vertices->at(parentID).appearances[vertices->at(parentID).best]->getLength(); ++a)
			{
				out << intermediateResults[parentID][a] << " ";
			}
			out << endl;
			out.close();
			//
#endif

#ifdef DEBUG_SPEED
			tim_part = timerPart.toc();
			std::cout << "Solver: parentID: " << parentID << ", time: " << tim_part << std::endl;
#endif
		}

	}

	// Find max and argmax over the root node
	for (int i=0; i < vertices->at(order.back()).appearances[vertices->at(order.back()).best]->getLength(); ++i)
	{
		if (intermediateResults.at(order.back())[i] > maximum)
		{
			maximum = intermediateResults.at(order.back())[i];
			argmaxIndices.at(order.back()) = i;
		}
	}

	// Trace back the argmax indices (from root to leaves)
	for (unsigned int i=order.size()-1; i > 0; --i)
	{
		for (unsigned int child=0; child < graph.at(order.at(i)).children.size(); ++child)
		{
			int childID = graph.at(order.at(i)).children[child];
			argmaxIndices.at(childID) = intermediateIndices.at(childID)[argmaxIndices.at(order.at(i))];
		}
	}

	// Transform the argmax indices to 2D coordinates (=output)
	for (unsigned int i=0; i < argmaxIndices.size(); ++i)
	{
		const int *size = vertices->at(i).appearances[vertices->at(i).best]->getSize();
		int offset[2] = { vertices->at(i).appearances[vertices->at(i).best]->getSearchSpace()[0],
						  vertices->at(i).appearances[vertices->at(i).best]->getSearchSpace()[1] };

		output[INDEX(0, i, 2)] = argmaxIndices[i] / size[1] + offset[0];	// x-coordinate
		output[INDEX(1, i, 2)] = argmaxIndices[i] % size[1] + offset[1];	// y-coordinate
	}

#ifdef DEBUG_SPEED
	tim_overall = timer.toc();
	std::cout << "Solver: Over all time = " << tim_overall << std::endl;
#endif
}
