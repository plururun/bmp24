#ifndef BMP_H
#define BMP_H

// some typedef's
typedef	uint8_t	 BYTE;
typedef	uint16_t WORD;
typedef uint32_t DWORD;
typedef  int32_t LONG;

// BITMAP FILE HEADER
typedef struct BITMAPFILEHEADER_tag
{
	WORD	bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
} BITMAPFILEHEADER;

// BITMAP INFO HEADER
typedef struct BITMAPINFOHEADER_tag
{
	DWORD	biSize;
	LONG	biWidth;
	LONG	biHeight;
	WORD	biPlanes;
	WORD	biBitCount;
	DWORD   biCompression;
	DWORD   biSizeImage;
	LONG	biXPelsPerMeter;
	LONG	biYPelsPerMeter;
	DWORD   biClrUsed;
	DWORD   biClrImportant;
} BITMAPINFOHEADER;

// struct for handle pixels
typedef struct RGBTRIPLE_tag
{
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
} RGBTRIPLE;

// struct for whole bmp file
typedef struct BMPFILE_tag
{
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER bmiHeader;
	RGBTRIPLE**		 rgb;
} BMPFILE;


// read 2 unsigned bytes
static WORD read_u16(FILE* fp)
{
	BYTE b0, b1;

	b0 = getc(fp);
	b1 = getc(fp);

	return ((b1 << 8) | b0); // offest b1 8 bits left
}

// read 4 unsigned bytes
static DWORD read_u32(FILE* fp)
{
	BYTE b0, b1, b2, b3;

	b0 = getc(fp);
	b1 = getc(fp);
	b2 = getc(fp);
	b3 = getc(fp);

	return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0); // offset b3 24 bits, b2 16 bits, b1 8 bits left
}

// read 4 signed bytes
static LONG read_s32(FILE* fp)
{
	BYTE b0, b1, b2, b3;

	b0 = getc(fp);
	b1 = getc(fp);
	b2 = getc(fp);
	b3 = getc(fp);

	return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0); // offset b3 24 bits, b2 16 bits, b1 8 bits left and cast to int type
}


// write 2 unsigned bytes
static void write_u16(WORD input, FILE *fp)
{
	putc(input, fp);
	putc(input >> 8, fp);
}

// write 4 unsigned bytes
static void write_u32(DWORD input, FILE *fp)
{
	putc(input, fp);
	putc(input >> 8, fp);
	putc(input >> 16, fp);
	putc(input >> 24, fp);
}

// write 4 signed bytes
static void write_s32(LONG input, FILE *fp)
{
	putc(input, fp);
	putc(input >> 8, fp);
	putc(input >> 16, fp);
	putc(input >> 24, fp);
}

// write pixels
static void write_pixels(BMPFILE* bmp, FILE* fp)
{
	int i = 0, j = 0;
	float progress = 0.f;
	float pixels = (float)bmp->bmiHeader.biWidth * bmp->bmiHeader.biHeight; // calculate pixel's offset
	int padding = bmp->bmiHeader.biWidth % 4;
	int tmp = padding;

	for (i = 0; i < bmp->bmiHeader.biHeight; i++)
	{
		padding = tmp;
		for (j = 0; j < bmp->bmiHeader.biWidth; j++)
		{
			putc(bmp->rgb[i][j].rgbBlue, fp);	// write Blue byte
			putc(bmp->rgb[i][j].rgbGreen, fp);	// write Green byte
			putc(bmp->rgb[i][j].rgbRed, fp);	// write Red byte
			progress++;
		}
		while (padding > 0)
		{
			putc(255, fp); // write 'extra' byte for align the boundary
			padding--;
		}
		printf("%.2f%%\n", progress / pixels * 100);
	}
}

// read bitmap file header
static void readBitMapFileHeader(BMPFILE* bmp, FILE* fp)
{
	bmp->header.bfType = read_u16(fp);
	bmp->header.bfSize = read_u32(fp);
	bmp->header.bfReserved1 = read_u16(fp);
	bmp->header.bfReserved2 = read_u16(fp);
	bmp->header.bfOffBits = read_u32(fp);
}

// read bitmap info eader
static void readBitMapInfoHeader(BMPFILE* bmp, FILE* fp)
{
	bmp->bmiHeader.biSize = read_u32(fp);
	bmp->bmiHeader.biWidth = read_s32(fp);
	bmp->bmiHeader.biHeight = read_s32(fp);
	bmp->bmiHeader.biPlanes = read_u16(fp);
	bmp->bmiHeader.biBitCount = read_u16(fp);
	bmp->bmiHeader.biCompression = read_u32(fp);
	bmp->bmiHeader.biSizeImage = read_u32(fp);
	bmp->bmiHeader.biXPelsPerMeter = read_s32(fp);
	bmp->bmiHeader.biYPelsPerMeter = read_s32(fp);
	bmp->bmiHeader.biClrUsed = read_u32(fp);
	bmp->bmiHeader.biClrImportant = read_u32(fp);
}

static void readPixels(BMPFILE* bmp, FILE* fp)
{
	int padding = bmp->bmiHeader.biWidth % 4; // calculate pixel's offset
	int tmp = padding;
	int i, j;

	// read pixels
	for (i = 0; i < bmp->bmiHeader.biHeight; i++)
	{
		padding = tmp;
		for (j = 0; j < bmp->bmiHeader.biWidth; j++)
		{
			bmp->rgb[i][j].rgbBlue = getc(fp);
			bmp->rgb[i][j].rgbGreen = getc(fp);
			bmp->rgb[i][j].rgbRed = getc(fp);
		}
		while (padding > 0) // read 'extra' boundary pixels
		{
			getc(fp);
			padding--;
		}
	}
}

static void allocatePixels(BMPFILE* bmp)
{
	int i;
	bmp->rgb = (RGBTRIPLE**)malloc(bmp->bmiHeader.biHeight * sizeof(RGBTRIPLE*));
	for (i = 0; i < bmp->bmiHeader.biHeight; i++)
	{
		bmp->rgb[i] = (RGBTRIPLE*)malloc(bmp->bmiHeader.biWidth * sizeof(RGBTRIPLE));
	}
}

static void freePixels(BMPFILE* bmp)
{
	int i;
	for (i = 0; i < bmp->bmiHeader.biHeight; i++)
	{
		free(bmp->rgb[i]);
	}
	free(bmp->rgb);
}

static void writeBitMapFileHeader(BMPFILE* bmp, FILE* fp)
{
	write_u16(bmp->header.bfType, fp);
	write_u32(bmp->header.bfSize, fp);
	write_u16(bmp->header.bfReserved1, fp);
	write_u16(bmp->header.bfReserved2, fp);
	write_u32(bmp->header.bfOffBits, fp);
}

static void writeBitMapInfoHeader(BMPFILE* bmp, FILE* fp)
{
	write_u32(bmp->bmiHeader.biSize, fp);
	write_s32(bmp->bmiHeader.biWidth, fp);
	write_s32(bmp->bmiHeader.biHeight, fp);
	write_u16(bmp->bmiHeader.biPlanes, fp);
	write_u16(bmp->bmiHeader.biBitCount, fp);
	write_u32(bmp->bmiHeader.biCompression, fp);
	write_u32(bmp->bmiHeader.biSizeImage, fp);
	write_s32(bmp->bmiHeader.biXPelsPerMeter, fp);
	write_s32(bmp->bmiHeader.biYPelsPerMeter, fp);
	write_u32(bmp->bmiHeader.biClrUsed, fp);
	write_u32(bmp->bmiHeader.biClrImportant, fp);
}

static void print_pixels(BMPFILE* bmp)
{
	int i, j;
	for (i = 0; i < bmp->bmiHeader.biHeight; i++)
	{
		for (j = 0; j < bmp->bmiHeader.biWidth; j++)
		{
			printf("%d %d %d\n", bmp->rgb[i][j].rgbBlue, bmp->rgb[i][j].rgbGreen, bmp->rgb[i][j].rgbRed);
		}
		printf("\n");
	}
}

static void inverse_func(BMPFILE* bmp, FILE* fp)
{
	int i, j;
	int padding = bmp->bmiHeader.biWidth % 4; // calculate pixel's offset
	int tmp = padding;
	
	for (i = 0; i < bmp->bmiHeader.biHeight; i++)
	{
		padding = tmp;
		for (j = 0; j < bmp->bmiHeader.biWidth; j++)
		{
			putc(~bmp->rgb[i][j].rgbBlue, fp);
			putc(~bmp->rgb[i][j].rgbGreen, fp);
			putc(~bmp->rgb[i][j].rgbRed, fp);
		}
		while (padding > 0)
		{
			putc(0, fp);
			padding--;
		}
	}
}

static void binarization_func(BMPFILE* bmp, BYTE threshold, FILE* fp)
{
	int i, j;
	int padding = bmp->bmiHeader.biWidth % 4; // calculate pixel's offset
	int tmp = padding;
	int gs; // gs stands for grayscale

	for (i = 0; i < bmp->bmiHeader.biHeight; i++)
	{
		padding = tmp;
		for (j = 0; j < bmp->bmiHeader.biWidth; j++)
		{
			gs = (int)(0.11 * bmp->rgb[i][j].rgbBlue + 0.59 * bmp->rgb[i][j].rgbGreen + 0.3 * bmp->rgb[i][j].rgbRed);

			if (gs < threshold)
				gs = 0;
			else
				gs = 255;

			putc(gs, fp);
			putc(gs, fp);
			putc(gs, fp);
		}
		while (padding > 0)
		{
			putc(255, fp);
			padding--;
		}
	}
}


static void halftone_func(BMPFILE* bmp, FILE* fp)
{
	int i, j;
	int padding = bmp->bmiHeader.biWidth % 4; // calculate pixel's offset
	int tmp = padding;
	int gs;

	for (i = 0; i < bmp->bmiHeader.biHeight; i++)
	{
		padding = tmp;
		for (j = 0; j < bmp->bmiHeader.biWidth; j++)
		{
			gs = (int)(0.11 * bmp->rgb[i][j].rgbBlue + 0.59 * bmp->rgb[i][j].rgbGreen + 0.3 * bmp->rgb[i][j].rgbRed);
			putc(gs, fp);
			putc(gs, fp);
			putc(gs, fp);
		}
		while (padding > 0)
		{
			putc(0, fp);
			padding--;
		}
	}
}


static void gamma_correction_func(BMPFILE* bmp, double G, FILE* fp)
{
	int i, j;
	int padding = bmp->bmiHeader.biWidth % 4; // calculate pixel's offset
	int tmp = padding;
	BYTE R;

	for (i = 0; i < bmp->bmiHeader.biHeight; i++)
	{
		padding = tmp;
		for (j = 0; j < bmp->bmiHeader.biWidth; j++)
		{
			R = (BYTE)(255 * pow(bmp->rgb[i][j].rgbBlue / 255.0, G));
			putc(R, fp);
			R = (BYTE)(255 * pow(bmp->rgb[i][j].rgbGreen / 255.0, G));
			putc(R, fp);
			R = (BYTE)(255 * pow(bmp->rgb[i][j].rgbRed / 255.0, G));
			putc(R, fp);
		}
		while (padding > 0)
		{
			putc(0, fp);
			padding--;
		}
	}
}


static void brightness_func(BMPFILE* bmp, int bValue, FILE* fp)
{
	int i, j;
	int padding = bmp->bmiHeader.biWidth % 4; // calculate pixel's offset
	int tmp = padding;

	for (i = 0; i < bmp->bmiHeader.biHeight; i++)
	{
		padding = tmp;
		for (j = 0; j < bmp->bmiHeader.biWidth; j++)
		{
			if (bmp->rgb[i][j].rgbBlue + bValue < 0) putc(0, fp);
			else if (bmp->rgb[i][j].rgbBlue + bValue > 255) putc(255, fp);
			else putc(bmp->rgb[i][j].rgbBlue + bValue, fp);

			if (bmp->rgb[i][j].rgbGreen + bValue < 0) putc(0, fp);
			else if (bmp->rgb[i][j].rgbGreen + bValue > 255) putc(255, fp);
			else putc(bmp->rgb[i][j].rgbGreen + bValue, fp);

			if (bmp->rgb[i][j].rgbRed + bValue < 0) putc(0, fp);
			else if (bmp->rgb[i][j].rgbRed + bValue > 255) putc(255, fp);
			else putc(bmp->rgb[i][j].rgbRed + bValue, fp);
		}
		while (padding > 0)
		{
			putc(0, fp);
			padding--;
		}
	}
}

static void histo_stretch_func(BMPFILE* bmp, BYTE Q1, BYTE Q2, FILE* fp)
{
	BYTE R;
	int i, j;
	int padding = bmp->bmiHeader.biWidth % 4; // calculate pixel's offset
	int tmp = padding;

	for (i = 0; i < bmp->bmiHeader.biHeight; i++)
	{
		padding = tmp;
		for (j = 0; j < bmp->bmiHeader.biWidth; j++)
		{
			R = (BYTE)(Q1 + bmp->rgb[i][j].rgbBlue * ((Q2 - Q1) / 255.f));
			putc(R, fp);
			R = (BYTE)(Q1 + bmp->rgb[i][j].rgbGreen * ((Q2 - Q1) / 255.f));
			putc(R, fp);
			R = (BYTE)(Q1 + bmp->rgb[i][j].rgbRed * ((Q2 - Q1) / 255.f));
			putc(R, fp);
		}
		while (padding > 0)
		{
			putc(0, fp);
			padding--;
		}
	}
}

static void histo_stretch_low_func(BMPFILE* bmp, BYTE Q1, BYTE Q2, FILE* fp)
{
	BYTE R;
	int i, j;
	int padding = bmp->bmiHeader.biWidth % 4; // calculate pixel's offset
	int tmp = padding;

	for (i = 0; i < bmp->bmiHeader.biHeight; i++)
	{
		padding = tmp;
		for (j = 0; j < bmp->bmiHeader.biWidth; j++)
		{
			R = (BYTE)((bmp->rgb[i][j].rgbBlue - Q1) * (255.f / (Q2 - Q1)));
			putc(R, fp);
			R = (BYTE)((bmp->rgb[i][j].rgbGreen - Q1) * (255.f / (Q2 - Q1)));
			putc(R, fp);
			R = (BYTE)((bmp->rgb[i][j].rgbRed - Q1) * (255.f / (Q2 - Q1)));
			putc(R, fp);
		}
		while (padding > 0)
		{
			putc(0, fp);
			padding--;
		}
	}
}

#endif