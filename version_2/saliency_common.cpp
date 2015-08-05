#include "saliency_common.hpp"
#include "math.h"
#include <sys/time.h>

//time stamp
long timestamp()
{
	struct timeval current_time;
	gettimeofday(&current_time, NULL);
	return(current_time.tv_sec * 1000000 + current_time.tv_usec);
}

//Build Gaussian Pyramids
vector<Mat> Get_GaussianPyramid(Mat inputImage)
{
	vector<Mat> retImage;
	buildPyramid(inputImage, retImage, 9);
	return retImage;
}

//common function for doing center surround
vector<Mat> censur_maps(vector<Mat> IntPyr)
{
Mat temp, cen_sur;
vector<Mat> intense(6);

for(int c = 2; c<=4; c++)
	{
		for(int delta = 3; delta<=4; delta++)
		{
			temp = IntPyr[c+delta];
			for(int bCtr = 1; bCtr<=delta; bCtr++)		
			{						
			pyrUp(temp, cen_sur, Size(temp.cols*2, temp.rows*2));
			temp = cen_sur;
			}	

		if(IntPyr[c].size() != temp.size())
		{
			intense[2*c+delta-7] = abs(IntPyr[c] - cen_sur(Range(0, IntPyr[c].rows), Range(0, IntPyr[c].cols)));
		}
		else
		{
			intense[2*c+delta-7] = abs(IntPyr[c] - cen_sur);
		}
		
	}
}
return intense;

}


Mat aggregateMaps(vector<Mat> inputMap)
{
vector<Mat> outputMap(4);
Mat temp;

pyrDown(inputMap[0], outputMap[0]);//c {2,3}
pyrDown(outputMap[0], outputMap[0]);

pyrDown(inputMap[1], outputMap[1]); //c {2,4}
pyrDown(outputMap[1], outputMap[1]);

pyrDown(inputMap[2], outputMap[2]); //c {3, 3}
pyrDown(inputMap[3], outputMap[3]); //c {3, 4}

addWeighted(outputMap[0], 1.0/6, outputMap[1], 1.0/6, 0, temp, -1);

addWeighted(temp, 1, outputMap[2], 1.0/6, 0, temp, -1);
addWeighted(temp, 1, outputMap[3], 1.0/6, 0, temp, -1);
addWeighted(temp, 1, inputMap[4], 1.0/6, 0, temp, -1);
addWeighted(temp, 1, inputMap[5], 1.0/6, 0, temp, -1);

return temp;
}

