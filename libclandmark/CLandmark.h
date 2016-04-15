/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2013, 2014, 2015 Michal Uricar
 * Copyright (C) 2013, 2014, 2015 Michal Uricar
 */

#ifndef _CLANDMARK__H__
#define _CLANDMARK__H__

#include "CAppearanceModel.h"
#include "CDeformationCost.h"
#include "CMaxSumSolver.h"
#include "CFeaturePool.h"

#define cimg_verbosity 1		// we don't need window output capabilities of CImg
#define cimg_display 0			// we don't need window output capabilities of CImg

#include "CImg.h"
#include "CTypes.h"

#include <vector>

// IO functions
#include <iostream>
#include <iomanip>

namespace clandmark {

/**
 *
 */
typedef enum {
	TREE=1,
	SIMPLE_NET=2,
	GENERAL_GRAPH=3
} EGraphType;

/**
 *
 */
typedef struct timings_struct {
	fl_double_t overall;				/**< */
	fl_double_t normalizedFrame;		/**< */
	fl_double_t features;				/**< */
	fl_double_t maxsum;					/**< */
} Timings;


/**
 * @brief The CLandmark class
 */
class CLandmark {

public:

	/**
	 * @brief CLandmark
	 * @param landmarksCount
	 * @param edgesCount
	 * @param base_window_width
	 * @param base_window_height
	 * @param base_window_margin_x
	 * @param base_window_margin_y
	 */
	CLandmark(
		int landmarksCount,
		int edgesCount,
		int base_window_width,
		int base_window_height,
		fl_double_t base_window_margin_x,
		fl_double_t base_window_margin_y
	);

	/**
	 * @brief Default CLandmark
	 */
	CLandmark();

	/**
	 * @brief init
	 * @param landmarksCount
	 * @param edgesCount
	 * @param base_window_width
	 * @param base_window_height
	 * @param base_window_margin_x
	 * @param base_window_margin_y
	 */
	void init(int landmarksCount,
			  int edgesCount,
			  int base_window_width,
			  int base_window_height,
			  fl_double_t base_window_margin_x,
			  fl_double_t base_window_margin_y);

	/**
	 * @brief init_optimized;
	 * @param landmarksCount
	 * @param edgesCount
	 * @param base_window_width
	 * @param base_window_height
	 * @param base_window_margin_x
	 * @param base_window_margin_y
	 */
	void init_optimized(int landmarksCount,
						int edgesCount,
						int base_window_width,
						int base_window_height,
						fl_double_t base_window_margin_x,
						fl_double_t base_window_margin_y);

	/**
	 * @brief ~CLandmark destructor
	 */
	virtual ~CLandmark();

	/**
	 * @brief Function detects landmarks within given bounding box in a given image.
	 * @param inputImage	Input image
	 * @param boundingBox	Bounding box (format: [min_x, min_y, max_x, max_y]) of object of interest (i.e. axis aligned)
	 * @param ground_truth
	 */
	void detect(cimg_library::CImg<unsigned char> *inputImage, int *boundingBox, fl_double_t * const ground_truth=0);

	/**
	 * @brief detect_optimized
	 * @param inputImage
	 * @param boundingBox
	 * @param ground_truth
	 */
	void detect_optimized(cimg_library::CImg<unsigned char> *inputImage, int *boundingBox, fl_double_t * const ground_truth=0);

	/**
	 * @brief detect_optimizedFromPool
	 * @param ground_truth
	 */
	void detect_optimizedFromPool(int *boundingBox, fl_double_t * const ground_truth=0);

	/**
	 * @brief detect_mirrored
	 * @param inputImage
	 * @param boundingBox
	 * @param ground_truth
	 */
	void detect_mirrored(cimg_library::CImg<unsigned char> *inputImage, int *boundingBox, fl_double_t * const ground_truth=0);

	/**
	 * @brief detect
	 * @param inputImage	normalized image frame
	 * @param ground_truth
	 */
	void detect_base(cimg_library::CImg<unsigned char> *inputImage, int * const ground_truth=0);

	/**
	 * @brief detect_base_optimized
	 * @param nf_features_mipmap
	 * @param ground_truth
	 */
	void detect_base_optimized(int * const ground_truth=0);

	/**
	 * @brief detect_base_optimized
	 * @param inputImage
	 * @param ground_truth
	 */
	void detect_base_optimized(cimg_library::CImg<unsigned char> *inputImage, int * const ground_truth=0);

	/**
	 * @brief nodemax_base
	 * @param inputImage
	 * @param ground_truth
	 */
	void nodemax_base(cimg_library::CImg<unsigned char> *inputImage, int * const ground_truth=0);

	/**
	 * @brief getFeatures
	 * @param inputImage
	 * @param boundingBox
	 * @return
	 */
	fl_double_t *getFeatures(cimg_library::CImg<unsigned char> *inputImage, int * const boundingBox, int * const configuration);

	/**
	 * @brief getFeatures_base
	 * @param nf
	 * @param configuration
	 * @return
	 */
	fl_double_t *getFeatures_base(cimg_library::CImg<unsigned char> *nf, int * const configuration);

	/**
	 * @brief getFeatures_base_optimized
	 * @param configuration
	 * @return
	 */
	fl_double_t *getFeatures_base_optimized(int * const configuration);

	/**
	 * @brief getFeatures
	 * @param configuration
	 * @return
	 */
	fl_double_t *getFeatures(int * const configuration);

	/**
	 * @brief getPsiNodes_base
	 * @param nf
	 * @param configuration
	 * @return
	 */
	fl_double_t *getPsiNodes_base(cimg_library::CImg<unsigned char> *nf, int * const configuration);

	/**
	 * @brief getPsiNodes
	 * @param configuration
	 * @return
	 */
	fl_double_t *getPsiNodes(int * const configuration);

	/**
	 * @brief setNormalizationFactor
	 * @param factor
	 */
	inline void setNormalizationFactor(fl_double_t factor) { normalizationFactor = factor; }

	/**
	 * @brief getName
	 * @return
	 */
	inline std::string getName(void) { return name; }

	/**
	 * @brief setName
	 * @param name_
	 */
	inline void setName(std::string name_) { name = name_; }

	/**
	 * @brief getLandmarks
	 * @return
	 */
	inline fl_double_t *getLandmarks(void) { return landmarksPositions; }

	/**
	 * @brief getLandmarksNF
	 * @return
	 */
	inline int *getLandmarksNF(void) { return landmarksPositionsNF; }

	/**
	 * @brief getLandmarksCount
	 * @return
	 */
	inline int getLandmarksCount(void) { return vertices.size(); }

	/**
	 * @brief getEdgesCount
	 * @return
	 */
	inline int getEdgesCount(void) { return kEdgesCount; }

	/**
	 * @brief computeWdimension
	 */
	void computeWdimension(void);

	/**
	 * @brief getWdimension
	 * @return
	 */
	inline int getWdimension(void) { return wDimension; }

	/**
	 * @brief getPsiNodesDimension
	 * @return
	 */
	inline int getPsiNodesDimension(void) { return psiNodesDimension; }

	/**
	 * @brief getPsiEdgesDimension
	 * @return
	 */
	inline int getPsiEdgesDimension(void) { return wDimension-psiNodesDimension; }

	/**
	 * @brief getNodesDimensions
	 * @return
	 */
	int *getNodesDimensions(void);

	/**
	 * @brief getEdgesDimensions
	 * @return
	 */
	int *getEdgesDimensions(void);

	/**
	 * @brief setW
	 * @param input_w
	 */
	void setW(fl_double_t * const input_w);

	/**
	 * @brief getW
	 * @return joint weight vector w, allocates memory, does not care about its freeing!
	 */
	fl_double_t * getW(void);

	/**
	 * @brief setNodesW
	 * @param input_w
	 */
	void setNodesW(fl_double_t * const input_w);

	/**
	 * @brief getQvalues
	 * @return
	 */
	inline fl_double_t *getQvalues(void) { return Q; }

	/**
	 * @brief getGvalues
	 * @return
	 */
	inline fl_double_t *getGvalues(void) { return G; }

	/**
	 * @brief getLossValues
	 * @return
	 */
	fl_double_t *getLossValues(int *position);

	/**
	 * @brief write
	 * @param filename
	 */
	void write(const char *filename, bool writeW=true);

	/**
	 * @brief getEdges
	 * @return
	 */
	int *getEdges(void);

	/**
	* @brief getNodesSearchSpaces
	* @return
	*/
	int *getNodesSearchSpaces(void);

	/**
	 * @brief getWindowSizes
	 * @return
	 */
	int *getWindowSizes(void);

	/**
	 * @brief getH
	 * @return
	 */
	inline fl_double_t * getH(void) { return &H[0]; }

	/**
	 * @brief getHinv
	 * @return
	 */
	inline fl_double_t * getHinv(void) { return &Hinv[0]; }

	/**
	 * @brief nodeHasLoss
	 * @param nodeID
	 * @return
	 */
	inline bool nodeHasLoss(int nodeID) { return vertices[nodeID].appearances[0]->hasLoss(); }

    /**
	 * @brief setSmoothingSigma
	 * @param sigma
	 */
	inline void setSmoothingSigma(fl_double_t sigma) { this->sigma = sigma; }

	/**
	 * @brief getSmoothingSigma
	 * @return
	 */
	inline fl_double_t getSmoothingSigma(void) { return this->sigma; }
    
	/// NEW FEATURE (NOT IMPLEMENTED YET)
	void changeSearchSpace();
	/// -----------

	/// ========================= SPEED UP ================================

	/**
	 * @brief setNFfeaturesPool
	 * @param pool
	 */
	inline void setNFfeaturesPool(CFeaturePool * const pool) { NFfeaturesPool = pool; }

	/// ========================= ========== ================================

	/**
	 * @brief getIntermediateResults
	 * @param output
	 * @return
	 */
	inline std::vector<fl_double_t *> getIntermediateResults(void) { return solver->getIntermediateResults(); }

    /**
	 * @brief getLandmarkNames Returns individual landmarks names
	 * @return
	 */
	std::vector<std::string> getLandmarkNames(void);

	/**
	 * @brief getQs Returns feature response of individual landmarks (for visualization in MATLAB interface)
	 * @return
	 */
	std::vector< std::vector< fl_double_t* > > getQs(void);
    
protected:

	// update values of functions q and g
	/**
	 * @brief getQG
	 */
	void getQG(void);

	/**
	 * @brief getQG_optimized
	 */
	void getQG_optimized();

	/**
	 * @brief getQGTableLoss
	 */
	void getQGTableLoss(void);

	/**
	 * @brief setGroundTruth
	 * @param ground_truth
	 */
	void setGroundTruth(fl_double_t *ground_truth);

	/**
	 * @brief setGroundTruth
	 * @param ground_truth
	 */
	void setGroundTruth(int *ground_truth);

	/**
	 * @brief buildHandHinv
	 * @param boundingBox
	 */
	void buildHandHinv(int *boundingBox);


	// crop & resize enlarged face box from input image
	/**
	 * @brief getNormalizedFrame
	 * @param inputImage
	 * @param boundingBox
	 */
	void getNormalizedFrame(cimg_library::CImg<unsigned char> *inputImage, int *boundingBox);

	/**
	 * @brief applyNormalizationFactor
	 */
	void applyNormalizationFactor(void);

	/**
	 * @brief transformCoordinatesImage2NF
	 * @param input
	 * @param output
	 */
	void transformCoordinatesImage2NF(fl_double_t * const input, int *output);

	/**
	 * @brief transformCoordinatesNF2Image
	 * @param input
	 * @param output
	 */
	void transformCoordinatesNF2Image(int * const input, fl_double_t *output);

private:

	/**
	 * @brief CLandmark
	 */
	CLandmark(const CLandmark&)
	{}

//	/**
//	 * @brief createMipmap
//	 * @param mipmap
//	 */
//	void createMipmap(unsigned char* mipmap);

public:

	Timings timings;									/**< */

protected:

	std::string name;									/**< */

	// internal parameters
	int kLandmarksCount;								/**< */
	int kEdgesCount;									/**< */

	// convention for size [width x height]
	int baseWindow[2];									/**< */
	fl_double_t baseWindowMargin[2];					/**< */

    // sigma for optional Gaussian filter smoothing of normalized frame
	fl_double_t sigma;                                  /**< */
    
	// normalized image frame related variables
	cimg_library::CImg<unsigned char> *normalizedFrame;	/**< */
	fl_double_t BB[8];									/**< */
	fl_double_t H[9];									/**< */
	fl_double_t Hinv[9];								/**< */

	// Values of functions q and g (passed to solver)
	std::vector< std::vector< fl_double_t* > > q;		/**< */
	fl_double_t **g;									/**< */

	// individual parts of weight vector
	std::vector< std::vector< fl_double_t* > > w;		/**< */

	// dimension of weight vector
	int wDimension;										/**< */

	// Output
	fl_double_t *landmarksPositions;					/**< */
	int *landmarksPositionsNF;							/**< */

	// Graph
	std::vector< Vertex > vertices;						/**< */
	std::vector< CDeformationCost* > edges;				/**< */

	// MaxSum Solver
	CMaxSumSolver *solver;								/**< */

	// Q & G response for the detected landmarks positions
	fl_double_t * Q;									/**< */
	fl_double_t * G;									/**< */
	fl_double_t * L;									/**< */

	// GroundTruth (for training only)
	fl_double_t *groundTruthPositions;					/**< */
	int *groundTruthPositionsNF;						/**< */

	fl_double_t normalizationFactor;					/**< */

	fl_double_t *psi;									/**< */

	int psiNodesDimension;								/**< */

	//============ Optimized version ==========================
	CFeaturePool * NFfeaturesPool;						/**< */
};

} /* namespace clandmark */

#endif /* _CLANDMARK__H__ */
