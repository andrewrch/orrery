#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "drawable_object.h"
#include "shader_program.h"
#include "sphere.h"
#include "star.h"
#include "text.h"
#include "camera.h"

#define MAX_BODIES 20
#define NUM_STARS 1000

typedef struct {
  bool show_constellations;
  bool show_asteriums;
} OrreryOptions;

Camera camera;
OrreryOptions options;

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
  if (action != GLFW_RELEASE) {
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
  if (key == GLFW_KEY_C && action == GLFW_PRESS) {
    options.show_asteriums = !options.show_asteriums;
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
  ShaderProgram* star_shader;
  ShaderProgram* sun_shader;
  ShaderProgram* line_shader;
  // The planets
  DrawableObject* body;
  unsigned int num_bodies;
  ShaderProgram* body_shader;
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

unsigned int num_indices(Asterium* a, unsigned int num_asteriums) {
  unsigned int count = 0;
  for (unsigned int i = 0; i < num_asteriums; i++) {
    count += a[i].num_connections;
  }
  return count;
}

void generate_starfield(DrawableObject* starfield,
                        Star* stars, unsigned int num_stars,
                        Asterium* asteriums, unsigned int num_asteriums) {
  Mesh s;
  Vertex v[num_stars];
  s.vertices = &v[0];
  s.num_vertices = num_stars;
  s.num_indices = 2 * num_indices(asteriums, num_asteriums);
  unsigned int i[s.num_indices];
  s.indices = &i[0];
  unsigned int count = 0;
  for (unsigned int a = 0; a < num_asteriums; a++) {
    for (unsigned int c = 0; c < asteriums[a].num_connections; c++) {
      i[count++] = asteriums[a].connections[c].first;
      i[count++] = asteriums[a].connections[c].second;
    }
  }

  for (unsigned int i = 0; i < num_stars; i++) {
    // Set star colour
    s.vertices[i].colour[0] = stars[i].colour[0];
    s.vertices[i].colour[1] = stars[i].colour[1];
    s.vertices[i].colour[2] = stars[i].colour[2];
    // Stuff mag in as a texcoord
    s.vertices[i].tex_coords[0] = stars[i].mag;
    // Get star positions
    vec3 v;
    vec3_norm(v, stars[i].pos);
    s.vertices[i].position[0] = 100000 * v[0];
    s.vertices[i].position[1] = 100000 * v[1];
    s.vertices[i].position[2] = 100000 * v[2];
  }
  create_object(starfield, &s);
}

void calculate_world_matrices(mat4x4 body_world,
                              mat4x4 text_world,
                              BodyProperties* bodies,
                              unsigned int body,
                              double t) {
  BodyProperties* b = &bodies[body];
  BodyProperties* p = &bodies[bodies[body].orbits_body];
  mat4x4 S, R, T;
  float trans_x, trans_y, trans_z;

  mat4x4_identity(body_world);
  // First scale to the correct size
  mat4x4_scale(S, b->radius, b->radius, b->radius);
  mat4x4_mul(body_world, S, body_world);
  // Rotate to hour in the day
  mat4x4_rotate(R, 0, 1, 0, t / b->rot_period);
  mat4x4_mul(body_world, R, body_world);
  // And now rotate for axial tilt
  mat4x4_rotate(R, 1, 0, 0, deg_to_rad(b->axis_tilt));
  mat4x4_mul(body_world, R, body_world);
  // If b is a moon
  if (b->orbits_body) {
    trans_x = p->orbital_radius * sin(t / p->orbital_period);
    trans_z = p->orbital_radius * cos(t / p->orbital_period);
    mat4x4_translate(T, trans_x, 0.0, trans_z);
    mat4x4_mul(body_world, T, body_world);
    // Rotate by orbital tilt
    mat4x4_rotate(R, 0, 0, 1, deg_to_rad(p->orbital_tilt));
    mat4x4_mul(body_world, R, body_world);
  }
  // Duplicate matrix up to this point
  mat4x4_dup(text_world, body_world);
  // Move body to correct position
  trans_x = b->orbital_radius * sin(t / b->orbital_period);
  trans_z = b->orbital_radius * cos(t / b->orbital_period);
  mat4x4_translate(T, trans_x, 0.0, trans_z);
  mat4x4_mul(body_world, T, body_world);

  // Move text to correct position
  trans_y = 1.1 * b->radius;
  mat4x4_translate(T, trans_x, trans_y, trans_z);
  mat4x4_mul(text_world, T, text_world);
  // Rotate by orbital tilt
  mat4x4_rotate(R, 0, 0, 1, deg_to_rad(b->orbital_tilt));
  mat4x4_mul(body_world, R, body_world);
  mat4x4_mul(text_world, R, text_world);
}

void get_proj_matrix(mat4x4 proj, int w, int h) {
  // Sort out view and proj
  mat4x4_perspective(proj, deg_to_rad(30.0),
                     w/ (float) h, 0.1, 200000.0f);
}

void get_text_screen_coords(vec4 coords, mat4x4 wvp) {
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

void draw_stars(DrawableObject* starfield,
                ShaderProgram* star_shader,
                ShaderProgram* line_shader,
                mat4x4 wvp) {
  bind_program(star_shader);
  add_mat4x4_uniform(star_shader, "WVP", wvp);
  draw_points(starfield);
  if (options.show_asteriums) {
    // Draw asteriums
    bind_program(line_shader);
    add_mat4x4_uniform(line_shader, "WVP", wvp);
    draw_lines(starfield);
  }
}

static void display(GLFWwindow* window,
                    SolarSystem* system) {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  float sx = 2.0 / width;
  float sy = 2.0 / height;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mat4x4 view, proj, vp;
  get_proj_matrix(proj, width, height);
  get_view_matrix(view, &camera);
  mat4x4_mul(vp, proj, view);
  // Draw stars
  draw_stars(system->starfield, system->star_shader,
             system->line_shader, vp);

  double t = 30 * glfwGetTime();
  for (unsigned int i = 0; i < system->num_bodies; i++) {
    mat4x4 body_world, text_world;
    calculate_world_matrices(body_world, text_world,
                             system->config, i, t);
    mat4x4 body_wvp;
    mat4x4_mul(body_wvp, vp, body_world);
    if (i == 0) {
      bind_program(system->sun_shader);
      add_float_uniform(system->body_shader, "time", t);
    } else {
      bind_program(system->body_shader);
    }
    bind_texture(system->body_textures[i]);
    add_int_uniform(system->body_shader, "texture_unit", 0);
    add_mat4x4_uniform(system->body_shader, "WVP", body_wvp);
    // World view matrix
    mat4x4 wv;
    mat4x4_mul(wv, view, body_world);
    add_mat4x4_uniform(system->body_shader, "WV", wv);

    // Normal matrix
    mat4x4 wv_inv, n;
    mat4x4_invert(wv_inv, wv);
    mat4x4_transpose(n, wv_inv);
    add_mat4x4_uniform(system->body_shader, "N", n);

    // Light position
    vec4 sun = {0, 0, 0, 1};
    mat4x4_mul_vec4(sun, view, sun);
    vec3 sun_pos = {sun[0], sun[1], sun[2]};
    add_vec3_uniform(system->body_shader, "sun_pos", sun_pos);

    // Draw body
    draw_triangles(system->body);
    vec4 coords;
    mat4x4 text_wvp;
    mat4x4_mul(text_wvp, vp, text_world);
    get_text_screen_coords(coords, text_wvp);
    set_font_colour(system->fonts->r, 1.0, 1.0, 1.0, 0.5);
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
  if (argc < 1) {
    printf("Please supply star text file\n");
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
  glEnable(GL_PROGRAM_POINT_SIZE);

  // Make a sphere
  Mesh sphere_mesh;
  create_sphere(&sphere_mesh, 1.0f, 50, 50);

  DrawableObject sphere;
  create_object(&sphere, &sphere_mesh);

  // Read the data
  unsigned int num_bodies;
  BodyProperties bodies[MAX_BODIES];
  read_solar_system(bodies, &num_bodies);
  ShaderProgram body_shader, star_shader, line_shader, sun_shader;
  create_program_from_files(&body_shader, 2,
                            "../shaders/body.glslf",
                            "../shaders/body.glslv");
  create_program_from_files(&sun_shader, 2,
                            "../shaders/sun.glslf",
                            "../shaders/sun.glslv");
  create_program_from_files(&star_shader, 2,
                            "../shaders/star.glslf",
                            "../shaders/star.glslv");
  create_program_from_files(&line_shader, 2,
                            "../shaders/lines.glslf",
                            "../shaders/lines.glslv");
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

  // Read stars from file
  Star* stars = NULL;
  unsigned int num_stars;
  read_stars(&stars, &num_stars, argv[1]);
  Asterium* asteriums = NULL;
  unsigned int num_asteriums;
  read_asteriums(&asteriums, &num_asteriums, argv[2]);
  DrawableObject starfield;
  generate_starfield(&starfield, stars, num_stars, asteriums, num_asteriums);


  SolarSystem system;
  system.starfield = &starfield;
  system.num_stars = num_stars;
  system.star_shader = &star_shader;
  system.line_shader = &line_shader;
  system.sun_shader = &sun_shader;
  system.config = bodies;
  system.body = &sphere;
  system.num_bodies = num_bodies;
  system.body_textures = t;
  system.body_shader = &body_shader;
  system.fonts = &f;

  // Set up camera
  vec3 pos = {-20000.0, 0.0, 0.0};
  vec3 up = {0.0, 1.0, 0.0};
  init_camera(&camera, pos, up);

  glfwSetTime(0.0);
  while (!glfwWindowShouldClose(window)) {
    display(window, &system);
  }

  delete_program(&star_shader);
  delete_program(&body_shader);
  delete_mesh(&sphere_mesh);
  delete_object(&sphere);
  for (unsigned int i = 0; i < num_bodies; i++) {
    delete_texture(t[i]);
    free(t[i]);
  }
  delete_font_atlas(&a);
  delete_font_renderer(&r);
  free(stars);

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
