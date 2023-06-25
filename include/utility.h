#ifndef UTILITY_H
#define UTILITY_H

#include "glad/gl.h"

#include <functional>
#include <glm/glm.hpp>
#include <string_view>

void glClearError();
void glPrintErrors();

#define GLCall(x)                                                              \
  (glGetError(), x);                                                           \
  glPrintErrors();

struct Vertex {
  glm::vec3 pos;
  glm::vec4 color;
  glm::vec2 texCoord;
};

struct WindowUserData {
  int height;
  int width;
  bool shouldResizeViewport;
};

void printShaderInfoLog(GLuint shader, GLenum shaderKind);

GLuint compileProgram(const std::string_view vertexSource,
                      const std::string_view fragmentSource);

struct Defer {
  Defer(std::function<void()> func) : deffered(func) {}
  ~Defer() { deffered(); }

  std::function<void()> deffered;
};

#endif
