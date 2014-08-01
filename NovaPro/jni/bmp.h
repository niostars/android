#ifndef __BMP_H__
#define __BMP_H__

typedef short WORD;
typedef int DWORD;
typedef long LONG;

/*
 * 1: BITMAPFILEHEADER 14bytes
 * 2: BITMAPINFOHEADER 40bytes
 * 3: BYTE*  位图信息
 *
 * */


typedef struct tagBITMAPFILEHEADER {
WORD bfReserved0;  //用于四字节对其
WORD bfType;  //bit[0:1]说明文件类型 必须是 0x4d42  == "BM"
DWORD bfSize;  //bit[2:5]该文件的大小 用字节表示
WORD bfReserved1;  //bit[6:7]保留 设置为0
WORD bfReserved2;	//bit[8:9]保留 设置为0
DWORD bfOffBits;    //bit[10:13]该值说明实际像素值距离文件开头的偏移量
} BITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER {
DWORD biSize;    //bit[14:17]说明BITMAPINFOHEADER结构所需的字数
LONG biWidth;    //bit[18:21]说明图像的宽度  以像素为单位
LONG biHeight;   //bit[22:25]图像的高度 以位图为单位
WORD biPlanes;  //bit[26:27]为目标设置说面数  总被设置为1
WORD biBitCount;  //bit[28:29]说像素比特数  一般为1，4,8,16,24,32  一般使用较多为 24和32  0x18
DWORD biCompression; //bit[30:33]说明图像的压缩类型 这里只讨论没有压缩的类型 ：BI_RGB  可以设置为0
DWORD biSizeImage;  //bit[34:37]说明图像的大小 以字节为单位 当使用BI_RGB 可以设置为0
LONG biXPelsPerMeter; //bit[38:41]说明图像水平分辨率  0xEC4
LONG biYPelsPerMeter; //bit[42:45]说明图像垂直分辨率
DWORD biClrUsed; 	  //bit[46:49]说明图像实际使用的彩色表中的颜色索引 （设置为0的话，说明使用所以的调试版）
DWORD biClrImportant; //bit[50:53]说明对图像显示有重要影响的索引的数目  如果为0 表示都重要
} BITMAPINFOHEADER;


#endif //__BMP_H__
