/*
 * iphone-specific implementation of the imageResize extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "imageResize_internal.h"
#include "s3eEdk.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

// testing
//#define DLog(fmt, ...) NSLog((@"%s [Line %d] " fmt), __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__);
// finishing
#define DLog(...)

//void* g_imageResize_last_error_message = NULL;

@interface ImageResizeC :  NSObject
{
}

- (void) resizeImage:(const char*)src dest:(const char*)dest maxWidth:(int)maxWidth maxHeight:(int)maxHeight;
- (void) saveImageBufferToGallery:(int*)buffer width:(int)width height:(int)height;
  
@end

@implementation ImageResizeC

- (void) resizeImage:(const char*)src dest:(const char*)dest maxWidth:(int)maxWidth maxHeight:(int)maxHeight
{
  NSString* source = [[NSString alloc] initWithUTF8String:src];
  NSString* destination = [[NSString alloc] initWithUTF8String:dest];
  
  
  DLog(@"loading %@ to %@ with max %d * %d", source, destination, maxWidth, maxHeight);
  
  
  UIImage* tmpImage = [[UIImage alloc] initWithContentsOfFile:source];
  DLog(@"uiimage initialized");
  
  CGSize newSize;
  
  CGSize imgSize = tmpImage.size;
  DLog(@"original image size: %f*%f", imgSize.width, imgSize.height);
  
  double scale = 0.0f;
  
  if(imgSize.height > imgSize.width)
  {
    if(imgSize.height > maxHeight)
    {
      newSize.height = (float)maxHeight;
      float factor =  imgSize.height / (float)maxHeight;
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
      newSize.width = (float)maxWidth;
      float factor =  imgSize.width / (float)maxWidth;
      newSize.height = imgSize.height / factor;
    }
    else
    {
      newSize = imgSize;
    }
  }
  DLog(@"scaling calculated");
  
  
  UIGraphicsBeginImageContextWithOptions(newSize, NO, 0.0);
  DLog(@"img context begin");
  [tmpImage drawInRect:CGRectMake(0, 0, newSize.width, newSize.height)];
  DLog(@"img draw in rect");
  UIImage* destImg = UIGraphicsGetImageFromCurrentImageContext();
  DLog(@"img current context");
  UIGraphicsEndImageContext();
  DLog(@"img context end");
  [UIImageJPEGRepresentation(destImg, 0.5) writeToFile:destination atomically:YES];
  
  DLog(@"img save");
}

- (void) saveImageBufferToGallery:(int*)buffer width:(int)width height:(int)height
{
  DLog(@"saveImageBufferToGallery");
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
  
  
  UIImageWriteToSavedPhotosAlbum(myImage, self, @selector(image:didFinishSavingWithError:contextInfo:), nil);
  
  //delete[] buffer2;
  //free(buffer2);
}

- (void)image:(UIImage*)image didFinishSavingWithError:(NSError*)error contextInfo:(void*)contextinfo
{
  /*
  if(g_imageResize_last_error_message)
    s3eEdkFreeOS(g_imageResize_last_error_message);
  */
  
  if(error)
  {
    /*
    NSString *description = [error localizedDescription];
    //NSString *description = [error localizedFailureReason];
  
    DLog(@"description: %@", description);
    int len = [description lengthOfBytesUsingEncoding:NSUTF8StringEncoding] +1;
    g_imageResize_last_error_message = s3eEdkMallocOS(len);
    const char* desc = [description UTF8String];
    memcpy(g_imageResize_last_error_message, desc, len);
    */                                    
    
    s3eEdkCallbacksEnqueue(S3E_EXT_IMAGERESIZE_HASH,
                           IMAGERESIZE_CALLBACK_SAVETOGALLERY_FAILED);
    /*
    alert = [[UIAlertView alloc]      initWithTitle:@"Error"
             message:@"Unable to save image to Photo Album."
             delegate:self
             cancelButtonTitle:@"Ok"
             otherButtonTitles:nil];
     */
  }
  else
  {
    s3eEdkCallbacksEnqueue(S3E_EXT_IMAGERESIZE_HASH,
                           IMAGERESIZE_CALLBACK_SAVETOGALLERY_SUCCESS);
    /*
    alert = [[UIAlertView alloc]  initWithTitle:@"Success"
                                  message:@"Image saved to Photo Album."
                                  delegate:self
                                  cancelButtonTitle:@"Ok"
                                  otherButtonTitles:nil];
     */
  }
  //[alert show];
  //[alert release];
}

@end

ImageResizeC* g_ImageResize = nil;

s3eResult imageResizeInit_platform()
{
  g_ImageResize = [[ImageResizeC alloc] init];
  return S3E_RESULT_SUCCESS;
}

void imageResizeTerminate_platform()
{
  [g_ImageResize release];
  g_ImageResize = nil;
}

bool resizeImage_platform(const char* src, const char* dest, int maxWidth, int maxHeight)
{
  [g_ImageResize resizeImage:src dest:dest maxWidth:maxWidth maxHeight:maxHeight];
  return YES;
}

bool cnsSaveImageBufferToGallery_platform(const char* appname, int* buffer, int width, int height)
{
  [g_ImageResize saveImageBufferToGallery:buffer width:width height:height];
  return YES;
}