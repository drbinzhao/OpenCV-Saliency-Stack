#include "common.hpp"
#include "saliency_common.hpp"
#include "intensity.hpp"

extern Mat IntensityImg;
//Generates Intensity Image

gpu::GpuMat Get_Intensity_Image_GPU(Mat inputImage)
{
 	gpu::GpuMat d_src(inputImage);
	gpu::GpuMat gputemp;
	vector<gpu::GpuMat> d_vec;
	gpu::split(d_src, d_vec);
	gpu::addWeighted(d_vec[0], 1.0/3, d_vec[1], 1.0/3, 0.0, gputemp, -1);
	gpu::addWeighted(gputemp, 1, d_vec[2], 1.0/3, 0.0, gputemp, -1);
	return gputemp; 
}

Mat Get_Intensity_Image(Mat inputImage)
{
	vector<Mat> rgb;
	Mat cputemp;
	Mat retImage;
	split(inputImage, rgb);
	addWeighted(rgb[0], 1.0/3, rgb[1], 1.0/3, 0.0, cputemp, -1);
	addWeighted(cputemp, 1, rgb[2], 1.0/3, 0.0, retImage, -1);
	return retImage;	
}

//Does gaussian pyramid and center surround for intensity map
vector<Mat> Pyr_CenSur(void)
{
	vector<Mat> IntPyr, retmap;
	
	IntPyr = Get_GaussianPyramid(IntensityImg);
	retmap = censur_maps(IntPyr);
	return retmap;		
}

vector<gpu::GpuMat> Pyr_CenSur_GPU(gpu::GpuMat input)
{
	vector<gpu::GpuMat> IntPyr, retmap;
	IntPyr = Get_GaussianPyramid_GPU(input);
	retmap = censur_maps_GPU(IntPyr);
	return retmap;		
}


