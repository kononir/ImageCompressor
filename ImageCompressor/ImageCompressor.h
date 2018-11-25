#pragma once

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <ctime>
#include <math.h>
#include <tchar.h>
#include <Windows.h>
#include <atlimage.h>

#define PIXEL_COLOR_MAX_VALUE 255
#define PIXEL_COLOR_MIN_VALUE 0
#define COLORS_NUMBER 3
#define SQUARE 2

typedef struct NeuralNetwork 
{
	double** trainingSample;
	double** currFirstLayerWeightMatrix;
	double** currSecondLayerWeightMatrix;

	int imagerysNumber;
	int firstLayerNeuronsNumber;
	int secondLayerNeuronsNumber;

	double firstLayerTrainingCoefficient;
	double secondLayerTrainingCoefficient;
	double maximumAllowableError;
} NeuralNetwork;

double** sliceImage(CImage image, int rectWidth, int rectHeight, int overlap);
double** compressAndDecompressImageRectangles(NeuralNetwork neuralNetwork);
CImage createDecompressedImage(double** decompressedImageRectangles, int imageWidth, int imageHeight, int rectWidth, int rectHeight, int overlap, int nBPP);
void trainNeuralNetwork(NeuralNetwork &neuralNetwork);
