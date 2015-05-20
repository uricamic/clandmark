#include "Flandmark.h"

#include "CTreeMaxSumSolver.h"
//#include "CSparseLBPFeatures.h"
#include "CSparseLBPAppearanceModel.h"
//#include "CExtendedSparseLBPAppearanceModel.h"
//#include "CHOGAppearanceModel.h"
#include "CDisplacementDeformationCost.h"
#include "CNormalizedEuclideanLoss.h"
#include "CTableLoss.h"
#include "CZeroLoss.h"

#include <fstream>
#include <cmath>
#include <cstring>

using namespace clandmark;

Flandmark::Flandmark(
		int landmarksCount,
		int edgesCount,
		int base_window_width,
		int base_window_height,
		fl_double_t base_window_margin_x,
		fl_double_t base_window_margin_y
		) : CLandmark(
				landmarksCount,	edgesCount,
				base_window_width, base_window_height,
				base_window_margin_x, base_window_margin_y)
{
	// Hardcoded version for testing (and also producing the model XML file)
	//										id, search_space _ _ _, window_size _, height_of_pyramid
	//vertex = new CSparseLBPAppearanceModel(0, 15, 17, 25, 27, 20, 20, 4);

	this->version = "NA";

	CSparseLBPAppearanceModel *appearance = 0;
	CDisplacementDeformationCost *edge = 0;
	Vertex vertex = Vertex();

	// 0. vertex
	vertex.best = 0;
	vertex = Vertex(0, "center", 15, 17, 25, 27, 20, 20);
	appearance = new CSparseLBPAppearanceModel(vertex.nodeID, vertex.ss[0], vertex.ss[1], vertex.ss[2], vertex.ss[3], vertex.winSize[0], vertex.winSize[1], 4);
	vertex.appearances.push_back(appearance);
	vertices.push_back(vertex);

	// 1. vertex
	vertex = Vertex(1, "canthus-rl", 10, 10, 24, 19, 10, 10);
	vertex.best = 0;
	appearance = new CSparseLBPAppearanceModel(vertex.nodeID, vertex.ss[0], vertex.ss[1], vertex.ss[2], vertex.ss[3], vertex.winSize[0], vertex.winSize[1], 4);
	vertex.appearances.push_back(appearance);
	vertices.push_back(vertex);

	// 2. vertex
	vertex = Vertex(2, "canthus-lr", 16, 10, 30, 19, 10, 10);
	vertex.best = 0;
	appearance = new CSparseLBPAppearanceModel(vertex.nodeID, vertex.ss[0], vertex.ss[1], vertex.ss[2], vertex.ss[3], vertex.winSize[0], vertex.winSize[1], 4);
	vertex.appearances.push_back(appearance);
	vertices.push_back(vertex);

	// 3. vertex
	vertex = Vertex(3, "mouth-corner-r", 5, 21, 23, 35, 10, 10);
	vertex.best = 0;
	appearance = new CSparseLBPAppearanceModel(vertex.nodeID, vertex.ss[0], vertex.ss[1], vertex.ss[2], vertex.ss[3], vertex.winSize[0], vertex.winSize[1], 4);
	vertex.appearances.push_back(appearance);
	vertices.push_back(vertex);

	// 4. vertex
	vertex = Vertex(4, "mouth-corner-l", 17, 21, 35, 35, 10, 10);
	vertex.best = 0;
	appearance = new CSparseLBPAppearanceModel(vertex.nodeID, vertex.ss[0], vertex.ss[1], vertex.ss[2], vertex.ss[3], vertex.winSize[0], vertex.winSize[1], 4);
	vertex.appearances.push_back(appearance);
	vertices.push_back(vertex);

	// 5. vertex
	vertex = Vertex(5, "canthus-rr", 5, 8, 20, 22, 10, 10);
	vertex.best = 0;
	appearance = new CSparseLBPAppearanceModel(vertex.nodeID, vertex.ss[0], vertex.ss[1], vertex.ss[2], vertex.ss[3], vertex.winSize[0], vertex.winSize[1], 4);
	vertex.appearances.push_back(appearance);
	vertices.push_back(vertex);

	// 6. vertex
	vertex = Vertex(6, "canthus-ll", 20, 8, 35, 22, 10, 10);
	vertex.best = 0;
	appearance = new CSparseLBPAppearanceModel(vertex.nodeID, vertex.ss[0], vertex.ss[1], vertex.ss[2], vertex.ss[3], vertex.winSize[0], vertex.winSize[1], 4);
	vertex.appearances.push_back(appearance);
	vertices.push_back(vertex);

	// 7. vertex
	vertex = Vertex(7, "nose", 12, 14, 28, 31, 10, 10);
	vertex.best = 0;
	appearance = new CSparseLBPAppearanceModel(vertex.nodeID, vertex.ss[0], vertex.ss[1], vertex.ss[2], vertex.ss[3], vertex.winSize[0], vertex.winSize[1], 4);
	vertex.appearances.push_back(appearance);
	vertices.push_back(vertex);

	// 0. Edge (0, 1) // Order is given as (parent, child)
	edge = new CDisplacementDeformationCost(vertices[0].appearances[0], vertices[1].appearances[0]);
	edges.push_back(edge);
	// 1. Edge (0, 2)
	edge = new CDisplacementDeformationCost(vertices[0].appearances[0], vertices[2].appearances[0]);
	edges.push_back(edge);
	// 2. Edge (0, 3)
	edge = new CDisplacementDeformationCost(vertices[0].appearances[0], vertices[3].appearances[0]);
	edges.push_back(edge);
	// 3. Edge (0, 4)
	edge = new CDisplacementDeformationCost(vertices[0].appearances[0], vertices[4].appearances[0]);
	edges.push_back(edge);
	// 4. Edge (0, 7)
	edge = new CDisplacementDeformationCost(vertices[0].appearances[0], vertices[7].appearances[0]);
	edges.push_back(edge);
	// 5. Edge (1, 5)
	edge = new CDisplacementDeformationCost(vertices[1].appearances[0], vertices[5].appearances[0]);
	edges.push_back(edge);
	// 6. Edge (2, 6)
	edge = new CDisplacementDeformationCost(vertices[2].appearances[0], vertices[6].appearances[0]);
	edges.push_back(edge);

	solver = new CTreeMaxSumSolver(&vertices, &edges, base_window_width, base_window_height);

	// Allocate w's, q's and g's
	for (int i=0; i<landmarksCount; ++i)
	{
		for (unsigned int j=0; j < vertices[i].appearances.size(); ++j)
		{
			q[i].push_back(new fl_double_t[vertices[i].appearances[j]->getFeatureDimension()]);
			w[i].push_back(new fl_double_t[vertices[i].appearances[j]->getFeatureDimension()]);
		}
	}

	for (int i=0; i<edgesCount; ++i)
	{
		g[i] = new fl_double_t[edges[i]->getChild()->getLength()];
		w[kLandmarksCount+i].push_back(new fl_double_t[edges[i]->getDimension()]);
	}

	// Load W from a file
	std::ifstream file("w.txt");

	// w - vertices
	for (unsigned int i=0; i < vertices.size(); ++i)
	{
		for (unsigned int k=0; k < vertices[i].appearances.size(); ++k)
		{
			for (int j=0; j < vertices[i].appearances[k]->getFeatureDimension(); ++j)
				file >> w[i][k][j];
		}
	}

	// w - nodes
	for (unsigned int i=0; i < edges.size(); ++i)
	{
		for (int j=0; j < edges[i]->getDimension(); ++j)
		{
			file >> w[kLandmarksCount+i][0][j];
		}
	}
	// -----------------------------

	computeWdimension();

	// allocate psi
	psi = new fl_double_t[wDimension];
}

Flandmark::Flandmark(const char *filename, bool train) throw (int)
	: CLandmark()
{
	// General
	int landmarksCount;
	int edgesCount;
	int graph_type;
	int bw_height, bw_width;
	fl_double_t bw_margin_x, bw_margin_y;

	// Nodes
	int nodeID;
	std::string name;
	int node_type;
	int ss_min_x, ss_min_y, ss_max_x, ss_max_y;
	int win_size_width, win_size_height;
	int nodeLossType;

	// Edges
	int parentID, childID;
	int edge_type;
	int edgeLossType;
	int dimension;

	// Loss
	CLoss* loss = 0;

	// Allocate and read W
	XmlNode nodeW;

	XmlStorage fs(filename);
	if (fs.empty())
	{
		throw 1;			// file does not exists
	}

//	fs["num_nodes"] >> landmarksCount;			// TODO: overload operator >>
	landmarksCount = (int)fs["num_nodes"];

	if (landmarksCount == 0 || landmarksCount < 0)
	{
		throw 2;		// file does not exists or it does not contain flandmark_model
	}

	this->version = (std::string)fs["version"];
	this->name = (std::string)fs["name"];
	edgesCount = (int)fs["num_edges"];
	graph_type = (int)fs["graph_type"];
	bw_width = (int)fs["bw_width"];
	bw_height = (int)fs["bw_height"];
	bw_margin_x = (fl_double_t)fs["bw_margin_x"];
	bw_margin_y = (fl_double_t)fs["bw_margin_y"];

	CLandmark::init(landmarksCount, edgesCount, bw_width, bw_height, bw_margin_x, bw_margin_y);

	// create nodes
	std::vector<XmlNode> nodes = fs["Nodes"].getSet("Node");
	for (unsigned int index = 0; index < nodes.size(); ++index)
	{
		// Create Node
		XmlNode it = nodes.at(index);
		XmlNode Header = it["Header"];
		nodeID = (int)Header["NodeID"];
		name = (std::string)Header["Name"];
		ss_min_x = (int)Header["ss_minX"];
		ss_min_y = (int)Header["ss_minY"];
		ss_max_x = (int)Header["ss_maxX"];
		ss_max_y = (int)Header["ss_maxY"];
		win_size_width = (int)Header["win_size_width"];
		win_size_height = (int)Header["win_size_height"];

		XmlNode Loss = Header["LossType"];
		if (!Loss.empty())
		{
			std::string lossType = (std::string)Loss;
			if (!lossType.empty())
			{
				if (lossType.compare("NORMALIZED_EUCLIDEAN") == 0)
					nodeLossType = NORMALIZED_EUCLIDEAN;
				else if (lossType.compare("TABLE_LOSS") == 0)
					nodeLossType = TABLE_LOSS;
				else if (lossType.compare("ZERO_LOSS") == 0)
					nodeLossType = ZERO_LOSS;
				else
					nodeLossType = NORMALIZED_EUCLIDEAN;
			} else {
				nodeLossType = -1;
			}
		} else {
			nodeLossType = -1;
		}

		Vertex vertex(nodeID, name, ss_min_x, ss_min_y, ss_max_x, ss_max_y, win_size_width, win_size_height);

		std::vector<XmlNode> appearances = it["Appearances"].getSet("Appearance");
		for (unsigned int index2 = 0; index2 < appearances.size(); ++index2)
		{
			XmlNode it2 = appearances.at(index2);
			//node_type = (int)it2["Type"];
			std::string appearance_type = (std::string)it2["Type"];

			if (appearance_type.compare("SPARSE_LBP") == 0)
				node_type = SPARSE_LBP;
			else if (appearance_type.compare("EXTENDED_SPARSE_LBP") == 0)
				node_type = EXTENDED_SPARSE_LBP;
			else if (appearance_type.compare("HOG") == 0)
				node_type = HOG;
			else
				node_type = SPARSE_LBP;

//				CLoss *loss = 0x0;

			if (train && (nodeLossType > 0))
			{
				switch (nodeLossType)
				{
					case NORMALIZED_EUCLIDEAN:
						loss = new CNormalizedEuclideanLoss((ss_max_x-ss_min_x+1), (ss_max_y-ss_min_y+1));
					break;
					case TABLE_LOSS:
						loss = new CTableLoss((ss_max_x-ss_min_x+1), (ss_max_y-ss_min_y+1));
					break;
					case ZERO_LOSS:
						loss = new CZeroLoss((ss_max_x-ss_min_x+1), (ss_max_y-ss_min_y+1));
					break;
					default:
						loss = new CNormalizedEuclideanLoss((ss_max_x-ss_min_x+1), (ss_max_y-ss_min_y+1));
					break;
				}
			}

			int hop;							// LBPs
//			int nbins, cell_size, block_size;	// HOGs

			switch(node_type)
			{
				case SPARSE_LBP:
					hop = (int)it2["hop"];
					if (train && (nodeLossType > 0))
					{
						vertex.appearances.push_back(
									new CSparseLBPAppearanceModel(
										nodeID,
										vertex.ss[0],
										vertex.ss[1],
										vertex.ss[2],
										vertex.ss[3],
										vertex.winSize[0],
										vertex.winSize[1],
										hop,
										loss)
								);
					} else {
						vertex.appearances.push_back(
									new CSparseLBPAppearanceModel(
										nodeID,
										vertex.ss[0],
										vertex.ss[1],
										vertex.ss[2],
										vertex.ss[3],
										vertex.winSize[0],
										vertex.winSize[1],
										hop)
								);
					}
////					vertices.push_back(vertex);
//					if (NFfeaturesPool.empty())
//					{
//						NFfeaturesPool.push_back(new CSparseLBPFeatures(baseWindow[0], baseWindow[1], maxPyrLevelLBP, cumWidths));
//					}
				break;
			}
			vertices.push_back(vertex);

			// Allocate and read W
			nodeW = it2["w"];
			w[index].push_back(new fl_double_t[vertices.at(index).appearances[index2]->getFeatureDimension()]);
//			int feature_dimension = vertices.at(index).appearances[index2]->getFeatureDimension();
//			fl_double_t *p_double = new fl_double_t[feature_dimension];
//			w[index].push_back(p_double);
			if (!nodeW.empty())
			{
				int binsize;
				unsigned char *binData = nodeW.readRaw(&binsize);
				fl_double_t *w_ptr = (fl_double_t*)binData;
				// TODO !!! - check if w_ptr is not a null
				memcpy(w[index][index2], w_ptr, sizeof(fl_double_t)*vertices.at(index).appearances[index2]->getFeatureDimension());
				free(binData);
			} else {
				memset(w[index].back(), 0, sizeof(fl_double_t)*vertices.at(index).appearances[index2]->getFeatureDimension());
				// TODO - ^ probably not needed at all
			}

			// Allocate q
			q[index].push_back(new fl_double_t[vertices.at(index).appearances[index2]->getFeatureDimension()]);
			memset(q[index].back(), 0, sizeof(fl_double_t)*vertices.at(index).appearances[index2]->getFeatureDimension());

			///DEBUG
			vertices.at(index).appearances[index2]->setName(vertices[index].name);
			///-----
		}
	}

	// create edges
	std::vector<XmlNode> Edges = fs["Edges"].getSet("Edge");
	for (unsigned int index = 0; index < Edges.size(); ++index)
	{
		// Create Edge
		XmlNode it = Edges.at(index);
		parentID = (int)it["ParentID"];
		childID = (int)it["ChildID"];
		edge_type = (int)it["Type"];
		dimension = (int)it["Dims"];

		XmlNode Loss = it["LossType"];
		if (!Loss.empty())
		{
			std::string lossType = (std::string)Loss;
			if (lossType.compare("NORMALIZED_EUCLIDEAN") == 0)
				edgeLossType = NORMALIZED_EUCLIDEAN;
			else if (lossType.compare("TABLE_LOSS") == 0)
				edgeLossType = TABLE_LOSS;
			else if (lossType.compare("ZERO_LOSS") == 0)
				edgeLossType = ZERO_LOSS;
			else
				edgeLossType = NORMALIZED_EUCLIDEAN;

			switch (edgeLossType)
			{
				case NORMALIZED_EUCLIDEAN:
					//loss = new CNormalizedEuclideanLoss();
				break;
			}

		} else {
			edgeLossType = -1;
		}

		switch (edge_type)
		{
			case DISPLACEMENT_VECTOR:
				// TODO - LOSS (Not for flandmark, but possible future configurations of CLandmark)
				edges.push_back(new CDisplacementDeformationCost(vertices[parentID].appearances[0], vertices[childID].appearances[0], dimension));
			break;
		}

		// Allocate and read W
		nodeW = it["w"];
		w[kLandmarksCount+index].push_back(new fl_double_t[edges[index]->getDimension()]);
		if (!nodeW.empty())
		{
			int binsize;
			unsigned char *binData = nodeW.readRaw(&binsize);
			fl_double_t *w_ptr = (fl_double_t*)binData;
			// TODO !!! - check if w_ptr is not a null
			memcpy(w[kLandmarksCount+index][0], w_ptr, sizeof(fl_double_t)*edges[index]->getDimension());
			free(binData);
		} else {
			memset(w[kLandmarksCount+index][0], 0, sizeof(fl_double_t)*edges[index]->getDimension());
		}
		// Allocate g
		g[index] = new fl_double_t[edges[index]->getChild()->getLength()];
	}

	// create solver
	switch (graph_type)
	{
		case TREE:
			solver = new CTreeMaxSumSolver(&vertices, &edges, bw_width, bw_height);
		break;
	}

	computeWdimension();

	psi = new fl_double_t[wDimension];
}

Flandmark::~Flandmark()
{
	// delete vertices
	for (unsigned int i=0; i < vertices.size(); ++i)
	{
		for (unsigned int j=0; j < vertices[i].appearances.size(); ++j)
			delete vertices[i].appearances[j];
		vertices[i].appearances.clear();
	}
	vertices.clear();

	// delete edges
	for (unsigned int i=0; i < edges.size(); ++i)
	{
		delete edges[i];
	}

	delete solver;
}

cimg_library::CImg<unsigned char> * Flandmark::getNF(cimg_library::CImg<unsigned char> *img, int * const bbox, fl_double_t * const ground_truth)
{
	getNormalizedFrame(img, bbox);

	if (ground_truth)
	{
		setGroundTruth(ground_truth);
	}

	return normalizedFrame;
}

void Flandmark::setLossTable(fl_double_t *loss_data, const int landmark_id)
{
	for (unsigned int j=0; j < vertices[landmark_id].appearances.size(); ++j)
	{
		fl_double_t *table = new fl_double_t[vertices[landmark_id].appearances[j]->getLength()];

		int k = 0;
		for (int x = vertices[landmark_id].ss[0]; x <= vertices[landmark_id].ss[2]; ++x)
			for (int y = vertices[landmark_id].ss[1]; y <= vertices[landmark_id].ss[3]; ++y)
				table[k++] = loss_data[INDEX(y, x, baseWindow[1])];

		static_cast< CTableLoss* >(vertices[landmark_id].appearances[j]->getLoss())->setLossTable(table);

		delete [] table;
	}
}

const int * Flandmark::getSearchSpace(const int landmark_id)
{
	return vertices[landmark_id].ss;
}

const int * Flandmark::getBaseWindowSize()
{
	return baseWindow;
}

fl_double_t Flandmark::getScore()
{
	fl_double_t score = 0.0;

	for (int i=0; i < kLandmarksCount; ++i)
	{
		score += Q[i];
	}
	for (int i=0; i < kEdgesCount; ++i)
	{
		score += G[i];
	}

	return score;
}

Flandmark* Flandmark::getInstanceOf(const char *filename, bool train)
{
	try
	{
		return new Flandmark(filename, train);
	} catch (...) {
		return 0x0;
	}
}
