/*
 * locateimage.h
 *
 *  Created on: 2014-7-29
 *      Author: WJ
 */

#ifndef LOCATEIMAGE_H_
#define LOCATEIMAGE_H_

#include "imageinfo.h"


typedef struct location{
	int x;
	int y;
	int width;
	bool isOk;

}Location;



#ifdef __cplusplus
extern "C" {
#endif

int locataImage(ImageInfo* picInfo,unsigned char* tmpBuffer,RegionInfo* regionInfo);



#ifdef __cplusplus
}
#endif




#endif /* LOCATEIMAGE_H_ */
