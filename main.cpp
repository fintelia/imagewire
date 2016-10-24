
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include "glhelper.h"

using namespace std;

GLFWwindow *window;

GLuint heightmap;
GLuint overlay_texture;

GLuint overlay_shader;
GLuint heightmap_shader;

struct vertex {
  float x, y, z;
};


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void initialize() {
  if (!glfwInit()) exit(-1);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  window = glfwCreateWindow(640, 480, "ImageWire", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(-1);
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);
  glfwSwapInterval(1);

  glewInit();
  
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  vertex vertices[4] = {{-1,0,-1}, {1,0,-1}, {-1,0,-1}, {1,0,1}};
  
  glGenBuffers(1, &heightmap);
  glBindBuffer(GL_ARRAY_BUFFER, heightmap);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  string line;
  int overlay_width, overlay_height;
  ifstream attributes("intermediate/attributes.ini");
  getline(attributes, line);
  sscanf(line.c_str(), "image_size = (%d,%d)", &overlay_width, &overlay_height);
    
  overlay_shader = loadShaders("overlay.vert", "overlay.frag");
  heightmap_shader = loadShaders("heightmap.vert", "heightmap.frag");

  overlay_texture =
      loadTexture("intermediate/image.raw", overlay_width, overlay_height);
}

void render() {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  int error = glGetError();
  if(error)
	  cout << "Got error #" << error << endl;
  
  // Render overlay
  glUseProgram(overlay_shader);
  glUniform2f(0, width, height);
  glUniform1i(1, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

int main(int argc, const char **argv) {
  if (argc <= 1) {
    cout << "Must specify image file.\n";
  }

  string python_command = "python3 main.py "s + argv[1];
  //system(python_command.c_str());

  initialize();
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	render();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
