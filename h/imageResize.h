/*
 * (C) 2001-2012 Marmalade. All Rights Reserved.
 *
 * This document is protected by copyright, and contains information
 * proprietary to Marmalade.
 *
 * This file consists of source code released by Marmalade under
 * the terms of the accompanying End User License Agreement (EULA).
 * Please do not use this program/source code before you have read the
 * EULA and have agreed to be bound by its terms.
 */
/*
 * WARNING: this is an autogenerated file and will be overwritten by
 * the extension interface script.
 */
#ifndef S3E_EXT_IMAGERESIZE_H
#define S3E_EXT_IMAGERESIZE_H

#include <s3eTypes.h>

typedef enum imageResizeCallback
{
	IMAGERESIZE_CALLBACK_SAVETOGALLERY_SUCCESS,
	IMAGERESIZE_CALLBACK_SAVETOGALLERY_FAILED,
	IMAGERESIZE_CALLBACK_MAX
}imageResizeCallback;
// \cond HIDDEN_DEFINES
S3E_BEGIN_C_DECL
// \endcond

/**
 * Returns S3E_TRUE if the imageResize extension is available.
 */
s3eBool imageResizeAvailable();

/**
 * Registers a callback to be called for an operating system event.
 *
 * The available callback types are listed in @ref imageResizeCallback.
 * @param cbid ID of the event for which to register.
 * @param fn callback function.
 * @param userdata Value to pass to the @e userdata parameter of @e NotifyFunc.
 * @return
 *  - @ref S3E_RESULT_SUCCESS if no error occurred.
 *  - @ref S3E_RESULT_ERROR if the operation failed.\n
 *
 * @see imageResizeUnRegister
 * @note For more information on the system data passed as a parameter to the callback
 * registered using this function, see the @ref imageResizeCallback enum.
 */
s3eResult imageResizeRegister(imageResizeCallback cbid, s3eCallback fn, void* userData);

/**
 * Unregister a callback for a given event.
 * @param cbid ID of the callback for which to register.
 * @param fn Callback Function.
 * @return
 * - @ref S3E_RESULT_SUCCESS if no error occurred.
 * - @ref S3E_RESULT_ERROR if the operation failed.\n
 * @note For more information on the systemData passed as a parameter to the callback
 * registered using this function, see the imageResizeCallback enum.
 * @note It is not necessary to define a return value for any registered callback.
 * @see imageResizeRegister
 */
s3eResult imageResizeUnRegister(imageResizeCallback cbid, s3eCallback fn);

bool resizeImage(const char* src, const char* dest, int maxWidth, int maxHeight);

bool cnsSaveImageBufferToGallery(const char* appname, int* buffer, int width, int height);

S3E_END_C_DECL

#endif /* !S3E_EXT_IMAGERESIZE_H */
