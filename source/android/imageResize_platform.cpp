/*
 * android-specific implementation of the imageResize extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "imageResize_internal.h"

#include "s3eEdk.h"
#include "s3eEdk_android.h"
#include <jni.h>
#include "IwDebug.h"

static jobject g_Obj;
static jmethodID g_resizeImage;
static jmethodID g_cnsSaveImageBufferToGallery;

s3eResult imageResizeInit_platform()
{
    // Get the environment from the pointer
    JNIEnv* env = s3eEdkJNIGetEnv();
    jobject obj = NULL;
    jmethodID cons = NULL;

    // Get the extension class
    jclass cls = s3eEdkAndroidFindClass("imageResize");
    if (!cls)
        goto fail;

    // Get its constructor
    cons = env->GetMethodID(cls, "<init>", "()V");
    if (!cons)
        goto fail;

    // Construct the java class
    obj = env->NewObject(cls, cons);
    if (!obj)
        goto fail;

    // Get all the extension methods
    g_resizeImage = env->GetMethodID(cls, "resizeImage", "(Ljava/lang/String;Ljava/lang/String;II)Z");
    if (!g_resizeImage)
        goto fail;

   g_cnsSaveImageBufferToGallery = env->GetMethodID(cls, "cnsSaveImageBufferToGallery", "(Ljava/lang/String;[III)Z" );
   if(!g_cnsSaveImageBufferToGallery)
        goto fail;


    IwTrace(IMAGERESIZE, ("IMAGERESIZE init success"));
    g_Obj = env->NewGlobalRef(obj);
    env->DeleteLocalRef(obj);
    env->DeleteGlobalRef(cls);

    // Add any platform-specific initialisation code here
    return S3E_RESULT_SUCCESS;

fail:
    jthrowable exc = env->ExceptionOccurred();
    if (exc)
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        IwTrace(imageResize, ("One or more java methods could not be found"));
    }
    return S3E_RESULT_ERROR;

}

void imageResizeTerminate_platform()
{
    // Add any platform-specific termination code here
}

bool resizeImage_platform(const char* src, const char* dest, int maxWidth, int maxHeight)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring src_jstr = env->NewStringUTF(src);
    jstring dest_jstr = env->NewStringUTF(dest);
    return (bool)env->CallBooleanMethod(g_Obj, g_resizeImage, src_jstr, dest_jstr, maxWidth, maxHeight);
}

bool cnsSaveImageBufferToGallery(const char* appname, int* buffer, int width, int height)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring appname_jstr = env->NewStringUTF(appname);
    int bufferlen = width*height;
    jintArray intArray = env->NewIntArray(bufferlen);    
    env->SetIntArrayRegion(intArray, 0, bufferlen, (const jint*)buffer);
    
    bool ret = (bool)env->CallBooleanMethod(g_Obj, g_cnsSaveImageBufferToGallery, appname_jstr, intArray, width, height);
    env->DeleteLocalRef(intArray);
    return ret;
}
