// ImageCompressor.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "bitmap/bitmap_image.hpp"
#include "Matrix.h"
#include "NeuralNetwork.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	string fileName;
	int rectHeight, rectWidth, overlap, firstLayerNeuronsNumber, secondLayerNeuronsNumber;
	int colorsNumber = 3;
	double firstLayerTrainingCoefficient, secondLayerTrainingCoefficient, maximumAllowableError;

	cout << "Input file name: " << endl;
	getline(cin, fileName, '\n');

	cout << "Input rectangle width: ";
	cin >> rectWidth;

	cout << "Input reactangle height: ";
	cin >> rectHeight;

	cout << "Input overlap: ";
	cin >> overlap;

	cout << "Input neurons number at second layer (p <= 2 * N): ";
	cin >> secondLayerNeuronsNumber;

	cout << "Input coefficient of training at first layer (0 < a <= 0.01): ";
	cin >> firstLayerTrainingCoefficient;

	cout << "Input coefficient of training at second layer (0 < a' <= 0.01): ";
	cin >> secondLayerTrainingCoefficient;

	cout << "Input maximum allowable error (0 < e <= 0.1 * p, a <= е): ";
	cin >> maximumAllowableError;

	bitmap_image image(fileName);

	if (!image) {
      cout << "Error - Failed to open: " + fileName << endl;
      return 1;
    }

	vector<Matrix> trainingSample = sliceImage(image, rectWidth, rectHeight, overlap);

	firstLayerNeuronsNumber = rectWidth * rectHeight * colorsNumber;

	Matrix firstLayerWeightMatrix(firstLayerNeuronsNumber, secondLayerNeuronsNumber);
	Matrix secondLayerWeightMatrix(firstLayerWeightMatrix.getMatrix());
	
	NeuralNetwork neuralNetwork(
		trainingSample, 
		firstLayerWeightMatrix, 
		secondLayerWeightMatrix, 
		firstLayerTrainingCoefficient, 
		secondLayerTrainingCoefficient,
		maximumAllowableError
	);

	neuralNetwork.train();

	return 0;
}


vector<Matrix> sliceImage(bitmap_image image, int rectWidth, int rectHeight, int overlap) {
	int minReserve = 10;
	
	vector<Matrix> trainingSample;
	trainingSample.reserve(minReserve);

	for (int currRectX = 0; currRectX < image.width(); currRectX += rectWidth) {
		if (image.width() - currRectX - 1 < rectWidth) {
			currRectX -= overlap;
		}

		for (int currRectY = 0; currRectY < image.height(); currRectY += rectHeight) {
			if (image.height() - currRectY - 1 < rectHeight) {
				currRectY -= overlap;
			}

			bitmap_image imagePart;

			image.region(currRectX, currRectY, rectWidth, rectHeight, imagePart);

			Matrix imagery(imagePart);

			trainingSample.push_back(imagery);
		}
	}

	return trainingSample;
}
