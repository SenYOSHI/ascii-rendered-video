#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bmpstruct.h"

double grayscale(RGBTRIPLE pixel);
int interpolate(double p00, double p01, double p10, double p11, int dx, int dy);

#define theight 600

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
        fread(image[i], sizeof(RGBTRIPLE), width, infile);
        fseek(infile, padding, SEEK_CUR);
    }
    float aspratio = width / height;
    int twidth = floor(theight * aspratio);
    float ratioheight = height / theight;
    float ratiowidth = width / twidth;
    char map[14] = {'.', ',', ':', ';', '-', '=', '+', 'x', 'X', '8', '0', 'S','#','@'};
    // moved writing into the file and writing the row into the same loop as calculations : 24 ms -> 18 ms
    for (int i = 0; i < theight; i++)
    {
        float src_x = i / ratioheight; // 18 ms -> 17 ms
        char row[twidth + 2];
        for (int j = 0; j < twidth; j++)
        {
            // replace bilinear with nearest neighbor interpolation : 17 ms -> 11 ms
            float src_y = j / ratiowidth;
            int x = (int)(src_x + 0.5f);
            int y = (int)(src_y + 0.5f);
            int pixel = grayscale(image[x][y]);
            row[j] = map[(pixel * 13 + 127) / 255];
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

double grayscale(RGBTRIPLE pixel)
{
    return (pixel.rgbtRed + pixel.rgbtGreen + pixel.rgbtBlue) / 3.0; 
}

int interpolate(double p00, double p01, double p10, double p11, int dx, int dy)
{
    // simplified bi-interpolation equation
    return round(p00 * (1 - dx) * (1 - dy) + p10 * dx * (1 - dy) + p01 * (1 - dx) * dy + p11 * dx * dy);
}