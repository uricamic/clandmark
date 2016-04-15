/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2013, 2014, 2015 Michal Uricar
 * Copyright (C) 2013, 2014, 2015 Michal Uricar
 */

#ifndef _FLANDMARK_H__
#define _FLANDMARK_H__

#include "CLandmark.h"

namespace clandmark {

/**
 * @brief The Flandmark class
 */
class Flandmark : public CLandmark {

public:

	/**
	 * @brief Flandmark
	 * @param landmarksCount
	 * @param edgesCount
	 * @param base_window_width
	 * @param base_window_height
	 * @param base_window_margin_x
	 * @param base_window_margin_y
	 */
	Flandmark(
		int landmarksCount=8,
		int edgesCount=7,
		int base_window_width=40,
		int base_window_height=40,
		fl_double_t base_window_margin_x=1.2,
		fl_double_t base_window_margin_y=1.2
	);

	/**
	 * @brief Flandmark
	 * @param filename
	 */
	Flandmark(const char *filename, bool train=false) throw (int);

	/**
	 * @brief getInstanceOf
	 * @param filename
	 * @param train
	 * @return
	 */
	//TODO - this should be protected!!!
	static Flandmark* getInstanceOf(const char *filename, bool train=false);

	/** Destructor */
	virtual ~Flandmark();

	/**
	 * @brief getNF
	 * @return
	 */
	inline cimg_library::CImg<unsigned char> * getNF()
	{  return normalizedFrame; }

	/**
	 * @brief getNF
	 * @param img
	 * @param bbox
	 * @return
	 */
	cimg_library::CImg<unsigned char> * getNF(cimg_library::CImg<unsigned char> *img, int * const bbox, fl_double_t * const ground_truth=0);

	/**
	 * @brief getGroundTruthNF
	 * @return
	 */
	inline int * getGroundTruthNF(void)
	{ return groundTruthPositionsNF; }

	/**
	 * @brief getGroundTruth
	 * @return
	 */
	inline fl_double_t * getGroundTruth(void)
	{ return groundTruthPositions; }

	/**
	 * @brief getNormalizationFactor
	 * @return
	 */
	inline fl_double_t getNormalizationFactor(void)
	{ return normalizationFactor; }

	/**
	 * @brief setLossTables
	 * @param loss_data
	 * @param landmark_id
	 */
	void setLossTable(fl_double_t *loss_data, const int landmark_id);

	/**
	 * @brief getVersion
	 * @return
	 */
	inline std::string getVersion(void)
	{ return this->version; }

	/**
	 * @brief getName
	 * @return
	 */
	inline std::string getName(void)
	{
		return this->name;
	}

	/**
	 * @brief getSearchSpace
	 * @param landmark_id
	 * @return
	 */
	const int * getSearchSpace(const int landmark_id);

	/**
	 * @brief getBaseWindowSize
	 * @return
	 */
	const int * getBaseWindowSize();
    
    /**
	 * @brief getBaseWindowMargin
	 * @return
	 */
	const fl_double_t* getBaseWindowMargin();

	/**
	 * @brief getScore
	 * @return
	 */
	fl_double_t getScore(void);

private:

	/**
	 * @brief Flandmark
	 */
	Flandmark(const Flandmark&)
	{}

private:

	std::string version;		/**< */

};

}

#endif // _FLANDMARK_H__
