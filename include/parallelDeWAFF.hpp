/*
 * ParallelDeWAFF.hpp
 *
 *  Created on: Nov 5, 2015
 *      Author: davidp
 */

#ifndef PARALLELDEWAFF_HPP_
#define PARALLELDEWAFF_HPP_

#include <string>
#include "bfilterDeceived.hpp"
#include "noAdaptiveLaplacian.hpp"
using namespace std;

class ParallelDeWAFF{
public:
	void help();
	Mat processImage(const Mat& U);
	NoAdaptiveLaplacian* getNAL();

private:
	NoAdaptiveLaplacian nal;
	BFilterDeceived bfd;

	Mat filterDeceivedBilateral(const Mat& U, int wSize, double sigma_s, int sigma_r, int lambda);
};
#endif /* PARALLELDEWAFF_HPP_ */
