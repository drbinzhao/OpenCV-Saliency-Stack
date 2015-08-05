#include "common.hpp"
#include "color.hpp"
#include "math.h"
#include "saliency_common.hpp"

vector<Mat> Normalize_color(Mat inputImage, Mat IntensityImg)
{

	double maxInt;

	minMaxLoc(IntensityImg, NULL, &maxInt, NULL, NULL);

//Normalize all color channels
int i = 0, j = 0;
Vec3b intensity = inputImage.at<Vec3b>(i,j);

	for(i=0; i<inputImage.rows; i++)//row
	{
		for(j=0; j<inputImage.cols; j++)//
		{
			if(inputImage.at<uchar>(i, j) >= 0.1 * maxInt)
			{
				intensity.val[0] = (intensity.val[0] * 255)/maxInt;//b
				intensity.val[1] = (intensity.val[1] * 255)/maxInt;//g
				intensity.val[2] = (intensity.val[2] * 255)/maxInt;//r	
				
			}
		}
	}

//generate the channel y
vector<Mat> rgby(4);
vector<Mat> temp;
split(inputImage, temp);

rgby[0] = temp[0] - (temp[1] + temp[2])/2;
rgby[1] = temp[1] - (temp[0] + temp[2])/2;
rgby[2] = temp[2] - (temp[1] + temp[0])/2;
rgby[3] = (temp[2] + temp[1])/2 - abs((temp[2] - temp[1])/2) - temp[0];

threshold(rgby[3], rgby[3], 0, 255, THRESH_TOZERO);
vector<Mat> red_pyr(9);
vector<Mat> green_pyr(9);
vector<Mat> blue_pyr(9);
vector<Mat> yellow_pyr(9);

red_pyr = Get_GaussianPyramid(rgby[2]);
green_pyr = Get_GaussianPyramid(rgby[1]);
blue_pyr = Get_GaussianPyramid(rgby[0]);
yellow_pyr = Get_GaussianPyramid(rgby[3]);

Mat r_g, b_y, output_rg, output_by;
vector<Mat> color_map(12);

for(int c = 2; c<=4; c++)
{
	for(int delta = 3; delta<=4; delta++)
	{
		r_g = green_pyr[c+delta] - red_pyr[c+delta];
		b_y = yellow_pyr[c+delta] - blue_pyr[c+delta];
		for(int bCtr =1; bCtr<=delta; bCtr++)
		{
			pyrUp(r_g, output_rg, Size(r_g.cols*2, r_g.rows*2));
			r_g = output_rg;
			pyrUp(b_y, output_by, Size(b_y.cols*2, b_y.rows*2));
			b_y = output_by;
		}
		if(red_pyr[c].size() != output_rg.size())
		{
			color_map[2*c+delta-7] = abs((red_pyr[c] - green_pyr[c]) - output_rg(Range(0, red_pyr[c].rows), Range(0, red_pyr[c].cols)));
color_map[2*c+delta-1] = abs((blue_pyr[c] - yellow_pyr[c]) - output_by(Range(0, red_pyr[c].rows), Range(0, red_pyr[c].cols)));
		}
		else
		{
			color_map[2*c+delta-7] = abs((red_pyr[c] - green_pyr[c]) - output_rg);
			color_map[2*c+delta-1] = abs((blue_pyr[c] - yellow_pyr[c]) - output_by);
		}
	}
}

return color_map;
}


vector<gpu::GpuMat> Normalize_color_GPU(gpu::GpuMat inputImage)
{

	//double maxInt;

	//gpu::minMaxLoc(IntensityImg, NULL, &maxInt, NULL, NULL);

//Normalize all color channels
//int i = 0, j = 0;
/*Vec3b intensity = inputImage.at<Vec3b>(i,j);

	for(i=0; i<inputImage.rows; i++)//row
	{
		for(j=0; j<inputImage.cols; j++)//
		{
			if(inputImage.at<uchar>(i, j) >= 0.1 * maxInt)
			{
				intensity.val[0] = (intensity.val[0] * 255)/maxInt;//b
				intensity.val[1] = (intensity.val[1] * 255)/maxInt;//g
				intensity.val[2] = (intensity.val[2] * 255)/maxInt;//r	
				
			}
		}
	}
*/
//generate the channel y
vector<gpu::GpuMat> rgby(4);
vector<gpu::GpuMat> temp;
gpu::split(inputImage, temp);

//rgby[0] = temp[0] - (temp[1] + temp[2])/2;

gpu::GpuMat interm4;
gpu::addWeighted(temp[1], 0.5, temp[2], 0.5, 0, interm4, -1);
gpu::subtract(temp[0],interm4,rgby[0]);


//rgby[1] = temp[1] - (temp[0] + temp[2])/2;

gpu::GpuMat interm5;
gpu::addWeighted(temp[0], 0.5, temp[2], 0.5, 0, interm5, -1);
gpu::subtract(temp[1],interm5,rgby[1]);


//rgby[2] = temp[2] - (temp[1] + temp[0])/2;


gpu::GpuMat interm6;
gpu::addWeighted(temp[1], 0.5, temp[0], 0.5, 0, interm6, -1);
gpu::subtract(temp[2],interm6,rgby[2]);






//rgby[3] = (temp[2] + temp[1])/2 - interm - temp[0];

gpu::GpuMat interm7;
gpu::addWeighted(temp[2], 0.5, temp[1], 0.5, 0, interm7, -1);
gpu::GpuMat interm; 
gpu::absdiff(temp[2],temp[1], interm); 
gpu::divide(0.5,interm,interm);
gpu::GpuMat interm8;
gpu::subtract(interm7,interm,interm8);

gpu::subtract(interm8,temp[0],rgby[3]);




gpu::threshold(rgby[3], rgby[3], 0, 255, THRESH_TOZERO);

vector<gpu::GpuMat> red_pyr(9);
vector<gpu::GpuMat> green_pyr(9);
vector<gpu::GpuMat> blue_pyr(9);
vector<gpu::GpuMat> yellow_pyr(9);

red_pyr = Get_GaussianPyramid_GPU(rgby[2]);
green_pyr = Get_GaussianPyramid_GPU(rgby[1]);
blue_pyr = Get_GaussianPyramid_GPU(rgby[0]);
yellow_pyr = Get_GaussianPyramid_GPU(rgby[3]);

gpu::GpuMat r_g, b_y, output_rg, output_by;
vector<gpu::GpuMat> color_map(12);

for(int c = 2; c<=4; c++)
{
	for(int delta = 3; delta<=4; delta++)
	{
		//r_g = green_pyr[c+delta] - red_pyr[c+delta];
                gpu::subtract(green_pyr[c+delta],red_pyr[c+delta],r_g);

		//b_y = yellow_pyr[c+delta] - blue_pyr[c+delta];
                 gpu::subtract(yellow_pyr[c+delta],blue_pyr[c+delta],b_y);

		for(int bCtr =1; bCtr<=delta; bCtr++)
		{
			gpu::pyrUp(r_g, output_rg);
			//r_g = output_rg;
			gpu::pyrUp(b_y, output_by);
			//b_y = output_by;
		}
		if(red_pyr[c].size() != output_rg.size())
		{
			//color_map[2*c+delta-7] = abs((red_pyr[c] - green_pyr[c]) - output_rg(Range(0, red_pyr[c].rows), Range(0, red_pyr[c].cols)));
                        gpu::GpuMat interm2;
                        gpu::resize(output_rg, output_rg,red_pyr[c].size(),1.0,1.0);  
                        gpu::subtract(red_pyr[c],green_pyr[c],interm2);
                        gpu::absdiff(interm2, output_rg, color_map[2*c+delta-7]);

//color_map[2*c+delta-1] = abs((blue_pyr[c] - yellow_pyr[c]) - output_by(Range(0, red_pyr[c].rows), Range(0, red_pyr[c].cols)));

                        gpu::GpuMat interm3;
                        gpu::resize(output_by, output_by,red_pyr[c].size(),1.0,1.0);  
                        gpu::subtract(blue_pyr[c],yellow_pyr[c],interm3);
                        gpu::absdiff(interm3, output_by, color_map[2*c+delta-1]);
		}
		else
		{
                        gpu::GpuMat interm2;
                        gpu::subtract(red_pyr[c],green_pyr[c],interm2);
                        gpu::GpuMat interm3;
                        gpu::subtract(blue_pyr[c],yellow_pyr[c],interm3);
                    
			//color_map[2*c+delta-7] = abs((red_pyr[c] - green_pyr[c]) - output_rg);
			//color_map[2*c+delta-1] = abs((blue_pyr[c] - yellow_pyr[c]) - output_by);
                        
                        gpu::absdiff(interm2, output_rg, color_map[2*c+delta-7]);
                        gpu::absdiff(interm3, output_by, color_map[2*c+delta-1]);
		}
	}
}

return color_map;
}
