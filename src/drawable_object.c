#include "drawable_object.h"

// I don't like this it's weird
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void buffer_vertices(Vertex* vec,
                     unsigned int len,
                     unsigned int buffer,
                     GLenum buffer_type)
{
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(*vec) * len,
               vec,
               buffer_type);
}

void buffer_vertices_static(Vertex* vec,
                            unsigned int len,
                            unsigned int buffer)
{
  buffer_vertices(vec, len, buffer, GL_STATIC_DRAW);
}

void buffer_indices(unsigned int* indices,
                    unsigned int len,
                    unsigned int buffer)
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(*indices) * len,
               indices,
               GL_STATIC_DRAW);
}

void add_attribute(unsigned int attrib)
{
  // Little bit hacky here might need to remove if I ever use another
  // uchar attribute in my vertex
  GLenum type = attrib == COLOUR_LOCATION ? GL_UNSIGNED_BYTE :
                                            GL_FLOAT;
  GLenum norm = attrib == COLOUR_LOCATION ? GL_TRUE : GL_FALSE;
  glVertexAttribPointer(
    attrib,
    get_attrib_size(attrib),
    type, norm,
    sizeof(Vertex),
    BUFFER_OFFSET(get_attrib_offset(attrib)));
}

void create_object(DrawableObject* obj,
                   Mesh* mesh)
{
  glGenVertexArrays(1, &obj->VAO);
  glBindVertexArray(obj->VAO);

  // Generate and buffer arrays
  glGenBuffers(NUM_BUFFERS, obj->buffers);
  buffer_vertices_static(mesh->vertices,
                         mesh->num_vertices,
                         obj->buffers[VERTEX_BUFFER]);
  buffer_indices(mesh->indices,
                 mesh->num_indices,
                 obj->buffers[INDEX_BUFFER]);
  obj->num_indices = mesh->num_indices;
  glBindBuffer(GL_ARRAY_BUFFER, obj->buffers[VERTEX_BUFFER]);
  // Add attributes
  for (unsigned int i = 0; i < NUM_ATTRIBUTES; i++) 
  {
    add_attribute(i);
  }
  glBindVertexArray(0);
}

void draw(DrawableObject* obj)
{
  glBindVertexArray(obj->VAO);
  if (obj->texture) {
    // Can bind a texture here
  }
  glDrawElements(GL_TRIANGLES,
                 obj->num_indices,
                 GL_UNSIGNED_INT,
                 NULL);
  glBindVertexArray(0);
}



