#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "bmpstruct.h"

double grayscale(RGBTRIPLE pixel);
int interpolate(double p00, double p01, double p10, double p11, int dx, int dy);
char get_character(char map[], int pixel);

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
    fread(&fileheader, 14, 1, infile);
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
    int theight = 600;
    int twidth = floor(theight * aspratio);
    float ratioheight = height / theight;
    float ratiowidth = width / twidth;
    int (*text)[twidth] = calloc(theight, twidth * sizeof(int));
    for (int i = 0; i < theight; i++)
    {
        for (int j = 0; j < twidth; j++)
        {
            float src_x = i / ratioheight;
            float src_y = j / ratiowidth;
            int x0 = floor(src_x);
            int y0 = floor(src_y);
            int x1 = x0 + 1;
            int y1 = y0 + 1;
            int dx = src_x - x0;
            int dy = src_y - y0;
            double p00 = grayscale(image[x0][y0]);
            double p01 = grayscale(image[x0][y1]);
            double p10 = grayscale(image[x1][y0]);
            double p11 = grayscale(image[x1][y1]);
            text[i][j] = interpolate(p00, p01, p10, p11, dx, dy);
        }
    }
    free(image);
    fclose(infile);
    char map[14] = {'.', ',', ':', ';', '-', '=', '+', 'x', 'X', '8', '0', 'S','#','@'};
    char character;
    for (int i = 0; i < theight; i++)
    {
        for (int j = 0; j < twidth; j++)
        {
            character = get_character(map, text[i][j]);
            fprintf(outfile, "%c",character);
        }
        fprintf(outfile, "%c", '\n');
    }
    free(text);
    fclose(outfile);
    return 0;
}

double grayscale(RGBTRIPLE pixel)
{
    return (pixel.rgbtRed + pixel.rgbtGreen + pixel.rgbtBlue) / 3.0; 
}

int interpolate(double p00, double p01, double p10, double p11, int dx, int dy)
{
    return round(p00 * (1 - dx) * (1 - dy) + p10 * dx * (1 - dy) + p01 * (1 - dx) * dy + p11 * dx * dy);
}

char get_character(char map[], int pixel)
{
    return map[(pixel * 13 + 127) / 255];
}
