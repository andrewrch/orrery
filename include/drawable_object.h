#ifndef DRAWABLE_OBJECT_H
#define DRAWABLE_OBJECT_H

#include <GL/glew.h>
#include "mesh.h"
#include "texture.h"
#include "shader_program.h"

typedef enum buffer_type
{
  VERTEX_BUFFER = 0,
  INDEX_BUFFER,
  NUM_BUFFERS
} BufferType;

typedef struct
{
  GLuint VAO;
  GLuint buffers[NUM_BUFFERS];
  unsigned int num_indices;
  Texture* texture;
} DrawableObject;

void create_object(DrawableObject* obj, Mesh* mesh);
void set_texture(DrawableObject* obj, Texture* t);
void draw(DrawableObject* obj, ShaderProgram* sp, mat4x4 wvp);

#endif
