/*
 * Internal header for the imageResize extension.
 *
 * This file should be used for any common function definitions etc that need to
 * be shared between the platform-dependent and platform-indepdendent parts of
 * this extension.
 */

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */


#ifndef IMAGERESIZE_INTERNAL_H
#define IMAGERESIZE_INTERNAL_H

#include "s3eTypes.h"
#include "imageResize.h"
#include "imageResize_autodefs.h"


/**
 * Initialise the extension.  This is called once then the extension is first
 * accessed by s3eregister.  If this function returns S3E_RESULT_ERROR the
 * extension will be reported as not-existing on the device.
 */
s3eResult imageResizeInit();

/**
 * Platform-specific initialisation, implemented on each platform
 */
s3eResult imageResizeInit_platform();

/**
 * Terminate the extension.  This is called once on shutdown, but only if the
 * extension was loader and Init() was successful.
 */
void imageResizeTerminate();

/**
 * Platform-specific termination, implemented on each platform
 */
void imageResizeTerminate_platform();
bool resizeImage_platform(const char* src, const char* dest, int maxWidth, int maxHeight);
bool cnsSaveImageBufferToGallery_platform(const char* appname, int* buffer, int width, int height);


#endif /* !IMAGERESIZE_INTERNAL_H */