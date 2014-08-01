/*
 * saveDate2BMP.cpp
 *
 *  Created on: 2014-7-25
 *      Author: WJ
 */


#include <stdio.h>
#include <stdlib.h>
#include "saveDate2BMP.h"
#include "imageinfo.h"
#include "bmp.h"

BITMAPFILEHEADER bitmapfileheader;
BITMAPINFOHEADER bitmapinfoheader;

int test()
{
	return 0;
}

int saveDate2BMP(ImageInfo* picInfo,unsigned char* picBuffer,const char* fileName)
{
	int size;
	unsigned char* p;
	size = picInfo->xres * picInfo->yres * picInfo->bpp;
	FILE* outfile;
	//1. write headinfo to file
	//下面完成对bmp文件头的写入
	bitmapfileheader.bfReserved0 = 0xffff;
	bitmapfileheader.bfType = 0x4d42;
	bitmapfileheader.bfSize = 54+size;
	bitmapfileheader.bfReserved1 = 0;
	bitmapfileheader.bfReserved2 = 0;
	bitmapfileheader.bfOffBits = 54;
	//LOGD("bitmapfileheader is %d\n",sizeof(bitmapfileheader));
	//下面完成对文件信息头的写入
	bitmapinfoheader.biSize = sizeof(bitmapinfoheader);
	bitmapinfoheader.biWidth = picInfo->xres;
	bitmapinfoheader.biHeight = -picInfo->yres;
	bitmapinfoheader.biPlanes = 1;
	bitmapinfoheader.biBitCount = 0x18;
	bitmapinfoheader.biCompression = 0;
	bitmapinfoheader.biSizeImage = size;
	bitmapinfoheader.biXPelsPerMeter = 0; //0xB13
	bitmapinfoheader.biYPelsPerMeter = 0; //0xB13
	bitmapinfoheader.biClrUsed = 0;
	bitmapinfoheader.biClrImportant = 0;
	//LOGD("bitmapinfoheader is %d\n",sizeof(bitmapinfoheader));

	//2. write bmp date to file
	if ((outfile = fopen(fileName, "wb")) == NULL) {
	//	LOGD("Can not create dest file");
		exit(1);
	}

	p = (unsigned char *)&bitmapfileheader;
	p = p+2;


	fseek(outfile,0,SEEK_SET);
	fwrite((void *)p,14,1,outfile);
	fwrite((void *)&bitmapinfoheader,40,1,outfile);

	//写入文件数据
	fwrite(picBuffer,size,1,outfile);

	fclose(outfile);

	return 0;
}




