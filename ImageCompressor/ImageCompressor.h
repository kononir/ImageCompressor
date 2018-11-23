#pragma once

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <math.h>
#include <atlimage.h>
#include <tchar.h>
#include <Windows.h>

#define PIXEL_COLOR_MAX_VALUE 255
#define VECTOR_ROW 0
#define COLORS_NUMBER 3
#define ONE_DIMENSIONAL 1

typedef std::vector<std::vector<double>> Matrix;
typedef std::vector<double> Vector;

typedef struct NeuralNetwork 
{
	std::vector<Matrix> trainingSample;
	Matrix currFirstLayerWeightMatrix;
	Matrix currSecondLayerWeightMatrix;

	Matrix prevCompressedRezult;
	Matrix prevRezultDelta;

	int firstLayerNeuronsNumber;
	int secondLayerNeuronsNumber;

	double firstLayerTrainingCoefficient;
	double secondLayerTrainingCoefficient;
	double maximumAllowableError;
} NeuralNetwork;

std::vector<Matrix> sliceImage(CImage image, int rectWidth, int rectHeight, int overlap);
Matrix makeMatrixWithRandomValues(int rowNumber, int colNumber);
Matrix transposeMatrix(Matrix transposableMatrix);
void trainNeuralNetwork(NeuralNetwork &neuralNetwork);
Matrix composeMatrixes(Matrix firstMatrix, Matrix secondMatrix);
Matrix subtractMatrixes(Matrix firstMatrix, Matrix secondMatrix);
Matrix composeCoefficientAndMatrix(double coefficient, Matrix matrix);
double computeRMSEforRectangle(Matrix deltaVectorX);