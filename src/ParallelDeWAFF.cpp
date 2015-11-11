/*
 * main.cpp
 *
 *  Created on: Oct 27, 2015
 *      Author: davidp
 */
#include "ParallelDeWAFF.hpp"

int main(int argc, char* argv[]){
	ParallelDeWAFF deWAFF;
	deWAFF.init(argc,argv);
    return deWAFF.start();
}

ParallelDeWAFF::ParallelDeWAFF(){
	this->mask = -1 * Laplacian::logKernel(17, 0.005);
	this->mode = 0x0;
	this->numIter = 0x0;
	this->progName = "";
}

void ParallelDeWAFF::init(int argc, char* argv[]){
	this->progName = argv[0];

	//Check input arguments
    if (argc < 3)
        exitError("ERROR: Not enough arguments. Check syntax");

    int c;
    while ((c = getopt(argc,argv,"hb:v:i:")) != -1){
		switch(c){
			case 'v': //Process a video
				if(this->mode & 0x2)//Check if flag for image enabled
					exitError("Options -v and -i are mutually exclusive");
				this->mode |= 0x1;
				this-> inputFile = optarg;
				break;
			case 'i': //Process an image
				if(this->mode & 0x1)//Check if flag for video enabled
					exitError("Options -v and -i are mutually exclusive");
				this->mode |= 0x2;
				this->inputFile = optarg;
				break;
			case 'b'://Enable benchmarking
				this->mode |= 0x4;
				this->numIter = atoi(optarg);
				if(this->numIter<=0)
					exitError("Number of benchmark iterations must be >= 1");
				break;
			case 'h':
				this->help();
				exit(0);
				break;
			case '?':
		        if (optopt == 'v' || optopt== 'i')
		        	exitError(string("Option -" ) + optopt + " requires an input file argument.");
		        else if (isprint (optopt))
		        	exitError(string("Unknown option -") + optopt);
		        else
		        	exitError(string("Unknown option character ") + optopt);
		        break;
			default:
				abort();
		}
    }

    cout << "mode = " << mode << endl;

	//Define output file name
	string::size_type pAt = this->inputFile.find_last_of('.');
	this->outputFile = this->inputFile.substr(0, pAt) + "_DeWAFF.avi"; //+ this->inputFile.substr(pAt,this->inputFile.length());
}

void ParallelDeWAFF::help(){
	cout 	<< "------------------------------------------------------------------------------" << endl
			<< "Usage:"                                                                         << endl
			<< this->progName << " [-b N] <-v|-i> inputFile"                                    << endl
			<< "-v:\tProcess a video"                                                           << endl
			<< "-i:\tProcess an image"                                                          << endl
			<< "-b:\tRun benchmark. Process image N times. Only for images"                     << endl
			<< "------------------------------------------------------------------------------" << endl
			<< endl;
}

void ParallelDeWAFF::exitError(string msg){
	cerr << "ERROR: " << msg << endl;
	this->help();
	exit(-1);
}

int ParallelDeWAFF::start(){
	int success = -1;

	if(this->mode & 0x1)
		this->processVideo();
	else if (this->mode & 0x2)
		this->processImage();

	return success;
}

int ParallelDeWAFF::processVideo(){
	//Open input video file
	VideoCapture iVideo = VideoCapture(inputFile);
	if (!iVideo.isOpened())
		exitError("ERROR: Could not open the input video for read: "+inputFile);

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
	VideoWriter oVideo(outputFile, iCodec, iRate , iSize, true);
    if (!oVideo.isOpened())
        exitError("ERROR: Could not open the output video for write: " + outputFile);

	Mat iFrame,oFrame;

	double elapsed_secs = 0.0;
	for(int frame = 1;frame<=iFrameCount;frame++){

		//Read one frame from input video
		if(!iVideo.read(iFrame)){
			cout << "Could not read frame from video file" << endl;
			break;
		}

		timerStart();

		//Process current frame
		oFrame = this->processFrame(iFrame);

		elapsed_secs += timerStop();

		//Write frame to output video
		oVideo.write(oFrame);
	}
	if(0x4 & this->mode)
		cout << "Processing time = " << elapsed_secs << " seconds." << endl;

	//Release video resources
	iVideo.release();
	oVideo.release();

	return 0;
}

int ParallelDeWAFF::processImage(){
	Mat iFrame = imread(inputFile);
	if(iFrame.data==NULL)
		exitError("ERROR: Could not open the input file for read: " + inputFile);

	//Process image
	cout << "Processing image..." << endl;
	Mat oFrame;
	if(0x4 & this->mode){
		double elapsed_secs;
		for(int i = 1; i<=this->numIter; i++){
			cout << "Iteration " << i << endl;
			timerStart();

			oFrame = this->processFrame(iFrame);

			elapsed_secs = timerStop();
			cout << "Processing time = " << elapsed_secs << " seconds." << endl;
		}
	}
	else
		oFrame = this->processFrame(iFrame);

	if(!imwrite(outputFile, oFrame))
		exitError("ERROR: Could not open the output file for write: " + outputFile);

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
