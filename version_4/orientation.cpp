#include "orientation.hpp"
#include "saliency_common.hpp"
#include "unistd.h"
extern Mat IntensityImg;


Mat angle0, angle45, angle90, angle135;

void* process_angle(void * ptr)
{
  int *choice;
  choice= (int *)ptr;

  int kernel_size =5;
  double sig = 3;
  double th = CV_PI/4; 
  double lm = CV_PI;
  double gm = 1;
  double ps = 0;

//gpu::GpuMat dsrc(IntensityImg);
Mat receivedKernel, xtemp;
//gpu::GpuMat viz;

  Mat viz;
  receivedKernel = getGaborKernel(Size(kernel_size,kernel_size),  sig,  th*(*choice),  lm,  gm,  ps, CV_32F);

  //gpu::filter2D(dsrc,viz,-1,receivedKernel);
  filter2D(IntensityImg,viz,-1,receivedKernel);
  
  //vector<gpu::GpuMat> gPyr = Get_GaussianPyramid_GPU(viz);
  vector<Mat> gPyr = Get_GaussianPyramid(viz);
  gPyr = censur_maps(gPyr);
  //Mat anglegpu= aggregateMaps(gPyr);
  switch(*choice)
  {
    case 0:
      angle0= aggregateMaps(gPyr);
     break;
    case 1:
       angle45= aggregateMaps(gPyr);  
      break;
      
    case 2:
       angle90= aggregateMaps(gPyr);
      break;
      
    case 3:
       angle135= aggregateMaps(gPyr);
      break;
    default:
      break;
      
    }
  pthread_exit (NULL);
  //return NULL;
}



Mat getGaborImage(/*Mat intensityImg*/)
{

int *ptr1, *ptr2, *ptr3, *ptr4;
int a = 0, b = 1, c = 2, d = 3;

pthread_t angle_th[4];
//pthread_t thread1;

ptr1 = &a;
pthread_create(&angle_th[0], NULL, process_angle, (void *) ptr1);
ptr2 = &b;
pthread_create(&angle_th[1], NULL, process_angle, (void *) ptr2);


ptr3 = &c;
pthread_create(&angle_th[2], NULL, process_angle, (void *) ptr3);


ptr4 = &d;
pthread_create(&angle_th[3], NULL, process_angle, (void *) ptr4);



pthread_join(angle_th[0], NULL);
pthread_join(angle_th[1], NULL);
pthread_join(angle_th[2], NULL);
pthread_join(angle_th[3], NULL);
/*
for(int i = 0; i<4; i++)
{
receivedKernel = getGaborKernel(Size(kernel_size,kernel_size),  sig,  th*i,  lm,  gm,  ps, CV_32F);

gpu::filter2D(dsrc,viz[i],-1,receivedKernel);

}*/
/*
vector<gpu::GpuMat> gPyr0 = Get_GaussianPyramid_GPU(viz[0]);
vector<gpu::GpuMat> gPyr1 = Get_GaussianPyramid_GPU(viz[1]);
vector<gpu::GpuMat> gPyr2 = Get_GaussianPyramid_GPU(viz[2]);
vector<gpu::GpuMat> gPyr3 = Get_GaussianPyramid_GPU(viz[3]);

gPyr1[0].download(xtemp);
imwrite("test0.jpg", xtemp);
gPyr1[1].download(xtemp);
imwrite("test1.jpg", xtemp);
gPyr1[2].download(xtemp);
imwrite("test2.jpg", xtemp);
gPyr1[3].download(xtemp);
imwrite("test3.jpg", xtemp);
gPyr1[4].download(xtemp);
imwrite("test4.jpg", xtemp);
gPyr1[5].download(xtemp);
imwrite("test5.jpg", xtemp);
gPyr1[6].download(xtemp);
imwrite("test6.jpg", xtemp);
gPyr1[7].download(xtemp);
imwrite("test7.jpg", xtemp);
gPyr1[8].download(xtemp);
imwrite("test8.jpg", xtemp);
*/
/*
gPyr0 = censur_maps_GPU(gPyr0);
gPyr1 = censur_maps_GPU(gPyr1);
gPyr2 = censur_maps_GPU(gPyr2);
gPyr3 = censur_maps_GPU(gPyr3);

gpu::GpuMat angle0 = aggregateMaps_GPU(gPyr0);
gpu::GpuMat angle45 = aggregateMaps_GPU(gPyr1);
gpu::GpuMat angle90 = aggregateMaps_GPU(gPyr2);
gpu::GpuMat angle135 = aggregateMaps_GPU(gPyr3);
*/
//Mat xtemp;
Mat temp;
addWeighted(angle0, 0.25, angle45, 0.25, 0, temp, -1);
addWeighted(temp, 1, angle90, 0.25, 0, temp, -1);
addWeighted(temp, 1, angle135, 0.25, 0, temp, -1);

//temp.download(xtemp);
return temp;

}	
