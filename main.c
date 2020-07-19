#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <conio.h>
#include "bmp.h"

int main()
{
	char filename[64];
	unsigned threshold = 0;
	unsigned Q1 = 0, Q2 = 0;
	int tmp = 0;
	int bValue = 0;
	double G = 0.0;
	FILE* pFile;
	FILE* oFile;
	BMPFILE bmpFile; // структура bmp-файла	

	printf("Input filename: ");
	scanf("%s", filename);

	pFile = fopen(filename, "rb");
	getc(stdin);
	printf("Reading %s...\n", filename);
	
	readBitMapFileHeader(&bmpFile, pFile);	// read bit map file header
	readBitMapInfoHeader(&bmpFile, pFile);	// read bit map info header
	allocatePixels(&bmpFile);				// allocate memory for pixels array
	readPixels(&bmpFile, pFile);			// read pixels
	fclose(pFile);							// close file

	printf("Done. What to do next?\n");
	printf("0 - exit\n1 - Inverse\n2 - Binarization\n3 - Halftone\n4 - Gamma correction\n5 - Brightness\n6 - Histogramm\n7 - Histogramm_low\nChoose: ");
	scanf("%d", &tmp);

	switch (tmp)
	{
	case 0: 
		break;
	case 1:
		printf("Starting Inverse_func...\n");
		oFile = fopen("./output/output (inverse).bmp", "wb");
		writeBitMapFileHeader(&bmpFile, oFile); 
		writeBitMapInfoHeader(&bmpFile, oFile);
		inverse_func(&bmpFile, oFile);
		fclose(oFile);
		printf("Inverse_func done\n");
		break;
	case 2:
		printf("Starting Binarization_func...\n");
		printf("Set threshold [0, 255]: ");
		scanf("%u", &threshold);
		oFile = fopen("./output/output (binarization).bmp", "wb");
		writeBitMapFileHeader(&bmpFile, oFile);
		writeBitMapInfoHeader(&bmpFile, oFile);
		binarization_func(&bmpFile, (BYTE)threshold, oFile);
		fclose(oFile);
		printf("Binarization_func done\n");
		break;
	case 3:
		printf("Starting Halftone_func...\n");
		oFile = fopen("./output/output (halftone).bmp", "wb");
		writeBitMapFileHeader(&bmpFile, oFile);
		writeBitMapInfoHeader(&bmpFile, oFile);
		halftone_func(&bmpFile, oFile);
		fclose(oFile);
		printf("Halftone_func done\n");
		break;
	case 4:
		printf("Starting Gamma_func...\n");
		printf("Set function power [0.01, 0.99]: ");
		scanf("%lf", &G);
		oFile = fopen("./output/output (gamma).bmp", "wb");
		writeBitMapFileHeader(&bmpFile, oFile);
		writeBitMapInfoHeader(&bmpFile, oFile);
		gamma_correction_func(&bmpFile, G, oFile);
		fclose(oFile);
		printf("Gamma_func done...\n");
		break;
	case 5:
		printf("Starting Brightness_func...\n");
		printf("Set brightess [-255, 255]: ");
		scanf("%d", &bValue);
		oFile = fopen("./output/output (brightness).bmp", "wb");
		writeBitMapFileHeader(&bmpFile, oFile);
		writeBitMapInfoHeader(&bmpFile, oFile);
		brightness_func(&bmpFile, bValue, oFile);
		fclose(oFile);
		printf("Brightness_func done\n");
		break;
	case 6:
		printf("Starting Histo_stretch_func...\n");
		printf("Set Q1 [0,255]: ");
		scanf("%u", &Q1);
		printf("Set Q2 [0,255]: ");
		scanf("%u", &Q2);
		oFile = fopen("./output/output (histo).bmp", "wb");
		writeBitMapFileHeader(&bmpFile, oFile);
		writeBitMapInfoHeader(&bmpFile, oFile);
		histo_stretch_func(&bmpFile, (BYTE)Q1, (BYTE)Q2, oFile);
		fclose(oFile);
		printf("Histo_stretch_func done\n");
		break;
	case 7:
		printf("Starting Histo_stretch_low_func...\n");
		printf("Set Q1 [0,255]: ");
		scanf("%u", &Q1);
		printf("Set Q2 [0,255]: ");
		scanf("%u", &Q2);
		oFile = fopen("./output/output (histo_low).bmp", "wb");
		writeBitMapFileHeader(&bmpFile, oFile);
		writeBitMapInfoHeader(&bmpFile, oFile);
		histo_stretch_low_func(&bmpFile, (BYTE)Q1, (BYTE)Q2, oFile);
		fclose(oFile);
		printf("Histo_stretch_low_func done\n");
		break;
	default:
		printf("No such a command\n");
		break;
	}

	freePixels(&bmpFile);
	printf("Complete. Press any key: ");
	_getch();

	return 0;
}