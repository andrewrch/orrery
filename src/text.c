#include "text.h"
#include "GL/glew.h"
#include <stdio.h>
#include <string.h>
#include "shader_program.h"

#define max(a,b) (a>b?a:b)
#define MAXWIDTH 1024

typedef struct {
  FT_Library ft;
  FT_Face face;
} Font;

void init_atlas_texture(FontAtlas* fa) {
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &fa->t.id);
  bind_texture(&fa->t);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, fa->w, fa->h,
               0, GL_RED, GL_UNSIGNED_BYTE, 0);
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

void delete_font(Font* f) {
  FT_Done_Face(f->face);
  FT_Done_FreeType(f->ft);
}

void create_font_atlas(FontAtlas* fa, char* filename, unsigned int height) {
  Font f;
  if (!load_font(&f, filename)) {
    fprintf(stderr, "Couldn't load font from file: %s\n", filename);
  }
  FT_Set_Pixel_Sizes(f.face, 0, height);
  FT_GlyphSlot g = f.face->glyph;
  int roww = 0;
  int rowh = 0;
  fa->w = 0;
  fa->h = 0;
  memset(fa->c, 0, sizeof fa->c);
  /* Find minimum size for a texture holding all visible ASCII characters */
  for (int i = 32; i < 128; i++) {
    if (FT_Load_Char(f.face, i, FT_LOAD_RENDER)) {
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
  copy_glyphs_to_texture(fa, &f);
  // Clean up
  delete_font(&f);
}

void delete_font_atlas(FontAtlas* fa) {
  delete_texture(&fa->t);
}

void vec3_assign(vec3 v, float a, float b, float c) {
  v[0] = a;
  v[1] = b;
  v[2] = c;
}
void vec2_assign(vec2 v, float a, float b) {
  v[0] = a;
  v[1] = b;
}

typedef struct {
  vec3 pos;
  vec2 tex;
} BufferData;

void render_text(FontRenderer* r, FontAtlas* a, const char *text,
                 vec4 coords, float sx, float sy) {
  bind_program(&r->sp);
  const uint8_t *p;
  /* Use the texture containing the atlas */
  bind_texture(&a->t);
  add_int_uniform(&r->sp, "tex", 0);

  glBindVertexArray(r->vao);
  BufferData b[6* strlen(text)];
  int c = 0;
  float x = coords[0], y = coords[1], z = coords[2];
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

    float tx0, tx1, ty0, ty1, px0, px1, py0, py1;
    // Clockwise winding with positions named like this
    //  x0,y1 o---o x1,y1
    //        |  /|
    //        | / |
    //        |/  |
    //  x0,y0 o---o x1,y0

    px0 = x2;
    px1 = x2 + w;
    py0 = -y2 - h;
    py1 = -y2;

    tx0 = a->c[*p].tx;
    tx1 = a->c[*p].tx + a->c[*p].bw / a->w,
    ty0 = a->c[*p].ty + a->c[*p].bh / a->h;
    ty1 = a->c[*p].ty;

    vec3_assign(b[c].pos,   px0, py0, z);
    vec2_assign(b[c++].tex, tx0, ty0);
    vec3_assign(b[c].pos,   px1, py1, z);
    vec2_assign(b[c++].tex, tx1, ty1);
    vec3_assign(b[c].pos,   px0, py1, z);
    vec2_assign(b[c++].tex, tx0, ty1);
    // Second triangle
    vec3_assign(b[c].pos,   px1, py1, z);
    vec2_assign(b[c++].tex, tx1, ty1);
    vec3_assign(b[c].pos,   px0, py0, z);
    vec2_assign(b[c++].tex, tx0, ty0);
    vec3_assign(b[c].pos,   px1, py0, z);
    vec2_assign(b[c++].tex, tx1, ty0);
  }
  /* Draw all the character on the screen in one go */
  glBufferData(GL_ARRAY_BUFFER, sizeof(b), b, GL_DYNAMIC_DRAW);
  glDrawArrays(GL_TRIANGLES, 0, c);
  glBindVertexArray(0);
}

void create_font_renderer(FontRenderer* r) {
  // First create shader program
  create_program_from_files(&r->sp, 2, "../shaders/text.glslv",
                                       "../shaders/text.glslf");
  // Create the vertex buffer object
  glGenVertexArrays(1, &r->vao);
  glBindVertexArray(r->vao);
  glGenBuffers(1, &r->vbo);
  // Set up attribute array
  glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
  unsigned int pos_attrib = get_attrib_location(&r->sp, "position");
  glEnableVertexAttribArray(pos_attrib);
  glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE,
                        sizeof(BufferData), 0);
  unsigned int tex_attrib = get_attrib_location(&r->sp, "tex_coord");
  glEnableVertexAttribArray(tex_attrib);
  glVertexAttribPointer(tex_attrib, 2, GL_FLOAT, GL_FALSE,
                        sizeof(BufferData), NULL+sizeof(vec3));
  glBindVertexArray(0);
}

void delete_font_renderer(FontRenderer* r) {
  delete_program(&r->sp);
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
