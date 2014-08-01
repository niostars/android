#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <setjmp.h>
#include <android/log.h>
#include <string.h>
#include <malloc.h>
#include "convert.h"
#include "imageinfo.h"
#include "saveDate2BMP.h"
#include "locateimage.h"

#define LOG_TAG "System.out"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#include "com_nova_novapro_MainActivity.h"


static ImageInfo picInfo;


JNIEXPORT jstring JNICALL Java_com_nova_novapro_MainActivity_StringFromC
  (JNIEnv *env, jobject obj)
{
	return env->NewStringUTF("Hello world From C code");
}


JNIEXPORT jint JNICALL Java_com_nova_novapro_MainActivity_Jpeg2Bmp
  (JNIEnv *env, jobject obj)
{
	char* srcfile = "/sdcard/capture/1.jpg";
	char* desfile = "/sdcard/capture/1.bmp";

	LOGD("Hello World");
	int ret = jpeg2bmp(srcfile,100,desfile,&picInfo);
	LOGD("Hello World end");
	return ret;
}

/*
 * 用于对图像数据的处理
 * */
JNIEXPORT jint JNICALL Java_com_nova_novapro_MainActivity_TransImage
  (JNIEnv *env, jobject obj)
{
	char* source = "/sdcard/capture/1.bmp";
	char* dest = "/sdcard/capture/dest1.bmp";
	int ret;
	int arrage;
	unsigned char* tmpBuffer;
	unsigned char* locationMark;
	RegionInfo regionInfo;

	regionInfo.xres = 32;
	regionInfo.yres = 16;

	int pic_size = picInfo.xres * picInfo.yres * picInfo.bpp;
	LOGD("pic_size=%d\n",pic_size);

	//为tmpBuffer分配内存
	tmpBuffer = (unsigned char*)malloc(pic_size);
	if(tmpBuffer == NULL){
		LOGD("malloc date error!");
	}

	//将bmp图像中的数据读取到tmpbuffer中
	ret = ReadBmp2Buffer(source,dest,&picInfo,tmpBuffer);
	//进行一个RGB到灰度值的转换
	ret = RGB2GREY(&picInfo,tmpBuffer);

	//获取图像的二值化阈值
	arrage = GetThreshold(&picInfo,tmpBuffer);
	LOGD("Threshold=%d\n",arrage);


	//将图片中小于arrage的数据设置为0 二值化
	Thresholding(arrage,&picInfo,tmpBuffer);

	//从图像中心开始对图像进行定位操作
	ret = locataImage(&picInfo,tmpBuffer,&regionInfo);

	//将date数据保存到bmp中
	ret = saveDate2BMP(&picInfo,tmpBuffer,dest);

	free(tmpBuffer);

	return ret;

}
