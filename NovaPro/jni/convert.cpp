/*
 * jpg2bmp.cpp
 *
 *  Created on: 2014-7-28
 *      Author: WJ
 */
#include <android/log.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include "jpeglib.h"
#include "convert.h"
#include "bmp.h"
#include "imageinfo.h"


#define LOG_TAG "System.out"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)


struct jpeg_decompress_struct cinfo;
struct jpeg_error_mgr jerr;

FILE * infile;
FILE * outfile;

char str[14];



//构造一个函数 用于实现jpeg到bmp的转换
int jpeg2bmp(const char* srcfile,int quaility,const char* desfile,ImageInfo* picInfo)
{

	int size;
	int row_stride;
	unsigned char *buffer;
	unsigned char *buffer2;
	unsigned char *buffer3;
	char *p;
	int i = 0;

	tagBITMAPFILEHEADER bitmapfileheader;
	tagBITMAPINFOHEADER bitmapinfoheader;

	LOGD("Before 1");
	//1. Allocate and initialize a JPEG decompression object.
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	LOGD("Before 2");
	//2. Specify the source of the compressed data (eg, a file).
	if ((infile = fopen(srcfile, "rb")) == NULL) {
		LOGD("srcfile not exsit");
	    exit(1);
	}

	if ((outfile = fopen(desfile, "w+")) == NULL) {
		LOGD("desfile not exsit");
		exit(1);
	}
	jpeg_stdio_src(&cinfo, infile);

	LOGD("Before 3");
	//3. Call jpeg_read_header() to obtain image info.
	jpeg_read_header(&cinfo, TRUE);

	LOGD("image_width = %d\n", cinfo.image_width);
	LOGD("image_height = %d\n", cinfo.image_height);
	LOGD("num_components = %d\n", cinfo.num_components);
	picInfo->xres = cinfo.image_width;
	picInfo->yres = cinfo.image_height;
	picInfo->bpp  = cinfo.num_components;

	cinfo.out_color_space = JCS_RGB;


	LOGD("Before 5");
	//5. jpeg_start_decompress(...);
	jpeg_start_decompress(&cinfo);

	/* 输出的图象的信息 */
	LOGD("output_width = %d\n", cinfo.output_width);
	LOGD("output_height = %d\n", cinfo.output_height);
	LOGD("output_components = %d\n", cinfo.output_components);


	/*写入图像数据头*/
	size = cinfo.output_width*cinfo.output_height*cinfo.output_components;
	//下面完成对bmp文件头的写入
	bitmapfileheader.bfReserved0 = 0xffff;
	bitmapfileheader.bfType = 0x4d42;
	bitmapfileheader.bfSize = 54+size;
	bitmapfileheader.bfReserved1 = 0;
	bitmapfileheader.bfReserved2 = 0;
	bitmapfileheader.bfOffBits = 54;
	LOGD("bitmapfileheader is %d\n",sizeof(bitmapfileheader));
	//下面完成对文件信息头的写入
	bitmapinfoheader.biSize = sizeof(bitmapinfoheader);
	bitmapinfoheader.biWidth = cinfo.output_width;
	bitmapinfoheader.biHeight = -cinfo.output_height;
	bitmapinfoheader.biPlanes = 1;
	bitmapinfoheader.biBitCount = 0x18;
	bitmapinfoheader.biCompression = 0;
	bitmapinfoheader.biSizeImage = size;
	bitmapinfoheader.biXPelsPerMeter = 0xB13;
	bitmapinfoheader.biYPelsPerMeter = 0xB13;
	bitmapinfoheader.biClrUsed = 0;
	bitmapinfoheader.biClrImportant = 0;
	LOGD("bitmapinfoheader is %d\n",sizeof(bitmapinfoheader));

	p = (char *)&bitmapfileheader;
	p = p+2;


	fseek(outfile,0,SEEK_SET);
	fwrite((void *)p,14,1,outfile);
	fwrite((void *)&bitmapinfoheader,40,1,outfile);



	// 一行的数据长度
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (unsigned char *)malloc(row_stride);
	buffer2 = (unsigned char *)malloc(row_stride);
	buffer3 = (unsigned char *)malloc(size);


	// 循环调用jpeg_read_scanlines来一行一行地获得解压的数据
	while (cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, &buffer, 1);
		//将数据进行搬移 形成真确的位置  现在的数据形式为从高到低为BBGGRR搬移成 RRGGBB
		for(i = 0;i<cinfo.output_width;i++){
			buffer2[3*i+0] = buffer[3*i+2];
			buffer2[3*i+1] = buffer[3*i+1];
			buffer2[3*i+2] = buffer[3*i+0];
		}
		// 写到bmp文件中
		fwrite(buffer2,row_stride,1,outfile);
	}

	free(buffer);
	free(buffer2);
	free(buffer3);

	LOGD("Before 7");
	//7. jpeg_finish_decompress(...);
	jpeg_finish_decompress(&cinfo);



	LOGD("Before 8");
	//8. Release the JPEG decompression object.
	jpeg_destroy_decompress(&cinfo);


	LOGD("short is = %d\n",sizeof(short));
	LOGD("int is = %d\n",sizeof(int));
	LOGD("Long is = %d\n",sizeof(long));

	fclose(outfile);
	fclose(infile);
	return 0;
}


int ReadBmp2Buffer(const char* srcfile,const char* destfile,ImageInfo* picInfo,unsigned char* tmpBuffer)
{
	FILE* infile;
	int count;
	int pic_size = picInfo->xres * picInfo->yres * picInfo->bpp;
	if((infile = fopen(srcfile,"rb")) == NULL){
		LOGD("RGB2GREY-->open srcfile failed!");
		return -1;
	}

	//下面对数据进行处理
	// 1. 将RGB数据读入buffer中
	if(fseek(infile,54,SEEK_SET)!=0){
		LOGD("RGB2GREY-->fseek failed!");
		return -1;
	}


//	if(fread(tmpBuffer,pic_size,1,infile)!=pic_size){
//		LOGD("RGB2GREY-->fread read data error!");
//		return -1;
//	}
	count = fread(tmpBuffer,1,pic_size,infile);
	LOGD("RGB2GREY-->fread count=%d\n",count);
	if(count != pic_size){
		LOGD("RGB2GREY-->fread read data error!");
    	return -1;
	}
	return 0;

}


/*
 * 返回值为中间10000个像素点的平均值
 * */
int RGB2GREY(ImageInfo* picInfo,unsigned char* tmpBuffer)
{

	int ret,x,y;
	int Red,Green,Blue;
	unsigned char Grey;
	unsigned char* buffer;
	int totRed=0,totGreen=0,totBlue=0;
	int max,id;
	int arrage;
	//获取目标图像的颜色通道
	for(y=picInfo->yres/2-50;y<picInfo->yres/2+50;y++){
		for(x=picInfo->xres/2-50;x<picInfo->xres/2+50;x++){
			Blue = tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0];
			Green = tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+1];
			Red = tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+2];
			totRed += Red;
			totGreen += Green;
			totBlue += Blue;
		}
	}
	max = totRed > totGreen ? totRed : totGreen;
	id = totRed > totGreen ? 1: 2;
	max = max > totBlue ? max : totBlue;
	id = max > totBlue ? id : 3;
	LOGD("color id=%d",id);
	arrage = max/10000;
	LOGD("Arrage = %d\n",arrage);

	//开始对RGB进行处理
	for(y=0;y<picInfo->yres;y++){
		for(x=0;x<picInfo->xres;x++){
			Blue = tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0];
			Green = tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+1];
			Red = tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+2];
			if(id == 1) Grey = Red;
			else if(id == 2) Grey = Green;
			else if(id == 3) Grey = Blue;
			tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0]=Grey;
			tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+1]=Grey;
			tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+2]=Grey;

		}
	}

	return 0;

}




int Thresholding(int arrage,ImageInfo* picInfo,unsigned char* tmpBuffer)
{
	int x,y;
	int Grey;
	//开始对RGB进行处理
	for(y=0;y<picInfo->yres;y++){
		for(x=0;x<picInfo->xres;x++){
			Grey = tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0];

			if(Grey > (arrage+50)){
				Grey = 255;
			}else{
				Grey = 0;
			}
			tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0] = Grey;
			tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+1] = Grey;
			tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+2] = Grey;

		}
	}
}


//使用大津算法获取阈值
int GetThreshold(ImageInfo* picInfo,unsigned char* tmpBuffer)
{

	int Threshold;
	int n,n1,n2;
	double m1,m2,sum,csum,fmax,sb;
	unsigned char Grey;
	int greycount[256];
	int x,y,k;
	//初始化数组
	for(x=0;x<256;x++){
		greycount[x] = 0;
	}
	for(y=(picInfo->yres/2)-50;y<(picInfo->yres/2)+50;y++)
	{
		for(x=(picInfo->xres/2)-50;x<(picInfo->xres/2)+50;x++)
		{
			Grey = tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0];
			greycount[Grey]+=1;;
		}
	}
	sum = csum = 0.0;
	n = 0;

	for(k=0;k<=255;k++){
		sum += (double)k*(double)greycount[k];
		n += greycount[k];
	}

	fmax = -1.0;
	n1 = 0;
	for(k=0;k<=255;k++){
		 n1 += greycount[k];
		 if(n1 == 0) continue;
		 n2 = n - n1;
		 if(n2 == 0) break;
		 csum += (double)k*greycount[k];
		 m1 = csum / n1;
		 m2 = (sum - csum)/n2;
		 sb = (double)n1*(double)n2*(m1-m2)*(m1-m2);
		 if(sb > fmax){
			 fmax = sb;
			 Threshold = k;
		 }
	}

	LOGD("Threshold=%d\n",Threshold);

	return Threshold;
}

