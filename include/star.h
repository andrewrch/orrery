#ifndef STAR_H
#define STAR_H

typedef struct {
  unsigned int id;
  char proper_name[20], bayer_name[20];
  float ascen, declin; // Equitorial coordinates
  float r, g, b; // Colour
  float mag; // Star magnitude
} Star;

void read_stars(Star* s, unsigned int* num_stars, char* filename);

#endif
