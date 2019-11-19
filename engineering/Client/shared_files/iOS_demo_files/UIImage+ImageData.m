//
//  ImagePublic.m
//  MeituXX
//
//  Created by fu songlin on 12-5-16.
//  Copyright (c) 2012年 Xiamen Meitu Technology Co., Ltd. All rights reserved.
//

#import "UIImage+ImageData.h"

@implementation UIImage (ImageData)



#pragma mark
#pragma mark UIImage与ARGB数据的转换
// 返回一个基于c带有图片数据的的bitmap
- (unsigned char*) ARGBData
{


    int width = CGImageGetWidth(self.CGImage);
    int height = CGImageGetHeight(self.CGImage);
    if(width == 0 || height == 0)
        return 0;
    unsigned char* imageData = (unsigned char *) malloc(4 * width * height);

    CGColorSpaceRef cref = CGColorSpaceCreateDeviceRGB();
    CGContextRef gc = CGBitmapContextCreate(imageData,
                                            width,height,
                                            8,width*4,
                                            cref,kCGImageAlphaPremultipliedFirst);
    CGColorSpaceRelease(cref);
    UIGraphicsPushContext(gc);

    CGRect rect = {{ 0 , 0 }, {width, height }}; 
    CGContextDrawImage( gc, rect, self.CGImage ); 
    UIGraphicsPopContext();
    CGContextRelease(gc);


    return imageData;// CGBitmapContextGetData(gc);
}

- (unsigned char*) RGBAData
{
    
    
    int width = CGImageGetWidth(self.CGImage);
    int height = CGImageGetHeight(self.CGImage);
    if(width == 0 || height == 0)
        return 0;
    unsigned char* imageData = (unsigned char *) malloc(4 * width * height);
    
    CGColorSpaceRef cref = CGColorSpaceCreateDeviceRGB();
    CGContextRef gc = CGBitmapContextCreate(imageData,
                                            width,height,
                                            8,width*4,
                                            cref,kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(cref);
    UIGraphicsPushContext(gc);
    
    CGRect rect = {{ 0 , 0 }, {width, height }};
    CGContextDrawImage( gc, rect, self.CGImage );
    UIGraphicsPopContext();
    CGContextRelease(gc);
    
    
    return imageData;// CGBitmapContextGetData(gc);
}

+ (UIImage*) imageWithRGBAData:(unsigned char*)data withWidth:(int) width withHeight:(int) height
{
    // Create a bitmap context with the image data
    CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(data, width, height, 8, width*4, colorspace, kCGImageAlphaPremultipliedLast);
    CGImageRef cgImage = nil;
    if (context != nil) {
        cgImage = CGBitmapContextCreateImage (context);
        CGContextRelease(context);
    }
    CGColorSpaceRelease(colorspace);
    
    UIImage * image = nil;
    
    if(cgImage != nil)
        image = [[UIImage alloc ]initWithCGImage:cgImage];
    
    // Release the cgImage when done
    CGImageRelease(cgImage);
    return image;
}

+ (UIImage*) imageWithARGBData:(unsigned char*) data withSize:(CGSize) size
{
    return [self imageWithARGBData:data withWidth:size.width withHeight:size.height];
}

+ (UIImage*)  imageWithARGBData:(unsigned char*)data withWidth:(int) width withHeight:(int) height;
{
    return [self imageWithARGBData:data withWidth:width withHeight:height];
}

- (id) initWithARGBData:(unsigned char*) data withSize:(CGSize) size
{
    return [self initWithARGBData:data withWidth:size.width withHeight:size.height];
}
- (id) initWithARGBData:(unsigned char*)data withWidth:(int) width withHeight:(int) height
{
    self = [super init];
    if (self)
    {
        // Create a bitmap context with the image data
        CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
        CGContextRef context = CGBitmapContextCreate(data, width, height, 8, width*4, colorspace, kCGImageAlphaPremultipliedFirst);
        CGImageRef cgImage = nil;
        if (context != nil) {
            cgImage = CGBitmapContextCreateImage (context);
            CGContextRelease(context);
        }
        CGColorSpaceRelease(colorspace);
        
        
        if(cgImage != nil)
            if (!(self = [self initWithCGImage:cgImage])) return nil;
        
        // Release the cgImage when done
        CGImageRelease(cgImage);
    }
    return self;
}


+ (UIImage*) imageWithGrayData:(unsigned char*) pGrayData width:(int)width height:(int) height
{
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
    CGContextRef context = CGBitmapContextCreate (pGrayData,width,height,8,width,colorSpace,kCGImageAlphaNone);
    CGColorSpaceRelease(colorSpace);
    if (context == NULL) {
        return nil;
    }
    CGImageRef cgImageRef = CGBitmapContextCreateImage(context);
    UIImage *grayImage = [[UIImage alloc] initWithCGImage:cgImageRef];
    CGImageRelease(cgImageRef);
    CGContextRelease(context);
    return grayImage;
}

- (UIImage*) imageWithScaleWidth:(CGFloat) scaleWidth withScaleHeight:(CGFloat) scaleHeight isFill:(BOOL) bFill
{
    return [self imageWithScaleWidth:scaleWidth withScaleHeight:scaleHeight isFill:bFill orientation:self.imageOrientation];
}
- (UIImage*) imageWithScaleWidth:(CGFloat) scaleWidth withScaleHeight:(CGFloat) scaleHeight isFill:(BOOL) bFill orientation:(UIImageOrientation)orient
{
    CGImageRef imgRef = self.CGImage;
	
	CGFloat width = CGImageGetWidth(imgRef);
	CGFloat height = CGImageGetHeight(imgRef);
	
	CGAffineTransform transform = CGAffineTransformIdentity;
	
	CGRect bounds = CGRectMake(0, 0, width, height);
	CGFloat boundHeight;
	
	switch(orient)
	
	{
			
		case UIImageOrientationUp: //EXIF = 1
			
			transform = CGAffineTransformIdentity;
			
			break;
			
			
		case UIImageOrientationUpMirrored: //EXIF = 2
			
			transform = CGAffineTransformMakeTranslation(width, 0.0);
			
			transform = CGAffineTransformScale(transform, -1.0, 1.0);
			
			break;
			
			
		case UIImageOrientationDown: //EXIF = 3
			
			transform = CGAffineTransformMakeTranslation(width, height);
			
			transform = CGAffineTransformRotate(transform, M_PI);
			
			break;
			
			
		case UIImageOrientationDownMirrored: //EXIF = 4
			
			transform = CGAffineTransformMakeTranslation(0.0, height);
			
			transform = CGAffineTransformScale(transform, 1.0, -1.0);
			
			break;
			
			
		case UIImageOrientationLeftMirrored: //EXIF = 5
			
			boundHeight = bounds.size.height;
			
			bounds.size.height = bounds.size.width;
			
			bounds.size.width = boundHeight;
			
			transform = CGAffineTransformMakeTranslation(height, width);
			
			transform = CGAffineTransformScale(transform, -1.0, 1.0);
			
			transform = CGAffineTransformRotate(transform, 3.0 * M_PI / 2.0);
			
			break;
			
			
		case UIImageOrientationLeft: //EXIF = 6
			
			boundHeight = bounds.size.height;
			
			bounds.size.height = bounds.size.width;
			
			bounds.size.width = boundHeight;
			
			transform = CGAffineTransformMakeTranslation(0.0, width);
			
			transform = CGAffineTransformRotate(transform, 3.0 * M_PI / 2.0);
			
			break;
			
			
		case UIImageOrientationRightMirrored: //EXIF = 7
			
			boundHeight = bounds.size.height;
			
			bounds.size.height = bounds.size.width;
			
			bounds.size.width = boundHeight;
			
			transform = CGAffineTransformMakeScale(-1.0, 1.0);
			
			transform = CGAffineTransformRotate(transform, M_PI / 2.0);
			
			break;
			
			
		case UIImageOrientationRight: //EXIF = 8
			
			boundHeight = bounds.size.height;
			
			bounds.size.height = bounds.size.width;
			
			bounds.size.width = boundHeight;
			
			transform = CGAffineTransformMakeTranslation(height, 0.0);
			
			transform = CGAffineTransformRotate(transform, M_PI / 2.0);
			
			break;
			
			
		default:
			transform = CGAffineTransformIdentity;
			[NSException raise:NSInternalInconsistencyException format:@"Invalid image orientation"];
			
	}
	CGSize boundSize = bounds.size;
    
    
    boundSize.width = bounds.size.width*scaleWidth;
    boundSize.height= bounds.size.height*scaleHeight;
    
    
    
	boundSize = CGSizeMake((int)boundSize.width, (int)boundSize.height);
	UIGraphicsBeginImageContext(boundSize);
	
	
	CGContextRef context = UIGraphicsGetCurrentContext();
    
	if (orient == UIImageOrientationRight || orient == UIImageOrientationLeft)
    {
		
		CGContextScaleCTM(context, -scaleWidth, scaleHeight);
		
		CGContextTranslateCTM(context, -height, 0);
		
	}
    else if (orient == UIImageOrientationLeftMirrored || orient == UIImageOrientationRightMirrored)
    {
        CGContextScaleCTM(context, scaleWidth, -scaleHeight);
		
		CGContextTranslateCTM(context, 0, -width);
    }
	else
    {
		
		CGContextScaleCTM(context, scaleWidth, -scaleHeight);
		
		CGContextTranslateCTM(context, 0, -height);
		
	}
    CGRect rectClear = CGRectMake(0, 0, boundSize.width/scaleWidth, boundSize.height/scaleHeight);
	//默认将底图刷为白色
    if (bFill ==YES)
    {
        CGContextSetRGBFillColor(context, 1.0, 1.0, 1.0, 1.0);
        CGContextFillRect(context, rectClear);
    }
    
	
	CGContextConcatCTM(context, transform);
	
	
    if (scaleWidth * scaleHeight <1)//缩小的，改为低是为了多保留细节
    {
        CGContextSetInterpolationQuality(context, kCGInterpolationLow);
    }
    else
    {
        CGContextSetAllowsAntialiasing(context, YES);
        CGContextSetInterpolationQuality(context, kCGInterpolationHigh);
    }
	
	CGContextDrawImage(UIGraphicsGetCurrentContext(), CGRectMake(0, 0, width, height), imgRef);
	UIImage *imageCopy = UIGraphicsGetImageFromCurrentImageContext();
	UIGraphicsEndImageContext();
    
	return imageCopy;
}
- (UIImage*) imageWithScaleWidth:(CGFloat) scaleWidth withScaleHeight:(CGFloat) scaleHeight
{
    return [self imageWithScaleWidth:scaleWidth withScaleHeight:scaleHeight isFill:YES];
}

- (UIImage*) imageWithWidth:(CGFloat) width withHeight:(CGFloat) height
{
    CGFloat scaleWidth = width/self.size.width;
    CGFloat scaleHeight= height/self.size.height;
    return [self imageWithScaleWidth:scaleWidth withScaleHeight:scaleHeight];
}
- (UIImage*) imageWithWidth:(CGFloat) width withHeight:(CGFloat) height isFill:(BOOL) bIsFill
{
    CGFloat scaleWidth = width/self.size.width;
    CGFloat scaleHeight= height/self.size.height;
    return [self imageWithScaleWidth:scaleWidth withScaleHeight:scaleHeight isFill:bIsFill];
}
- (UIImage*) imageWithScale:(CGFloat) scale
{
    return [self imageWithScaleWidth:scale withScaleHeight:scale];
}
- (UIImage*) imageWithMaxLength:(int) nMaxLen withUIImageOrientation:(UIImageOrientation)imageOrientation
{
    CGFloat width = CGImageGetWidth(self.CGImage);
	
	CGFloat height = CGImageGetHeight(self.CGImage);
    
    CGFloat wscale = nMaxLen / width;
    CGFloat hscale = nMaxLen / height;
    
    CGFloat scale = wscale;
    if (scale > hscale)
    {
        scale = hscale;
    }
    
    if (scale > 1.0)
    {
        scale = 1.0;
    }
    
    return [self imageWithScaleWidth:scale withScaleHeight:scale isFill:YES orientation:imageOrientation];
}
- (UIImage*) imageWithMaxLength:(int) nMaxLen
{
    
    CGFloat width = CGImageGetWidth(self.CGImage);
	
	CGFloat height = CGImageGetHeight(self.CGImage);
    
    CGFloat wscale = nMaxLen / width;
    CGFloat hscale = nMaxLen / height;
    
    CGFloat scale = wscale;
    if (scale > hscale)
    {
        scale = hscale;
    }
    
    if (scale > 1.0)
    {
        scale = 1.0;
    }
    
    return [self imageWithScaleWidth:scale withScaleHeight:scale];
}
- (UIImage*) imageWithScaleMinLength:(int) nMinLen
{
    
    CGFloat width = CGImageGetWidth(self.CGImage);
	
	CGFloat height = CGImageGetHeight(self.CGImage);
    
    CGFloat wscale = nMinLen / width;
    CGFloat hscale = nMinLen / height;
    
    CGFloat scale = wscale;
    if (scale < hscale)
    {
        scale = hscale;
    }
    
    
    return [self imageWithScaleWidth:scale withScaleHeight:scale];
}
- (UIImage*) blendWithImage:(UIImage*) blendImage rect:(CGRect) rect
{
    UIGraphicsBeginImageContext(self.size);
    
    // get context
    //
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    // push context to make it current
    // (need to do this manually because we are not drawing in a UIView)
    //
    UIGraphicsPushContext(context);
    
    //设置高清和抗锯齿
    CGContextSetAllowsAntialiasing(context, YES);
	CGContextSetInterpolationQuality(context, kCGInterpolationHigh);
    
    [self drawAtPoint:CGPointZero];
    [blendImage drawInRect:rect];
    
    UIGraphicsPopContext();
    
    
    // get a UIImage from the image context- enjoy!!!
    //
    UIImage *tmpImage = UIGraphicsGetImageFromCurrentImageContext();
    
    // clean up drawing environment
    //
    UIGraphicsEndImageContext();
    return tmpImage;
}
- (UIImage*) blendWithImage:(UIImage*) blendImage alpha:(CGFloat) alpha
{
    UIGraphicsBeginImageContext(self.size);
    
    // get context
    //
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    // push context to make it current
    // (need to do this manually because we are not drawing in a UIView)
    //
    UIGraphicsPushContext(context);
    
    //设置高清和抗锯齿
    CGContextSetAllowsAntialiasing(context, YES);
	CGContextSetInterpolationQuality(context, kCGInterpolationHigh);
    
    
    [self drawAtPoint:CGPointZero];
    [blendImage drawAtPoint:CGPointZero blendMode:kCGBlendModeNormal alpha:alpha];
    
    UIGraphicsPopContext();
    
    
    // get a UIImage from the image context- enjoy!!!
    //
    UIImage *tmpImage = UIGraphicsGetImageFromCurrentImageContext();
    
    // clean up drawing environment
    //
    UIGraphicsEndImageContext();
    return tmpImage;
}
- (UIImage *) thumbnailWithSize:(CGSize)size withIsCut:(BOOL)bIsCut
{
    CGSize imageSize = self.size;
    float scaleWidth = size.width/imageSize.width;
    float scaleHeight = size.height/imageSize.height;
    float scale = MAX(scaleWidth, scaleHeight);
    imageSize = CGSizeMake((int)(imageSize.width*scale), (int)(imageSize.height*scale));
    
    // create a new bitmap image context
    //
    UIGraphicsBeginImageContext(imageSize);
    
    // get context
    //
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    // push context to make it current
    // (need to do this manually because we are not drawing in a UIView)
    //
    UIGraphicsPushContext(context);
    
    //设置高清和抗锯齿
    CGContextSetAllowsAntialiasing(context, YES);
	CGContextSetInterpolationQuality(context, kCGInterpolationHigh);
    
    [self drawInRect:CGRectMake(0, 0, imageSize.width, imageSize.height)];
    
    // pop context
    //
    UIGraphicsPopContext();
    
    // get a UIImage from the image context- enjoy!!!
    //
    UIImage *tmpImage = UIGraphicsGetImageFromCurrentImageContext();
    
    // clean up drawing environment
    //
    UIGraphicsEndImageContext();
    
    if (size.width == 0 || size.height == 0 || !bIsCut)
    {
        return tmpImage;
    }
    CGImageRef temp_img;
    CGRect thumbRect = CGRectMake((imageSize.width-size.width)/2, (imageSize.height-size.height)/2, size.width, size.height);
    temp_img = CGImageCreateWithImageInRect(tmpImage.CGImage, thumbRect);
    UIImage *outputImage = [UIImage imageWithCGImage:temp_img];
    CGImageRelease(temp_img);
    
    return outputImage;
}


@end
