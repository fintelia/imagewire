
#pragma once

#include <string>

std::string readTextFile(const char* filename);
void printShaderLog(std::string prefix, GLuint shader);
GLuint loadShaders(std::string vert, std::string frag);
GLuint loadTexture(std::string filename, int width, int height);
