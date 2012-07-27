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

class imageResize
{
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
}
