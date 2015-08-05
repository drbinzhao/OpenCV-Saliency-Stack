#include "orientation.hpp"
#include "saliency_common.hpp"
#include "unistd.h"
extern Mat IntensityImg;


Mat angle0, angle45, angle90, angle135;

void* process_angle(void * ptr)
{
  int *choice;
  choice= (int *)ptr;
/*
  int kernel_size =5;
  double sig = 3;
  double th = CV_PI/4; 
  double lm = CV_PI;
  double gm = 1;
  double ps = 0;
*/
int kernel_size =31;
double sig = 1;
double th = CV_PI/4; 
double lm = 1.0;
double gm = 0.02;
double ps = 0;






Mat receivedKernel, xtemp;


  Mat viz;
  receivedKernel = getGaborKernel(Size(kernel_size,kernel_size),  sig,  th*(*choice),  lm,  gm,  ps, CV_32F);

  filter2D(IntensityImg,viz,-1,receivedKernel);
  
  vector<Mat> gPyr = Get_GaussianPyramid(viz);
  gPyr = censur_maps(gPyr);

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

Mat temp;
addWeighted(angle0, 0.25, angle45, 0.25, 0, temp, -1);
addWeighted(temp, 1, angle90, 0.25, 0, temp, -1);
addWeighted(temp, 1, angle135, 0.25, 0, temp, -1);

//temp.download(xtemp);
return temp;

}	
