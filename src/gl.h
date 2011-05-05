#ifdef __WXMAC__

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#elif __WXMSW__

#include <Windows.h>
#define sleep(x) Sleep(x*1000)

#include <glew.h>
#define __GLEWINIT__

#else

#include <GL/glew.h>
#define __GLEWINIT__
#include <unistd.h>

#endif