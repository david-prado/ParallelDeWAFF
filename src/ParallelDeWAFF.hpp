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
#include "Timer.h"
using namespace std;

typedef struct {
	int width;
	int height;
	int depth;
}frameInfo;

class ParallelDeWAFF{
public:
	ParallelDeWAFF();
	void init(int argc, char* argv[]);
	void help();
	int start();
	int processVideo();
	int processImage();
	Mat processFrame(const Mat & frame);

private:
	Mat mask; 				//A Laplacian of Gaussian mask
	int numIter;			//Number of iterations for benchmark
	unsigned int mode;		//Processing mode 00000001 = Video, 00000010 = Image,  00000101 = Video Benchmark, 00000110 = Image Benchmark
	string progName;		//This program name
	string inputFile;
	string outputFile;

	void exitError(string msg);
};
#endif /* PARALLELDEWAFF_HPP_ */
