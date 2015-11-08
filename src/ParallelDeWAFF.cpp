/*
 * main.cpp
 *
 *  Created on: Oct 27, 2015
 *      Author: davidp
 */
#include "ParallelDeWAFF.hpp"

void help(char* prog){
	cout 	<< "------------------------------------------------------------------------------" << endl
			<< "Usage:"                                                                         << endl
			<< prog << " [-v|-i] inputFile"                                                     << endl
			<< "-v:\tProcess a video"                                                           << endl
			<< "-i:\tProcess an image"                                                          << endl
			<< "------------------------------------------------------------------------------" << endl
			<< endl;
}

string getOutputFile(string inputFile){
	// Find extension point
	string::size_type pAt = inputFile.find_last_of('.');
	// Form the new name with container
	string outputFile = inputFile.substr(0, pAt) + "_DeWAFF" + inputFile.substr(pAt,inputFile.length());
	return outputFile;
}

int main(int argc, char* argv[]){
	ParallelDeWAFF deWAFF;
	string inputFile;
	string outputFile;
	int c;
	int success = -1;

	//Check input arguments
    if (argc != 3){
        cerr << "ERROR: Not enough parameters" << endl;
        help(argv[0]);
        return -1;
    }

    c = getopt(argc,argv,"v:i:");
    switch(c){
		case 'v': //Process a video
		    inputFile = optarg;
		    outputFile = getOutputFile(inputFile);
			success = deWAFF.processVideo(inputFile,outputFile);
			break;
		case 'i': //Process an image
		    inputFile = optarg;
		    outputFile = getOutputFile(inputFile);
			success = deWAFF.processImage(inputFile,outputFile);
			break;
		default:
			;
    }

    return success;
}

ParallelDeWAFF::ParallelDeWAFF(){
	this->mask = -1 * Laplacian::logKernel(17, 0.005);
}

int ParallelDeWAFF::processVideo(const string& inputFile, const string& outputFile){
	//Open input video file
	VideoCapture iVideo = VideoCapture(inputFile);
	if (!iVideo.isOpened()){
		cerr << "ERROR: Could not open the input video for read: " << inputFile << endl;
		return -1;
	}

    // Acquire input information: frame rate, number of frames, codec and size
	int iRate = static_cast<int>(iVideo.get(CV_CAP_PROP_FPS));
    int iFrameCount = static_cast<int>(iVideo.get(CV_CAP_PROP_FRAME_COUNT));
    int iCodec = static_cast<int>(iVideo.get(CV_CAP_PROP_FOURCC));
    Size iSize = Size((int) iVideo.get(CV_CAP_PROP_FRAME_WIDTH),(int) iVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

    // Transform from int to char via Bitwise operators
    char EXT[] = {(char)(iCodec & 0XFF) , (char)((iCodec & 0XFF00) >> 8),(char)((iCodec & 0XFF0000) >> 16),(char)((iCodec & 0XFF000000) >> 24), 0};
    string iCodecName = EXT;

    cout << "### Input Video Information ###" << endl
    	 << "Frame resolution: Width=" << iSize.width << "  Height=" << iSize.height << endl
         << "Number of frames: " << iFrameCount << endl
         << "Codec type: " << iCodecName << endl << endl;

	//Open output video
	VideoWriter oVideo;
	oVideo.open(outputFile, iCodec, iRate , iSize, true);
    if (!oVideo.isOpened()){
        cout  << "ERROR: Could not open the output video for write: " << outputFile << endl;
        return -1;
    }

	Mat iFrame,oFrame;
	int frame = 1;
	while(frame<=iFrameCount){
		cout << "Processing frame " << frame << " of " << iFrameCount << "." << endl;

		//Read one frame from input video
		if(!iVideo.read(iFrame)){
			cout << "Could not read frame from video file" << endl;
			break;
		}

		//Process current frame
		oFrame = this->processFrame(iFrame);

		//Write frame to output video
		oVideo.write(oFrame);
		frame++;
	}

	//Release video resources
	iVideo.release();
	oVideo.release();

	return 0;
}

int ParallelDeWAFF::processImage(const string& inputFile, const string& outputFile){
	Mat iFrame = imread(inputFile);
	if(iFrame.data==NULL){
		cerr << "ERROR: Could not open the input file for read: " << inputFile << endl;
		return -1;
	}

	//Process image
	cout << "Processing image... " << endl;
	Mat oFrame = this->processFrame(iFrame);

	if(!imwrite(outputFile, oFrame)){
		cerr << "ERROR: Could not open the output file for write: " << outputFile << endl;
		return -1;
	}

	return 0;
}

//Input image must be BGR from 0 to 255
Mat ParallelDeWAFF::processFrame(const Mat& F){
    //Set parameters for processing
    int wRSize = 21;
    double sigma_s = wRSize/1.5;
    int sigma_r = 10;
    int lambda = 2;

    int type = F.type();
	double minF, maxF;
	Tools::minMax(F,&minF,&maxF);
	if (!(type == CV_8UC1 || type == CV_8UC3) || minF < 0 || maxF > 255){
	   cerr << "Input frame must be a RGB unsigned integer matrix of size NxMx1 or NxMx3 on the closed interval [0,255]." << endl;
	}

    //The image has to to have values from 0 to 1 before convertion to CIELab
    Mat N;
    F.convertTo(N,CV_32F,1.0/255.0);

	//Convert input BGR image to CIELab color space.
	//CIELab 'a' and 'b' values go from -127 to 127
	Mat B;
	cout << "Using the CIELab color space." << endl;
	cvtColor(N,B,CV_BGR2Lab);

	Mat L = Laplacian::noAdaptive(B, this->mask, lambda);
	Mat C = DeWAFF::filter(B, L, wRSize, sigma_s,sigma_r);

	//Convert filtered image back to BGR color space.
	Mat D;
	cvtColor(C,D,CV_Lab2BGR);

    //Scale back
	Mat E;
    D.convertTo(E,CV_8U,255);
    return E;
}
