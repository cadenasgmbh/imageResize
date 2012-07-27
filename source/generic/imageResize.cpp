/*
Generic implementation of the imageResize extension.
This file should perform any platform-indepedentent functionality
(e.g. error checking) before calling platform-dependent implementations.
*/

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */


#include "imageResize_internal.h"
s3eResult imageResizeInit()
{
    //Add any generic initialisation code here
    return imageResizeInit_platform();
}

void imageResizeTerminate()
{
    //Add any generic termination code here
    imageResizeTerminate_platform();
}

bool resizeImage(const char* src, const char* dest, int maxWidth, int maxHeight)
{
	return resizeImage_platform(src, dest, maxWidth, maxHeight);
}
