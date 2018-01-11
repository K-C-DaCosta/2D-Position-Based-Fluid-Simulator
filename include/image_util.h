#ifndef IMAGE_UTIL_H
#define IMAGE_UTIL_H

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;
typedef unsigned short ushort;
typedef unsigned long  ulong;

typedef struct RGB_{
  byte r, g, b;
} RGB;

typedef struct RGBpixmap{
    int nRows, nCols;
    RGB *pixel;
} RGBpixmap;

namespace IMGUTL {
    void readBMPFile(RGBpixmap *pm, const char *file); // example code taken from texturecube.c
}

#endif // IMAGE_UTIL_H
