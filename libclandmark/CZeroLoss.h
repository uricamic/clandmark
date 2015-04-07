//
//  CZeroLoss.h
//
//
//  Created by Kostia on 1/27/14.
//
//

#ifndef ____CZeroLoss__
#define ____CZeroLoss__

#include "CLoss.h"

namespace clandmark {

	/**
	 * @brief The CNormalizedEuclideanLoss class
	 */
	class CZeroLoss : public CLoss {

	public:

		// Constructor
		/**
		 * @brief CNormalizedEuclideanLoss
		 * @param size_
		 */
		//CNormalizedEuclideanLoss(int * const size_);

		/**
		 * @brief CNormalizedEuclideanLoss
		 * @param size_w
		 * @param size_h
		 */
		CZeroLoss(int size_w, int size_h);

		/** Destructor */
		~CZeroLoss();

		/**
		 * @brief computeLoss
		 * @param ground_truth
		 * @param estimate
		 * @return
		 */
		fl_double_t computeLoss(fl_double_t * const ground_truth, fl_double_t * const estimate);

		/**
		 * @brief computeLoss
		 * @param ground_truth
		 * @param estimate
		 * @return
		 */
		fl_double_t computeLoss(int * const ground_truth, int * const estimate);

		/**
		 * @brief getLossAt
		 * @param position
		 * @return
		 */
		fl_double_t getLossAt(int position)
		{
			return 0;
		}

		/**
		 * @brief getLossType
		 * @return
		 */
		ELossType getType(void) { return ZERO_LOSS; }

		/**
		 * @brief getName
		 * @return
		 */
		std::string getName(void) { return "ZERO_LOSS"; }

	};

}

#endif /* defined(____CZeroLoss__) */
