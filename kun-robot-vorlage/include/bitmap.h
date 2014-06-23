/*
 * bitmap.h
 *
 *  Created on: 29.04.2014
 *      Author: fengelha
 */

#ifndef BITMAP_H_
#define BITMAP_H_

#include <stdint.h>

typedef struct __attribute__ ((__packed__)) _bitmapheader {
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
	uint32_t biSize;
	int32_t  biWidth;
	int32_t  biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t  biXPelsPerMeter;
	int32_t  biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
} bitmapheader;

void generateBMPHeader(bitmapheader* b, int width, int height) {
	b->bfType = 0x4D42;
	b->bfSize = (uint32_t) (width*height*3) + sizeof(bitmapheader);
	b->bfReserved1 = 0;
	b->bfReserved2 = 0;
	b->bfOffBits = sizeof(bitmapheader);
	b->biSize = 40;
	b->biWidth = width;
	b->biHeight = height;
	b->biPlanes = 1;
	b->biBitCount = 24;
	b->biCompression = 0;
	b->biSizeImage = 0;
	b->biXPelsPerMeter = 0;
	b->biYPelsPerMeter = 0;
	b->biClrUsed = 0;
	b->biClrImportant = 0;
}

#endif /* BITMAP_H_ */
