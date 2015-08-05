#include "common.hpp"
#include "intensity.hpp"
#include "color.hpp"
#include "orientation.hpp"
#include "saliency_common.hpp"


int main(int argc, char** argv)
{
long totaltime, intTime, intTime_o, colTime, colTime_o , orTime, orTime_o;
if(argc != 2)
{
	cout << "No image"<<endl;
	return -1;
}
cout<<"Loading Image: ";
cout<< argv[1]<<endl;
Mat inputImage = imread(argv[1], CV_LOAD_IMAGE_COLOR);

if(!inputImage.data)
{
	cout <<"Invalid Image"<<endl;
}

Mat IntensityImg, finalImage;
for(int counter = 0; counter < 1; counter++)
{
totaltime = timestamp();

intTime = timestamp();
IntensityImg = Get_Intensity_Image(inputImage);
vector<Mat> Intensity_Maps = Pyr_CenSur(IntensityImg);
Mat AggInt = aggregateMaps(Intensity_Maps);
normalize(AggInt, AggInt, 0, 255, NORM_MINMAX, -1);
intTime_o = timestamp() - intTime;

colTime = timestamp();
vector<Mat> color_map;
color_map = Normalize_color(inputImage, IntensityImg);
vector<Mat> RGBYMap(6); 
for(int i = 0; i<6; i++)
addWeighted(color_map[i], 0.5, color_map[i+6], 0.5, 0, RGBYMap[i], -1);
Mat AggColor = aggregateMaps(RGBYMap);
normalize(AggColor, AggColor, 0, 255, NORM_MINMAX, -1);
colTime_o = timestamp() - colTime;

orTime = timestamp();
Mat AggOr;
AggOr = getGaborImage(IntensityImg);
normalize(AggOr, AggOr, 0, 255, NORM_MINMAX, -1);
orTime_o = timestamp() - orTime;

finalImage = (AggInt + AggColor + AggOr) /3;
normalize(finalImage, finalImage, 0, 255, NORM_MINMAX, -1);

for(int bCtr = 0; bCtr<4; bCtr++)
{
	pyrUp(finalImage, finalImage);
}

cout <<"Intensity Time: "<< (intTime_o) << "\n";
cout <<"Color Time: "<< (colTime_o) << "\n";
cout <<"Orientation Time: "<< (orTime_o) << "\n";
cout <<"Total Time: "<< (timestamp() - totaltime) << "\n";
}



Mat contImg;
inRange(finalImage, 160, 230, contImg);
vector<vector<Point> > contours;
vector<Vec4i> hierarchy;

findContours(contImg, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
for(int i = 0; i>=0; i =hierarchy[i][0])
{
	Scalar color(rand()&255, rand()&255, rand()&255);
	drawContours(inputImage, contours, i, color, 3, 8, hierarchy);
}

imwrite("Salient_Image.jpg" , inputImage);

waitKey(0);
return 0;
}
