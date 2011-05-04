#ifdef __WXMAC__

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#else

#include <Windows.h>
#define sleep(x) Sleep(x*1000)

#include <glew.h>

#endif