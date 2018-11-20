#pragma once

#include <vector>
#include <iterator>
#include <ctime>
#include "bitmap\bitmap_image.hpp"

class Matrix
{
private:
	std::vector<std::vector<double>> matrix;

public:
	Matrix(bitmap_image imagePart);
	Matrix(int firstLayerNeuronsNumber, int secondLayerNeuronsNumber);
	Matrix(std::vector<std::vector<double>> transposableMatrix);
	~Matrix(void);

	std::vector<std::vector<double>> getMatrix();

	Matrix operator *(Matrix multiplyMatrix);
};

