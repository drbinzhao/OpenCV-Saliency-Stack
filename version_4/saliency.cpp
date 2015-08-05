#include "common.hpp"
#include "intensity.hpp"
#include "color.hpp"
#include "orientation.hpp"
#include "saliency_common.hpp"
#include "pthread.h"

Mat AggInt, AggColor, IntensityImg;
//gpu::GpuMat AggIntGPU, AggColorGPU, IntensityImage_GPU;

void *intensity_processing(void *pointer) {
	gpu::GpuMat IntensityImage_GPU(IntensityImg);
	vector <gpu::GpuMat > Intensity_Maps = Pyr_CenSur_GPU(IntensityImage_GPU);
	gpu::GpuMat AggIntGPU = aggregateMaps_GPU(Intensity_Maps);
	gpu::normalize(AggIntGPU, AggIntGPU, 0, 255, NORM_MINMAX, -1);
	AggIntGPU.download(AggInt);
	pthread_exit (NULL);
}

void *color_processing(void *pointer) {
	Mat *imgPtr = (Mat *) pointer;
	vector <gpu::GpuMat > color_map(12);
	gpu::GpuMat inputGPU(*imgPtr);
	//gpu::GpuMat IntensityImage_GPU(IntensityImg);	
	//color_map = Normalize_color_GPU(inputGPU, IntensityImage_GPU);

        color_map = Normalize_color_GPU(inputGPU);
	vector <gpu::GpuMat> RGBYMap(6);

	for (int i = 0; i < 6; i++)
		gpu::addWeighted(color_map[i], 0.5, color_map[i + 6], 0.5, 0, RGBYMap[i],
				-1);

	 gpu::GpuMat AggColGPU = aggregateMaps_GPU(RGBYMap);
	 gpu::normalize(AggColGPU, AggColGPU, 0, 255, NORM_MINMAX, -1);
         AggColGPU.download(AggColor);

	pthread_exit (NULL);
}

int main(int argc, char** argv) {
	if (argc != 2) {
		cout << "No image" << endl;
		return -1;
	}
	cout << "Loading Image: ";
	cout << argv[1] << endl;
	Mat inputImage = imread(argv[1], CV_LOAD_IMAGE_COLOR);

	if (!inputImage.data) {
		cout << "Invalid Image" << endl;
	}
	Mat finalImage;
	Mat * ptr = NULL;
	pthread_t intensityThread, colorThread;
for(int counter = 0; counter <1; counter++)	
{

	long totaltime = timestamp();
	
	//long intTime = timestamp();
	IntensityImg = Get_Intensity_Image(inputImage);
	//IntensityImage_GPU = Get_Intensity_Image_GPU(inputImage);
	pthread_create(&intensityThread, NULL, intensity_processing, (void *) ptr);
	//pthread_create(&intensityThread, NULL, intensity_processing, (void *) ptrGPU);
	


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


	ptr = &inputImage;
	//long colTime = timestamp();
	pthread_create(&colorThread, NULL, color_processing, (void *) ptr);
	//pthread_join(colorThread, NULL);
	//long colFinal = timestamp() - colTime;
	//cout << "Color Map Time: " << colFinal << "\n";
	
	//long orTime = timestamp();
         //Mat AggOr;
	 //Mat AggOr = getGaborImage(IntensityImg);
         Mat AggOr = getGaborImage();
	 normalize(AggOr, AggOr, 0, 255, NORM_MINMAX, -1);
	//long orFinal = timestamp() - orTime;
	//cout << "Orientation Map Time: " << orFinal << "\n";

        pthread_join(intensityThread, NULL);
        pthread_join(colorThread, NULL);

	//gpu::GpuMat temp = AggIntGPU;
	
	finalImage = (AggInt + AggColor + AggOr) / 3;
	normalize(finalImage, finalImage, 0, 255, NORM_MINMAX, -1);

	for (int bCtr = 0; bCtr < 4; bCtr++) {
		pyrUp(finalImage, finalImage);
	}
	
	long finaltime = timestamp() - totaltime;
	cout << "Total Time: " << finaltime << "\n";
}
	Mat contImg;
	inRange(finalImage, 120, 230, contImg);
	vector < vector<Point> > contours;
	vector < Vec4i > hierarchy;

	findContours(contImg, contours, hierarchy, CV_RETR_CCOMP,
			CV_CHAIN_APPROX_SIMPLE);
	for (int i = 0; i >= 0; i = hierarchy[i][0]) {
		Scalar color(rand() & 255, rand() & 255, rand() & 255);
		drawContours(inputImage, contours, i, color, 3, 8, hierarchy);
	}

	imwrite("Salient_Image.jpg", inputImage);

	waitKey(0);
	return 0;
}

