/*
 * imageinfo.h
 *
 *  Created on: 2014-7-31
 *      Author: niostar
 */

#ifndef IMAGEINFO_H_
#define IMAGEINFO_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int xres;
	int yres;
	int bpp;
}ImageInfo;


typedef struct {
	int left;
	int top;
	int right;
	int bottom;
	int xres;
	int yres;
}RegionInfo;

#ifdef __cplusplus
}
#endif




#endif /* IMAGEINFO_H_ */
