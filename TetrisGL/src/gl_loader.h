#pragma once

// Cross-platform OpenGL loader
// Supports: Desktop OpenGL (Windows/Linux) and OpenGL ES (Raspberry Pi)

#if defined(OPENGL_ES)
    // Raspberry Pi / OpenGL ES 2.0
#include <GLES2/gl2.h>
#define GL_VERSION_3 0
#else
    // Desktop OpenGL (Windows/Linux/macOS)
#define GLEW_NO_GLU
//#include <GL/glew.h> -> //this hshould be moved in the renderer class and switch to glad
#endif

