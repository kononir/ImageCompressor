// ImageCompressor.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "ImageCompressor.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	string filePath;
	int rectHeight, rectWidth, overlap;

	NeuralNetwork neuralNetwork;

	cout << "Input file path: " << endl;
	getline(cin, filePath, '\n');

	wstring widestr = wstring(filePath.begin(), filePath.end());
	const wchar_t *filePath_t = widestr.c_str();

	CImage image;

	if (image.Load(filePath_t) == E_FAIL) {
      cout << "Error - Failed to open: " + filePath << endl;

	  system("pause");
      return 1;
    }

	//image.Save(_T("D:\\file.bmp"));

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

	neuralNetwork.trainingSample = sliceImage(image, rectWidth, rectHeight, overlap);
	neuralNetwork.imagerysNumber = image.GetWidth() * image.GetHeight() / rectWidth / rectHeight;
	neuralNetwork.firstLayerNeuronsNumber = rectWidth * rectHeight * COLORS_NUMBER;

	trainNeuralNetwork(neuralNetwork);

	double** decompressedImageRectangles = compressAndDecompressImageRectangles(neuralNetwork);

	CImage &decompressedImage = createDecompressedImage(decompressedImageRectangles, image.GetHeight(), image.GetWidth(), rectWidth, rectHeight, overlap, image.GetBPP());

	if (decompressedImage.IsNull() || !decompressedImage.IsDIBSection()) {
		cout << "Error - Image isn't drow" << endl;
	}

	if (decompressedImage.Save(_T("D:\\decompresedImage.bmp")) == E_FAIL) {
      cout << "Error - Failed to save decompresed file" << endl;

	  system("pause");
      return 1;
    }

	system("pause");
	return 0;
}


double** sliceImage(CImage image, int rectWidth, int rectHeight, int overlap) {
	int imageWidth = (int) image.GetWidth();
	int imageHeight = (int) image.GetHeight();

	int imagerysNumber = imageWidth * imageHeight / rectWidth / rectHeight;

	double** trainingSample = new double*[imagerysNumber];
	int currImageryIndex = 0;

	for (int currRectX = 0; currRectX < imageWidth; currRectX += rectWidth) {
		if (imageWidth - currRectX - 1 < rectWidth) {
			currRectX -= overlap;
		}

		for (int currRectY = 0; currRectY < imageHeight; currRectY += rectHeight, currImageryIndex++) {
			if (imageHeight - currRectY - 1 < rectHeight) {
				currRectY -= overlap;
			}

			int imageryComponentsNumber = rectWidth * rectHeight * COLORS_NUMBER;
			trainingSample[currImageryIndex] = new double[imageryComponentsNumber];

			int currImageryComponentIndex = 0;

			// Цикл для создания эталонного вектора и преобразования его компонент для дальнейшей обработки 
			for (int currPixelX = currRectX; currPixelX < currRectX + rectWidth; currPixelX++) {
				
				for (int currPixelY = currRectY; currPixelY < currRectY + rectHeight; currPixelY++) {
					COLORREF color = image.GetPixel(currPixelX, currPixelY);

					int pixelRedValue = GetRValue(color);
					int pixelGreenValue = GetGValue(color);
					int pixelBlueValue = GetBValue(color);

					double transformedPixelRedValue = (2 * pixelRedValue / (double)PIXEL_COLOR_MAX_VALUE) - 1;
					double transformedpixelGreenValue = (2 * pixelGreenValue / (double)PIXEL_COLOR_MAX_VALUE) - 1;
					double transformedpixelBlueValue = (2 * pixelBlueValue / (double)PIXEL_COLOR_MAX_VALUE) - 1;

					trainingSample[currImageryIndex][currImageryComponentIndex++] = transformedPixelRedValue;
					trainingSample[currImageryIndex][currImageryComponentIndex++] = transformedpixelGreenValue;
					trainingSample[currImageryIndex][currImageryComponentIndex++] = transformedpixelBlueValue;
				}
			}
		}
	}

	return trainingSample;
}


void trainNeuralNetwork(NeuralNetwork &neuralNetwork) {
	double** q = neuralNetwork.trainingSample;
	double** W = NULL;
	double** Ws = NULL;

	double* Yi;
	double* Xdi;

	int imagerysNumber = neuralNetwork.imagerysNumber;
	int N = neuralNetwork.firstLayerNeuronsNumber;
	int p = neuralNetwork.secondLayerNeuronsNumber;

	double a = neuralNetwork.firstLayerTrainingCoefficient;
	double as = neuralNetwork.secondLayerTrainingCoefficient;
	double e = neuralNetwork.maximumAllowableError;
	double eForAverage = 2 * e;

	long double RMSEforTrainingSample;

	srand((unsigned int) time(0));

	do {
		RMSEforTrainingSample = 0;

		for (int currImageryIndex = 0; currImageryIndex < imagerysNumber; currImageryIndex++) {
			double* Xi = neuralNetwork.trainingSample[currImageryIndex];

			boolean isFirstStep = (W == NULL && Ws == NULL);

			if (isFirstStep) {
				/*
				W = makeMatrixWithRandomValues(N, p);
				Ws = transposeMatrix(W);
				*/

				double minWeight = -1;
				double maxWeight = 1;

				W = new double*[N];
				Ws = new double*[p];

				for (int currRowNumber = 0; currRowNumber < N; currRowNumber++) {
					W[currRowNumber] = new double[p];
					
					for (int currColNumber = 0; currColNumber < p; currColNumber++) {
						if (currRowNumber == 0) {
							Ws[currColNumber] = new double[N];
						}

						W[currRowNumber][currColNumber] = Ws[currColNumber][currRowNumber] 
							= (((double) rand() / RAND_MAX) * (maxWeight - minWeight)) + minWeight;
					}
				}
			} else {
				/*
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
				*/

				int currXdiCompNumber = 0;

				for (int currRowNumber = 0; currRowNumber < N; currRowNumber++) {
					for (int currColNumber = 0; currColNumber < p; currColNumber++) {
						for (int currWCompNumber = 0; currWCompNumber < N; currWCompNumber++) {
							W[currRowNumber][currColNumber] -= a * Xi[currRowNumber] * Xdi[currWCompNumber] * Ws[currColNumber][currWCompNumber];
						}
					}
				}

				for (int currRowNumber = 0; currRowNumber < p; currRowNumber++) {
					for (int currColNumber = 0; currColNumber < N; currColNumber++) {
						Ws[currRowNumber][currColNumber] -= as * Yi[currRowNumber] * Xdi[currColNumber];
					}
				}
			}
			
			/*
			Yi = composeMatrixes(Xi, W);
			Matrix &Xis = composeMatrixes(Yi, Ws);
			Xdi = subtractMatrixes(Xis, Xi);
			*/

			Yi = new double[p];

			for (int currColNumber = 0; currColNumber < p; currColNumber++) {
				Yi[currColNumber] = 0;

				for (int currRowNumber = 0; currRowNumber < N; currRowNumber++) {
					Yi[currColNumber] += Xi[currRowNumber] * W[currRowNumber][currColNumber];
				}
			}

			Xdi = new double[N];

			for (int currColNumber = 0; currColNumber < N; currColNumber++) {
				Xdi[currColNumber] = 0;

				for (int currRowNumber = 0; currRowNumber < p; currRowNumber++) {
					Xdi[currColNumber] += Yi[currRowNumber] * Ws[currRowNumber][currColNumber];
				}

				Xdi[currColNumber] -= Xi[currColNumber];

				RMSEforTrainingSample += pow(Xdi[currColNumber], SQUARE);
			}

			//cout << RMSEforTrainingSample << endl;
		}

		//cout << RMSEforTrainingSample << endl;

	} while (RMSEforTrainingSample > eForAverage);

	neuralNetwork.currFirstLayerWeightMatrix = W;
	neuralNetwork.currSecondLayerWeightMatrix = Ws;
}


double** compressAndDecompressImageRectangles(NeuralNetwork neuralNetwork) {
	int imagerysNumber = neuralNetwork.imagerysNumber;
	int N = neuralNetwork.firstLayerNeuronsNumber;
	int p = neuralNetwork.secondLayerNeuronsNumber;

	double** W = neuralNetwork.currFirstLayerWeightMatrix;
	double** Ws = neuralNetwork.currSecondLayerWeightMatrix;

	double** decompressImageRectangles = new double*[imagerysNumber];
	
	for (int currImageryIndex = 0; currImageryIndex < imagerysNumber; currImageryIndex++) {
		double* Xi = neuralNetwork.trainingSample[currImageryIndex];

		double* Yi = new double[p];

		for (int currColNumber = 0; currColNumber < p; currColNumber++) {
			Yi[currColNumber] = 0;

			for (int currRowNumber = 0; currRowNumber < N; currRowNumber++) {
				Yi[currColNumber] += Xi[currRowNumber] * W[currRowNumber][currColNumber];
			}
		}

		decompressImageRectangles[currImageryIndex] = new double[N];

		for (int currColNumber = 0; currColNumber < N; currColNumber++) {
			decompressImageRectangles[currImageryIndex][currColNumber] = 0;

			for (int currRowNumber = 0; currRowNumber < p; currRowNumber++) {
				decompressImageRectangles[currImageryIndex][currColNumber] += Yi[currRowNumber] * Ws[currRowNumber][currColNumber];
			}
		}
	}

	return decompressImageRectangles;
}


CImage createDecompressedImage(double** decompressedImageRectangles, int imageWidth, int imageHeight, int rectWidth, int rectHeight, int overlap, int nBPP) {
	CImage decompressedImage;

	decompressedImage.Create(imageWidth, imageHeight, nBPP);

	int imagerysNumber = imageWidth * imageHeight / rectWidth / rectHeight;

	int currImageryIndex = 0;
	
	for (int currRectX = 0; currRectX < imageWidth; currRectX += rectWidth) {
		if (imageWidth - currRectX - 1 < rectWidth) {
			currRectX -= overlap;
		}

		for (int currRectY = 0; currRectY < imageHeight; currRectY += rectHeight, currImageryIndex++) {
			if (imageHeight - currRectY - 1 < rectHeight) {
				currRectY -= overlap;
			}

			int currImageryComponentIndex = 0;

			for (int currPixelX = currRectX; currPixelX < currRectX + rectWidth; currPixelX++) {

				for (int currPixelY = currRectY; currPixelY < currRectY + rectHeight; currPixelY++) {				
					double transformedPixelRedValue = decompressedImageRectangles[currImageryIndex][currImageryComponentIndex++];
					double transformedPixelGreenValue = decompressedImageRectangles[currImageryIndex][currImageryComponentIndex++];
					double transformedPixelBlueValue = decompressedImageRectangles[currImageryIndex][currImageryComponentIndex++];

					int red = (int)(PIXEL_COLOR_MAX_VALUE * (transformedPixelRedValue + 1) / 2);
					int green = (int)(PIXEL_COLOR_MAX_VALUE * (transformedPixelGreenValue + 1) / 2);
					int blue = (int)(PIXEL_COLOR_MAX_VALUE * (transformedPixelBlueValue + 1) / 2);

					if (red < PIXEL_COLOR_MIN_VALUE) {
						red = PIXEL_COLOR_MIN_VALUE;
					}

					if (red > PIXEL_COLOR_MAX_VALUE) {
						red = PIXEL_COLOR_MAX_VALUE;
					}

					if (green < PIXEL_COLOR_MIN_VALUE) {
						green = PIXEL_COLOR_MIN_VALUE;
					}

					if (green > PIXEL_COLOR_MAX_VALUE) {
						green = PIXEL_COLOR_MAX_VALUE;
					}

					if (blue < PIXEL_COLOR_MIN_VALUE) {
						blue = PIXEL_COLOR_MIN_VALUE;
					}

					if (blue > PIXEL_COLOR_MAX_VALUE) {
						blue = PIXEL_COLOR_MAX_VALUE;
					}

					COLORREF color = RGB(red, green, blue);

					decompressedImage.SetPixel(currPixelX, currPixelY, color);

					//cout << (int) GetRValue(decompressedImage.GetPixel(currPixelX, currPixelY)) << endl;

					//cout << "X: " << currPixelX << "\tY: " << currPixelY << endl;
				}
			}
		}
	}

	return decompressedImage;
}
