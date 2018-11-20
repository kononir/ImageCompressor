#include "StdAfx.h"
#include "NeuralNetwork.h"


NeuralNetwork::NeuralNetwork(
	std::vector<Matrix> trainingSample, 
	Matrix firstLayerWeightMatrix, 
	Matrix secondLayerWeightMatrix, 
	double firstLayerTrainingCoefficient, 
	double secondLayerTrainingCoefficient, 
	double maximumAllowableError): 
trainingSample(trainingSample), 
	firstLayerWeightMatrix(firstLayerWeightMatrix), 
	secondLayerWeightMatrix(secondLayerWeightMatrix),
	firstLayerEducationCoefficient(firstLayerTrainingCoefficient),
	secondLayerEducationCoefficient(secondLayerTrainingCoefficient),
	maximumAllowableError(maximumAllowableError)
{
}


NeuralNetwork::~NeuralNetwork(void)
{
}


void NeuralNetwork::train() {
	std::vector<Matrix> rezultImagerys;

	for (int currImageryIndex = 0; currImageryIndex < trainingSample.size(); currImageryIndex++) {
	}
}
