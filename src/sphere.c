#include <stdlib.h>
#include "linear_algebra.h"
#include "sphere.h"

unsigned int calc_num_indices(unsigned int rings,
                              unsigned int slices)
{
  return (rings - 1) * (slices - 1) * 6;
}

unsigned int calc_num_vertices(unsigned int rings,
                               unsigned int slices)
{
  return rings * slices;
}

void create_sphere(Mesh* mesh,
                   float radius,
                   unsigned int slices,
                   unsigned int rings)
{
  unsigned int num_vertices = calc_num_vertices(rings, slices);
  unsigned int num_indices = calc_num_indices(rings, slices);

  mesh->vertices = (Vertex*) malloc(num_vertices * sizeof(Vertex));
  mesh->indices = (unsigned int*) malloc(num_indices * sizeof(unsigned int));
  mesh->num_vertices = num_vertices;
  mesh->num_indices = num_indices;

  float const R = 1.0f / (float)(rings - 1);
  float const S = 1.0f / (float)(slices - 1);

  for (unsigned int r = 0; r < rings; r++)
  {
    for (unsigned int s = 0; s < slices; s++)
    {
      float y = sin( -M_PI_2 + M_PI * r * R );
      float x = cos(2 * M_PI * s * S) * sin( M_PI * r * R );
      float z = sin(2 * M_PI * s * S) * sin( M_PI * r * R );

      // Vertex index
      unsigned int index = r * rings + s;
      // Assign vertex position
      float *p = mesh->vertices[index].position;
      *p++ = x * radius;
      *p++ = y * radius;
      *p++ = z * radius;
      // Assign vertex texcoord
      float *t = mesh->vertices[index].tex_coords;
      *t++ = s * S;
      *t++ = 1 - r * R; // Inverted so textures fit correctly
      // Assign normal
      float *n = mesh->vertices[index].normal;
      *n++ = x;
      *n++ = y;
      *n++ = z;
    }
  }

  unsigned int index = 0;
  for (unsigned int r = 0; r < rings - 1; r++)
  {
    for (unsigned int s = 0; s < slices - 1; s++)
    {
      mesh->indices[index++] = r * slices + s;
      mesh->indices[index++] = r * slices + (s + 1);
      mesh->indices[index++] = (r + 1) * slices + s;
      mesh->indices[index++] = r * slices + (s + 1);
      mesh->indices[index++] = (r + 1) * slices + (s + 1);
      mesh->indices[index++] = (r + 1) * slices + s;
    }
  }
}
