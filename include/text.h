#ifndef TEXT_H
#define TEXT_H

#include <freetype2/ft2build.h>
#include <freetype2/freetype.h>
#include "texture.h"
#include "shader_program.h"

typedef struct {
  Texture t;        // texture object
  int w;      // width of texture in pixels
  int h;      // height of texture in pixels

  struct {
    float ax;  // advance.x
    float ay;  // advance.y
    float bw;  // bitmap.width;
    float bh;  // bitmap.height;
    float bt;  // bitmap_top;
    float bl;  // bitmap_left;
    float tx;  // x offset of glyph in texture coordinates
    float ty;  // y offset of glyph in texture coordinates
  } c[128];    // character information

} FontAtlas;

typedef struct {
  FT_Library ft;
  FT_Face face;
} Font;

typedef struct {
  ShaderProgram sp; // Font shader
  GLuint vao, vbo;
  GLint attribute_coord;
} FontRenderer;

int load_font(Font* f, char* filename);

void create_font_renderer(FontRenderer* r);
void delete_font_renderer(FontRenderer* r);

void set_font_colour(FontRenderer*, unsigned int r, unsigned int g, unsigned int b,
                                    unsigned int a);

void create_font_atlas(FontAtlas* fa, Font* f, unsigned int height);
void delete_font_atlas(FontAtlas* fa);

void render_text(FontRenderer* r, FontAtlas* a, const char *text,
                 float x, float y, float sx, float sy);

#endif
