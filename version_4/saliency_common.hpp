#include "common.hpp"

vector<Mat> Get_GaussianPyramid(Mat);
//vector<gpu::GpuMat> censur_maps(vector<Mat>);
vector<Mat> censur_maps(vector<Mat>);
vector<gpu::GpuMat> Get_GaussianPyramid_GPU(gpu::GpuMat);
vector<gpu::GpuMat> censur_maps_GPU(vector<gpu::GpuMat>);
gpu::GpuMat aggregateMaps_GPU(vector<gpu::GpuMat> );
Mat aggregateMaps(vector<Mat>);
long timestamp(void);
