#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "linear_algebra.h"

#include "drawable_object.h"
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
                    DrawableObject* obj) {
  float ratio;
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  ratio = width / (float) height;
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT);
  draw(obj);
  glfwSwapBuffers(window);
  glfwPollEvents();
}

int main() {
  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  window = glfwCreateWindow(640, 480, "Orrery", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);

  if (glewInit() != GLEW_OK) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // Make a sphere
  Mesh m;
  create_sphere(&m, 1.0f, 20, 20);
  for (unsigned int i = 0; i < m.num_vertices; i++)
  {
    float* p = m.vertices[i].position;
    printf("%f ", *p++);
    printf("%f ", *p++);
    printf("%f\n", *p++);
  }
  DrawableObject obj;
  create_object(&obj, &m);

  while (!glfwWindowShouldClose(window)) {
    display(window, &obj);
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
