/*
 * jpg2bmp.h
 *
 *  Created on: 2014-7-28
 *      Author: WJ
 */

#ifndef JPG2BMP_H_
#define JPG2BMP_H_

#include "imageinfo.h"

#ifdef __cplusplus
extern "C" {
#endif


int jpeg2bmp(const char* srcfile,int quaility,const char* desfile,ImageInfo* picInfo);

int ReadBmp2Buffer(const char* srcfile,const char* destfile,ImageInfo* picInfo,unsigned char* tmpBuffer);

int RGB2GREY(ImageInfo* picInfo,unsigned char* tmpBuffer);

int Thresholding(int arrage,ImageInfo* picInfo,unsigned char* tmpBuffer);

int GetThreshold(ImageInfo* picInfo,unsigned char* tmpBuffer);


#ifdef __cplusplus
}
#endif




#endif /* JPG2BMP_H_ */
