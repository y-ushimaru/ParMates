/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* header file for autogl_graphics_opengl.c */

#ifndef __AUTOGL_GRAPHICS_OPENGL_INSIDE__
#define __AUTOGL_GRAPHICS_OPENGL_INSIDE__



#include <GL/gl.h>
#include <GL/glu.h>

void AutoGL_INSIDE_RealizeOpenGLWindow (void);

void AutoGL_INSIDE_StartDrawingOpenGLWindow (void);

void AutoGL_INSIDE_EndDrawingOpenGLWindow (void);

GLuint AutoGL_INSIDE_GetOpenGLCharBase (void);



#endif  /* __AUTOGL_GRAPHICS_OPENGL_INSIDE__ */

