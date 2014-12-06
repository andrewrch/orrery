#ifndef MESH_H
#define MESH_H

#include "linear_algebra.h"

enum attrib_location {
  POSITION_LOCATION = 0,
  COLOUR_LOCATION,
  TEX_LOCATION,
  NORMAL_LOCATION,
  NUM_ATTRIBUTES
};

unsigned int get_attrib_size(unsigned int attrib);
unsigned int get_attrib_offset(unsigned int attrib);

// For colours
typedef char ucvec4[4];

typedef struct vertex {
  vec3 position;
  ucvec4 colour;
  vec2 tex_coords;
  vec3 normal;
} Vertex;

typedef struct mesh {
  struct vertex* vertices;
  unsigned int* indices;
  // Required for buffering
  unsigned int num_indices, num_vertices;
} Mesh;

#endif