#pragma once

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <ctime>
#include <Windows.h>

/*
* Подключение библиотеки CImg для работы с изображениями
* Ссылка на используемую библиотеку: https://framagit.org/dtschump/CImg/tree/v.2.4.1
*/
#include "CImg-2.4.1/CImg.h"

#define PIXEL_COLOR_MAX_VALUE 255
#define PIXEL_COLOR_MIN_VALUE 0
#define COLORS_NUMBER 3
#define SQUARE 2
#define ONE_DIMENSIONAL 1
#define Z_VALUE 0

typedef cimg_library::CImg<unsigned char> Image;

typedef struct NeuralNetwork 
{
	float** trainingSample;
	float** currFirstLayerWeightMatrix;
	float** currSecondLayerWeightMatrix;

	int imagerysNumber;
	int firstLayerNeuronsNumber;
	int secondLayerNeuronsNumber;
	int numberOfTrainingSteps;

	float firstLayerTrainingCoefficient;
	float secondLayerTrainingCoefficient;
	float maximumAllowableError;
	float reachedError;
} NeuralNetwork;

float** sliceImage(Image image, int rectWidth, int rectHeight, int overlap);
float** compressAndDecompressImageRectangles(NeuralNetwork &neuralNetwork);
void saveDecompressedImage(float** decompressedImageRectangles, int imageWidth, int imageHeight, int rectWidth, int rectHeight, int overlap);
void trainNeuralNetwork(NeuralNetwork &neuralNetwork);
