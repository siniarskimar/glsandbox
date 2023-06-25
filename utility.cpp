#include "utility.h"
#include <iostream>

void glClearError() {
  while (glGetError()) {
  }
}

void glPrintErrors() {
  while (auto err = glGetError()) {
    std::cout << "GL_ERROR: " << err << std::endl;
  }
}
void printShaderInfoLog(GLuint shader, GLenum shaderKind) {
  const char *shaderName = nullptr;
  switch (shaderKind) {
  case GL_VERTEX_SHADER:
    shaderName = "Vertex";
    break;
  case GL_FRAGMENT_SHADER:
    shaderName = "Fragment";
    break;
  default:
    shaderName = "";
    break;
  }
  int infoLogLen = 0;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
  char *infoLog = new char[infoLogLen + 1];
  glGetShaderInfoLog(shader, infoLogLen, nullptr, infoLog);
  infoLog[infoLogLen] = '\0';

  std::cerr << shaderName << " shader compilation failed: \n"
            << infoLog << '\n';

  delete[] infoLog;
}

GLuint compileProgram(const std::string_view vertexSource,
                      const std::string_view fragmentSource) {
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

  if (vertexShader == 0) {
    std::cerr << "glCreateShader returned 0 for vertexShader" << std::endl;
    return 0;
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  if (fragmentShader == 0) {
    std::cerr << "glCreateShader returned 0 for fragmentShader" << std::endl;
    glDeleteShader(vertexShader);
    return 0;
  }

  const GLchar *vertexSourceArr = {vertexSource.data()};
  const GLchar *fragmentSourceArr = {fragmentSource.data()};

  glShaderSource(vertexShader, 1, &vertexSourceArr, nullptr);
  glShaderSource(fragmentShader, 1, &fragmentSourceArr, nullptr);

  GLint vertexCompileStatus = GL_TRUE;
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexCompileStatus);
  if (vertexCompileStatus == GL_FALSE) {
    printShaderInfoLog(vertexShader, GL_VERTEX_SHADER);
    glDeleteShader(vertexShader);
  }

  GLint fragmentCompileStatus = GL_TRUE;
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentCompileStatus);
  if (fragmentCompileStatus == GL_FALSE) {
    printShaderInfoLog(fragmentShader, GL_FRAGMENT_SHADER);
    glDeleteShader(fragmentShader);
  }

  if (fragmentCompileStatus == GL_FALSE || vertexCompileStatus == GL_FALSE) {
    return 0;
  }

  GLuint program = glCreateProgram();
  if (program == 0) {
    std::cerr << "glCreateProgram returned 0" << std::endl;
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return 0;
  }
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);
  GLint linkStatus = GL_TRUE;
  glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
  if (linkStatus == GL_FALSE) {
    int infoLogLen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
    char *infoLog = new char[infoLogLen + 1];
    glGetProgramInfoLog(program, infoLogLen, nullptr, infoLog);

    std::cerr << "Program link step failed: \n" << infoLog << std::endl;
    delete[] infoLog;
    return 0;
  }
  glDetachShader(program, vertexShader);
  glDetachShader(program, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  return program;
}
