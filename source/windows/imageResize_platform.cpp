/*
 * windows-specific implementation of the imageResize extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "imageResize_internal.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "s3eFile.h"

extern "C" {
#include "jpeglib.h"
#include "jerror.h"

}


/* Read JPEG image from a memory segment */
static void init_source (j_decompress_ptr cinfo) {}
static boolean fill_input_buffer (j_decompress_ptr cinfo)
{
  //ERREXIT(cinfo, JERR_INPUT_EMPTY);
  return TRUE;
}
static void skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  struct jpeg_source_mgr* src = (struct jpeg_source_mgr*) cinfo->src;

  if (num_bytes > 0) {
    src->next_input_byte += (size_t) num_bytes;
    src->bytes_in_buffer -= (size_t) num_bytes;
  }
}
static void term_source (j_decompress_ptr cinfo) {}
static void jpeg_mem_src (j_decompress_ptr cinfo, void* buffer, long nbytes)
{
  struct jpeg_source_mgr* src;

  if (cinfo->src == NULL) {   /* first time for this JPEG object? */
    cinfo->src = (struct jpeg_source_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
      sizeof(struct jpeg_source_mgr));

  }

  src = (struct jpeg_source_mgr*) cinfo->src;
  src->init_source = init_source;
  src->fill_input_buffer = fill_input_buffer;
  src->skip_input_data = skip_input_data;
  src->resync_to_restart = jpeg_resync_to_restart; /* use default method */
  src->term_source = term_source;
  src->bytes_in_buffer = nbytes;
  src->next_input_byte = (JOCTET*)buffer;
}


static void emit_message (j_common_ptr cinfo, int msg_level)
{
  struct jpeg_error_mgr * err = cinfo->err;

  if (msg_level < 0) {
    /* It's a warning message.  Since corrupt files may generate many warnings,
     * the policy implemented here is to show only the first warning,
     * unless trace_level >= 3.
     */
    if (err->num_warnings == 0 || err->trace_level >= 3)
      (*err->output_message) (cinfo);
    /* Always count warnings in num_warnings. */
    err->num_warnings++;
  } else {
    /* It's a trace message.  Show it if trace_level >= msg_level. */
    if (err->trace_level >= msg_level)
      (*err->output_message) (cinfo);
  }
}


struct my_error_mgr {
  struct jpeg_error_mgr pub;
//  jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr *my_error_ptr;
METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
  (*cinfo->err->output_message) (cinfo);
  //MessageBox(NULL, buffer, "JPEG Fatal Error", MB_ICONSTOP);
  //longjmp(myerr->setjmp_buffer, 1);
}


static void reset_err_mgr(j_common_ptr cinfo)
{
  cinfo->err->num_warnings = 0;
  cinfo->err->msg_code = 0;
}


int readJPEG(const char* filename, unsigned char** image_buffer, int* image_width, int *image_height)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  std::ifstream is(filename, std::ifstream::binary);

  
  if(!is.is_open())
  {
    return -3;
  }


  JSAMPARRAY jpeg_buffer;
  long int i_buffer, i_image;

  /*/
  if((infile = s3eFileOpen(filename, "rb")) == NULL)
  {
    char buf[500];
    sprintf(buf, "JPEG :\nCan't open %s\n", filename);
    //MessageBox(GetActiveWindow(), buf, "JPEG Error", MB_OK|MB_ICONERROR);
    return -1;
  }/*/

  char* buffer;

  is.seekg(0, std::ios::end);
  int length = is.tellg();
  is.seekg(0, std::ios::beg);

  buffer = new char[length];

  is.read(buffer, length);

  //jerr.error_exit = error_exit;
  jerr.emit_message = emit_message;// emit_message;
  jerr.output_message = NULL;//output_message;
  jerr.format_message = NULL;//format_message;
  jerr.reset_error_mgr = reset_err_mgr;//reset_error_mgr;

  jerr.trace_level = 0;		/* default = no tracing */
  jerr.num_warnings = 0;	/* no warnings emitted yet */
  jerr.msg_code = 0;		/* may be useful as a flag for "no error" */

  /* Initialize message table pointers */
  jerr.jpeg_message_table = NULL;
  jerr.last_jpeg_message = (int) JMSG_LASTMSGCODE - 1;

  jerr.addon_message_table = NULL;
  jerr.first_addon_message = 0;	/* for safety */
  jerr.last_addon_message = 0;


  cinfo.err = &jerr;
  jerr.error_exit = my_error_exit;

  jpeg_create_decompress(&cinfo);

  //jpeg_stdio_src(&cinfo, infile);
  jpeg_mem_src(&cinfo, (void*)buffer, length);

  (void)jpeg_read_header(&cinfo, 1);
  (void)jpeg_start_decompress(&cinfo);

  *image_width = cinfo.output_width;
  *image_height = cinfo.output_height;

  *image_buffer = (unsigned char*)malloc(*image_width * *image_height * 3 * sizeof(unsigned char));
  if(*image_buffer == NULL)
  {
    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    is.close();
    return -3;
  }

  jpeg_buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, cinfo.output_width * cinfo.output_components, 1);

  i_image = 0;
  while(cinfo.output_scanline < cinfo.output_height)
  {
    (void) jpeg_read_scanlines(&cinfo, jpeg_buffer, 1);
    if(cinfo.output_components == 3)
    {
      for(i_buffer = 0; i_buffer < cinfo.output_width * 3; i_buffer++, i_image++)
      {
        (unsigned char)((*image_buffer)[i_image]) = jpeg_buffer[0][i_buffer];
      }
    }
  }

  (void) jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  is.close();

  delete[] buffer;

  return 0;
}


int writeJPEG(const char* filename, int quality, unsigned char *image_buffer, int image_width, int image_height)
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;

  jerr.emit_message = emit_message;// emit_message;
  jerr.output_message = NULL;//output_message;
  jerr.format_message = NULL;//format_message;
  jerr.reset_error_mgr = reset_err_mgr;//reset_error_mgr;

  jerr.trace_level = 0;		/* default = no tracing */
  jerr.num_warnings = 0;	/* no warnings emitted yet */
  jerr.msg_code = 0;		/* may be useful as a flag for "no error" */

  /* Initialize message table pointers */
  jerr.jpeg_message_table = NULL;
  jerr.last_jpeg_message = (int) JMSG_LASTMSGCODE - 1;

  jerr.addon_message_table = NULL;
  jerr.first_addon_message = 0;	/* for safety */
  jerr.last_addon_message = 0;


  cinfo.err = &jerr;

  JSAMPROW row_pointer[1];

  FILE* outfile;

  if((outfile = fopen(filename, "wb")) == NULL)
  {
    char buf[500];
    sprintf(buf, "JPEG :\nCan't open %s\n", filename);
    //MessageBox(GetActiveWindow(), buf, "JPEG Error", MB_OK|MB_ICONERROR);
    return -1;
  }

  if(quality < 0)
  {
    quality = 0;
  }
  else if(quality > 100)
  {
    quality = 100;
  }

  //cinfo.err = jpeg_std_error(&jerr);

  jpeg_create_compress(&cinfo);

  jpeg_stdio_dest(&cinfo, outfile);
  cinfo.image_width = image_width;
  cinfo.image_height = image_height;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, false);
  jpeg_start_compress(&cinfo, true);

  while(cinfo.next_scanline < cinfo.image_height)
  {
    row_pointer[0] = &image_buffer[cinfo.next_scanline * image_width * cinfo.input_components];
    (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  jpeg_finish_compress(&cinfo);
  fclose(outfile);
  jpeg_destroy_compress(&cinfo);
  
  return 0;
}


int resizeJPEG(const char* src, const char* dest, int maxWidth, int maxHeight)
{
  int rc;
  unsigned char *big_pic, *small_pic;
  int big_pic_w, big_pic_h, small_pic_w, small_pic_h;

  int i, j, k, ib, jb;
  double ratio;

  rc = readJPEG(src, &big_pic, &big_pic_w, &big_pic_h);
  if(rc != 0)
  {
    return rc;
  }

  if(big_pic_w > big_pic_h)
  {
    if(big_pic_w >= maxWidth)
    {
      ratio = maxWidth / (double)big_pic_w;
      small_pic_w = maxWidth;
      small_pic_h = (int)((double)big_pic_h *ratio);
    }
    else
    {
      small_pic_w = big_pic_w;
      small_pic_h = big_pic_h;
      ratio = 1.0f;
    }
  }
  else
  {
    if(big_pic_h >= maxHeight)
    {
      ratio = maxHeight / (double)big_pic_h;
      small_pic_h = maxHeight;
      small_pic_w = (int)((double)big_pic_w * ratio);
    }
    else
    {
      small_pic_w = big_pic_w;
      small_pic_h = big_pic_h;
      ratio = 1.0f;
    }
  }

  small_pic = (unsigned char *)malloc(small_pic_w * small_pic_h * 3 * sizeof(unsigned char));

  for(j = 0; j < small_pic_h; j++)
  {
    jb = (int)((double) j / ratio);
    for(i = 0; i < small_pic_w; i++)
    {
      ib = (int)((double) i / ratio);
      for(k = 0; k < 3; k++)
      {
        small_pic[(j * small_pic_w + i) * 3 + k] = big_pic[(jb * big_pic_w + ib) * 3 + k];
      }
    }
  }

  free(big_pic);

  rc = writeJPEG(dest, 95, small_pic, small_pic_w, small_pic_h);

  if(rc != 0)
  {
    return rc;
  }

  free(small_pic);

  return 0;
}

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
  resizeJPEG(src, dest, maxWidth, maxHeight);
  return true;
}

bool cnsSaveImageBufferToGallery_platform(const char* appname, int* buffer, int width, int height)
{
  /* not implemented */
  return true;
}
