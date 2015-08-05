#include "orientation.hpp"
#include "saliency_common.hpp"

Mat getGaborImage(Mat intensityImg)
{
Mat src, dst;

int kernel_size =5;
double sig = 3;
double th = CV_PI/4; 
double lm = CV_PI;
double gm = 1;
double ps = 0;

gpu::GpuMat dsrc(intensityImg);
//gpu::GpuMat dsrc;
Mat receivedKernel, xtemp;

vector<gpu::GpuMat> viz(4);
gpu::GpuMat temp;

for(int i = 0; i<4; i++)
{
receivedKernel = getGaborKernel(Size(kernel_size,kernel_size),  sig,  th*i,  lm,  gm,  ps, CV_32F);

gpu::filter2D(dsrc,viz[i],-1,receivedKernel);

}
vector<gpu::GpuMat> gPyr0 = Get_GaussianPyramid_GPU(viz[0]);
vector<gpu::GpuMat> gPyr1 = Get_GaussianPyramid_GPU(viz[1]);
vector<gpu::GpuMat> gPyr2 = Get_GaussianPyramid_GPU(viz[2]);
vector<gpu::GpuMat> gPyr3 = Get_GaussianPyramid_GPU(viz[3]);

gPyr0 = censur_maps_GPU(gPyr0);
gPyr1 = censur_maps_GPU(gPyr1);
gPyr2 = censur_maps_GPU(gPyr2);
gPyr3 = censur_maps_GPU(gPyr3);

gpu::GpuMat angle0 = aggregateMaps_GPU(gPyr0);
gpu::GpuMat angle45 = aggregateMaps_GPU(gPyr1);
gpu::GpuMat angle90 = aggregateMaps_GPU(gPyr2);
gpu::GpuMat angle135 = aggregateMaps_GPU(gPyr3);


gpu::addWeighted(angle0, 0.25, angle45, 0.25, 0, temp, -1);
gpu::addWeighted(temp, 1, angle90, 0.25, 0, temp, -1);
gpu::addWeighted(temp, 1, angle135, 0.25, 0, temp, -1);

temp.download(xtemp);
return xtemp;

}	
