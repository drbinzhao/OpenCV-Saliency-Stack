#include "orientation.hpp"
#include "saliency_common.hpp"

Mat getGaborImage(Mat intensityImg)
{
Mat src;
//intensityImg.convertTo(src,CV_32F);

Mat dst;
/*
int kernel_size =31;
double sig = 1;
double th = CV_PI/4; 
double lm = 1.0;
double gm = 0.02;
double ps = 0;
*/
int kernel_size =5;
double sig = 3;
double th = CV_PI/4; 
double lm = CV_PI;
double gm = 1;
double ps = 0;


Mat receivedKernel;
vector<Mat> viz(4);

for(int i = 0; i<4; i++)
{
receivedKernel = getGaborKernel(Size(kernel_size,kernel_size),  sig,  th*i,  lm,  gm,  ps, CV_32F);
filter2D(intensityImg,viz[i],-1,receivedKernel);
//dst.convertTo(viz[i],CV_8U,1.0/255.0);

}
vector<Mat> gPyr0 = Get_GaussianPyramid(viz[0]);
vector<Mat> gPyr1 = Get_GaussianPyramid(viz[1]);
vector<Mat> gPyr2 = Get_GaussianPyramid(viz[2]);
vector<Mat> gPyr3 = Get_GaussianPyramid(viz[3]);

gPyr0 = censur_maps(gPyr0);
gPyr1 = censur_maps(gPyr1);
gPyr2 = censur_maps(gPyr2);
gPyr3 = censur_maps(gPyr3);

Mat angle0 = aggregateMaps(gPyr0);
Mat angle45 = aggregateMaps(gPyr1);
Mat angle90 = aggregateMaps(gPyr2);
Mat angle135 = aggregateMaps(gPyr3);

Mat temp;
addWeighted(angle0, 0.25, angle45, 0.25, 0, temp, -1);
addWeighted(temp, 1, angle90, 0.25, 0, temp, -1);
addWeighted(temp, 1, angle135, 0.25, 0, temp, -1);

return temp;

}	
