/*
 * iphone-specific implementation of the imageResize extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "imageResize_internal.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

// testing
//#define DLog(fmt, ...) DLog((@"%s [Line %d] " fmt), __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__);
// finishing
#define DLog(...)

s3eResult imageResizeInit_platform()
{
    // Add any platform-specific initialisation code here
    return S3E_RESULT_SUCCESS;
}

void imageResizeTerminate_platform()
{
    // Add any platform-specific termination code here
}

bool resizeImage_platform(const char* src, const char* dest, int maxWidth, int maxHeight)
{
  NSString* source = [[NSString alloc] initWithUTF8String:src];
  NSString* destination = [[NSString alloc] initWithUTF8String:dest];
  
  
  DLog(@"loading %@ to %@ with %d * %d", source, destination, maxWidth, maxHeight);
 
  
  UIImage* tmpImage = [[UIImage alloc] initWithContentsOfFile:source]; 
  DLog(@"uiimage initialized");
  
  CGSize newSize;
  
  CGSize imgSize = tmpImage.size;
  
  double scale = 0.0f;
  
  if(imgSize.height > imgSize.width)
  {
    if(imgSize.height > maxHeight)
    {
      newSize.height = maxHeight;
      double factor = (double) imgSize.height / (double)maxHeight;
      newSize.width = imgSize.width / factor;
    }
    else 
    {
      newSize = imgSize; 
    }
  }
  else 
  {
    if(imgSize.width > maxWidth)
    {
      newSize.width = maxWidth;
      double factor = (double) imgSize.width / (double)maxWidth;
      newSize.height = imgSize.height / factor;
    }
    else
    {
      newSize = imgSize;
    }
  }
  DLog(@"scaling calculated");
  
  
  UIGraphicsBeginImageContext(newSize);
  DLog(@"img context begin");
  [tmpImage drawInRect:CGRectMake(0, 0, newSize.width, newSize.height)];
  DLog(@"img draw in rect");
  UIImage* destImg = UIGraphicsGetImageFromCurrentImageContext();
  DLog(@"img current context");
  UIGraphicsEndImageContext();
  DLog(@"img context end");
  [UIImageJPEGRepresentation(destImg, 1.0) writeToFile:destination atomically:YES];
  
  
  
  DLog(@"img save");
  return YES;
  
}

bool cnsSaveImageBufferToGallery(const char* appname, int* buffer, int width, int height)
{
  DLog(@"cnsSaveImageBufferToGallery");
  /*
  GLubyte* b = (GLubyte*)buffer;
  GLubyte *buffer2 = new GLubyte[bufferlen];//(GLubyte *) malloc(bufferlen);
  for(int y = 0; y < height; y++)
  {
      for(int x = 0; x < width * 4; x++)
      {
          buffer2[(height-1 - y) * width * 4 + x] = b[y * 4 * width + x];
      }
  }
  */
  
 
  // make data provider with data.
  CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, (const void *)buffer, width*height*4, NULL);

  // prep the ingredients
  int bitsPerComponent = 8;
  int bitsPerPixel = 32;
  int bytesPerRow = 4 * width;
  CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
  CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
  CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;

  // make the cgimage
  DLog(@"CGImageCreate");
  CGImageRef imageRef = CGImageCreate(width, height, bitsPerComponent, bitsPerPixel, bytesPerRow, colorSpaceRef, bitmapInfo, provider, NULL, NO, renderingIntent);

  // then make the uiimage from that
  UIImage *myImage = [UIImage imageWithCGImage:imageRef];
  DLog(@"UIImageWriteToSavedPhotosAlbum");
  UIImageWriteToSavedPhotosAlbum(myImage, nil, nil, nil);
  
  //delete[] buffer2;
  //free(buffer2);
  
  return YES;
}