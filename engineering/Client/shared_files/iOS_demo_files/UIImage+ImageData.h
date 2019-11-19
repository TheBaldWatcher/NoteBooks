//
//  ImagePublic.h


#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface UIImage (ImageData)


/************************************
 *UIImage数据转换
 ***********************************/
//UIImage to ARGB
- (unsigned char*) ARGBData;

- (unsigned char*) RGBAData;

//ARGB to UIImage
+ (UIImage*) imageWithARGBData:(unsigned char*) data withSize:(CGSize) size;
+ (UIImage*) imageWithARGBData:(unsigned char*)data withWidth:(int) width withHeight:(int) height;



- (id) initWithARGBData:(unsigned char*) data withSize:(CGSize) size;
- (id) initWithARGBData:(unsigned char*)data withWidth:(int) width withHeight:(int) height;


/**
 *  将灰度图转化为UIImage
 *
 *  @param pGrayData 灰度的内存地址，单通道
 *  @param width     图片的宽
 *  @param height    图片的高
 *
 *  @return 生成的UIImage
 */
+ (UIImage*) imageWithGrayData:(unsigned char*) pGrayData width:(int)width height:(int) height;

- (UIImage*) imageWithWidth:(CGFloat) width withHeight:(CGFloat) height;
- (UIImage*) imageWithWidth:(CGFloat) width withHeight:(CGFloat) height isFill:(BOOL) bIsFill;
//根据比例来放缩图片
- (UIImage*) imageWithScale:(CGFloat) scale;
- (UIImage*) imageWithScaleWidth:(CGFloat) scaleWidth withScaleHeight:(CGFloat) scaleHeight;
//限制最长边的大小为nMaxLen
- (UIImage*) imageWithMaxLength:(int) nMaxLen;
//限制最长边的大小为nMaxLen
- (UIImage*) imageWithMaxLength:(int) nMaxLen withUIImageOrientation:(UIImageOrientation)imageOrientation;
//最小边的长度设置为nMinLen，可以放大
- (UIImage*) imageWithScaleMinLength:(int) nMinLen;

- (UIImage*) thumbnailWithSize:(CGSize)size withIsCut:(BOOL)bCut;

- (UIImage*) blendWithImage:(UIImage*) blendImage alpha:(CGFloat) alpha;
- (UIImage*) blendWithImage:(UIImage*) blendImage rect:(CGRect) rect;



@end
