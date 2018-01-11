#include <GL/glew.h>
#include "../include/glfw_util.h"
#include "../include/macro_util.h"
#include <iostream>


void GLFWU::setupGLFW(GLFWInfoBlock *glfwInfo,std::string title,int width,int height){
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()){
        throw "glfwInit(...) failed";
    }

    //I will try to use opengl 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); OPENGL_ERROR();
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);OPENGL_ERROR();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);OPENGL_ERROR();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);OPENGL_ERROR();
    glfwWindowHint(GLFW_CONTEXT_REVISION, 0);OPENGL_ERROR();
    glfwWindowHint(GLFW_RED_BITS  , 8);OPENGL_ERROR();
    glfwWindowHint(GLFW_GREEN_BITS, 8);OPENGL_ERROR();
    glfwWindowHint(GLFW_BLUE_BITS , 8);OPENGL_ERROR();
    glfwWindowHint(GLFW_ALPHA_BITS, 8);OPENGL_ERROR();
    glfwWindowHint(GLFW_DEPTH_BITS,24);OPENGL_ERROR();

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width,height,title.c_str(), NULL, NULL);OPENGL_ERROR();

    if (!window){
        glfwTerminate();
        throw "glfwCreateWindow(...) failed";
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);OPENGL_ERROR();
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();OPENGL_ERROR();

    if (GLEW_OK != err){
        /* Problem: glewInit failed, something is seriously wrong. */
        throw glewGetErrorString(err);
    }

    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    // Display OpenGL version
    int major, minor, rev;//client, forward, profile;
    glfwGetVersion(&major, &minor, &rev);
    std::cout << "OpenGL - " << major << "." << minor << "." << rev << std::endl;


    GLFWWindowInfoBlock wib = {window,float(width),float(height),0.0f,0.0f};
    glfwInfo->windowBlocks.enqueue( new SingleNode<GLFWWindowInfoBlock>(wib) );

    glfwSetKeyCallback(window,key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    for(bool & e: glfwGlobalInfo.keyBuffer){
        e = false;
    }
    for(bool & e: glfwGlobalInfo.mouseBuffer){
        e = false;
    }
}

void GLFWU::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
    if(action == GLFW_PRESS){
        glfwGlobalInfo.keyBuffer[key] = true;
    }
    if(action == GLFW_RELEASE){
        glfwGlobalInfo.keyBuffer[key] = false;
    }
}

void GLFWU::cursor_pos_callback(GLFWwindow *window, double xpos, double ypos){
    static double xp = 0.0f,yp = 0.0f;
    double dx,dy;
    Vec2 & mouseDisp = glfwGlobalInfo.mouseDisp;
    Vec2 & mousePos = glfwGlobalInfo.mousePos;

    mousePos.x = xpos;
    mousePos.y = ypos;

    dx = xpos - xp;
    dy = ypos - yp;

    if( (dx*dx+dy*dy) > 100.0){
        dx = 0.0f;
        dy = 0.0f;
    }

    mouseDisp.x = dx;
    mouseDisp.y = dy;
    xp = xpos;
    yp = ypos;
}

void GLFWU::mouse_button_callback(GLFWwindow *window, int button, int action, int mods){
    if(action == GLFW_PRESS){
        glfwGlobalInfo.mouseBuffer[button] = true;
    }
    if(action == GLFW_RELEASE){
        glfwGlobalInfo.mouseBuffer[button] = false;
    }
}

void GLFWU::window_size_callback(GLFWwindow *window, int width, int height){
    glViewport(0,0,width,height);
}

void GLFWU::resolveEvents(MatrixStack & matStack){
    /*simple camera code below.
     * This code enables users to:
     *-look around with trackpad or mouse
     *-change position of camera with with WASD keys
     */
    const float CAM_DAMP_Y = -0.01f;
    const float CAM_DAMP_X = -0.01f;

    CMAT44 rotMat;
    CMAT44 & basisMatrix = matStack.getCameraBasis();
    CMAT44 & viewMatrix  = matStack.getViewMat();
    CVec4 & R = basisMatrix.getRowVector(2);

//    //rotate R about the x axis by mouse Y
//    rotMat.setNRotation(viewMatrix.getRowVector(0),glfwGlobalInfo.mouseDisp.y*CAM_DAMP_Y);
//    R = rotMat*R;

//    //rotate R about the y axis by mouse X
//    rotMat.setNRotation(viewMatrix.getRowVector(1),glfwGlobalInfo.mouseDisp.x*CAM_DAMP_X);
//    R = rotMat*R;

    //recompute a new viewtransform from camera basis
    basisMatrix.orthonormalize({0,1,0,0});
    viewMatrix.setViewTransform(&basisMatrix);

    if(glfwGlobalInfo.keyBuffer[GLFW_KEY_S])
       basisMatrix.getRowVector(3)+=basisMatrix.getRowVector(2);
    if(glfwGlobalInfo.keyBuffer[GLFW_KEY_W])
       basisMatrix.getRowVector(3)-=basisMatrix.getRowVector(2);
    if(glfwGlobalInfo.keyBuffer[GLFW_KEY_D])
       basisMatrix.getRowVector(3)+=basisMatrix.getRowVector(0);
    if(glfwGlobalInfo.keyBuffer[GLFW_KEY_A])
       basisMatrix.getRowVector(3)-=basisMatrix.getRowVector(0);

    if(glfwGlobalInfo.keyBuffer[GLFW_KEY_UP])
       basisMatrix.getRowVector(3)+=basisMatrix.getRowVector(1);
    if(glfwGlobalInfo.keyBuffer[GLFW_KEY_DOWN])
       basisMatrix.getRowVector(3)-=basisMatrix.getRowVector(1);

    matStack.updateModelView();



    glfwGlobalInfo.mouseDisp = {0.0f,0.0f};

}
