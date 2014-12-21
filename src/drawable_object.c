#include "drawable_object.h"
#include <stdio.h>

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
  glEnableVertexAttribArray(attrib);
  glVertexAttribPointer(
    attrib,
    get_attrib_size(attrib),
    GL_FLOAT, GL_FALSE,
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
  obj->num_vertices = mesh->num_vertices;
  if (mesh->indices) {
    buffer_indices(mesh->indices,
                   mesh->num_indices,
                   obj->buffers[INDEX_BUFFER]);
    obj->num_indices = mesh->num_indices;
  }
  // Add attributes
  glBindBuffer(GL_ARRAY_BUFFER, obj->buffers[VERTEX_BUFFER]);
  for (unsigned int i = 0; i < NUM_ATTRIBUTES; i++) 
  {
    add_attribute(i);
  }
  glBindVertexArray(0);
  obj->texture = NULL;
}

void delete_object(DrawableObject* obj) {
  glDeleteBuffers(NUM_BUFFERS, obj->buffers);
  glDeleteVertexArrays(1, &obj->VAO);
  delete_texture(obj->texture);
}

void set_texture(DrawableObject* obj, Texture* t) {
  obj->texture = t;
}

void draw_points(DrawableObject* obj)
{
  glBindVertexArray(obj->VAO);
  glDrawArrays(GL_POINTS, 0, obj->num_vertices);
  glBindVertexArray(0);
}

void draw_lines(DrawableObject* obj)
{
  glBindVertexArray(obj->VAO);
  glDrawElements(GL_LINES,
                 obj->num_indices,
                 GL_UNSIGNED_INT,
                 NULL);
  glBindVertexArray(0);
}


void draw_triangles(DrawableObject* obj)
{
  glBindVertexArray(obj->VAO);
  if (obj->texture) {
    printf("binding texture\n");
    glActiveTexture(GL_TEXTURE0);
    bind_texture(obj->texture);
  }
  glDrawElements(GL_TRIANGLES,
                 obj->num_indices,
                 GL_UNSIGNED_INT,
                 NULL);
  glBindVertexArray(0);
}



