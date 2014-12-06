#ifndef DRAWABLE_OBJECT_H
#define DRAWABLE_OBJECT_H

#include <GL/glew.h>
#include "mesh.h"
#include "texture.h"

typedef enum buffer_type
{
  VERTEX_BUFFER = 0,
  INDEX_BUFFER,
  NUM_BUFFERS
} BufferType;

typedef struct drawable_object
{
  GLuint VAO;
  GLuint buffers[NUM_BUFFERS];
  unsigned int num_indices;
  Texture* texture;
} DrawableObject;

void create_object(DrawableObject* obj, Mesh* mesh);
void draw(DrawableObject* obj);

#endif
