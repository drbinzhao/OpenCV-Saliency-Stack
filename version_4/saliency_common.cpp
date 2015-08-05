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

vector<gpu::GpuMat> Get_GaussianPyramid_GPU(gpu::GpuMat inputImage)
{
gpu::GpuMat img, ret;
Mat xtemp;
    vector<gpu::GpuMat> retImage(9);
    ret = inputImage;
    retImage[0] = inputImage;
    for(int i = 1; i<9; i++)
    {
	gpu::GaussianBlur(ret, img, Size(3,3), 0, 0);
	
	gpu::pyrDown(img, ret);
      //gpu::resize(ret, img, Size(),0.5,0.5);  
      //ret = img;
        
	retImage[i] = ret;
    }
	//buildPyramid(inputImage, retImage, 9);

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

vector<gpu::GpuMat> censur_maps_GPU(vector<gpu::GpuMat> IntPyrG)
{
//Mat temp, cen_sur;
//vector<Mat> intense(6);
  /*
vector <gpu::GpuMat> IntPyrG(9);
//gpu::GpuMat IntPyrG(first,second,third,fourth,fifth,sixth);

Mat temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

temp0 = IntPyr[0];
temp1 = IntPyr[1];
temp2 = IntPyr[2];
temp3 = IntPyr[3];
temp4 = IntPyr[4];
temp5 = IntPyr[5];
temp6 = IntPyr[6];
temp7 = IntPyr[7];
temp8 = IntPyr[8];


IntPyrG[0].upload(temp0);
IntPyrG[1].upload(temp1);
IntPyrG[2].upload(temp2);
IntPyrG[3].upload(temp3);
IntPyrG[4].upload(temp4);
IntPyrG[5].upload(temp5);
IntPyrG[6].upload(temp6);
IntPyrG[7].upload(temp7);
IntPyrG[8].upload(temp8);
*/


gpu::GpuMat temp, cen_sur, xtemp;
vector <gpu::GpuMat> intense(6);
vector <Mat> intensed(6);
  
	for(int c = 2; c<=4; c++)
	{
		for(int delta = 3; delta<=4; delta++)
		{
			temp = IntPyrG[c+delta];
			for(int bCtr = 1; bCtr<=delta; bCtr++)		
			{
                        // Mat dsur;	
                        //cen_sur = gpu::GpuMat::create(temp.rows*2 , temp.cols*2, CV_8UC1);	
                            //cen_sur.create(temp.rows*2 , temp.cols*2,temp.depth());				
			
                         //resize(cen_sur,cen_sur,temp.size());
                       
                        // gpu::pyrUp(temp, cen_sur);  /*, gpu::Size(temp.cols*2, temp.rows*2)*/
                         gpu::pyrUp(temp, cen_sur);  
                        
                         //gpu::resize(temp, cen_sur,temp.size(),2.0,2.0);  
			 temp = cen_sur;
			}	

			if(IntPyrG[c].size() != temp.size())
			{
			  //gpu::subtract(cen_sur, IntPyrG[c], xtemp); /*cen_sur/*(Range(0, IntPyrG[c].rows), Range(0, IntPyrG[c].cols))*/ 
			  //gpu::abs((xtemp), intense[2*c+delta-7]);
                          gpu::resize(temp, cen_sur,IntPyrG[c].size(),1.0,1.0);  
                          gpu::absdiff(IntPyrG[c], cen_sur, intense[2*c+delta-7]);
			}
			else
			{
			  // gpu::subtract(IntPyrG[c], temp, xtemp);
			   //gpu::abs(xtemp, intense[2*c+delta-7]);

                            gpu::absdiff(IntPyrG[c], temp, intense[2*c+delta-7]);
			}
		
		}
	
	}
/*intense[0].download(intensed[0]);
intense[1].download(intensed[1]);
intense[2].download(intensed[2]);
intense[3].download(intensed[3]);
intense[4].download(intensed[4]);
intense[5].download(intensed[5]);
*/
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

gpu::GpuMat aggregateMaps_GPU(vector<gpu::GpuMat> inputMap)
{
vector<gpu::GpuMat> outputMap(4);
gpu::GpuMat temp;

gpu::pyrDown(inputMap[0], outputMap[0]);//c {2,3}
gpu::pyrDown(outputMap[0], outputMap[0]);

gpu::pyrDown(inputMap[1], outputMap[1]); //c {2,4}
gpu::pyrDown(outputMap[1], outputMap[1]);

gpu::pyrDown(inputMap[2], outputMap[2]); //c {3, 3}
gpu::pyrDown(inputMap[3], outputMap[3]); //c {3, 4}

gpu::addWeighted(outputMap[0], 1.0/6, outputMap[1], 1.0/6, 0, temp, -1);

gpu::addWeighted(temp, 1, outputMap[2], 1.0/6, 0, temp, -1);
gpu::addWeighted(temp, 1, outputMap[3], 1.0/6, 0, temp, -1);
gpu::addWeighted(temp, 1, inputMap[4], 1.0/6, 0, temp, -1);
gpu::addWeighted(temp, 1, inputMap[5], 1.0/6, 0, temp, -1);

return temp;
}

