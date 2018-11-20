#include "StdAfx.h"
#include "Matrix.h"


Matrix::Matrix(bitmap_image imagePart)
{
	int colorsNumber = 3;
	int oneDimensional = 1;

	matrix.reserve(oneDimensional);

	matrix.front().reserve(imagePart.height() * imagePart.width() * colorsNumber);

	// Цикл для создания эталонного вектора и преобразования его компонент для дальнейшей обработки 
	for (int currPixelY = 0; currPixelY < imagePart.height(); currPixelY++) {
		for (int currPixelX = 0; currPixelX < imagePart.width(); currPixelX++) {
			rgb_t pixel;

			imagePart.get_pixel(currPixelX, currPixelY, pixel);

			int pixelColorMaxValue = 255;
			double transformedPixelRedValue = (2 * pixel.red / pixelColorMaxValue) - 1;
			double transformedPixelGreenValue = (2 * pixel.green / pixelColorMaxValue) - 1;
			double transformedPixelBlueValue = (2 * pixel.blue / pixelColorMaxValue) - 1;

			matrix.front().push_back(transformedPixelRedValue);
			matrix.front().push_back(transformedPixelGreenValue); 
			matrix.front().push_back(transformedPixelBlueValue); 
		}
	}
}


Matrix::Matrix(int rowNumber, int colNumber)
{
	double minWeight = -1.0;
	double maxWeight = 1.0;

	srand(time(0));

	matrix.reserve(rowNumber);

	for (int currRowNumber = 0; currRowNumber < rowNumber; currRowNumber++) {
		matrix[currRowNumber].reserve(colNumber);

		for (int currColNumber = 0; currColNumber < colNumber; currColNumber++) {
			matrix[currRowNumber][currColNumber] = (((double) rand() / RAND_MAX) * (maxWeight - minWeight)) + minWeight;
		}
	}
}


Matrix::Matrix(std::vector<std::vector<double>> transposableMatrix)
{
	int colNumber = transposableMatrix.size();
	int rowNumber = transposableMatrix.front().size();

	matrix.reserve(rowNumber);

	for (int currRowNumber = 0; currRowNumber < rowNumber; currRowNumber++) {
		matrix[currRowNumber].reserve(colNumber);

		for (int currColNumber = 0; currColNumber < colNumber; currColNumber++) {
			matrix[currRowNumber][currColNumber] = transposableMatrix[currColNumber][currRowNumber];
		}
	}
}


Matrix::Matrix(std::vector<std::vector<double>> matrix): 
{
}


Matrix::~Matrix(void)
{
}


std::vector<std::vector<double>> Matrix::getMatrix() {
	return this -> matrix;
}


/*Matrix Matrix::operator *(Matrix multiplyMatrix) {
	int matrixRowNumber = matrix.size();
	int matrixColNumber = matrix.front().size();
	int multiplyMatrixRowNumber = multiplyMatrix.getMatrix().size();
	int multiplyMatrixColNumber = multiplyMatrix.getMatrix().front().size();

	std::vector<std::vector<double>> rezult(rezultMatrixRowNumber);

	for (int currRowNumber = 0; currRowNumber < rezultMatrixRowNumber; currRowNumber++) {
		rezult[currRowNumber].reserve(rezultMatrixColNumber);

		for (int currColNumber = 0; currColNumber < rezultMatrixColNumber; currColNumber++) {
			rezult[currRowNumber][currColNumber] += matrix[currRowNumber][] * multiplyMatrix[][];
		}
	}

	Matrix rezultMatrix(rezult);

	return rezultMatrix;
}


std::vector<double> Matrix::multiplyOneAndTwoDimensiaonalsMatrix(std::vector<double> oneDimensionalMatrix, std::vector<std::vector<double>> twoDimensionalMatrix) {
	std::vector<double> rezultOneDimensionalMatrix;
	rezultOneDimensionalMatrix.reserve(twoDimensionalMatrix.front().size());
	
	for (int currTwoDimensionalMatrixCol = 0; currTwoDimensionalMatrixCol < twoDimensionalMatrix.front().size(); currTwoDimensionalMatrixCol++) {
		std::vector<double>::iterator currTwoDimensionalMatrixColIter = twoDimensionalMatrix[currTwoDimensionalMatrixCol].begin();
		std::vector<double>::iterator oneDimensionalMatrixIter = oneDimensionalMatrix.begin();
		std::vector<double>::iterator rezultOneDimensionalMatrixIter = rezultOneDimensionalMatrix.begin();

		while (oneDimensionalMatrixIter != oneDimensionalMatrix.end()) {
			*rezultOneDimensionalMatrixIter = (*currTwoDimensionalMatrixColIter) * (*oneDimensionalMatrixIter);
		}
	}
}*/
