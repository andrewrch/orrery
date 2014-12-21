#include "texture.h"
#include <stdio.h>
#include <jpeglib.h>
#include <stdbool.h>

void bind_texture(Texture* t)
{
  glBindTexture(GL_TEXTURE_2D, t->id);
}

void unbind_texture()
{
  glBindTexture(GL_TEXTURE_2D, 0);
}

void bind_texture_cube(Texture* t)
{
  glBindTexture(GL_TEXTURE_CUBE_MAP, t->id);
}

void unbind_texture_cube()
{
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void init_texture(Texture* t) {
  glGenTextures(1, &t->id);
}

bool create_texture_from_file(Texture* t, char* filename)
{
  FILE *file = fopen(filename, "rb");
  if (!file)
  {
    fprintf(stderr, "Failed to open texture %s\n", filename);
    return 0;
  }

  struct jpeg_decompress_struct info;
  struct jpeg_error_mgr err;

  info.err = jpeg_std_error(&err);
  jpeg_create_decompress(&info); //fills info structure
  jpeg_stdio_src(&info, file);
  jpeg_read_header(&info, true);
  jpeg_start_decompress(&info);

  int w = info.output_width;
  int h = info.output_height;
  int num_channels = info.num_components; // 3 = RGB, 4 = RGBA
  unsigned long data_size = w * h * num_channels;

  unsigned char *data = (unsigned char *) malloc(data_size);
  unsigned char* rowptr; // array or pointers
  while (info.output_scanline < info.output_height)
  {
    rowptr = data + info.output_scanline * w * num_channels;
    jpeg_read_scanlines( &info, &rowptr, 1 );
  }
  jpeg_finish_decompress(&info);
  jpeg_destroy_decompress(&info);
  fclose(file);

  init_texture(t);
  bind_texture(t);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
               num_channels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);
  free(data);
  return 1;
}

void delete_texture(Texture* t) {
  glDeleteTextures(1, &t->id);
}


