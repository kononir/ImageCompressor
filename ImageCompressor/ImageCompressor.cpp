// ImageCompressor.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "ImageCompressor.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	string fileName;
	int rectHeight, rectWidth, overlap;

	NeuralNetwork neuralNetwork;

	cout << "Input file name: " << endl;
	getline(cin, fileName, '\n');

	wstring widestr = wstring(fileName.begin(), fileName.end());
	const wchar_t *fileName_t = widestr.c_str();

	CImage image;
	image.Load(fileName_t);

	if (!image) {
      cout << "Error - Failed to open: " + fileName << endl;

	  system("pause");
      return 1;
    }

	cout << "Input rectangle width: ";
	cin >> rectWidth;

	cout << "Input reactangle height: ";
	cin >> rectHeight;

	cout << "Input overlap: ";
	cin >> overlap;

	cout << "Input neurons number at second layer (p <= 2 * N): ";
	cin >> neuralNetwork.secondLayerNeuronsNumber;

	cout << "Input coefficient of training at first layer (0 < a <= 0.01): ";
	cin >> neuralNetwork.firstLayerTrainingCoefficient;

	cout << "Input coefficient of training at second layer (0 < a' <= 0.01): ";
	cin >> neuralNetwork.secondLayerTrainingCoefficient;

	cout << "Input maximum allowable error (0 < e <= 0.1 * p, a <= е): ";
	cin >> neuralNetwork.maximumAllowableError;

	neuralNetwork.firstLayerNeuronsNumber = rectWidth * rectHeight * COLORS_NUMBER;

	neuralNetwork.trainingSample = sliceImage(image, rectWidth, rectHeight, overlap);

	trainNeuralNetwork(neuralNetwork);

	system("pause");
	return 0;
}


vector<Matrix> sliceImage(CImage image, int rectWidth, int rectHeight, int overlap) {
	vector<Matrix> trainingSample;

	for (int currRectX = 0; currRectX < (int) image.GetHeight(); currRectX += rectWidth) {
		if ((int) image.GetWidth() - currRectX - 1 < rectWidth) {
			currRectX -= overlap;
		}

		for (int currRectY = 0; currRectY < (int) image.GetHeight(); currRectY += rectHeight) {
			if ((int) image.GetHeight() - currRectY - 1 < rectHeight) {
				currRectY -= overlap;
			}

			Matrix imagery(ONE_DIMENSIONAL);

			// Цикл для создания эталонного вектора и преобразования его компонент для дальнейшей обработки 
			for (int currPixelY = currRectY; currPixelY < currRectY + rectHeight; currPixelY++) {

				for (int currPixelX = currRectX; currPixelX < currRectX + rectWidth; currPixelX++) {
					COLORREF color = image.GetPixel(currPixelX, currPixelY);

					double red = (double) GetRValue(color);
					double green = (double) GetGValue(color);
					double blue = (double) GetBValue(color);

					double transformedPixelRedValue = (2 * red / (double) PIXEL_COLOR_MAX_VALUE) - 1;
					double transformedPixelGreenValue = (2 * green / (double) PIXEL_COLOR_MAX_VALUE) - 1;
					double transformedPixelBlueValue = (2 * blue / (double) PIXEL_COLOR_MAX_VALUE) - 1;

					imagery[VECTOR_ROW].push_back(transformedPixelRedValue);
					imagery[VECTOR_ROW].push_back(transformedPixelGreenValue); 
					imagery[VECTOR_ROW].push_back(transformedPixelBlueValue); 
				}
			}

			trainingSample.push_back(imagery);
		}
	}

	return trainingSample;
}


Matrix makeMatrixWithRandomValues(int rowNumber, int colNumber)
{
	double minWeight = -1.0;
	double maxWeight = 1.0;

	srand((unsigned int) time(0));

	Matrix matrix(rowNumber);

	for (int currRowNumber = 0; currRowNumber < rowNumber; currRowNumber++) {

		for (int currColNumber = 0; currColNumber < colNumber; currColNumber++) {
			matrix[currRowNumber].push_back((((double) rand() / RAND_MAX) * (maxWeight - minWeight)) + minWeight);
		}
	}

	return matrix;
}


Matrix transposeMatrix(Matrix transposableMatrix)
{
	int colNumber = transposableMatrix.size();
	int rowNumber = transposableMatrix.front().size();

	Matrix matrix(rowNumber);

	for (int currRowNumber = 0; currRowNumber < rowNumber; currRowNumber++) {

		for (int currColNumber = 0; currColNumber < colNumber; currColNumber++) {
			matrix[currRowNumber].push_back(transposableMatrix[currColNumber][currRowNumber]);
		}
	}

	return matrix;
}


void trainNeuralNetwork(NeuralNetwork &neuralNetwork) {
	vector<Matrix> &q = neuralNetwork.trainingSample;
	Matrix &W = neuralNetwork.currFirstLayerWeightMatrix;
	Matrix &Ws = neuralNetwork.currSecondLayerWeightMatrix;

	int N = neuralNetwork.firstLayerNeuronsNumber;
	int p = neuralNetwork.secondLayerNeuronsNumber;

	double a = neuralNetwork.firstLayerTrainingCoefficient;
	double as = neuralNetwork.secondLayerTrainingCoefficient;
	double e = neuralNetwork.maximumAllowableError;
	double eForAverage = 2 * e;

	double RMSEforTrainingSample;

	do {
		RMSEforTrainingSample = 0;

		for (int currImageryIndex = 0; currImageryIndex < (int) q.size(); currImageryIndex++) {
			Matrix &Xi = neuralNetwork.trainingSample[currImageryIndex];
			Matrix &Yi = neuralNetwork.prevCompressedRezult;
			Matrix &Xdi = neuralNetwork.prevRezultDelta;

			boolean isFirstStep = Yi.empty();

			if (isFirstStep) {
				W = makeMatrixWithRandomValues(N, p); //можно совместить
				Ws = transposeMatrix(W);
			} else {
				Matrix &YiT = transposeMatrix(Yi);
				Matrix &as_YiT = composeCoefficientAndMatrix(as, YiT);
				Matrix &as_YiT_Xdi = composeMatrixes(as_YiT, Xdi);
				Ws = subtractMatrixes(Ws, as_YiT_Xdi);

				Matrix &XiT = transposeMatrix(Xi);
				Matrix &a_XiT = composeCoefficientAndMatrix(a, XiT);
				Matrix &WsT = transposeMatrix(Ws);
				Matrix &Xdi_WsT = composeMatrixes(Xdi, WsT);
				Matrix &a_XiT_Xdi_WsT = composeMatrixes(a_XiT, Xdi_WsT);
				W = subtractMatrixes(W, a_XiT_Xdi_WsT);
			}
			
			Yi = composeMatrixes(Xi, W);
			Matrix &Xis = composeMatrixes(Yi, Ws);
			Xdi = subtractMatrixes(Xis, Xi);

			RMSEforTrainingSample += computeRMSEforRectangle(Xdi);
		}
	} while (RMSEforTrainingSample > eForAverage);
}


Matrix composeMatrixes(Matrix firstMatrix, Matrix secondMatrix) {
	int rezultMatrixRowNumber = firstMatrix.size();
	int rezultMatrixColNumber = secondMatrix.front().size();
	int summedElementsNumber = secondMatrix.size();

	Matrix rezultMatrix(rezultMatrixRowNumber);

	for (int currRowNumber = 0; currRowNumber < rezultMatrixRowNumber; currRowNumber++) {

		for (int currColNumber = 0; currColNumber < rezultMatrixColNumber; currColNumber++) {
			double beginValue = 0;
			rezultMatrix[currRowNumber].push_back(beginValue);

			for (int currSummedEl = 0; currSummedEl < summedElementsNumber; currSummedEl++) {
				rezultMatrix[currRowNumber][currColNumber] += firstMatrix[currRowNumber][currSummedEl] * secondMatrix[currSummedEl][currColNumber];
			}
		}
	}

	return rezultMatrix;
}


Matrix subtractMatrixes(Matrix firstMatrix, Matrix secondMatrix) {
	int rezultMatrixRowNumber = firstMatrix.size();
	int rezultMatrixColNumber = firstMatrix.front().size();

	Matrix rezultMatrix(rezultMatrixRowNumber);

	for (int currRowNumber = 0; currRowNumber < rezultMatrixRowNumber; currRowNumber++) {

		for (int currColNumber = 0; currColNumber < rezultMatrixColNumber; currColNumber++) {
			rezultMatrix[currRowNumber].push_back(firstMatrix[currRowNumber][currColNumber] - secondMatrix[currRowNumber][currColNumber]);
		}
	}

	return rezultMatrix;
}


Matrix composeCoefficientAndMatrix(double coefficient, Matrix matrix) {
	int rezultMatrixRowNumber = matrix.size();
	int rezultMatrixColNumber = matrix.front().size();

	Matrix rezultMatrix(rezultMatrixRowNumber);

	for (int currRowNumber = 0; currRowNumber < rezultMatrixRowNumber; currRowNumber++) {

		for (int currColNumber = 0; currColNumber < rezultMatrixColNumber; currColNumber++) {
			rezultMatrix[currRowNumber].push_back(coefficient * matrix[currRowNumber][currColNumber]);
		}
	}

	return rezultMatrix;
}


double computeRMSEforRectangle(Matrix deltaVectorX) {
	double RMSE = 0;
	int square = 2;

	for (int currColNumber = 0; currColNumber < (int) deltaVectorX.front().size(); currColNumber++) {
		RMSE += pow(deltaVectorX.front()[currColNumber], square);
	}

	return RMSE;
}
