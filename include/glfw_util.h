#include <GLFW/glfw3.h>
#include "queue.h"
#include "vector.h"
#include "matrix_stack.h"

#ifndef GLFW_UTIL_H
#define GLFW_UTIL_H

typedef struct GLFWWindowInfoBlock_{
    GLFWwindow* window;
    float window_width;
    float window_height;
    float mouse_x;
    float mouse_y;
}GLFWWindowInfoBlock;

typedef struct GLFWInfoBlock_{
    Queue<GLFWWindowInfoBlock> windowBlocks;
    bool mouseBuffer[16];
    bool keyBuffer[512];
    Vec2 mouseDisp;
    Vec2 mousePos;
}GLFWInfoBlock;

namespace GLFWU {
    void setupGLFW(GLFWInfoBlock * glfwInfo, std::string title, int width, int height);
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void window_size_callback(GLFWwindow* window, int width, int height);
    void resolveEvents(MatrixStack &matStack);
}

extern GLFWInfoBlock glfwGlobalInfo;

#endif // GLFW_UTIL_H
