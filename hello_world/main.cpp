#include "glad/gl.h"

#include <GLFW/glfw3.h>
#include <functional>
#include <glm/glm.hpp>
#include <iostream>
#include <string_view>
#include <vector>

#include "utility.h"

int main() {

  glfwSetErrorCallback([](int errorCode, const char *errorMsg) {
    std::cerr << "GLFW: " << errorMsg << std::endl;
  });
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return 2;
  }
  Defer deferGLFWterminate([]() { glfwTerminate(); });

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

  GLFWwindow *window =
      glfwCreateWindow(800, 600, "glsandobx", nullptr, nullptr);
  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    return 2;
  }
  Defer deferGLFWwindowDestroy([&window]() { glfwDestroyWindow(window); });
  glfwMakeContextCurrent(window);
  {
    int glversion = gladLoadGL(glfwGetProcAddress);
    if (glversion == 0) {
      std::cerr << "Failed to initialize OpenGL context" << std::endl;
      return 2;
    }
    std::cerr << "Loaded OpenGL " << GLAD_VERSION_MAJOR(glversion) << '.'
              << GLAD_VERSION_MINOR(glversion) << std::endl;
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(
        [](GLenum source, GLenum type, GLuint id, GLenum severity,
           GLsizei length, const GLchar *message, const void *) {
          std::cerr << "GL_DEBUG: "
                    << (type == GL_DEBUG_TYPE_ERROR ? "GL_ERROR" : "")
                    << message << std::endl;
        },
        nullptr);
  }
  WindowUserData windowUserData;
  glfwGetWindowSize(window, &windowUserData.width, &windowUserData.height);
  glfwSetWindowUserPointer(window, &windowUserData);

  glfwSwapInterval(1);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  bool shouldClose = false;
  glfwSetWindowSizeCallback(
      window, [](GLFWwindow *window, int width, int height) {
        auto *windowUserData =
            static_cast<WindowUserData *>(glfwGetWindowUserPointer(window));
        if (windowUserData == nullptr) {
          return;
        }
        windowUserData->width = width;
        windowUserData->height = height;
        windowUserData->shouldResizeViewport = true;
      });

  // clang-format off
  GLuint quadProgram = compileProgram(
  R"(
    #version 330 core
    
    layout (location = 0) in vec3 pos;
    layout (location = 1) in vec4 color;
    layout (location = 2) in vec2 texCoord;

    out vec4 fColor;
    out vec2 fTexCoord;
    
    void main(){
      gl_Position = vec4(pos.xyz, 1.0);
      fColor = color;
      fTexCoord = texCoord;
    }
  )",
  R"(
    #version 330 core

    in vec4 fColor;
    in vec2 fTexCoord;

    uniform sampler2D tex;
    uniform bool enableTex;

    out vec4 FragColor;

    void main() {
      FragColor = fColor;
      if(enableTex) {
        FragColor = texture(tex, fTexCoord);
      }
    }
  )");
  // clang-format on
  if (quadProgram == 0) {
    std::cerr << "Quad shader compilation failed!" << std::endl;
    return 2;
  }

  GLuint vao = 0;
  GLuint vbo = 0;
  GLuint ibo = 0;
  GLCall(glGenVertexArrays(1, &vao));
  GLCall(glBindVertexArray(vao));
  GLCall(glGenBuffers(1, &vbo));
  GLCall(glGenBuffers(1, &ibo));

  GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
  GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 1, nullptr,
                      GL_DYNAMIC_DRAW));

  GLCall(glEnableVertexAttribArray(0));
  GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                               (const void *)offsetof(Vertex, pos)));
  GLCall(glEnableVertexAttribArray(1));
  GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                               (const void *)offsetof(Vertex, color)));
  GLCall(glEnableVertexAttribArray(2));
  GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                               (const void *)offsetof(Vertex, texCoord)));

  GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
  GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 1, nullptr,
                      GL_DYNAMIC_DRAW));
  GLCall(glBindVertexArray(0));

  // glEnable(GL_DEPTH_TEST);

  Vertex triangleVertexBuffer[] = {
      {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
      {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
      {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
      {{0.5f, 0.5f, -0.1f}, {1.0f, 0.0f, 0.0f, 1.0f}},
      {{1.0f, -0.5f, -0.1f}, {0.0f, 1.0f, 0.0f, 1.0f}},
      {{-0.0f, -0.5f, -0.1f}, {0.0f, 0.0f, 1.0f, 1.0f}},
  };

  GLuint triangleIndexBuffer[] = {0, 1, 2, 3, 4, 5};

  GLCall(glBindVertexArray(vao));
  while (!shouldClose) {
    glfwPollEvents();
    if (glfwWindowShouldClose(window)) {
      shouldClose = true;
    }
    if (windowUserData.shouldResizeViewport) {
      const auto &windowWidth = windowUserData.width;
      const auto &windowHeight = windowUserData.height;
      GLCall(glViewport(0, 0, windowWidth, windowHeight));
      windowUserData.shouldResizeViewport = false;
    }
    /// ==== DRAW

    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    GLCall(glEnable(GL_DEPTH_TEST));

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertexBuffer),
                        triangleVertexBuffer, GL_DYNAMIC_DRAW));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangleIndexBuffer),
                        triangleIndexBuffer, GL_DYNAMIC_DRAW));
    GLCall(glUseProgram(quadProgram));
    GLCall(glDrawElements(GL_TRIANGLES,
                          sizeof(triangleIndexBuffer) / sizeof(GLuint),
                          GL_UNSIGNED_INT, nullptr));
    /// ==== END DRAW

    glfwSwapBuffers(window);
  }

  return 0;
}
