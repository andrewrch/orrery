#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "drawable_object.h"
#include "shader_program.h"
#include "sphere.h"

#define MAX_BODIES 20
#define NUM_STARS 1000

#define DEG_TO_RAD 0.017453293

static void error_callback(int error, const char* description) {
  fprintf(stderr, "%s: %d\n", description, error);
}

static void key_callback(GLFWwindow* window, int key, int scancode,
                 int action, int mods) {
  if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)
      && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}

// Borrowed from COMP27112 code
typedef struct {
  char           name[20];       /* name */
  float          r, g, b;        /* colour */
  float          orbital_radius; /* distance to parent body (km) */
  float          orbital_tilt;   /* angle of orbit wrt ecliptic (deg) */
  float          orbital_period; /* time taken to orbit (days) */
  float          radius;         /* radius of body (km) */
  float          axis_tilt;      /* tilt of axis wrt body's orbital plane (deg) */
  float          rot_period;     /* body's period of rotation (days) */
  unsigned int   orbits_body;    /* identifier of parent body */
  float          spin;           /* current spin value (deg) */
  float          orbit;          /* current orbit value (deg) */
} BodyProperties;

typedef struct {
  DrawableObject* stars;
  unsigned int num_stars;
  DrawableObject* body;
  unsigned int num_bodies;
  BodyProperties* config;
} SolarSystem;

float my_rand (void)
{
  /* return a random float in the range [0,1] */
  return (float) rand() / RAND_MAX;
}

void draw_starfield (void)
{
}

void read_solar_system(BodyProperties* bodies, unsigned int* num_bodies)
{
  FILE *f;
  f= fopen("sys", "r");
  if (f == NULL) {
     printf("ex2.c: Couldn't open the datafile 'sys'\n");
     printf("To get this file, use the following command:\n");
     printf("  cp /opt/info/courses/COMP27112/ex2/sys .\n");
     exit(0);
  }
  fscanf(f, "%u", num_bodies);
  for (unsigned int i = 0; i < *num_bodies; i++)
  {
    fscanf(f, "%s %f %f %f %f %f %f %f %f %f %d",
       bodies[i].name,
      &bodies[i].r, &bodies[i].g, &bodies[i].b,
      &bodies[i].orbital_radius,
      &bodies[i].orbital_tilt,
      &bodies[i].orbital_period,
      &bodies[i].radius,
      &bodies[i].axis_tilt,
      &bodies[i].rot_period,
      &bodies[i].orbits_body);

    /* Initialise the body's state */
    bodies[i].spin= 0.0;
    bodies[i].orbit= my_rand() * 360.0; /* Start each body's orbit at a
                                          random angle */
    bodies[i].radius*= 1000.0; //[> Magnify the radii to make them visible <]*/
    bodies[i].orbital_radius /= 10.0;
  }
  fclose(f);
}

void generate_stars(DrawableObject* stars, unsigned int num_stars) {
  Mesh s;
  Vertex v[num_stars];
  unsigned int i[num_stars];
  s.vertices = &v[0];
  s.indices = &i[0];
  s.num_indices = num_stars;
  s.num_vertices = num_stars;
  for (unsigned int i = 0; i < num_stars; i++) {
    float *p = s.vertices[i].position;
    // Random 3D star position
    *p++ = 60000000 * my_rand();
    *p++ = 60000000 * my_rand();
    *p++ = 60000000 * my_rand();
  }
  create_object(stars, &s);
}

void calculate_body_world_matrix(mat4x4 world,
                                 BodyProperties* bodies,
                                 unsigned int body,
                                 double t) {
  mat4x4_identity(world);
  mat4x4_scale_aniso(world, world, bodies[body].radius,
                                   bodies[body].radius,
                                   bodies[body].radius);
  mat4x4_rotate(world, world, 0, 1, 0, bodies[body].rot_period * t * DEG_TO_RAD);
  mat4x4_rotate(world, world, 1, 0, 0, bodies[body].axis_tilt);
  float pos_x, pos_z;
  pos_x = bodies[body].orbital_radius * sin(t * DEG_TO_RAD);
  pos_z = bodies[body].orbital_radius * cos(t * DEG_TO_RAD);
  printf("%f %f\n", pos_x, pos_z);
  mat4x4_translate_in_place(world, pos_x / 10000, 0.0, pos_z / 10000);
  //mat4x4_rotate(world, world, 0, 0, 1, bodies[body].orbital_tilt);
}

void calculate_vp_matrix(mat4x4 vp, float ratio) {
  // Sort out view and proj
  mat4x4 proj;
  mat4x4_perspective(proj, 45, ratio, 0.1, 10000000000.f);

  mat4x4 view;
  vec3 up = {0.0, 1.0, 0.0};
  vec3 pos = {500000000.1, 500000000.0, 500000000.0};
  //vec3 pos = {100000.1, 100000.0, 100000.0};
  //vec3 pos = {0.0, 10.0, 0.1};
  vec3 target = {0.0, 0.0, 0.0};
  mat4x4_look_at(view, pos, target, up);

  // Combine in VP matrix
  mat4x4_mul(vp, proj, view);
}

static void display(GLFWwindow* window,
                    SolarSystem* system,
                    ShaderProgram* sp) {
  float ratio;
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  ratio = width / (float) height;
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mat4x4 vp;
  calculate_vp_matrix(vp, ratio);

  //bind_program(sp);
  //draw(system->stars);

  double t = glfwGetTime();
  /*double t = 1;*/
  for (unsigned int i = 0; i < system->num_bodies; i++) {
    mat4x4 world;
    calculate_body_world_matrix(world, system->config, i, t);
    mat4x4 wvp;
    mat4x4_mul(wvp, vp, world);
    bind_program(sp);
    add_mat4x4_uniform(sp, "WVP", wvp);
    draw(system->body);
  }

  glfwSwapBuffers(window);
  glfwPollEvents();
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("Please supply some vertex and frag shader location\n");
    exit(EXIT_FAILURE);
  }
  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  window = glfwCreateWindow(640, 480, "Orrery", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);

  // Compatibility for Intel drivers
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  // Have to do this because of glewExperimental
  glGetError();
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glClearColor(.0f, .0f, .0f, .0f);
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE  );

  // Make a sphere
  Mesh sphere_mesh;
  create_sphere(&sphere_mesh, 2.0f, 10, 10);
  DrawableObject sphere;
  create_object(&sphere, &sphere_mesh);
  DrawableObject stars;
  generate_stars(&stars, 1000);

  // Read the data
  unsigned int num_bodies;
  BodyProperties bodies[MAX_BODIES];
  read_solar_system(bodies, &num_bodies);

  SolarSystem system;
  system.stars = &stars;
  system.num_stars = NUM_STARS;
  system.config = bodies;
  system.body = &sphere;
  system.num_bodies = num_bodies;

  Shader vert, frag;
  create_vert_shader(&vert, argv[1]);
  create_frag_shader(&frag, argv[2]);
  ShaderProgram sp;
  create_shader_program(&sp, 2, &vert, &frag);

  glfwSetTime(0.0);
  while (!glfwWindowShouldClose(window)) {
    display(window, &system, &sp);
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
