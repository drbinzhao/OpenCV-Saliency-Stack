#include "common.hpp"
#include "intensity.hpp"
#include "color.hpp"
#include "orientation.hpp"
#include "saliency_common.hpp"
#include "pthread.h"

Mat AggInt, AggColor, IntensityImg;

void *intensity_processing(void *pointer) {

	vector < Mat > Intensity_Maps = Pyr_CenSur(IntensityImg);
	AggInt = aggregateMaps(Intensity_Maps);
	normalize(AggInt, AggInt, 0, 255, NORM_MINMAX, -1);


	pthread_exit (NULL);
}

void *color_processing(void *pointer) {
	Mat *imgPtr = (Mat *) pointer;
	vector < Mat > color_map;
	
	color_map = Normalize_color(*imgPtr, IntensityImg);
	vector < Mat > RGBYMap(6);

	for (int i = 0; i < 6; i++)
		addWeighted(color_map[i], 0.5, color_map[i + 6], 0.5, 0, RGBYMap[i],
				-1);

	AggColor = aggregateMaps(RGBYMap);
	normalize(AggColor, AggColor, 0, 255, NORM_MINMAX, -1);

	pthread_exit (NULL);
}

int main(int argc, char** argv) {
	cv::VideoCapture stream(0); // open video stream from any video source
	int count = 0;   
int frame_width = stream.get(CV_CAP_PROP_FRAME_WIDTH);
int frame_height = stream.get(CV_CAP_PROP_FRAME_HEIGHT);

VideoWriter outputVideo("salient_video.avi", CV_FOURCC('M', 'J', 'P', 'G'), 20 , Size(frame_width, frame_height), true); 
while(stream.isOpened())
{
  cv::Mat inputImage;
   if ( ! stream.read(inputImage) ) // try to read a frame
            break;
   
   Mat finalImage;
	Mat * ptr = NULL;
	pthread_t intensityThread, colorThread;
   long totaltime = timestamp();
	
	//long intTime = timestamp();
	IntensityImg = Get_Intensity_Image(inputImage);
	pthread_create(&intensityThread, NULL, intensity_processing, (void *) ptr);
	//pthread_join(intensityThread, NULL);
	//long intFinal = timestamp() - intTime;
	//cout << "Intensity Map Time: " << intFinal << "\n";
	
	ptr = &inputImage;
	//long colTime = timestamp();
	pthread_create(&colorThread, NULL, color_processing, (void *) ptr);
	//pthread_join(colorThread, NULL);
	//long colFinal = timestamp() - colTime;
	//cout << "Color Map Time: " << colFinal << "\n";
	
	//long orTime = timestamp();
	Mat AggOr = getGaborImage();
	normalize(AggOr, AggOr, 0, 255, NORM_MINMAX, -1);
	//long orFinal = timestamp() - orTime;
	//cout << "Orientation Map Time: " << orFinal << "\n";

        pthread_join(intensityThread, NULL);
        pthread_join(colorThread, NULL);

	
	finalImage = (AggInt + AggColor + AggOr) / 3;
	normalize(finalImage, finalImage, 0, 255, NORM_MINMAX, -1);

	for (int bCtr = 0; bCtr < 4; bCtr++) {
		pyrUp(finalImage, finalImage);
	}
	
	long finaltime = timestamp() - totaltime;
	cout << "Total Time: " << finaltime << "\n";
	Mat contImg;
	inRange(finalImage, 130, 255, contImg);
	vector < vector<Point> > contours;
	vector < Vec4i > hierarchy;

	findContours(contImg, contours, hierarchy, CV_RETR_CCOMP,
			CV_CHAIN_APPROX_SIMPLE);
	for (int i = 0; i >= 0; i = hierarchy[i][0]) {
		Scalar color(rand() & 255, rand() & 255, rand() & 255);
		drawContours(inputImage, contours, i, color, 3, 8, hierarchy);
	}


   
   outputVideo.write(inputImage);
  
}
	return 0;
}

