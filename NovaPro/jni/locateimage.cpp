/*
 * locateimage.cpp
 *
 *  Created on: 2014-7-29
 *      Author: WJ
 */
/*
 * 该文件中主要对图片中的灯点进定位
 * 与以往不同，此处定位从图片的中心开始进定位。
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <android/log.h>
#include "imageinfo.h"
#include "locateimage.h"
#include "saveDate2BMP.h"

#define LOG_TAG "System.out"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

static int averageDistance;
static Location CentraLocation;
static unsigned char* locationMark;

/*
 * 	用于获取一个目标区域中的像素平均值
 *
 * */
int getRectAverage(ImageInfo* picInfo,unsigned char* tmpBuffer,Location centraLocation,int halfwidth)
{
	int totoAverage;
	int x,y;
	double sum;

	sum = 0;

	for(y=centraLocation.y-halfwidth;y<centraLocation.y+halfwidth;y++){
		for(x=centraLocation.x-halfwidth;x<centraLocation.x+halfwidth;x++){
			sum += (unsigned char)tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0];
		}
	}

	totoAverage = sum / ((2*halfwidth)*(2*halfwidth));

	return totoAverage;


}






//获取一个指定矩形中的一个LED灯点
int getCentraLed(ImageInfo* picInfo,unsigned char* tmpBuffer,Location* preLed,int halfwidth)
{

	int x,y;
	for(y=picInfo->yres/2-halfwidth;y<picInfo->yres/2+halfwidth;y++){
		for(x=picInfo->xres/2-halfwidth;x<picInfo->xres/2+halfwidth;x++){
			if(tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0] == 255){
				preLed->x = x;
				preLed->y = y;
				return 0;
			}
		}
	}
	return 1;

}


//用于在一个指定点周围找到一个LED灯点
//获取一个指定矩形中的一个LED灯点
int getPassbyLed(ImageInfo* picInfo,unsigned char* tmpBuffer,Location* preLed,int halfwidth)
{

	int x,y;
	for(y=preLed->y-halfwidth;y<preLed->y+halfwidth;y++){
		for(x=preLed->x-halfwidth;x<preLed->x+halfwidth;x++){
			if(tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0] == 255){
				preLed->x = x;
				preLed->y = y;
				return 0;
			}
		}
	}
	return 1;

}






/*
	 * isAllInRect
	 * 判断当前传入的半径值是否能将该led等包围 如果不能对矩形半径进行修正
	 *
	 * 需要
	 * */
void isAllInRect(unsigned char* tmpBuffer,Location* preLed,ImageInfo* picInfo)
{
	bool top=false,bottom=false,left=false,right=false;
	int localX,localY;
	int tmp = 0;

	//判断top
	localY = preLed->y-(preLed->width/2)-1;
	for(localX=preLed->x-(preLed->width/2)-1;localX<=preLed->x+(preLed->width/2)+1;localX++)
	{
		tmp |= tmpBuffer[(localY*picInfo->xres+localX)*picInfo->bpp];
	}
	if(tmp != 0)
	{
		top = true;
		tmp =0;
	}

	//判断left
	localX = preLed->x-(preLed->width/2)-1;
	for(localY=preLed->y-(preLed->width/2)-1;localY<=preLed->y+(preLed->width/2)+1;localY++)
	{
		tmp |= tmpBuffer[(localY*picInfo->xres+localX)*picInfo->bpp];
	}
	if(tmp != 0)
	{
		left = true;
		tmp = 0;
	}

	//判断 right
	localX = preLed->x+(preLed->width/2)+1;
	for(localY=preLed->y-(preLed->width/2)-1;localY<=preLed->y+(preLed->width/2)+1;localY++)
	{
		tmp |= tmpBuffer[(localY*picInfo->xres+localX)*picInfo->bpp];
	}
	if(tmp != 0)
	{
		right = true;
		tmp = 0;
	}

	//判断 bottom
	localY = preLed->y+(preLed->width/2)+1;
	for(localX=preLed->x-(preLed->width/2)-1;localX<=preLed->x+(preLed->width/2)+1;localX++)
	{
		tmp |= tmpBuffer[(localY*picInfo->xres+localX)*picInfo->bpp];
	}
	if(tmp != 0)
	{
		bottom = true;
		tmp =0;
	}

	if(top)
		preLed->y = preLed->y-1;
	if(bottom)
		preLed->y = preLed->y+1;
	if(left)
		preLed->x = preLed->x-1;
	if(right)
		preLed->x = preLed->x +1;

	if(top || left || right || bottom)
	{
		//说明矩形外面还有数据
		preLed->isOk = false;
		preLed->width = preLed->width + 2;

	}
	else
	{
		//说明矩形外面没有数据了
		preLed->isOk = true;
	}

}




/*
 * 通过一个像素确定一个灯点的中心坐标
 *
*/
void locateOneLed(ImageInfo* picInfo,unsigned char* tmpBuffer,Location* preLed,unsigned char* locationMark)
{
	int x,y;
	preLed->isOk = false;
	while(!preLed->isOk){
		isAllInRect(tmpBuffer,preLed,picInfo);
	}

	//在locationMark中画一个方框
	for(y=preLed->y-preLed->width/2;y<preLed->y+preLed->width/2;y++){
		for(x=preLed->x-preLed->width/2;x<preLed->x+preLed->width/2;x++){
			locationMark[(y*picInfo->xres+x)*picInfo->bpp+0] = 255;
			locationMark[(y*picInfo->xres+x)*picInfo->bpp+1] = 255;
			locationMark[(y*picInfo->xres+x)*picInfo->bpp+2] = 255;
		}
	}
}


int  locateOneLeft(ImageInfo* picInfo,unsigned char* tmpBuffer,Location* preLed,unsigned char* locationMark)
{
	Location tmpLocation;
	int averageWidth;
	int x,y;
	tmpLocation.x = preLed->x;
	tmpLocation.y = preLed->y;
	y = preLed->y;
	tmpLocation.isOk = false;
	tmpLocation.width = preLed->width;

	// 1 通过改点向左扫描
	for(x=tmpLocation.x-tmpLocation.width;;x--){
		if(tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0] == 255){
			tmpLocation.x = x;
			tmpLocation.isOk = false;
			tmpLocation.width = 1;
			break;
		}
	}
	locateOneLed(picInfo,tmpBuffer,&tmpLocation,locationMark);
	averageWidth = preLed->x - tmpLocation.x;
	LOGD("averageWidth=%d\n",averageWidth);

	return averageWidth;
}



void locateGoodLED(ImageInfo* picInfo,unsigned char* tmpBuffer,unsigned char* locationMark)
{
	Location centraLed;
	//获取LED灯点上的一个像素坐标
	getCentraLed(picInfo,tmpBuffer,&centraLed,100);

	//通过一个像素确定一个灯点
	locateOneLed(picInfo,tmpBuffer,&centraLed,locationMark);
	LOGD("centraLed.x=%d,centraLed.y=%d",centraLed.x,centraLed.y);

	//通过该好点获取一行的灯点
	locateOneLeft(picInfo,tmpBuffer,&centraLed,locationMark);

}



//通过对比获取到目标图片区域的边界
void locateRegion(ImageInfo* picInfo,unsigned char* tmpBuffer,RegionInfo* regionInfo)
{
	int left;
	int top;
	int right;
	int bottom;

	int x,y;
	int tmp;
	left = CentraLocation.x;
	top = CentraLocation.y;
	right = CentraLocation.x;
	bottom = CentraLocation.y;

	//暴力算法 来框住矩形灯板
	//1首先判断上边
	top = CentraLocation.y-5*averageDistance;
	for(y=top;y>top-2*averageDistance/3;){
		LOGD("y top = %d",y);
		for(x=left-15*averageDistance,tmp=0;x<right+15*averageDistance;x++){
			tmp |= tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0];
		}
		y--;
		if(tmp != 0){ //说明还没有到达上边界
			top -= averageDistance/4;  //更新top继续查找
			y = top;
			continue;
		}

	}
	LOGD("top=%d",top);


	//暴力算法 确定下边界
	bottom = CentraLocation.y+5*averageDistance;
	for(y=bottom;y<bottom+2*averageDistance/3;){
			LOGD("y bottom = %d",y);
			for(x=left-15*averageDistance,tmp=0;x<right+15*averageDistance;x++){
				tmp |= tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0];
			}
			y++;
			if(tmp != 0){ //说明还没有到达下边界
				bottom += averageDistance/4;  //更新top继续查找
				y = bottom;
				continue;
			}

		}
	LOGD("bottom=%d",bottom);

	//确定左边界
	left = CentraLocation.x-10*averageDistance;
	for(x=left;x>left-2*averageDistance/3;){
			LOGD("x left = %d",x);
			for(y=top,tmp=0;y<=bottom;y++){
				tmp |= tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0];
			}
			x--;
			if(tmp != 0){ //说明还没有到达左边界
				left -= averageDistance/4;  //更新top继续查找
				x = left;
				continue;
			}

		}
	LOGD("left=%d",left);

	//确定右边界
	right = CentraLocation.x+10*averageDistance;
	for(x=right;x<right+2*averageDistance/3;){
			LOGD("x right = %d",x);
			for(y=top,tmp=0;y<=bottom;y++){
				tmp |= tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0];
			}
			x++;
			if(tmp != 0){ //说明还没有到达左边界
				right += averageDistance/4;  //更新top继续查找
				x = right;
				continue;
			}

		}
	LOGD("right=%d",right);



	//将边界之外的区域全部设置为黑色
	for(y=0;y<picInfo->yres;y++){
		for(x=0;x<picInfo->xres;x++){
			if(x>=left && x<=right && y>=top && y<=bottom){
				//do nothing  //画一个边界  看看效果
			/*	if((x==left)||(x==right)||(y==top)||(y==bottom)){
					tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0]= 255;
					tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+1]= 255;
					tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+2]= 255;
				}
			 	0*/
			}else{
				tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0]= 0;
				tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+1]= 0;
				tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+2]= 0;
			}
		}
	}

	regionInfo->left = left;
	regionInfo->top = top;
	regionInfo->right = right;
	regionInfo->bottom = bottom;




}


//获取两个灯点之间的距离
int getdistance(ImageInfo* picInfo,unsigned char* tmpBuffer,unsigned char* locationMark)
{
	Location centraLed;
	int preX;
	//获取LED灯点上的一个像素坐标
	getCentraLed(picInfo,tmpBuffer,&centraLed,100);
	centraLed.width = 1;
	CentraLocation.x = centraLed.x;
	CentraLocation.y = centraLed.y;

	//通过一个像素确定一个灯点
	locateOneLed(picInfo,tmpBuffer,&centraLed,locationMark);
	LOGD("centraLed.x=%d,centraLed.y=%d",centraLed.x,centraLed.y);
	preX = centraLed.x;
	//通过该好点获取一行的灯点
	return locateOneLeft(picInfo,tmpBuffer,&centraLed,locationMark);
}


//用于在指定区域中定位一个灯点 目前该区与选择为正方形
void locateOneLedInRect(ImageInfo* picInfo,unsigned char* tmpBuffer,location* fromLocation){
	int halfwidth,ret;
	//找到一个非零点
	for(halfwidth=1;;halfwidth=halfwidth+2){
		LOGD("falfwidth=%d\n",halfwidth);
		ret = getPassbyLed(picInfo,tmpBuffer,fromLocation,halfwidth);
		if(!ret){
			LOGD("fromLocation->x=%d\n",fromLocation->x);
			LOGD("fromLocation->y=%d\n",fromLocation->y);
			break;
		}
	}
	fromLocation->isOk = false;
	fromLocation->width = 1;
	//通过该非零点定位灯点
	locateOneLed(picInfo,tmpBuffer,fromLocation,locationMark);
}


//用于定位四个角落灯点的位置
void locate4Led(ImageInfo* picInfo,unsigned char* tmpBuffer,RegionInfo* regionInfo,
		location* lefttop,location* leftbottom,location* righttop,location* rightbottom)
{

	//首先定位左上角灯点位置
	LOGD("lefttop->x=%d\n",lefttop->x);
	LOGD("lefttop->y=%d\n",lefttop->y);
	locateOneLedInRect(picInfo,tmpBuffer,lefttop);
	//定位左下角灯点位置
	locateOneLedInRect(picInfo,tmpBuffer,leftbottom);
	//定位右上角灯点位置
	locateOneLedInRect(picInfo,tmpBuffer,righttop);
	//定位右下角灯点位置
	locateOneLedInRect(picInfo,tmpBuffer,rightbottom);
}


//将上下左右四条边上的距离进行平分。。然后将坐标存入数组中
void getAllLedPos(location* lefttop,location* leftbottom,location* righttop,location* rightbottom,location* allLedPos,RegionInfo* regionInfo)
{
	double x1,y1,x2,y2,k,b;
	double averDistance;
	double x,y;
	int i;

	//写确定左右两边的直线的方程以及该方程上的各个点的坐标然后保存到数组当中
	//x=(y-b)/k
	//1 先算最左边的直线
	x1 = (double)lefttop->x;
	y1 = (double)lefttop->y;
	x2 = (double)leftbottom->x;
	y2 = (double)leftbottom->y;
	if(x2 != x1){
		k = (double)(y2-y1)/(x2-x1);
		b = (double)(y1-k*x1);
	}
	//对该直线进行均分
	averDistance = (double)(y2-y1)/(regionInfo->yres - 1);
	//获得上面直线上的各个点的位置
	if(x2 != x1){
		for(y=y1,i=0;i<regionInfo->yres;i++,y+=averDistance){
			allLedPos[i*regionInfo->xres+0].y = (int)y;
			allLedPos[i*regionInfo->xres+0].x = (int)((y-b)/k);
		}
	}else{
		for(y=y1,i=0;i<regionInfo->yres;i++,y+=averDistance){
			allLedPos[i*regionInfo->xres+0].y = (int)y;
			allLedPos[i*regionInfo->xres+0].x = (int)x1;
		}
	}


	//2 计算最右边的直线
	x1 = righttop->x;
	y1 = righttop->y;
	x2 = rightbottom->x;
	y2 = rightbottom->y;
	if(x2 != x1){
		k = (double)(y2-y1)/(x2-x1);
		b = (double)(y1-k*x1);
	}
	//对该直线进行均分
	averDistance = (double)(y2-y1)/(regionInfo->yres - 1);
	//获得上面直线上的各个点的位置
	if(x2 != x1){
		for(y=y1,i=0;i<regionInfo->yres;i++,y+=averDistance){
			allLedPos[i*regionInfo->xres+regionInfo->xres-1].y = (int)y;
			allLedPos[i*regionInfo->xres+regionInfo->xres-1].x = (int)((y-b)/k);
		}
	}else{
		for(y=y1,i=0;i<regionInfo->yres;i++,y+=averDistance){
			allLedPos[i*regionInfo->xres+regionInfo->xres-1].y = (int)y;
			allLedPos[i*regionInfo->xres+regionInfo->xres-1].x = (int)x1;
		}
	}



/*

	// y=kx+b
	//确定最上面的坐标
	//1 确定上面直线的方程
	x1 = (double)lefttop->x;
	y1 = (double)lefttop->y;
	x2 = (double)righttop->x;
	y2 = (double)righttop->y;
	k = (y2-y1)/(x2-x1);
	b = y1-k*x1;
	//已经获得直线方程 对该方程直线进行均份
	averDistance = (double)(x2-x1)/(regionInfo->xres - 1);
	//获得最上面直线上个点位置
	for(x=x1,i=0;i<regionInfo->xres;i++,x+=averDistance){
		allLedPos[i].x = (int)x;
		allLedPos[i].y = (int)(k*x+b);
	}

	//确定最下的坐标
*/
}

//在原图的指定位置画一个指定大小的红色灯点
void drawOnLed(ImageInfo* picInfo,unsigned char* tmpBuffer,location* LedPos,int width)
{
	int x,y;
	for(y=LedPos->y-width;y<LedPos->y+width;y++){
		for(x=LedPos->x-width;x<LedPos->x+width;x++){
			tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+0] = 255;
			tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+1] = 0;
			tmpBuffer[(y*picInfo->xres+x)*picInfo->bpp+2] = 0;
		}
	}

}




//将led灯点在图片中显示看看效果
void drawCurrentLed(ImageInfo* picInfo,unsigned char* tmpBuffer,RegionInfo* regionInfo,location* allLedPos)
{
	int i;
	for(i=0;i<regionInfo->xres*regionInfo->yres;i++){
		if(allLedPos[i].x != 0){
			drawOnLed(picInfo,tmpBuffer,&allLedPos[i],5);
		}
	}
}



//对图像中所有的信息进行定位 为该文件的外部访问函数接口
int locataImage(ImageInfo* picInfo,unsigned char* tmpBuffer,RegionInfo* regionInfo)
{

	int size = picInfo->xres*picInfo->yres*picInfo->bpp;
	locationMark = (unsigned char*)malloc(size);
	memset((void *)locationMark,0,size);
	location lefttop,leftbottom,righttop,rightbottom;
	//fuck c
	//将所有的LED灯点坐标保存在数组中，用数组来模拟一个链表 我们可以通过regionInfo中得到灯点的个数
	int sizet;
	location* allLedPos;
	sizet = regionInfo->xres * regionInfo->yres * sizeof(location);
	allLedPos = (location*)malloc(sizet);
	memset((void*)allLedPos,0,sizet);

	//获取两个灯点之间的距离
	averageDistance = getdistance(picInfo,tmpBuffer,locationMark);

	LOGD("averageDistance=%d\n",averageDistance);

	//定位灯板目标区域
	locateRegion(picInfo,tmpBuffer,regionInfo);

	LOGD("regionInfo left=%d\n",regionInfo->left);
	LOGD("regionInfo right=%d\n",regionInfo->right);
	LOGD("regionInfo top=%d\n",regionInfo->top);
	LOGD("regionInfo bottom=%d\n",regionInfo->bottom);


	//定位四个角落灯点的位置
	lefttop.x = regionInfo->left;
	lefttop.y = regionInfo->top;

	leftbottom.x = regionInfo->left;
	leftbottom.y = regionInfo->bottom;

	righttop.x = regionInfo->right;
	righttop.y = regionInfo->top;

	rightbottom.x = regionInfo->right;
	rightbottom.y = regionInfo->bottom;

	locate4Led(picInfo,tmpBuffer,regionInfo,
			&lefttop,&leftbottom,&righttop,&rightbottom);


	//将上下左右四条边上的距离进行平分。。然后将坐标存入数组中
	getAllLedPos(&lefttop,&leftbottom,&righttop,&rightbottom,allLedPos,regionInfo);


	for(size=0;size<regionInfo->xres*regionInfo->yres;size++){
		LOGD("size=%d\n",size);
		LOGD("x=%d,y=%d\n",allLedPos[size].x,allLedPos[size].y);

	}

	//将目前的灯点画出来
	drawCurrentLed(picInfo,tmpBuffer,regionInfo,allLedPos);


	//2 在该区域中进灯点定位



	//将locationMark保存成图片
	saveDate2BMP(picInfo,locationMark,"/sdcard/capture/locationMark.bmp");


	//释放内存
	free(allLedPos);
	free(locationMark);
	return 0;
}


