/*
 * ParallelDeWAFF.hpp
 *
 *  Created on: Nov 5, 2015
 *      Author: davidp
 */

#ifndef PARALLELDEWAFF_HPP_
#define PARALLELDEWAFF_HPP_

#include <string>
#include <unistd.h>
#include "DeWAFF.hpp"
#include "Laplacian.hpp"
using namespace std;

typedef struct {
	int width;
	int height;
	int depth;
}frameInfo;

class ParallelDeWAFF{
public:
	ParallelDeWAFF();
	int processVideo(const string& inputFile, const string& outputFile);
	int processImage(const string& inputFile, const string& outputFile);
	Mat processFrame(const Mat & frame);

private:
	Mat mask; 	//A Laplacian of Gaussian mask
};
#endif /* PARALLELDEWAFF_HPP_ */
