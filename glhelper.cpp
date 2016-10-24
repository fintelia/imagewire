
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <memory>
#include <iostream>

#include "glhelper.h"

using namespace std;

string readTextFile(const char* filename) {
    ifstream fin(filename);
    return string(istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
}

void printShaderLog(string prefix, GLuint shader) {
    int logLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if(logLength <= 1) return;

    unique_ptr<GLchar[]> log(new GLchar[logLength]);
    glGetShaderInfoLog(shader, logLength, nullptr, log.get());
    cout << prefix << log.get() << endl;
}
GLuint loadShaders(string vert, string frag) {
  GLint success = 0;

  GLuint v = glCreateShader(GL_VERTEX_SHADER);
  string vs = readTextFile(vert.c_str());
  const char* vv = vs.c_str();
  glShaderSource(v, 1, &vv, nullptr);
  glCompileShader(v);
  printShaderLog(vert + ": ", v);

  glGetShaderiv(v, GL_COMPILE_STATUS, &success);
  if (success == GL_FALSE) exit(-1);

  GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
  string fs = readTextFile(frag.c_str());
  const char* ff = fs.c_str();
  glShaderSource(f, 1, &ff, nullptr);
  glCompileShader(f);
  printShaderLog(frag + ": ", f);

  glGetShaderiv(f, GL_COMPILE_STATUS, &success);
  if (success == GL_FALSE) exit(-1);

  GLuint program = glCreateProgram();
  glAttachShader(program, v);
  glAttachShader(program, f);

  glLinkProgram(program);
  glUseProgram(program);
  return program;
}

GLuint loadTexture(string filename, int width, int height) {
  string data = readTextFile(filename.c_str());

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, data.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);
  
  return texture;
}
