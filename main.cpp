#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unistd.h>
#include <cmath>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

#include "glhelper.h"

using namespace std;

GLFWwindow *window;

GLuint overlay_texture;
GLuint overlay_shader;
glm::vec2 overlay_size;

GLuint heightmap_texture;
GLuint heightmap_shader;

int heightmap_width, heightmap_height;
float world_width, world_height;

glm::vec2 coordinates;
float altitude;

glm::vec3 eye;
glm::vec3 fwd = glm::vec3(-1,0,0);
glm::vec3 up = glm::vec3(0,1,0);

float angle = 0;
float pitch = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
		angle += 0.005;
	if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
		angle -= 0.005;

	if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
		pitch += 0.005;
	if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
		pitch -= 0.005;
}

void initialize() {
  ifstream attributes("intermediate/attributes.ini");
  auto parse = [&attributes] (const char* format, auto... params) {
	  string line;
	  getline(attributes, line);
	  sscanf(line.c_str(), format, params...);
  };
  
  glm::vec2 dataset_origin;
  float dataset_size;
  parse("image_size = (%f,%f)", &overlay_size.x, &overlay_size.y);
  parse("heightmap_size = (%d,%d)", &heightmap_width, &heightmap_height);
  parse("world_size = (%f,%f)", &world_width, &world_height);
  parse("coordinates = (%f,%f)", &coordinates.x, &coordinates.y);
  parse("altitude = (%f)", &altitude);
  parse("dataset_origin = (%f,%f)", &dataset_origin.x, &dataset_origin.y);
  parse("dataset_size = %f", &dataset_size);

  glm::vec2 ws = overlay_size;
  if(ws.x > 1024) ws *=  1024 / ws.x;
  ws.x = floor(ws.x);
  ws.y = floor(ws.y);
  
  if (!glfwInit()) exit(-1);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  window = glfwCreateWindow(ws.x, ws.y, "ImageWire", NULL, NULL);
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

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // vertex vertices[4] = {{-1,0,-1}, {1,0,-1}, {-1,0,-1}, {1,0,1}};
  
  // glGenBuffers(1, &heightmap);
  // glBindBuffer(GL_ARRAY_BUFFER, heightmap);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


  glm::vec2 eye2d = (coordinates - dataset_origin) / dataset_size;
  eye = glm::vec3(world_width * eye2d.x, altitude, world_height*eye2d.y);
	
  overlay_shader = loadShaders("overlay.vert", "overlay.frag");
  heightmap_shader = loadShaders("heightmap.vert", "heightmap.frag");

  overlay_texture =
      loadTexture("intermediate/image.raw", overlay_size.x, overlay_size.y);
  heightmap_texture = loadTextureI32toF32("intermediate/heightmap.raw",
                                          heightmap_width, heightmap_height);
}

void render() {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  int error = glGetError();
  if(error)
	  cout << "Got error #" << error << endl;
  
  // Render heightmap
  fwd = glm::vec3(cos(angle)*cos(pitch), sin(pitch), sin(angle)*cos(pitch));
  glm::mat4 viewMatrix = glm::lookAt(eye, eye+fwd, up);
  glm::mat4 projectionMatrix = glm::perspective(
      glm::radians(60.0f), (float)width / (float)height, 1.0f, 1000000.0f);
  glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
  
  glBindTexture(GL_TEXTURE_2D, heightmap_texture);
  glUseProgram(heightmap_shader);
  glUniform2f(0, width, height);
  glUniform1i(1, 0);
  glUniform2ui(2, heightmap_width, heightmap_height);
  glUniform2f(3, world_width, world_height);
  glUniformMatrix4fv(4, 1, GL_FALSE, &viewProjectionMatrix[0][0]);
  glUniform3f(5, eye.x, eye.y, eye.z);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDrawArrays(GL_TRIANGLES, 0,
               (heightmap_width - 1) * (heightmap_height - 1) * 6);

  // Render overlay
  glBindTexture(GL_TEXTURE_2D, overlay_texture);
  glUseProgram(overlay_shader);
  glUniform2f(0, width, height);
  glUniform1i(1, 0);
  glUniform2f(2, overlay_size.x, overlay_size.y);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

int main(int argc, const char **argv) {
  if (argc <= 1) {
    cout << "Must specify image file.\n";
  }

  string python_command = "python3 main.py "s + argv[1];
  system(python_command.c_str());

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
