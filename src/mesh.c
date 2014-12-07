#include "mesh.h"

unsigned int get_attrib_size(unsigned int attrib) {
  unsigned int s;
  switch (attrib) {
    case 0: s = 3;
            break;
    case 1: s = 4;
            break;
    case 2: s = 2;
            break;
    case 3: s = 3;
            break;
    default: s = 0;
  }
  return s;
}

unsigned int get_attrib_offset(unsigned int attrib) {
  unsigned int o;
  switch (attrib) {
    case 0: o = 0;
            break;
    case 1: o = sizeof(vec3);
            break;
    case 2: o = sizeof(ucvec4) + get_attrib_offset(1);
            break;
    case 3: o = sizeof(vec2) + get_attrib_offset(2);
            break;
    default: o = 0;
  }
  return o;
}

