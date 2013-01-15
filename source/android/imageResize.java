/*
java implementation of the imageResize extension.

Add android-specific functionality here.

These functions are called via JNI from native code.
*/
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
import java.io.FileOutputStream;

import com.ideaworks3d.marmalade.LoaderAPI;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
       
import java.io.File;
import android.os.Environment;
import java.lang.Byte;

import android.util.Log;

import android.media.MediaScannerConnection;
import android.media.MediaScannerConnection.MediaScannerConnectionClient;
import android.net.Uri;

class imageResize
{
    private final String LOGTAG = "IMAGERESIZE";
    public boolean resizeImage(String src, String dest, int maxWidth, int maxHeight)
    {	
      BitmapFactory.Options bmOptions = new BitmapFactory.Options();
      bmOptions.inJustDecodeBounds = true;
      BitmapFactory.decodeFile(src, bmOptions);
      int imgWidth = bmOptions.outWidth;
      int imgHeight = bmOptions.outHeight;
      
      int scaleFactor = 1;
      if(imgHeight > imgWidth)
      {
        if(imgHeight > maxHeight)
        {
          scaleFactor = imgHeight / maxHeight;
        }
      }
      else
      {
        if(imgWidth > maxWidth)
        {
          scaleFactor = imgWidth / maxWidth;
        }
      }
      
      /* Set bitmap options to scale the image decode target */
      bmOptions.inJustDecodeBounds = false;
      bmOptions.inSampleSize = scaleFactor;
      bmOptions.inPurgeable = true;
      
      /* Decode the JPEG file into a Bitmap */
      Bitmap bitmap = BitmapFactory.decodeFile(src, bmOptions);
      
      try{
        FileOutputStream out = new FileOutputStream(dest);
        bitmap.compress(Bitmap.CompressFormat.JPEG, 100, out);
      }
      catch(Exception e)
      {
        e.printStackTrace();
      }
      return true;
    }
    
    private MediaScannerConnection _msConn = null;
    
    public boolean cnsSaveImageBufferToGallery(String appname, int[] buffer, int width, int height)
    { 
      Log.d(LOGTAG, "cnsSaveImageBufferToGallery called!");
      /*
      for(int i=0, k=0; i<height; i++, k++)
      {//remember, that OpenGL bitmap is incompatible with Android bitmap
      //and so, some correction need.        
        for(int j=0; j<width; j++)
        {
          int pix=buffer[i*width+j];
          int pb=(pix>>16)&0xff;
          int pr=(pix<<16)&0x00ff0000;
          int pix1=(pix&0xff00ff00) | pr | pb;
          bt[(height-k-1)*width+j]=pix1;
        }
      }
      */

      /*
      BitmapFactory.Options opt = new BitmapFactory.Options();
      
      opt.inDither = true;
      opt.inPreferredConfig = Bitmap.Config.ARGB_8888;
      */
      File imageFileFolder = new File(Environment.getExternalStorageDirectory(),appname);
      imageFileFolder.mkdir();
      
      
      
      String name = "screenshot_" + System.currentTimeMillis() + ".png";
      
      Log.d(LOGTAG, "name: "+name);
      
      Log.d(LOGTAG, "bufferlength: "+buffer.length);
      
      final File imageFile = new File(imageFileFolder, name);
      Bitmap bmp = Bitmap.createBitmap(buffer, width, height, Bitmap.Config.ARGB_8888);
      //BitmapFactory.decodeByteArray(buffer, 0, buffer.length/*, opt*/);
      
      Log.d(LOGTAG, "isNull: "+(bmp==null));
      
      try 
      {
        FileOutputStream out = new FileOutputStream(imageFile);
        bmp.compress(Bitmap.CompressFormat.PNG, 100, out);
        out.flush();
        out.close();
        // scan here!
        _msConn = new MediaScannerConnection(LoaderAPI.getActivity(), new MediaScannerConnectionClient()
        {
          public void onMediaScannerConnected()
          {
            _msConn.scanFile(imageFile.toString(), null);
          }
          public void onScanCompleted(String path, Uri uri)
          {
            _msConn.disconnect();
          }
        });
        _msConn.connect();
        
      } 
      catch (Exception e) 
      {
        e.printStackTrace();
        return false;
      }     
      return true;
    }
}
