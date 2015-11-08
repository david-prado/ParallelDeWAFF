/*
 * bfilterDeceived.cpp
 *
 *  Created on: Oct 29, 2015
 *      Author: davidp
 */

#include "DeWAFF.hpp"

Mat DeWAFF::filter(const Mat& A, const Mat& Laplacian, int w, double sigma_d, int sigma_r){
    Mat B = Mat::zeros(A.size(),A.type());

    int iMin, iMax, jMin, jMax;
    Mat F, G, H, I, L, S, dL, da, db;
	Mat1i X,Y;
	vector<Mat> channels(3);
	Vec3f pixel;
	double norm_F;

    //Pre-compute Gaussian domain weights.
    Tools::meshgrid(Range(-w,w),Range(-w,w),X,Y);
    pow(X,2,X);
    pow(Y,2,Y);
    S = X+Y;
    S.convertTo(S,CV_32F);
    S /= (-2*pow(sigma_d,2));

    exp(S,G);

    //Apply bilateral filter.
    cout << "Applying the deceived bilateral filter..." << endl;

	//#pragma omp target//Uncomment if using g++ version 5
	#pragma omp parallel for private(I,iMin,iMax,jMin,jMax,pixel,channels,dL,da,db,H,F,norm_F,L) shared(A,B,G,Laplacian,w,sigma_d,sigma_r)
    for(int i = 0; i < A.rows; i++){
       for(int j = 0; j < A.cols; j++){
             //Extract local region.
             iMin = max(i - w,0);
             iMax = min(i + w,A.rows-1);
             jMin = max(j - w,0);
             jMax = min(j + w,A.cols-1);

             //Compute Gaussian range weights.
             //done in the three layers
             I = A(Range(iMin,iMax), Range(jMin,jMax));
             split(I,channels);
             pixel = A.at<Vec3f>(i,j);
             pow(channels[0] - pixel.val[0], 2, dL);
             pow(channels[1] - pixel.val[1], 2, da);
             pow(channels[2] - pixel.val[2], 2, db);
             exp((dL + da + db) / (-2 * pow(sigma_r,2)),H);

             //Calculate bilateral filter response.
             F = H.mul(G(Range(iMin-i+w, iMax-i+w), Range(jMin-j+w, jMax-j+w)));
             norm_F = sum(F).val[0];

             //The laplacian deceive consists on weighting the gaussian function with
             //the original image, and using the image values of the laplacian image.
             L = Laplacian(Range(iMin,iMax), Range(jMin,jMax));
             split(L,channels);
             B.at<Vec3f>(i,j)[0] = (sum(sum(F.mul(channels[0])))/norm_F).val[0];
             B.at<Vec3f>(i,j)[1] = (sum(sum(F.mul(channels[1])))/norm_F).val[0];
             B.at<Vec3f>(i,j)[2] = (sum(sum(F.mul(channels[2])))/norm_F).val[0];
       }
    }

    return B;
}
