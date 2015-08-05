#include "common.hpp"
#include "saliency_common.hpp"
#include "intensity.hpp"

//Generates Intensity Image
Mat Get_Intensity_Image(Mat inputImage)
{
#ifdef GPU_MODE
	gpu::GpuMat d_src(inputImage);
	Mat retImage;
	gpu::GpuMat gputemp;
	vector<gpu::GpuMat> d_vec;
	gpu::split(d_src, d_vec);
	gpu::addWeighted(d_vec[0], 1.0/3, d_vec[1], 1.0/3, 0.0, gputemp, -1);
	gpu::addWeighted(gputemp, 1, d_vec[2], 1.0/3, 0.0, gputemp, -1);
	gputemp.download(retImage);
asdf
#else
	vector<Mat> rgb;
	Mat cputemp;
	Mat retImage;
	split(inputImage, rgb);
	addWeighted(rgb[0], 1.0/3, rgb[1], 1.0/3, 0.0, cputemp, -1);
	addWeighted(cputemp, 1, rgb[2], 1.0/3, 0.0, retImage, -1);
#endif
	return retImage;
}

//Does gaussian pyramid and center surround for intensity map
vector<Mat> Pyr_CenSur(Mat intensityImg)
{
	vector<Mat> IntPyr, retmap;
	
	IntPyr = Get_GaussianPyramid(intensityImg);
	retmap = censur_maps(IntPyr);
	return retmap;		
}


