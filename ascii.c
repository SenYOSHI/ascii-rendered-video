#include <stdio.h>
#include <stdlib.h>
#include "bmpstruct.h"

float grayscale(RGBTRIPLE pixel);

// coefficient of height and width for each character cell
#define cellheight 20
#define cellwidth 10

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("command line argument required");
        return 1;
    }
    char *infilename = argv[1];
    char *outfilename = "out.txt";
    FILE *infile = fopen(infilename, "rb");
    FILE *outfile = fopen(outfilename, "w");
    if (infile == NULL)
    {
        printf("error opening your file");
        return 2;
    }
    if (outfile == NULL)
    {
        printf("not enough writing room");
        return 3;
    }
    BITMAPFILEHEADER fileheader;
    BITMAPINFOHEADER fileinfoheader;
    fread(&fileheader, sizeof(BITMAPFILEHEADER), 1, infile);
    fread(&fileinfoheader, sizeof(BITMAPINFOHEADER), 1, infile);
    if (fileheader.bfType != 0x4D42 || fileheader.bfOffset != 54 ||fileinfoheader.biSize != 40 
        || fileinfoheader.biCompression != 0 || fileinfoheader.biBitCount != 24)
    {
        fclose(infile);
        printf("unsupported file type");
        return 4;
    }
    int height = abs(fileinfoheader.biHeight);
    int width = fileinfoheader.biWidth;
    RGBTRIPLE (*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (image == NULL)
    {
        fclose(infile);
        printf("not enough memory to store image\n");
        return 5;
    }
    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;
    for (int i = 0; i < height; i++)
    {
        fread(image[height - i - 1], sizeof(RGBTRIPLE), width, infile);
        fseek(infile, padding, SEEK_CUR);
    }
    int twidth = width / cellwidth;
    int theight = height / cellheight;
    char map[14] = {'.', ',', ':', ';', '-', '=', '+', 'x', 'X', '8', '0', 'S','#','@'};
    // 1) moved writing into the file and writing the row into the same loop as calculations : 24 ms -> 18 ms
    for (int i = 0; i < theight; i++)
    {
        int x0 = i * height / theight;
        int x1 = (i + 1) * height / theight;
        char row[twidth + 2];
        // changed from bilinear to box averaging -> 18 ms -> 11 ms and produces a better ascii image
        for (int j = 0; j < twidth; j++)
        {
            int y0 = j * width / twidth;
            int y1 = (j + 1) * width / twidth;
            long sum = 0;
            int count = 0;
            for (int k = x0; k < x1; k++)
            {
                for (int l = y0; l < y1; l++)
                {
                    sum += grayscale(image[k][l]);
                    count++;
                }
            }
            sum = sum / count;
            row[j] = map[(sum * 13 + 127) / 255];
        }
        row[twidth] = '\n';
        row[twidth + 1] = '\0';
        fputs(row, outfile);
    }
    free(image);
    fclose(infile);
    fclose(outfile);
    return 0;
}

float grayscale(RGBTRIPLE pixel)
{
    return (pixel.rgbtRed + pixel.rgbtGreen + pixel.rgbtBlue) / 3.0; 
}