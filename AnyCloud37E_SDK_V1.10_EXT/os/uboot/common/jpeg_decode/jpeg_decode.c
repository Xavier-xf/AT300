
#include "jinclude.h"
#include "jpeglib.h"

int jpeg_decode(const char *jpeg_buffer, int jpeg_size, char *frame_buffer)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    int ret = 0;
    int row_stride = 0;
    unsigned char *line_buffer = NULL;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, jpeg_buffer, jpeg_size);

    ret = jpeg_read_header(&cinfo, TRUE);
    if (ret != 1)
    {
        ret = -1;
        printf("[%s:%d] read head failed\n", __func__, __LINE__);
        goto finish;
    }
    cinfo.out_color_space = JCS_RGB;
    jpeg_start_decompress(&cinfo);
    
    unsigned char *buffer_array[1];
    while (cinfo.output_scanline < cinfo.output_height)
    {

        buffer_array[0] = &frame_buffer[cinfo.output_scanline * cinfo.output_width * cinfo.output_components];
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }
finish:
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return ret;
}