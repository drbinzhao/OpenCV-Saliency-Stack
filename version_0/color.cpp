#include "common.hpp"
#include "color.hpp"
#include "math.h"
#include "saliency_common.hpp"

vector<Mat> Normalize_color(Mat inputImage, Mat IntensityImg)
{
	double maxInt;
minMaxLoc(IntensityImg, NULL, &maxInt, NULL, NULL);

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
threshold(rgby[3], rgby[3], 0, UINT8_MAX, THRESH_TOZERO);

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
