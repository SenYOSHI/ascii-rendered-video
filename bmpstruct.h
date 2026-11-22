#include <stdint.h>

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef void *LPVOID;

#pragma pack(push, 1) // forces the compiler to store the structs without padding

typedef struct tagBITMAPFILEHEADER
{
    WORD  bfType;
    DWORD bfSize;
    WORD  bfReserved1;
    WORD  bfReserved2;
    DWORD bfOffset;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYpelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAP
{
    LONG bmType;
    LONG bmHeight;
    LONG bmWidth;
    LONG bmWidthBytes;
    WORD bmPlanes;
    DWORD bmBitsPixels;
    LPVOID bmBits;
} BITMAP;

typedef struct tagRGBTRIPLE
{
    BYTE rgbtRed; 
    BYTE rgbtGreen;
    BYTE rgbtBlue;
} RGBTRIPLE;