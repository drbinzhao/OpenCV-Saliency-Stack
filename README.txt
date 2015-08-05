Version Descriptions for the projects
*************************************
version_0 : No threading or No GPU used
version_1 : One thread each for color and intensity and orientation implemented in CPU with no GPU
version_1a : One thread each for color, intensity and orientation implemented in CPU with no GPU
version_2 : One thread each for color and intensity, 4 threads for orientation. No GPU
version_2_webcam : Webcam interface for version_2 code
version_3 : Intensity and orientation multithreaded in GPU, orientation in CPU
version_4 : Intensity and orientation multithreaded in GPU, orientation as 4 threads in CPU
version_5 : All the streams in GPU with individual threads


Compiling the code
******************

1. From terminal, cd into the project which you need to compile
2. run 'make' on terminal. This generates the executable saliency.o
3. Run './saliency.o donald_duck_in.bmp' to execute saliency calculation.

Output file 'Salient_image.jpg' will be generated showing the salient part


Note:
For 'version_2_webcam' project, no input file is required. Make sure that a webcam is connected
to the board before running the code
