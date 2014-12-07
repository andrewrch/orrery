#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "drawable_object.h"
#include "shader_program.h"
#include "sphere.h"

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

static void display(GLFWwindow* window,
                    DrawableObject* obj,
                    ShaderProgram* sp) {
  float ratio;
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  ratio = width / (float) height;
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Sort out view and proj
  mat4x4 proj;
  mat4x4_perspective(proj, 45, ratio, 0.1, 10000.f);

  mat4x4 view;
  double t = glfwGetTime();
  vec3 up = {0.0, 1.0, 0.0};
  vec3 pos = {5.0 * sin(t), 5.0 * cos(t), sin(t)};
  vec3 target = {0.0, 0.0, 0.0};
  mat4x4_look_at(view, pos, target, up);

  mat4x4 vp;
  mat4x4_mul(vp, proj, view);

  add_mat4x4_uniform(sp, "WVP", vp);

  bind_program(sp);
  draw(obj);
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
  // Make a sphere
  Mesh m;
  create_sphere(&m, 2.0f, 6, 6);
  DrawableObject obj;
  create_object(&obj, &m);
  Shader vert, frag;
  create_vert_shader(&vert, argv[1]);
  create_frag_shader(&frag, argv[2]);
  ShaderProgram sp;
  create_shader_program(&sp, 2, &vert, &frag);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glClearColor(.0f, .0f, .0f, .0f);
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE  );

  glfwSetTime(0.0);
  while (!glfwWindowShouldClose(window)) {
    display(window, &obj, &sp);
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
