#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy infile outfile\n");
        return 1;
    }

    // remember filenames
    int cFactor = atoi(argv[1]);
    char* infile = argv[2];
    char* outfile = argv[3];
    
    if (cFactor < 1 || cFactor > 100)
    {
        printf("Factor out of expected range! 1 to 100, incluslve.");
        return 1;
    }

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    
    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // outfile parameters
    BITMAPFILEHEADER outFileBF;
    BITMAPINFOHEADER outFileBI;
    outFileBF = bf;
    outFileBI = bi;
    outFileBI.biWidth = bi.biWidth * cFactor; 
    outFileBI.biHeight = bi.biHeight * cFactor;

    // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int biPadding =  (4 - (outFileBI.biWidth * sizeof(RGBTRIPLE)) %4) % 4;
    
    // determine new size
    outFileBF.bfSize = 54 + outFileBI.biWidth * abs(outFileBI.biHeight) * 3 + abs(outFileBI.biHeight) *  biPadding;
	outFileBI.biSizeImage = ((((outFileBI.biWidth * outFileBI.biBitCount) + 31) & ~31) / 8) * abs(outFileBI.biHeight);    
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&outFileBF, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&outFileBI, sizeof(BITMAPINFOHEADER), 1, outptr);

	for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
	{
		// Write each line factor-times
		for(int j = 0; j < cFactor; j++)
		{
		    // iterate over pixels in scanline
		    for (int k = 0; k < bi.biWidth; k++)
		    {
			    // temporary storage
			    RGBTRIPLE triple;

			    // read RGB triple from infile
			    fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

			    // write RGB triple to outfile
			    for(int l = 0; l < cFactor; l++) 
			    {
			        fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
			    }
		    }

			// skip over padding in infile
			fseek(inptr, padding, SEEK_CUR);

			// then add it to outfile
			for (int k = 0; k < biPadding; k++)
					fputc(0x00, outptr);

			fseek(inptr, -(bi.biWidth * 3 + padding), SEEK_CUR);
        }
		fseek(inptr, bi.biWidth* 3 + padding, SEEK_CUR);
	}


    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
