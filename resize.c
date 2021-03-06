// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize n infile outfile\n");
        return 1;
    }

    // remember filenames
    int scale = atoi(argv[1]);
    char *infile = argv[2];
    char *outfile = argv[3];

    if (scale < 0 && scale >= 100)
    {
        fprintf(stderr, "scale must be positive and less than 100.\n");
        return 1;
    }

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
    int old_biwidth = bi.biWidth;
    int old_biheight = bi.biHeight;

    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    bi.biWidth *= scale;
    bi.biHeight *= scale;

    int new_pitch = bi.biWidth * 3;
    if (new_pitch % 4 != 0)
    {
        new_pitch += 4 - (new_pitch % 4);
    }
    int new_padding  = new_pitch -  (bi.biWidth * 3);

    bf.bfSize = 54 + (bi.biWidth * sizeof(RGBTRIPLE) + new_padding) * abs(bi.biHeight);
    bi.biSizeImage = bf.bfSize - 54;



    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(old_biheight); i < biHeight; i++)
    {
        int counter = 0;

        while (counter < scale)
        {
        // iterate over pixels in scanline
            for (int j = 0; j < old_biwidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                int pixcount = 0;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                while (pixcount < scale)
                {
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                    pixcount++;
                }
            }

            for (int k = 0; k < new_padding; k++)
                fputc(0x00, outptr);

            if (counter < (scale - 1))
                fseek(inptr, -(old_biwidth * sizeof(RGBTRIPLE)), SEEK_CUR);

            counter++;
        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
