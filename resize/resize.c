/**
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./copy infile outfile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];
    int ratio= atoi(argv[1]);

    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
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
    int oldPadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int newPadding=(4 - (bi.biWidth *ratio* sizeof(RGBTRIPLE)) % 4) % 4;
    bf.bfSize=((bf.bfSize-0x36-(oldPadding*abs(bi.biHeight)))*ratio*ratio)+0x36+(newPadding*ratio*abs(bi.biHeight));
   
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    LONG nWidth=bi.biWidth;
    bi.biWidth=bi.biWidth*ratio;
    LONG nHeight = abs(bi.biHeight);
    bi.biHeight=bi.biHeight*ratio;
    bi.biSizeImage=((sizeof(RGBTRIPLE)*bi.biWidth)+newPadding)*abs(bi.biHeight);
    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines
    
    RGBTRIPLE scanLine[bi.biWidth];
    // iterate over infile's scanlines
    for (int i = 0; i < nHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < nWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;
            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            for(int k=0;k<ratio;k++)
            {
                scanLine[(j*ratio)+k]=triple;
            }
        }
        for(int m=0;m<ratio;m++)
        {
        for(int l=0;l<bi.biWidth;l++)
        {
            fwrite(&(scanLine[l]), sizeof(RGBTRIPLE),1, outptr);
        }
        for (int q = 0; q < newPadding; q++)
        {
            fputc(0x00, outptr);
        }
        }
        // skip over padding, if any
        fseek(inptr, oldPadding, SEEK_CUR);

        // then add it back (to demonstrate how)
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
