/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_graphics_opengl.c : OpenGL rendering */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
#include <autonosim.h>
#endif

#include "autogl_os.h"
#include "autogl_math.h"
#include "autogl_uims.h"
#include "autogl_uims_inside.h"
#include "autogl_graphics.h"
#include "autogl_graphics_inside.h"

#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
#include "autogl_com_inside.h"
#endif

#include "autogl_callback_inside.h"
#include "autogl_graphics_opengl_inside.h"



#if 0
/* automatic normal calculation */
#define AUTO_NORMAL
#endif



/* Module Variables */

/* background color of the view window */
static double View_backgroundRed;
static double View_backgroundGreen;
static double View_backgroundBlue;

/* ratio of Z direction (depth) against X and Y direction 
   in the viewing coordinate system of the clipping volume */
static const double View_depthRatio = 5.0;

/* size ratio between near plane and center plane */
/* 1.0 < perspectiveRatio */
static const double View_nearPerspectiveRatio = 3.0;
static const double View_farPerspectiveRatio = 50.0;

/* whether transparency is on/off */
static int View_transparencyIsOn;
static double View_opacity;

/* OpenGL list base for character string */
/* 128 OpenGL lists (No. 0 - 127) are reserved for drawing character string. */
static GLuint View_charBase;

/* OpenGL selection buffer and hit record */
#define MAX_SELECT_BUFFER_SIZE 1024
static GLuint View_selectBuffer[MAX_SELECT_BUFFER_SIZE];
static int View_nHits;

typedef enum {
  OPENGL_PRIMITIVE_DEFAULT,
  OPENGL_PRIMITIVE_LINE,
  OPENGL_PRIMITIVE_TRIANGLE,
  OPENGL_PRIMITIVE_NO_LIGHTING
} OpenGLPrimitiveType;

static OpenGLPrimitiveType View_currentPrimitiveType = OPENGL_PRIMITIVE_DEFAULT;
static int View_twoSideFaceFlag = 1;



#ifdef AUTOGL_MP

/* image composition */
static int ImageWidthDc;
static int ImageHeightDc;
static unsigned char ImageColors[MAX_PIXEL * 3];
static unsigned int ImageDepths[MAX_PIXEL];

static void ReadTaskImage (void)
{
  glPixelStorei (GL_PACK_ALIGNMENT, 1);
  glReadPixels (0, 0, ImageWidthDc, ImageHeightDc, 
		GL_RGB, GL_UNSIGNED_BYTE, 
		(GLubyte *)ImageColors);
  glReadPixels (0, 0, ImageWidthDc, ImageHeightDc, 
		GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 
		(GLubyte *)ImageDepths);
}

static void WriteFinalImage (void)
{
  double x, y, z;

  /* clear color and depth buffers */
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  AutoGL_GetPositionOfDc (&x, &y, &z,
			  -ImageWidthDc / 2 + 1, -ImageHeightDc / 2 + 1, 0);
  glRasterPos3d (x, y, z);

  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
  glDrawPixels (ImageWidthDc, ImageHeightDc, 
		GL_RGB, GL_UNSIGNED_BYTE, 
		(GLubyte *)ImageColors);

  glFlush ();
}

static void MakeFinalImage (void)
{
  AutoGL_GetViewRangeDc (&ImageWidthDc, &ImageHeightDc);
  assert(ImageWidthDc <= MAX_IMAGE_WIDTH);
  assert(ImageHeightDc <= MAX_IMAGE_HEIGHT);

  ReadTaskImage ();

  if (AutoGL_INSIDE_IsMaster ()) {
    AutoGL_INSIDE_GatherAndComposeImage 
      (ImageWidthDc, ImageHeightDc,
       ImageColors, ImageDepths);
    WriteFinalImage ();
  } else {
    AutoGL_INSIDE_SendTaskImage 
      (ImageWidthDc, ImageHeightDc,
       ImageColors, ImageDepths);
  }
}

#endif



#ifdef AUTOGL_MMX

/* difference between left and right eyes in stereo viewing */
static const double StereoRatio = 0.01;

/* modify viewing direction for stereo vision */
static void ModifyDirection 
(double *dirX, double *dirY, double *dirZ)
{  
  double upX, upY, upZ;
  double xAxisX, xAxisY, xAxisZ;
      
  AutoGL_GetViewUpVector 
    (&upX, &upY, &upZ);
  AutoGL_GetVectorProduct3D 
    (&xAxisX, &xAxisY, &xAxisZ,
     upX, upY, upZ,
     *dirX, *dirY, *dirZ);
  AutoGL_NormalizeVector3D (&xAxisX, &xAxisY, &xAxisZ,
			    xAxisX, xAxisY, xAxisZ);
  if (AutoNOSim_MyTaskId () % 2 == 0) {
    /* right hand side eye */
    *dirX += xAxisX * StereoRatio;
    *dirY += xAxisY * StereoRatio;
    *dirZ += xAxisZ * StereoRatio;
  } else {   
    /* left hand side eye */
    *dirX -= xAxisX * StereoRatio;
    *dirY -= xAxisY * StereoRatio;
    *dirZ -= xAxisZ * StereoRatio;
  }
}

#endif



static void SetProjection (void)
{
  GLfloat size;
  int widthDc, heightDc;
  GLfloat halfWidth, halfHeight;

  size = AutoGL_GetViewSize ();
  AutoGL_GetViewRangeDc 
    (&widthDc, &heightDc);

  if (widthDc <= heightDc) {
    halfWidth = size;
    halfHeight = size * heightDc / widthDc;
  } else {
    halfWidth = size * widthDc / heightDc;
    halfHeight = size;
  }

  if (AutoGL_GetPerspectiveViewFlag ()) {
    /* set perspective projection */
    glFrustum (-halfWidth / View_nearPerspectiveRatio, 
	       halfWidth / View_nearPerspectiveRatio, 
	       -halfHeight / View_nearPerspectiveRatio, 
	       halfHeight / View_nearPerspectiveRatio, 
	       size * 1.0, 
	       size * View_farPerspectiveRatio);
  } else {
    /* set orthogonal projection */
    glOrtho (-halfWidth, halfWidth, 
	     -halfHeight, halfHeight, 
	     -size * View_depthRatio, size * View_depthRatio);
  }
} 

static void SetViewingTransformation (void)
{
  double centerX, centerY, centerZ;
  double dirX, dirY, dirZ;
  double upX, upY, upZ;
  GLfloat size;

  AutoGL_GetViewCenter 
    (&centerX, &centerY, &centerZ);
  AutoGL_GetViewDirection 
    (&dirX, &dirY, &dirZ);
  AutoGL_GetViewUpVector 
    (&upX, &upY, &upZ);
  size = AutoGL_GetViewSize ();

#ifdef AUTOGL_MMX
  ModifyDirection 
    (&dirX, &dirY, &dirZ);
#endif

  /* set viewing transformation */
  if (AutoGL_GetPerspectiveViewFlag ()) {
    gluLookAt (centerX + dirX * size * View_nearPerspectiveRatio, 
	       centerY + dirY * size * View_nearPerspectiveRatio, 
	       centerZ + dirZ * size * View_nearPerspectiveRatio,
	       centerX, centerY, centerZ,
	       upX, upY, upZ);
  } else {
    gluLookAt (centerX + dirX, 
	       centerY + dirY, 
	       centerZ + dirZ,
	       centerX, centerY, centerZ,
	       upX, upY, upZ);
  }
}

static void SetDefaultPrimitive (void)
{
  if (View_currentPrimitiveType == OPENGL_PRIMITIVE_DEFAULT) {
    /* nothing */
  } else if (View_currentPrimitiveType == OPENGL_PRIMITIVE_LINE) {
    glEnd ();
    glEnable (GL_LIGHTING);
  } else if (View_currentPrimitiveType == OPENGL_PRIMITIVE_TRIANGLE) {
    glEnd ();
  } else if (View_currentPrimitiveType == OPENGL_PRIMITIVE_NO_LIGHTING) {
    glEnable (GL_LIGHTING);
  } else {
    assert(0);
  }

  View_currentPrimitiveType = OPENGL_PRIMITIVE_DEFAULT;
}

static void SetLinePrimitive (void)
{
  if (View_currentPrimitiveType == OPENGL_PRIMITIVE_DEFAULT) {
    glDisable (GL_LIGHTING);
    glBegin (GL_LINES);
  } else if (View_currentPrimitiveType == OPENGL_PRIMITIVE_LINE) {
    /* nothing */
  } else if (View_currentPrimitiveType == OPENGL_PRIMITIVE_TRIANGLE) {
    glEnd ();
    glDisable (GL_LIGHTING);
    glBegin (GL_LINES);
  } else if (View_currentPrimitiveType == OPENGL_PRIMITIVE_NO_LIGHTING) {
    glBegin (GL_LINES);
  } else {
    assert(0);
  }

  View_currentPrimitiveType = OPENGL_PRIMITIVE_LINE;
}

static void SetTrianglePrimitive (void)
{
  if (View_currentPrimitiveType == OPENGL_PRIMITIVE_DEFAULT) {
    glBegin (GL_TRIANGLES);
  } else if (View_currentPrimitiveType == OPENGL_PRIMITIVE_LINE) {
    glEnd ();
    glEnable (GL_LIGHTING);
    glBegin (GL_TRIANGLES);
  } else if (View_currentPrimitiveType == OPENGL_PRIMITIVE_TRIANGLE) {
    /* nothing */
  } else if (View_currentPrimitiveType == OPENGL_PRIMITIVE_NO_LIGHTING) {
    glEnable (GL_LIGHTING);
    glBegin (GL_TRIANGLES);
  } else {
    assert(0);
  }

  View_currentPrimitiveType = OPENGL_PRIMITIVE_TRIANGLE;
}

static void SetNoLightingPrimitive (void)
{
  if (View_currentPrimitiveType == OPENGL_PRIMITIVE_DEFAULT) {
    glDisable (GL_LIGHTING);
  } else if (View_currentPrimitiveType == OPENGL_PRIMITIVE_LINE) {
    glEnd ();
  } else if (View_currentPrimitiveType == OPENGL_PRIMITIVE_TRIANGLE) {
    glEnd ();
    glDisable (GL_LIGHTING);
  } else if (View_currentPrimitiveType == OPENGL_PRIMITIVE_NO_LIGHTING) {
    /* nothing */
  } else {
    assert(0);
  }

  View_currentPrimitiveType = OPENGL_PRIMITIVE_NO_LIGHTING;
}

void AutoGL_SetBackgroundColor 
(double red, double green, double blue)
{
  View_backgroundRed = red;
  View_backgroundGreen = green;
  View_backgroundBlue = blue;
}

void AutoGL_INSIDE_RealizeOpenGLWindow (void)
{
  GLfloat ambientLightColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
  GLfloat diffuseLightColor[] = { 0.8f, 0.8f, 0.8f, 1.0f };
  GLfloat specularLightColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat lightPosition[4];

  /* set background color */
  glClearColor (View_backgroundRed, 
		View_backgroundGreen, 
		View_backgroundBlue, 
		1.0f);
  
  /* set foreground color : white */
  glColor3f (1.0f, 1.0f, 1.0f);
  
  /* turn on depth test */
  glEnable (GL_DEPTH_TEST);
  
  /* turn on face culling */
  glFrontFace (GL_CCW);
  glEnable (GL_CULL_FACE);
  
  /* turn off transparency */
  glDisable (GL_BLEND);
  View_transparencyIsOn = 0;
  View_opacity = 0.5;

  /* turn on lighting */
  glEnable (GL_LIGHTING);
  
  /* set ambient light */
  glLightModelfv (GL_LIGHT_MODEL_AMBIENT, ambientLightColor);
  
  /* turn on material color tracking */
  glEnable (GL_COLOR_MATERIAL);
  
  /* specify ambient and diffuse as tracking color */
  glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  
  /* set color at light No. 0 */
  glLightfv (GL_LIGHT0, GL_DIFFUSE, diffuseLightColor);
  glLightfv (GL_LIGHT0, GL_SPECULAR, specularLightColor);
  
  /* set position at light No. 0 : directional light */
  lightPosition[0] = 0.0f;
  lightPosition[1] = 0.0f;
  lightPosition[2] = 1.0f;
  lightPosition[3] = 0.0f;
  glLightfv (GL_LIGHT0, GL_POSITION, lightPosition);

  /* turn on light No. 0 */
  glEnable (GL_LIGHT0);

  /* allocate lists of bitmaps for OpenGL character string drawing */
  View_charBase = glGenLists (128);

  /* turn on edge contrasting */
  glEnable (GL_POLYGON_OFFSET_FILL);
  glPolygonOffset( 1.0, 1.0 );

  /* set alignment of pixel-level operation as 1 */
  glPixelStorei (GL_PACK_ALIGNMENT, 1);
  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
} 

void AutoGL_INSIDE_StartDrawingOpenGLWindow (void)
{
  static GLfloat noColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
  static GLfloat noShininess[] = { 0.0f };
  
  /* begin rendering OpenGL */
  
  /* turn off transparency and enable Z buffer as writable */
  glDisable (GL_BLEND);
  glDepthMask (GL_TRUE);
  View_transparencyIsOn = 0;
  
  /* turn on lighting */
  glEnable (GL_LIGHTING);
  
  /* turn off specular lighting */
  glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, noColor);
  glMaterialfv (GL_FRONT_AND_BACK, GL_SHININESS, noShininess);
  
  /* turn off texture mapping */
  glDisable (GL_TEXTURE_2D);
  
  /* set OpenGL viewport */
  {
    int widthDc, heightDc;

    AutoGL_GetViewRangeDc 
      (&widthDc, &heightDc);
    glViewport (0, 0, (GLsizei)widthDc, (GLsizei)heightDc);
  }

  /* reset projection matrix stack */
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  
  SetProjection ();

  /* set background color */
  glClearColor (View_backgroundRed, 
		View_backgroundGreen, 
		View_backgroundBlue, 
		1.0f);

  /* clear color and depth buffers */
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* reset model view matrix stack */
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  SetViewingTransformation ();

  {
    double dirX, dirY, dirZ;

    /* set initial normal */
    AutoGL_GetViewDirection 
      (&dirX, &dirY, &dirZ);
    glNormal3d (dirX, dirY, dirZ); 

    /* set initial color : white */
    glColor4f (1.0f, 1.0f, 1.0f, 1.0f); 
  }

  View_twoSideFaceFlag = 1;
  View_currentPrimitiveType = OPENGL_PRIMITIVE_DEFAULT;

#ifdef AUTO_NORMAL
  /* automatic normal calculation */
  glEnable (GL_AUTO_NORMAL);
  glEnable (GL_NORMALIZE);
#endif
}

void AutoGL_INSIDE_EndDrawingOpenGLWindow (void)
{
  /* end rendering OpenGL */

  SetDefaultPrimitive ();

  /* flush drawing commands */
  glFlush ();

#ifdef AUTOGL_MP
  MakeFinalImage ();
#endif
}

GLuint AutoGL_INSIDE_GetOpenGLCharBase (void)
{
  return View_charBase;
}

void AutoGL_TurnOnSpecular (void)
{
  static GLfloat specularColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
  static GLfloat specularShininess[] = { 50.0f };

  SetDefaultPrimitive ();

  glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);
  glMaterialfv (GL_FRONT_AND_BACK, GL_SHININESS, specularShininess);
}

void AutoGL_TurnOnTransparency (void)
{
  SetDefaultPrimitive ();

  /* turn on transparency */
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
#if 0
  /* make Z buffer as read-only */
  glDepthMask (GL_FALSE);
#endif

  View_transparencyIsOn = 1;
  View_opacity = 0.5;
}

void AutoGL_SetOpacity (double opacity)
{
  View_opacity = opacity;
}

void AutoGL_SetTwoSideFaceFlag (int flag)
{
  View_twoSideFaceFlag = flag;
}

int AutoGL_GetTwoSideFaceFlag (void)
{
  return View_twoSideFaceFlag;
}

void AutoGL_SetColor2 
(float red, float green, float blue)
{
  SetDefaultPrimitive ();

  if (View_transparencyIsOn) {
    glColor4f (red, green, blue, View_opacity);
  } else {
    glColor4f (red, green, blue, 1.0f);
  }
}

void AutoGL_DrawLine2 
(float x0, float y0, float z0,
 float x1, float y1, float z1)
{
  SetLinePrimitive ();

  glVertex3f (x0, y0, z0);
  glVertex3f (x1, y1, z1);
}

void AutoGL_DrawTriangle2 
(float x0, float y0, float z0,
 float x1, float y1, float z1,
 float x2, float y2, float z2,
 float nx, float ny, float nz)
{
  SetTrianglePrimitive ();

#ifdef AUTO_NORMAL
#else
  glNormal3f (nx, ny, nz); 
#endif
  glVertex3f (x0, y0, z0);
  glVertex3f (x1, y1, z1);
  glVertex3f (x2, y2, z2);

  if (View_twoSideFaceFlag) {
#ifdef AUTO_NORMAL
#else
    glNormal3f (-nx, -ny, -nz); 
#endif
    glVertex3f (x0, y0, z0);
    glVertex3f (x2, y2, z2);
    glVertex3f (x1, y1, z1);
  }
}

void AutoGL_DrawColorInterpolatedTriangle2
(float x0, float y0, float z0, float red0, float green0, float blue0,  
 float x1, float y1, float z1, float red1, float green1, float blue1,  
 float x2, float y2, float z2, float red2, float green2, float blue2,
 float nx, float ny, float nz)
{
  SetTrianglePrimitive ();

#ifdef AUTO_NORMAL
#else
  glNormal3f (nx, ny, nz); 
#endif
  glColor3f (red0, green0, blue0);
  glVertex3f (x0, y0, z0);
  glColor3f (red1, green1, blue1);
  glVertex3f (x1, y1, z1);
  glColor3f (red2, green2, blue2);
  glVertex3f (x2, y2, z2);

  if (View_twoSideFaceFlag) {
#ifdef AUTO_NORMAL
#else
    glNormal3f (-nx, -ny, -nz); 
#endif
    glColor3f (red0, green0, blue0);
    glVertex3f (x0, y0, z0);
    glColor3f (red2, green2, blue2);
    glVertex3f (x2, y2, z2);
    glColor3f (red1, green1, blue1);
    glVertex3f (x1, y1, z1);
  }
}

void AutoGL_DrawNormalInterpolatedTriangle2 
(float x0, float y0, float z0, float nx0, float ny0, float nz0,
 float x1, float y1, float z1, float nx1, float ny1, float nz1,
 float x2, float y2, float z2, float nx2, float ny2, float nz2)
{
  SetTrianglePrimitive ();

  glNormal3f (nx0, ny0, nz0); 
  glVertex3f (x0, y0, z0);
  glNormal3f (nx1, ny1, nz1); 
  glVertex3f (x1, y1, z1);
  glNormal3f (nx2, ny2, nz2); 
  glVertex3f (x2, y2, z2);
  
  if (View_twoSideFaceFlag) {
    glNormal3f (-nx0, -ny0, -nz0); 
    glVertex3f (x0, y0, z0);
    glNormal3f (-nx2, -ny2, -nz2); 
    glVertex3f (x2, y2, z2);
    glNormal3f (-nx1, -ny1, -nz1); 
    glVertex3f (x1, y1, z1);
  }
}

void AutoGL_DrawString2 
(float x, float y, float z,
 const char *str)
{
  SetNoLightingPrimitive ();

  /* draw character string at the point */

  glRasterPos3f (x, y, z);

  glPushAttrib (GL_LIST_BIT);
  glListBase (View_charBase);
  glCallLists (strlen (str), GL_UNSIGNED_BYTE, str);
  glPopAttrib ();
}

void AutoGL_GetViewImage
(unsigned char image_OUT[])
{
  int widthDc, heightDc;

  SetDefaultPrimitive ();

  AutoGL_GetViewRangeDc 
    (&widthDc, &heightDc);

  /* read image data of the view window into <image_OUT> */

  glReadPixels (0, 0, widthDc, heightDc, GL_RGB, GL_UNSIGNED_BYTE, 
		(GLubyte *)image_OUT);
}

void AutoGL_StartSelection (int x, int y, int range)
{
  int widthDc, heightDc;
  GLint viewport[4];

  SetDefaultPrimitive ();

  AutoGL_GetViewRangeDc 
    (&widthDc, &heightDc);

  /* set OpenGL selection buffer */
  glSelectBuffer (MAX_SELECT_BUFFER_SIZE, View_selectBuffer);

  glGetIntegerv (GL_VIEWPORT, 
		 viewport);

  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();

  /* switch mode to OpenGL selection */
  (void)glRenderMode (GL_SELECT);

  /* set a new view volume around the mouse cursor (x, y) */
  glLoadIdentity ();
  gluPickMatrix 
    ((GLdouble)(widthDc / 2 + x), (GLdouble)(heightDc / 2 + y),
     (GLdouble)range, (GLdouble)range, 
     viewport);

  SetProjection ();

  /* reset model view matrix stack */
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  SetViewingTransformation ();

  glInitNames ();
  glPushName (100);
}

void AutoGL_SetSelectionId2 (int id)
{
  int name = id;

  assert(0 <= id);

  SetDefaultPrimitive ();

  /* set the current name (object ID) of OpenGL selection */
  glLoadName (name);
}

void AutoGL_EndSelection (void)
{
  SetDefaultPrimitive ();

  glFlush ();

  /* switch mode back to OpenGL rendering */
  /* get the number of hits */
  View_nHits = glRenderMode (GL_RENDER);

  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();
  
  glMatrixMode (GL_MODELVIEW);
}

int AutoGL_GetSelectedId (void)
{
  int result;
  int iHit;
  int count;
  GLuint minZ;

  result = -1;
  minZ = 0;
  count = 0;
  for (iHit = 0; iHit < View_nHits; iHit++) {
    int nNames;
    GLuint name;
    GLuint z0, z1;
    
    nNames = View_selectBuffer[count++];
    assert(nNames == 1);
    z0 = View_selectBuffer[count++];
    z1 = View_selectBuffer[count++];
    name = View_selectBuffer[count++];

    if (result == -1
	|| z0 < minZ) {
      minZ = z0;
      result = name;
    }
  }

#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
  result = AutoGL_INSIDE_GatherSelectedId (result, minZ);
#endif

  return result;
}

void AutoGL_BeginNativeCall (void)
{
  SetDefaultPrimitive ();
}

void AutoGL_EndNativeCall (void)
{
}






/* To Be Obsolate */

void AutoGL_SetTextureMap 
(int sizeLevel, const unsigned char image[])
{
  int i;
  int sizeDc;

  assert(0 <= sizeLevel);
  
  sizeDc = 1;
  for (i = 0; i < sizeLevel; i++) {
    sizeDc *= 2;
  }

  SetDefaultPrimitive ();

  glEnable (GL_TEXTURE_2D);
  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
		   GL_NEAREST_MIPMAP_NEAREST);
  gluBuild2DMipmaps (GL_TEXTURE_2D, 3, sizeDc, sizeDc, 
		     GL_RGB, GL_UNSIGNED_BYTE, 
		     image);
}

void AutoGL_DrawTexturedTriangle 
(double x0, double y0, double z0,
 double x1, double y1, double z1,
 double x2, double y2, double z2,
 double scaleFactor)
{
  double nx, ny, nz;
  double ux, uy, uz;
  double vx, vy, vz;
  double tx0, ty0;
  double tx1, ty1;
  double tx2, ty2;

  /* calculate the unit normal vector of the triangle */
  AutoGL_GetVectorProduct3D
    (&nx, &ny, &nz,
     x0 - x1, y0 - y1, z0 - z1,
     x1 - x2, y1 - y2, z1 - z2);
  AutoGL_NormalizeVector3D 
    (&nx, &ny, &nz,
     nx, ny, nz);

  AutoGL_GetAnyPerpendicularDirection3D 
    (&ux, &uy, &uz,
     &vx, &vy, &vz,
     nx, ny, nz);
  
  tx0 = AutoGL_GetScalarProduct3D (x0, y0, z0, 
				   ux, uy, uz) / scaleFactor;
  ty0 = AutoGL_GetScalarProduct3D (x0, y0, z0, 
				   vx, vy, vz) / scaleFactor;

  tx1 = AutoGL_GetScalarProduct3D (x1, y1, z1, 
				   ux, uy, uz) / scaleFactor;
  ty1 = AutoGL_GetScalarProduct3D (x1, y1, z1, 
				   vx, vy, vz) / scaleFactor;

  tx2 = AutoGL_GetScalarProduct3D (x2, y2, z2, 
				   ux, uy, uz) / scaleFactor;
  ty2 = AutoGL_GetScalarProduct3D (x2, y2, z2, 
				   vx, vy, vz) / scaleFactor;

  SetTrianglePrimitive ();

  /* draw two triangles (front side and back side) */

  glNormal3d (nx, ny, nz); 
  glTexCoord2d (tx0, ty0); 
  glVertex3d (x0, y0, z0);
  glTexCoord2d (tx1, ty1); 
  glVertex3d (x1, y1, z1);
  glTexCoord2d (tx2, ty2); 
  glVertex3d (x2, y2, z2);

  if (AutoGL_GetTwoSideFaceFlag ()) {
    glNormal3d (-nx, -ny, -nz); 
    glTexCoord2d (tx0, ty0); 
    glVertex3d (x0, y0, z0);
    glTexCoord2d (tx2, ty2); 
    glVertex3d (x2, y2, z2);
    glTexCoord2d (tx1, ty1); 
    glVertex3d (x1, y1, z1);
  }
}

void AutoGL_DrawImage 
(double x, double y, double z,
 int widthDc, int heightDc, const unsigned char image[])
{
  SetNoLightingPrimitive ();

  /* draw image of size (<widthDc>, <heightDc>) at the point */

  glRasterPos3d (x, y, z);

  glDrawPixels (widthDc, heightDc, GL_RGB, GL_UNSIGNED_BYTE, 
		(GLubyte *)image);
}



/* test */

#define MAX_MACROS 1000

/* OpenGL display list ID for macro */
static const int View_MacroListId = 1000;

void AutoGL_OpenMacro (int macroId)
{
  assert(0 <= macroId);
  assert(macroId < MAX_MACROS);

  SetDefaultPrimitive ();

  glNewList (View_MacroListId + macroId, 
	     GL_COMPILE);
}

void AutoGL_CloseMacro (void)
{
  SetDefaultPrimitive ();

  glEndList ();
}

void AutoGL_DrawMacro (int macroId)
{
  assert(0 <= macroId);
  assert(macroId < MAX_MACROS);

  glCallList (View_MacroListId + macroId);
}

void AutoGL_PushMatrix (void)
{
  SetDefaultPrimitive ();

  glPushMatrix ();
}

void AutoGL_PopMatrix (void)
{
  SetDefaultPrimitive ();

  glPopMatrix ();
}

void AutoGL_Translate (double x, double y, double z)
{
  SetDefaultPrimitive ();

  glTranslated (x, y, z);
}

void AutoGL_Rotate 
(double angle, 
 double axisX, double axisY, double axisZ)
{
  SetDefaultPrimitive ();

  glRotated (angle, axisX, axisY, axisZ);
}

void AutoGL_Scale (double ratio)
{
  SetDefaultPrimitive ();

  glScaled (ratio, ratio, ratio);
}

