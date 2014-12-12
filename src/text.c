#include "text.h"
#include "GL/glew.h"
#include <stdio.h>
#include <string.h>
#include "shader_program.h"

#define max(a,b) (a>b?a:b)
#define MAXWIDTH 1024

void init_atlas_texture(FontAtlas* fa) {
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &fa->t.id);
  bind_texture(&fa->t);
  GLenum e = glGetError();
  printf("init atlas tex %d %d\n", e, e == GL_INVALID_OPERATION);
  printf("%d %d\n", fa->w, fa->h);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, fa->w, fa->h,
               0, GL_RED, GL_UNSIGNED_BYTE, 0);
  e = glGetError();
  printf("init atlas tex %d %d\n", e, e == GL_INVALID_OPERATION);
  /* We require 1 byte alignment when uploading texture data */
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  /* Clamping to edges is important to prevent artifacts when scaling */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  /* Linear filtering usually looks best for text */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void copy_glyphs_to_texture(FontAtlas* fa, Font* f) {
  /* Paste all glyph bitmaps into the texture, remembering the offset */
  int ox = 0;
  int oy = 0;
  int rowh = 0;
  FT_GlyphSlot g = f->face->glyph;

  for (int i = 32; i < 128; i++) {
    if (FT_Load_Char(f->face, i, FT_LOAD_RENDER)) {
      fprintf(stderr, "Loading character %c failed!\n", i);
      continue;
    }
    if (ox + g->bitmap.width + 1 >= MAXWIDTH) {
      oy += rowh;
      rowh = 0;
      ox = 0;
    }

    glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows,
                    GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
    fa->c[i].ax = g->advance.x >> 6;
    fa->c[i].ay = g->advance.y >> 6;
    fa->c[i].bw = g->bitmap.width;
    fa->c[i].bh = g->bitmap.rows;
    fa->c[i].bl = g->bitmap_left;
    fa->c[i].bt = g->bitmap_top;
    fa->c[i].tx = ox / (float) fa->w;
    fa->c[i].ty = oy / (float) fa->h;

    rowh = max(rowh, g->bitmap.rows);
    ox += g->bitmap.width + 1;
  }
}

int load_font(Font* f, char* filename) {
  if (FT_Init_FreeType(&f->ft)) {
    fprintf(stderr, "Could not init freetype library\n");
    return 0;
  }
  if (FT_New_Face(f->ft, filename, 0, &f->face)) {
    fprintf(stderr, "Could not open font %s\n", filename);
    return 0;
  }
  return 1;
}

void create_font_atlas(FontAtlas* fa, Font* f, unsigned int height) {
  FT_Set_Pixel_Sizes(f->face, 0, height);
  FT_GlyphSlot g = f->face->glyph;
  int roww = 0;
  int rowh = 0;
  fa->w = 0;
  fa->h = 0;
  memset(fa->c, 0, sizeof fa->c);
  /* Find minimum size for a texture holding all visible ASCII characters */
  for (int i = 32; i < 128; i++) {
    if (FT_Load_Char(f->face, i, FT_LOAD_RENDER)) {
      fprintf(stderr, "Loading character %c failed!\n", i);
      continue;
    }
    if (roww + g->bitmap.width + 1 >= MAXWIDTH) {
      fa->w = max(fa->w, roww);
      fa->h += rowh;
      roww = 0;
      rowh = 0;
    }
    roww += g->bitmap.width + 1;
    rowh = max(rowh, g->bitmap.rows);
  }

  fa->w = max(fa->w, roww);
  fa->h += rowh;

  init_atlas_texture(fa);
  copy_glyphs_to_texture(fa, f);
}

void delete_font_atlas(FontAtlas* fa) {
  delete_texture(&fa->t);
}

// This just makes the following funciton a little nicer
typedef struct {
  GLfloat x;
  GLfloat y;
  GLfloat s;
  GLfloat t;
} Point;

// Currently works -1, -1 to 1, 1 -> maybe adjust to 0-1?
void render_text(FontRenderer* r, FontAtlas* a, const char *text,
                 float x, float y, float sx, float sy) {
  bind_program(&r->sp);
  const uint8_t *p;
  /* Use the texture containing the atlas */
  bind_texture(&a->t);
  add_int_uniform(&r->sp, "tex", 0);

  glBindVertexArray(r->vao);

  Point coords[6 * strlen(text)];
  int c = 0;

  /* Loop through all characters */
  for (p = (const uint8_t *)text; *p; p++) {
    /* Calculate the vertex and texture coordinates */
    float x2 = x + a->c[*p].bl * sx;
    float y2 = -y - a->c[*p].bt * sy;
    float w = a->c[*p].bw * sx;
    float h = a->c[*p].bh * sy;

    /* Advance the cursor to the start of the next character */
    x += a->c[*p].ax * sx;
    y += a->c[*p].ay * sy;

    /* Skip glyphs that have no pixels */
    if (!w || !h)
      continue;

    // Clockwise winding
    // First Triangle
    coords[c++] = (Point) {
    x2, -y2 - h, a->c[*p].tx, a->c[*p].ty + a->c[*p].bh / a->h};
    coords[c++] = (Point) {
    x2 + w, -y2, a->c[*p].tx + a->c[*p].bw / a->w, a->c[*p].ty};
    coords[c++] = (Point) {x2, -y2, a->c[*p].tx, a->c[*p].ty};
    // Second triangle
    coords[c++] = (Point) {
    x2 + w, -y2, a->c[*p].tx + a->c[*p].bw / a->w, a->c[*p].ty};
    coords[c++] = (Point) {
    x2, -y2 - h, a->c[*p].tx, a->c[*p].ty + a->c[*p].bh / a->h};
    coords[c++] = (Point) {
    x2 + w, -y2 - h, a->c[*p].tx + a->c[*p].bw / a->w, a->c[*p].ty + a->c[*p].bh / a->h};
  }

  /* Draw all the character on the screen in one go */
  glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);
  glDrawArrays(GL_TRIANGLES, 0, c);
  glBindVertexArray(0);
}

void create_font_renderer(FontRenderer* r) {
  // First create shader program
  Shader frag, vert;
  create_frag_shader(&frag, "../shaders/text.glslf");
  create_vert_shader(&vert, "../shaders/text.glslv");
  create_shader_program(&r->sp, 2, &vert, &frag);

  // Create the vertex buffer object
  glGenVertexArrays(1, &r->vao);
  glBindVertexArray(r->vao);
  glGenBuffers(1, &r->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, r->vbo);

  // Set up attribute array
  unsigned int attribute_coord = get_attrib_location(&r->sp, "coord");
  glEnableVertexAttribArray(attribute_coord);
  glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glBindVertexArray(0);
}

void delete_font_renderer(FontRenderer* r) {
  delete_shader_program(&r->sp);
  glDisableVertexAttribArray(r->attribute_coord);
  glDeleteBuffers(1, &r->vbo);
  glDeleteVertexArrays(1, &r->vao);
}

void set_font_colour(FontRenderer* fr,
                     unsigned int r,
                     unsigned int g,
                     unsigned int b,
                     unsigned int a) {

  bind_program(&fr->sp);
  GLfloat colour[4] = {(float) r / 255, (float) g / 255,
                       (float) b / 255, (float) a / 255};
  add_vec4_uniform(&fr->sp, "color", colour);
}
