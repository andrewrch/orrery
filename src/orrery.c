#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "drawable_object.h"
#include "shader_program.h"
#include "sphere.h"
#include "text.h"
#include "camera.h"

#define MAX_BODIES 20
#define NUM_STARS 1000

Camera camera;

static inline double deg_to_rad(double deg) {
  return deg * 0.017453293;
}

static void error_callback(int error, const char* description) {
  fprintf(stderr, "%s: %d\n", description, error);
}

static void key_callback(GLFWwindow* window, int key, int scancode,
                 int action, int mods) {
  if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)
      && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
  const float step = 300.0;
  if (key == GLFW_KEY_W) {
    move_forward(&camera, step);
  }
  if (key == GLFW_KEY_S) {
    move_backwards(&camera, step);
  }
  if (key == GLFW_KEY_A) {
    move_left(&camera, step);
  }
  if (key == GLFW_KEY_D) {
    move_right(&camera, step);
  }
  const float angle = 2;
  if (key == GLFW_KEY_L) {
    rotate_right(&camera, angle);
  }
  if (key == GLFW_KEY_J) {
    rotate_left(&camera, angle);
  }
  if (key == GLFW_KEY_I) {
    rotate_up(&camera, angle);
  }
  if (key == GLFW_KEY_K) {
    rotate_down(&camera, angle);
  }

  if (key == GLFW_KEY_N && (mods & GLFW_MOD_SHIFT)
      && action == GLFW_PRESS) {
    printf("Pressed N\n");
  }
  if (key == GLFW_KEY_N && !(mods & GLFW_MOD_SHIFT) &&
      action == GLFW_PRESS) {
    printf("Pressed n\n");
  }
}

// Borrowed from COMP27112 code
typedef struct {
  char           name[20];       /* name */
  char           tex_location[256];
  float          r, g, b;        /* colour */
  float          orbital_radius; /* distance to parent body (km) */
  float          orbital_tilt;   /* angle of orbit wrt ecliptic (deg) */
  float          orbital_period; /* time taken to orbit (days) */
  float          radius;         /* radius of body (km) */
  float          axis_tilt;      /* tilt of axis wrt body's orbital plane (deg) */
  float          rot_period;     /* body's period of rotation (days) */
  unsigned int   orbits_body;    /* identifier of parent body */
} BodyProperties;

typedef struct {
  FontAtlas* a;
  FontRenderer* r;
} Fonts;

typedef struct {
  // Properties about all the planets/moons
  BodyProperties* config;
  // The stars
  DrawableObject* starfield;
  unsigned int num_stars;
  // The planets
  DrawableObject* body;
  unsigned int num_bodies;
  // A texture for each body
  Texture** body_textures;
  // Fonts
  Fonts* fonts;
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
    fscanf(f, "%s %s %f %f %f %f %f %f %f %f %f %d",
       bodies[i].name,
       bodies[i].tex_location,
      &bodies[i].r, &bodies[i].g, &bodies[i].b,
      &bodies[i].orbital_radius,
      &bodies[i].orbital_tilt,
      &bodies[i].orbital_period,
      &bodies[i].radius,
      &bodies[i].axis_tilt,
      &bodies[i].rot_period,
      &bodies[i].orbits_body);

    bodies[i].radius*= 1000.0;
  }
  fclose(f);
}

void generate_starfield(DrawableObject* starfield, unsigned int num_stars) {
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
  create_object(starfield, &s);
}

void calculate_body_world_matrix(mat4x4 world,
                                 BodyProperties* bodies,
                                 unsigned int body,
                                 double t) {
  BodyProperties* b = &bodies[body];
  BodyProperties* p = &bodies[bodies[body].orbits_body];
  mat4x4 S, R, T;
  float trans_x, trans_z;

  mat4x4_identity(world);
  // First scale to the correct size
  mat4x4_scale(S, b->radius, b->radius, b->radius);
  mat4x4_mul(world, S, world);
  // Rotate to hour in the day
  mat4x4_rotate(R, 0, 1, 0, t / b->rot_period);
  mat4x4_mul(world, R, world);
  // And now rotate for axial tilt
  mat4x4_rotate(R, 1, 0, 0, deg_to_rad(b->axis_tilt));
  mat4x4_mul(world, R, world);
  // If b is a moon
  if (b->orbits_body) {
    trans_x = p->orbital_radius * sin(t / p->orbital_period);
    trans_z = p->orbital_radius * cos(t / p->orbital_period);
    mat4x4_translate(T, trans_x, 0.0, trans_z);
    mat4x4_mul(world, T, world);
    // Rotate by orbital tilt
    mat4x4_rotate(R, 0, 0, 1, deg_to_rad(p->orbital_tilt));
    mat4x4_mul(world, R, world);
  }
  // Move to correct position
  trans_x = b->orbital_radius * sin(t / b->orbital_period);
  trans_z = b->orbital_radius * cos(t / b->orbital_period);
  mat4x4_translate(T, trans_x, 0.0, trans_z);
  mat4x4_mul(world, T, world);
  // Rotate by orbital tilt
  mat4x4_rotate(R, 0, 0, 1, deg_to_rad(b->orbital_tilt));
  mat4x4_mul(world, R, world);
}

void calculate_vp_matrix(mat4x4 vp, int w, int h) {
  // Sort out view and proj
  mat4x4 proj;
  mat4x4_perspective(proj, deg_to_rad(30.0),
                     w/ (float) h, 0.1, 200000.0f);
  mat4x4 view;
  get_view_matrix(&camera, view);
  // Combine in VP matrix
  mat4x4_mul(vp, proj, view);
}

void get_body_screen_coords(vec4 coords, BodyProperties* bodies,
                            unsigned int body, mat4x4 vp,
                            double t) {
  BodyProperties* b = &bodies[body];
  BodyProperties* p = &bodies[bodies[body].orbits_body];
  mat4x4 R, T, world;
  float trans_x, trans_y, trans_z;

  mat4x4_identity(world);
  // If b is a moon
  if (b->orbits_body) {
    trans_x = p->orbital_radius * sin(t / p->orbital_period);
    trans_z = p->orbital_radius * cos(t / p->orbital_period);
    mat4x4_translate(T, trans_x, 0.0, trans_z);
    mat4x4_mul(world, T, world);
    // Rotate by orbital tilt
    mat4x4_rotate(R, 0, 0, 1, deg_to_rad(p->orbital_tilt));
    mat4x4_mul(world, R, world);
  }
  // Move to correct position
  trans_x = b->orbital_radius * sin(t / b->orbital_period);
  trans_z = b->orbital_radius * cos(t / b->orbital_period);
  trans_y = 1.1 * b->radius;
  mat4x4_translate(T, trans_x, trans_y, trans_z);
  mat4x4_mul(world, T, world);
  // Rotate by orbital tilt
  mat4x4_rotate(R, 0, 0, 1, deg_to_rad(b->orbital_tilt));
  mat4x4_mul(world, R, world);
  mat4x4 wvp;
  mat4x4_mul(wvp, vp, world);
  vec4 origin = {0, 0, 0, 1};
  mat4x4_mul_vec4(coords, wvp, origin);
  // Perspective division
  for (int i = 0; i < 4; i++) 
    coords[i] /= coords[3];
}

bool viewable(vec4 p) {
 bool r = true;
 for (int i = 0; i < 4; i++) {
   r = r && p[i] <= 1.0 && p[i] >= -1.0;
 }
 return r;
}

static void display(GLFWwindow* window,
                    SolarSystem* system,
                    ShaderProgram* sp) {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  float sx = 1.0 / width;
  float sy = 1.0 / height;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mat4x4 vp;
  calculate_vp_matrix(vp, width, height);
  double t = glfwGetTime();
  for (unsigned int i = 0; i < system->num_bodies; i++) {
    mat4x4 world;
    calculate_body_world_matrix(world, system->config, i, t);
    mat4x4 wvp;
    mat4x4_mul(wvp, vp, world);
    bind_texture(system->body_textures[i]);
    draw(system->body, sp, wvp);
    vec4 coords;
    get_body_screen_coords(coords, system->config, i, vp, t);
    set_font_colour(system->fonts->r, 255, 255, 255, 255);
    if (viewable(coords)) {
      render_text(system->fonts->r,
                  system->fonts->a,
                  system->config[i].name,
                  coords, sx, sy);
    }
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
  /*glfwWindowHint(GLFW_SAMPLES, 4);*/
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

  // Enable backface culling
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glClearColor(.0f, .0f, .0f, .0f);

  /*glPolygonMode( GL_FRONT_AND_BACK, GL_LINE  );*/

  // Make a sphere
  Mesh sphere_mesh;
  create_sphere(&sphere_mesh, 1.0f, 50, 50);

  DrawableObject sphere;
  create_object(&sphere, &sphere_mesh);

  DrawableObject starfield;
  //generate_starfield(&starfield, 1000);

  // Read the data
  unsigned int num_bodies;
  BodyProperties bodies[MAX_BODIES];
  read_solar_system(bodies, &num_bodies);

  ShaderProgram sp;
  create_program_from_files(&sp, 2, argv[1],
                                    argv[2]);

  Texture* t[num_bodies];
  for (unsigned int i = 0; i < num_bodies; i++) {
    t[i] = malloc(sizeof(Texture));
    create_texture(t[i], bodies[i].tex_location);
  }

  FontRenderer r;
  create_font_renderer(&r);
  FontAtlas a;
  create_font_atlas(&a, "../fonts/FreeSans.ttf", 24);
  Fonts f = { .r = &r, .a = &a};

  SolarSystem system;
  system.starfield = &starfield;
  system.num_stars = NUM_STARS;
  system.config = bodies;
  system.body = &sphere;
  system.num_bodies = num_bodies;
  system.body_textures = t;
  system.fonts = &f;

  // Set up camera
  vec3 pos = {20000.0, 0.0, 0.0};
  vec3 up = {0.0, 1.0, 0.0};
  init_camera(&camera, pos, up);

  glfwSetTime(0.0);
  int run = 1;
  /*while (run) {*/
  while (!glfwWindowShouldClose(window)) {
    display(window, &system, &sp);
    run = 0;
  }

  delete_program(&sp);
  delete_mesh(&sphere_mesh);
  delete_object(&sphere);
  for (unsigned int i = 0; i < num_bodies; i++) {
    delete_texture(t[i]);
    free(t[i]);
  }
  delete_font_atlas(&a);
  delete_font_renderer(&r);

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
