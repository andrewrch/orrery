#include "star.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum important_fields {
  ID = 0,
  BAYER = 27,
  NAME = 6,
  MAG = 13,
  X = 17,
  Y = 18,
  Z = 19,
  COLOUR = 16,
};

/* Borrowed from here:
 *
 * http://stackoverflow.com/questions/21977786/star-b-v-color-index-to-apparent-rgb-color
 */
void calculate_colour_from_index(vec4 c, float bv) {
  float t;
  float *r = &c[0];
  float *g = &c[1];
  float *b = &c[2];
  *r=0.0; *g=0.0; *b=0.0;
  if (bv < -0.4) t=-0.4; if (bv > 2.0) t= 2.0;
       if ((bv>=-0.40)&&(bv<0.00)) { t=(bv+0.40)/(0.00+0.40); *r=0.61+(0.11*t)+(0.1*t*t); }
  else if ((bv>= 0.00)&&(bv<0.40)) { t=(bv-0.00)/(0.40-0.00); *r=0.83+(0.17*t)          ; }
  else if ((bv>= 0.40)&&(bv<2.10)) { t=(bv-0.40)/(2.10-0.40); *r=1.00                   ; }

       if ((bv>=-0.40)&&(bv<0.00)) { t=(bv+0.40)/(0.00+0.40); *g=0.70+(0.07*t)+(0.1*t*t); }
  else if ((bv>= 0.00)&&(bv<0.40)) { t=(bv-0.00)/(0.40-0.00); *g=0.87+(0.11*t)          ; }
  else if ((bv>= 0.40)&&(bv<1.60)) { t=(bv-0.40)/(1.60-0.40); *g=0.98-(0.16*t)          ; }
  else if ((bv>= 1.60)&&(bv<2.00)) { t=(bv-1.60)/(2.00-1.60); *g=0.82         -(0.5*t*t); }

       if ((bv>=-0.40)&&(bv<0.40)) { t=(bv+0.40)/(0.40+0.40); *b=1.00                   ; }
  else if ((bv>= 0.40)&&(bv<1.50)) { t=(bv-0.40)/(1.50-0.40); *b=1.00-(0.47*t)+(0.1*t*t); }
  else if ((bv>= 1.50)&&(bv<1.94)) { t=(bv-1.50)/(1.94-1.50); *b=0.63         -(0.6*t*t); }
}

void read_star_from_line(Star* s, char* line) {
  char* current_c = line;
  unsigned int field = 0;
  float i;
  // Initialise names to empty
  s->proper_name[0] = '\0';
  s->bayer_name[0] = '\0';
  while(*current_c != '\0' && *current_c != '\n') {
    // Check for empty string
    if (*current_c != ',') {
      switch (field) {
        case ID:
          sscanf(current_c, "%d", &s->id);
          break;
        case BAYER:
          sscanf(current_c, "%[^,]", s->bayer_name);
          break;
        case NAME:
          sscanf(current_c, "%[^,]", s->proper_name);
          break;
        case MAG:
          sscanf(current_c, "%f", &s->mag);
          break;
        case X:
          sscanf(current_c, "%f", &s->pos[0]);
          break;
        case Y:
          sscanf(current_c, "%f", &s->pos[1]);
          break;
        case Z:
          sscanf(current_c, "%f", &s->pos[2]);
          break;
        case COLOUR:
          sscanf(current_c, "%f", &i);
          calculate_colour_from_index(s->colour, i);
          break;
        default:
          break;
      }
    }
    // Now iterate to end of field
    while (*current_c != '\0' && *current_c != '\n' && *(current_c++) != ',');
    field++;
  }
}

void read_stars(Star** s, unsigned int* num_stars, char* filename) {
  FILE* f = fopen(filename, "r");
  if (!f) {
    fprintf(stderr, "Could not open star file %s\n", filename);
  }
  // Count number of stars from file
  unsigned int num_lines = 0;
  char line[500];
  while (fgets(line, sizeof(line), f)) {
    num_lines++;
  }
  // Init star array
  *num_stars = num_lines - 1;
  *s = (Star*) malloc(*num_stars * sizeof(Star));
  // back to beginning of file
  fseek(f, 0, SEEK_SET);
  // Read file line by line
  unsigned int i = 0;
  // Throw away first line
  fgets(line, sizeof(line), f);
  while (fgets(line, sizeof(line), f)) {
    read_star_from_line(&((*s)[i++]), line);
  }
  fclose(f);
}

void read_asterium_from_line(Asterium* asterium, char* line){
  char* tok;
  tok = strtok(line, " ");
  strcpy(asterium->name, tok);
  tok = strtok(NULL, " ");
  sscanf(tok, "%d", &asterium->num_connections);
  asterium->connections = (AsteriumConnection*)
    malloc(asterium->num_connections * sizeof(AsteriumConnection));
  for (unsigned int i = 0; i < asterium->num_connections; i++) {
    tok = strtok(NULL, " ");
    sscanf(tok, "%d", &asterium->connections[i].first);
    tok = strtok(NULL, " ");
    sscanf(tok, "%d", &asterium->connections[i].second);
  }
}

void read_asteriums(Asterium** asteriums,
                    unsigned int* num_asteriums, char* filename) {
  FILE* f = fopen(filename, "r");
  if (!f) {
    fprintf(stderr, "Could not open asterium file %s\n", filename);
  }
  // Count number of asteriums from file
  unsigned int num_lines = 0;
  char line[500];
  while (fgets(line, sizeof(line), f)) {
    num_lines++;
  }
  *num_asteriums = num_lines;
  *asteriums = (Asterium*) malloc(*num_asteriums * sizeof(Asterium));
  // back to beginning of file
  fseek(f, 0, SEEK_SET);
  // Read file line by line
  unsigned int i = 0;
  while (fgets(line, sizeof(line), f)) {
    read_asterium_from_line(&((*asteriums)[i++]), line);
  }
  fclose(f);
}
