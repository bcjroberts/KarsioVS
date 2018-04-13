
//
// OpenGL init function heavily based on: https://learnopengl.com/code_viewer.php?code=getting-started/shaders-using-object
//


#include "OpenGL.h"

//Constructor to make the opengl window
OpenGL::OpenGL(int &screenWidth, int &screenHeight, const char *windowName) {
    this->window_width = screenWidth;
    this->window_height = screenHeight;

    if(!setupOpenGL(&window,windowName)){
        std::cout<<"ERROR: Unable to init OpenGL window"<<std::endl;
        exit(0);
    }
    // Cull triangles which normal is not towards the camera.
    // http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/
    glEnable(GL_CULL_FACE);
    //zbuffer enabled thanks to: https://stackoverflow.com/questions/1046029/depth-buffer-in-opengl
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

OpenGL::~OpenGL() {
    //Call the mainRender method which actually sets up the verts to be drawn
    //mainRender();
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glBindTexture(GL_TEXTURE_2D, 0);
    glfwDestroyWindow(window);
    glfwTerminate();
    //std::cout << "Closing program" << std::endl;
}

//setup glfw and optionally glew
bool OpenGL::setupOpenGL(GLFWwindow **window,const char *windowName) {
// initialize the GLFW windowing system
    if (!glfwInit()) {
        std::cout << "ERROR: GLFW failed to initialize, TERMINATING" << std::endl;
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    *window = glfwCreateWindow(window_width, window_height, windowName, glfwGetPrimaryMonitor(), nullptr);
    //*window = glfwCreateWindow(window_width, window_height, windowName, glfwGetPrimaryMonitor(), nullptr);
    //Check that window was properly setup
    if (!*window) {
        std::cout << "Program failed to create GLFW window, TERMINATING" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);

// Initialize GLEW if on windows
#ifdef _WIN32
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK) {
        std::cout << "glew init failed" << std::endl;
        return false;
    }
#endif
    return true;
}

//get the opengl window
GLFWwindow *OpenGL::getWindow() {
    return window;
}
