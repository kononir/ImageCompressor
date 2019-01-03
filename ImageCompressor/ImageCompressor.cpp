// ImageCompressor.cpp: определяет точку входа для консольного приложения.
//

#include "ImageCompressor.h"

using namespace std;

/*
* author: Novitskiy Vladislav
* group: 621701
* description: Главная функция; в ней происходит ввод входных параметров сети и вызываются остальные функции
*/
int _tmain(int argc, _TCHAR* argv[])
{
	string filePath;
	Image* image;

	int imageWidth, imageHeight, rectHeight, rectWidth, overlap;

	NeuralNetwork neuralNetwork;

	try {
		cout << "Input file path: " << endl;
		getline(cin, filePath, '\n');

		image = new Image(filePath.c_str());

		if (image == NULL) {
			throw "Error - Failed to open: " + filePath;
		}

		imageWidth = image->width();
		imageHeight = image->height();
	
		cout << "Input rectangle width: ";
		cin >> rectWidth;

		if (rectWidth <= 0 || rectWidth > imageWidth) {
			throw "Error - Invalid parameter: rectangle width";
		}

		cout << "Input reactangle height: ";
		cin >> rectHeight;

		if (rectHeight <= 0 || rectHeight > imageHeight) {
			throw "Error - Invalid parameter: rectangle height";
		}

		cout << "Input overlap: ";
		cin >> overlap;

		if (overlap < 0 || overlap >= rectWidth || overlap >= rectHeight
			|| (imageWidth - overlap) % (rectWidth - overlap) != 0
			|| (imageHeight - overlap) % (rectHeight - overlap) != 0) {
			throw "Error - Invalid parameter: overlap";
		}

		cout << "Input neurons number at second layer (p <= 2 * N): ";
		cin >> neuralNetwork.secondLayerNeuronsNumber;

		if (neuralNetwork.secondLayerNeuronsNumber <= 0) {
			throw "Error - Invalid parameter: neurons number at second layer";
		}

		cout << "Input coefficient of training at first layer (0 < a <= 0.01): ";
		cin >> neuralNetwork.firstLayerTrainingCoefficient;

		if (neuralNetwork.firstLayerTrainingCoefficient <= 0 || neuralNetwork.firstLayerTrainingCoefficient > 1) {
			throw "Error - Invalid parameter: coefficient of training at first layer";
		}

		cout << "Input coefficient of training at second layer (0 < a' <= 0.01): ";
		cin >> neuralNetwork.secondLayerTrainingCoefficient;

		if (neuralNetwork.secondLayerTrainingCoefficient <= 0 || neuralNetwork.secondLayerTrainingCoefficient > 1) {
			throw "Error - Invalid parameter: coefficient of training at second layer";
		}

		cout << "Input maximum allowable error (0 < e <= 0.1 * p, a <= e): ";
		cin >> neuralNetwork.maximumAllowableError;

		if (neuralNetwork.secondLayerTrainingCoefficient <= 0) {
			throw "Error - Invalid parameter: maximum allowable error";
		}
	}
	catch (const char* mesage) {
		cerr << mesage << endl;

		system("pause");
		return 1;
	}

	neuralNetwork.trainingSample = sliceImage(*image, rectWidth, rectHeight, overlap);

	neuralNetwork.imagerysNumber = (imageWidth - overlap) / (rectWidth - overlap) * (imageHeight - overlap) / (rectHeight - overlap);

	neuralNetwork.firstLayerNeuronsNumber = rectWidth * rectHeight * COLORS_NUMBER;

	trainNeuralNetwork(neuralNetwork);

	float** decompressedImageRectangles = compressAndDecompressImageRectangles(neuralNetwork);

	saveDecompressedImage(decompressedImageRectangles, imageWidth, imageHeight, rectWidth, rectHeight, overlap);

	cout << "Reached error: " << neuralNetwork.reachedError << endl;
	cout << "Number of training steps: " << neuralNetwork.numberOfTrainingSteps << endl;

	system("pause");
	return 0;
}


/*
* author: Novitskiy Vladislav
* group: 621701
* description: Функция разбиения изображения на прямоугольники
*/
float** sliceImage(Image image, int rectWidth, int rectHeight, int overlap) {
	int imageWidth = image.width();
	int imageHeight = image.height();

	int imagerysNumber = (imageWidth - overlap) / (rectWidth - overlap) * (imageHeight - overlap) / (rectHeight - overlap);
	int imageryComponentsNumber = rectWidth * rectHeight * COLORS_NUMBER;

	float** trainingSample = new float*[imagerysNumber];

	int currImageryIndex = 0;

	for (int currRectX = 0; currRectX < imageWidth; currRectX += rectWidth) {
		if (currRectX != 0) {
			currRectX -= overlap;
		}

		for (int currRectY = 0; currRectY < imageHeight; currRectY += rectHeight) {
			if (currRectY != 0) {
				currRectY -= overlap;
			}
			
			trainingSample[currImageryIndex] = new float[imageryComponentsNumber];

			int currImageryComponentIndex = 0;

			for (int currPixelX = currRectX; currPixelX < currRectX + rectWidth; currPixelX++) {
				
				for (int currPixelY = currRectY; currPixelY < currRectY + rectHeight; currPixelY++) {

					for (int currColorIndex = 0; currColorIndex < COLORS_NUMBER; currColorIndex++) {
						int pixelcolorValue = image(currPixelX, currPixelY, Z_VALUE, currColorIndex);

						float transformedPixelColorValue = (2 * pixelcolorValue / (float)PIXEL_COLOR_MAX_VALUE) - 1;

						trainingSample[currImageryIndex][currImageryComponentIndex++] = transformedPixelColorValue;
					}
				}
			}

			currImageryIndex++;
		}
	}

	return trainingSample;
}


/*
* author: Novitskiy Vladislav
* group: 621701
* description: Функция обучения линейной рециркуляционной нейронной сети
*/
void trainNeuralNetwork(NeuralNetwork &neuralNetwork) {
	int L = neuralNetwork.imagerysNumber;
	int N = neuralNetwork.firstLayerNeuronsNumber;
	int p = neuralNetwork.secondLayerNeuronsNumber;
	int numOfSteps = 0;

	float a = neuralNetwork.firstLayerTrainingCoefficient;
	float as = neuralNetwork.secondLayerTrainingCoefficient;
	float eForAverage = 2 * neuralNetwork.maximumAllowableError;
	float minWeight = -0.1;
	float maxWeight = 0.1;

	long double currError;
	
	float* Xi;
	float* XdiWs = new float[p];
	float* Yi = new float[p];
	float* Xdi = new float[N];

	float** X = neuralNetwork.trainingSample;

	float** W = new float*[N];
	for (int currRowNumber = 0; currRowNumber < N; currRowNumber++) {
		W[currRowNumber] = new float[p];
	}

	float** Ws = new float*[p];
	for (int currRowNumber = 0; currRowNumber < p; currRowNumber++) {
		Ws[currRowNumber] = new float[N];
	}

	srand((unsigned int)time(0));

	for (int currRowNumber = 0; currRowNumber < N; currRowNumber++) {
		for (int currColNumber = 0; currColNumber < p; currColNumber++) {
			W[currRowNumber][currColNumber] = Ws[currColNumber][currRowNumber]
				= (((float)rand() / RAND_MAX) * (maxWeight - minWeight)) + minWeight;
		}
	}

	do {
		for (int currImageryIndex = 0; currImageryIndex < L; currImageryIndex++) {
			Xi = X[currImageryIndex];

			for (int currColNumber = 0; currColNumber < p; currColNumber++) {
				Yi[currColNumber] = 0;

				for (int currRowNumber = 0; currRowNumber < N; currRowNumber++) {
					Yi[currColNumber] += Xi[currRowNumber] * W[currRowNumber][currColNumber];
				}
			}

			for (int currColNumber = 0; currColNumber < N; currColNumber++) {
				Xdi[currColNumber] = 0;

				for (int currRowNumber = 0; currRowNumber < p; currRowNumber++) {
					Xdi[currColNumber] += Yi[currRowNumber] * Ws[currRowNumber][currColNumber];
				}

				Xdi[currColNumber] -= Xi[currColNumber];
			}

			for (int currRowNumber = 0; currRowNumber < p; currRowNumber++) {
				for (int currColNumber = 0; currColNumber < N; currColNumber++) {
					Ws[currRowNumber][currColNumber] -= as * Yi[currRowNumber] * Xdi[currColNumber];
				}
			}

			for (int currRowNumber = 0; currRowNumber < p; currRowNumber++) {
				XdiWs[currRowNumber] = 0;

				for (int currColNumber = 0; currColNumber < N; currColNumber++) {
					XdiWs[currRowNumber] += Xdi[currColNumber] * Ws[currRowNumber][currColNumber];
				}
			}

			for (int currRowNumber = 0; currRowNumber < N; currRowNumber++) {
				for (int currColNumber = 0; currColNumber < p; currColNumber++) {
					W[currRowNumber][currColNumber] -= a * Xi[currRowNumber] * XdiWs[currColNumber];
				}
			}
		}

		currError = 0;

		for (int currImageryIndex = 0; currImageryIndex < L; currImageryIndex++) {
			Xi = X[currImageryIndex];

			for (int currColNumber = 0; currColNumber < p; currColNumber++) {
				Yi[currColNumber] = 0;

				for (int currRowNumber = 0; currRowNumber < N; currRowNumber++) {
					Yi[currColNumber] += Xi[currRowNumber] * W[currRowNumber][currColNumber];
				}
			}

			for (int currColNumber = 0; currColNumber < N; currColNumber++) {
				Xdi[currColNumber] = 0;

				for (int currRowNumber = 0; currRowNumber < p; currRowNumber++) {
					Xdi[currColNumber] += Yi[currRowNumber] * Ws[currRowNumber][currColNumber];
				}

				Xdi[currColNumber] -= Xi[currColNumber];
			}

			for (int currImageryComponentIndex = 0; currImageryComponentIndex < N; currImageryComponentIndex++) {
				currError += Xdi[currImageryComponentIndex] * Xdi[currImageryComponentIndex];
			}
		}

		cout /*<< "            " << "\r"*/ << currError << "\r";

		numOfSteps++;

	} while (currError > eForAverage);

	neuralNetwork.numberOfTrainingSteps = numOfSteps;
	neuralNetwork.currFirstLayerWeightMatrix = W;
	neuralNetwork.currSecondLayerWeightMatrix = Ws;
	neuralNetwork.reachedError = currError / 2;

	delete[] XdiWs;
	delete[] Yi;
	delete[] Xdi;
}


/*
* author: Novitskiy Vladislav
* group: 621701
* description: Функция сжатия и восстановления изображения
*/
float** compressAndDecompressImageRectangles(NeuralNetwork &neuralNetwork) {
	int imagerysNumber = neuralNetwork.imagerysNumber;
	int N = neuralNetwork.firstLayerNeuronsNumber;
	int p = neuralNetwork.secondLayerNeuronsNumber;

	float** W = neuralNetwork.currFirstLayerWeightMatrix;
	float** Ws = neuralNetwork.currSecondLayerWeightMatrix;

	float** decompressImageRectangles = new float*[imagerysNumber];
	
	for (int currImageryIndex = 0; currImageryIndex < imagerysNumber; currImageryIndex++) {
		float* Xi = neuralNetwork.trainingSample[currImageryIndex];

		float* Yi = new float[p];

		for (int currColNumber = 0; currColNumber < p; currColNumber++) {
			Yi[currColNumber] = 0;

			for (int currRowNumber = 0; currRowNumber < N; currRowNumber++) {
				Yi[currColNumber] += Xi[currRowNumber] * W[currRowNumber][currColNumber];
			}
		}

		decompressImageRectangles[currImageryIndex] = new float[N];

		for (int currColNumber = 0; currColNumber < N; currColNumber++) {
			decompressImageRectangles[currImageryIndex][currColNumber] = 0;

			for (int currRowNumber = 0; currRowNumber < p; currRowNumber++) {
				decompressImageRectangles[currImageryIndex][currColNumber] += Yi[currRowNumber] * Ws[currRowNumber][currColNumber];
			}
		}
	}

	return decompressImageRectangles;
}


/*
* author: Novitskiy Vladislav
* group: 621701
* description: Функция создания и сохранения изображения
*/
void saveDecompressedImage(float** decompressedImageRectangles, int imageWidth, int imageHeight, int rectWidth, int rectHeight, int overlap) {
	Image decompressedImage(imageWidth, imageHeight, ONE_DIMENSIONAL, COLORS_NUMBER);
	decompressedImage.fill(0);

	int imagerysNumber = (imageWidth - overlap) / (rectWidth - overlap) * (imageHeight - overlap) / (rectHeight - overlap);

	int currImageryIndex = 0;
	
	for (int currRectX = 0; currRectX < imageWidth; currRectX += rectWidth) {
		if (currRectX != 0) {
			currRectX -= overlap;
		}

		for (int currRectY = 0; currRectY < imageHeight; currRectY += rectHeight) {
			if (currRectY != 0) {
				currRectY -= overlap;
			}

			int currImageryComponentIndex = 0;

			for (int currPixelX = currRectX; currPixelX < currRectX + rectWidth; currPixelX++) {

				for (int currPixelY = currRectY; currPixelY < currRectY + rectHeight; currPixelY++) {				

					for (int currColorIndex = 0; currColorIndex < COLORS_NUMBER; currColorIndex++) {
						float transformedPixelColorValue = decompressedImageRectangles[currImageryIndex][currImageryComponentIndex++];

						int color = (int)(PIXEL_COLOR_MAX_VALUE * (transformedPixelColorValue + 1) / 2);

						if (color < PIXEL_COLOR_MIN_VALUE) {
							color = PIXEL_COLOR_MIN_VALUE;
						}

						if (color > PIXEL_COLOR_MAX_VALUE) {
							color = PIXEL_COLOR_MAX_VALUE;
						}

						decompressedImage(currPixelX, currPixelY, Z_VALUE, currColorIndex) = color;
					}
				}
			}

			currImageryIndex++;
		}
	}

	decompressedImage.save_bmp("decompresedImage.bmp");
}
