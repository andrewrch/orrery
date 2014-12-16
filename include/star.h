#ifndef STAR_H
#define STAR_H

#include "linear_algebra.h"

typedef struct {
  unsigned int id;
  char proper_name[20], bayer_name[20];
  vec3 pos;
  vec4 colour;
  float mag; // Star magnitude
} Star;

typedef struct {
  unsigned int first, second;
} AsteriumConnection;

typedef struct {
  char name[4];
  unsigned int num_connections;
  AsteriumConnection* connections;
} Asterium;


void read_stars(Star** s, unsigned int* num_stars, char* filename);
void read_asteriums(Asterium** asteriums,
                    unsigned int* num_asteriums, char* filename);
#endif
