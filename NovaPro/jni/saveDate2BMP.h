/*
 * saveDate2BMP.h
 *
 *  Created on: 2014-7-25
 *      Author: WJ
 */

#ifndef SAVEDATE2BMP_H_
#define SAVEDATE2BMP_H_
#include "imageinfo.h"

#ifdef __cplusplus
extern "C" {
#endif


int saveDate2BMP(ImageInfo* picInfo,unsigned char* picBuffer,const char* fileName);


#ifdef __cplusplus
}
#endif

#endif /* SAVEDATE2BMP_H_ */
