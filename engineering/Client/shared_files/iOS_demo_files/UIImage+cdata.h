//
//  ImagePublic.h


#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#define MT_ALPHALOCATION kCGImageAlphaPremultipliedFirst//ARGB

@interface UIImage (cdata)


/************************************
 *UIImage数据转换
 ***********************************/
//UIImage to ARGB
- (unsigned char*) segment_ARGBData;

- (unsigned char*) segment_RGBAData;

//ARGB to UIImage
+ (UIImage*) segment_imageWithARGBData:(unsigned char*) data withSize:(CGSize) size;
+ (UIImage*) segment_imageWithARGBData:(unsigned char*)data withWidth:(int) width withHeight:(int) height;
+ (UIImage*) segment_imageWithRGBAData:(unsigned char*) data withSize:(CGSize) size;
+ (UIImage*) segment_imageWithRGBAData:(unsigned char*)data withWidth:(int) width withHeight:(int) height;

/**
 *  将灰度图转化为UIImage
 *
 *  @param pGrayData 灰度的内存地址，单通道
 *  @param width     图片的宽
 *  @param height    图片的高
 *
 *  @return 生成的UIImage
 */
+ (UIImage*) segment_imageWithGrayData:(unsigned char*) pGrayData width:(int)width height:(int) height;

- (UIImage*) segment_imageWithWidth:(CGFloat) width withHeight:(CGFloat) height;
- (UIImage*) segment_imageWithWidth:(CGFloat) width withHeight:(CGFloat) height isFill:(BOOL) bIsFill;
//根据比例来放缩图片
- (UIImage*) segment_imageWithScale:(CGFloat) scale;
- (UIImage*) segment_imageWithScaleWidth:(CGFloat) scaleWidth withScaleHeight:(CGFloat) scaleHeight;
//限制最长边的大小为nMaxLen
- (UIImage*) segment_imageWithMaxLength:(int) nMaxLen;
//限制最长边的大小为nMaxLen
- (UIImage*) segment_imageWithMaxLength:(int) nMaxLen withUIImageOrientation:(UIImageOrientation)imageOrientation;
//最小边的长度设置为nMinLen，可以放大
- (UIImage*) segment_imageWithScaleMinLength:(int) nMinLen;

- (UIImage*) segment_thumbnailWithSize:(CGSize)size withIsCut:(BOOL)bCut;

- (UIImage*) segment_blendWithImage:(UIImage*) blendImage alpha:(CGFloat) alpha;
- (UIImage*) segment_blendWithImage:(UIImage*) blendImage rect:(CGRect) rect;



@end
