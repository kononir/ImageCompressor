#pragma once

#include <vector>
#include <iterator>
#include "Matrix.h"

class NeuralNetwork
{
private:
	std::vector<Matrix> trainingSample;

	Matrix firstLayerWeightMatrix;
	Matrix secondLayerWeightMatrix;

	double firstLayerEducationCoefficient;
	double secondLayerEducationCoefficient;
	double maximumAllowableError;

	double currError;
	int currTrainingStep;

public:
	NeuralNetwork(
		std::vector<Matrix> trainingSample,
		Matrix firstLayerWeightMatrix,
		Matrix secondLayerWeightMatrix,
		double firstLayerTrainingCoefficient,
		double secondLayerTrainingCoefficient,
		double maximumAllowableError
	);

	~NeuralNetwork(void);

	void train();

	std::vector<double> multiplyOneAndTwoDimensiaonalsMatrix(std::vector<double> oneDimensionalMatrix, std::vector<std::vector<double>> twoDimensionalMatrix);
};

