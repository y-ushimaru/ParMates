/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_graphics.c : graphics */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "autogl_os.h"
#include "autogl_math.h"
#include "autogl_uims.h"
#include "autogl_uims_inside.h"
#include "autogl_graphics.h"
#include "autogl_graphics_inside.h"



/* Primitive Buffer */

#define DEFAULT_PRIMITIVE_BUFFER_SIZE 10000000
static int View_maxPrimitiveBufferSize;
static int View_currentPrimitiveBufferSize;
static float *View_primitiveBuffer;

void AutoGL_INSIDE_ClearPrimitiveBuffer (void)
{
  if (View_maxPrimitiveBufferSize == 0) {
    View_maxPrimitiveBufferSize = DEFAULT_PRIMITIVE_BUFFER_SIZE;
    View_primitiveBuffer = (float *)malloc 
      (sizeof (float) * View_maxPrimitiveBufferSize);
    assert(View_primitiveBuffer != NULL);
  }
  View_currentPrimitiveBufferSize = 0;
}

static void ExtendPrimitiveBuffer (void)
{
  float *newBuffer;
  int i;
  
  View_maxPrimitiveBufferSize *= 2;
  newBuffer = (float *)malloc 
    (sizeof (float) * View_maxPrimitiveBufferSize);
  assert(newBuffer != NULL);
  for (i = 0; i < View_currentPrimitiveBufferSize; i++) {
    newBuffer[i] = View_primitiveBuffer[i];
  }
  free (View_primitiveBuffer);
  View_primitiveBuffer = newBuffer;
}

void AutoGL_INSIDE_AddPrimitiveBuffer (float value)
{
  if (View_maxPrimitiveBufferSize <= View_currentPrimitiveBufferSize) {
    ExtendPrimitiveBuffer ();
  }

  View_primitiveBuffer[View_currentPrimitiveBufferSize] = value;
  View_currentPrimitiveBufferSize++;
}

int AutoGL_INSIDE_PrimitiveBufferSize (void)
{
  return View_currentPrimitiveBufferSize;
}

float *AutoGL_INSIDE_PrimitiveBuffer (void)
{
  return View_primitiveBuffer;
}

void AutoGL_ReadDisplayList (FILE *fp)
{
  int size;
  float *buffer;

  if (View_primitiveBuffer != NULL) {
    free (View_primitiveBuffer);
    View_primitiveBuffer = NULL;
    View_currentPrimitiveBufferSize = 0;
    View_maxPrimitiveBufferSize = 0;
  }

  fread (&size, sizeof (int), 1, fp);
  buffer = (float *)malloc (sizeof (float) * size);
  assert(buffer != NULL);
  fread (buffer, sizeof (float), size, fp);

  View_primitiveBuffer = buffer;
  View_currentPrimitiveBufferSize = size;
  View_maxPrimitiveBufferSize = size;
}

void AutoGL_WriteDisplayList (FILE *fp)
{
  int size = View_currentPrimitiveBufferSize;
  float *buffer = View_primitiveBuffer;

  fwrite (&size, sizeof (int), 1, fp);
  fwrite (buffer, sizeof (float), size, fp);
}



/* View */

static double View_size = 1.0;
static double View_centerX = 0.0;
static double View_centerY = 0.0;
static double View_centerZ = 0.0;
static double View_directionX = 0.0;
static double View_directionY = 0.0;
static double View_directionZ = 1.0;
static double View_upVectorX = 0.0;
static double View_upVectorY = 1.0;
static double View_upVectorZ = 0.0;

static int View_widthDc = 500;
static int View_heightDc = 500;

static int View_perspectiveViewFlag = 0;

void AutoGL_SetViewSize (double size)
{
  View_size = size;
}

void AutoGL_SetViewCenter (double x, double y, double z)
{
  View_centerX = x;
  View_centerY = y;
  View_centerZ = z;
}

void AutoGL_SetViewDirection (double x, double y, double z)
{
  AutoGL_NormalizeVector3D 
    (&View_directionX, &View_directionY, &View_directionZ,
     x, y, z);
}

void AutoGL_SetViewUpVector (double x, double y, double z)
{
  AutoGL_NormalizeVector3D 
    (&View_upVectorX, &View_upVectorY, &View_upVectorZ,
     x, y, z);
}

void AutoGL_SetPerspectiveViewFlag (int flag)
{
  View_perspectiveViewFlag = flag;
}

void AutoGL_INSIDE_SetViewRangeDc (int widthDc, int heightDc)
{
  View_widthDc = (widthDc <= 0) ? 1 : widthDc;
  View_heightDc = (heightDc <= 0) ? 1 : heightDc;
}

double AutoGL_GetViewSize (void)
{
  return View_size;
}

void AutoGL_GetViewCenter 
(double *x_OUT, double *y_OUT, double *z_OUT)
{
  *x_OUT = View_centerX;
  *y_OUT = View_centerY;
  *z_OUT = View_centerZ;
}

void AutoGL_GetViewDirection 
(double *x_OUT, double *y_OUT, double *z_OUT)
{
  *x_OUT = View_directionX;
  *y_OUT = View_directionY;
  *z_OUT = View_directionZ;
}

void AutoGL_GetViewUpVector 
(double *x_OUT, double *y_OUT, double *z_OUT)
{
  *x_OUT = View_upVectorX; 
  *y_OUT = View_upVectorY;
  *z_OUT = View_upVectorZ;
}

int AutoGL_GetPerspectiveViewFlag (void)
{
  return View_perspectiveViewFlag;
}

void AutoGL_GetViewRangeDc 
(int *widthDc_OUT, int *heightDc_OUT)
{
  *widthDc_OUT = View_widthDc;
  *heightDc_OUT = View_heightDc;
}

int AutoGL_GetViewSizeDc (void)
{
  return (View_widthDc < View_heightDc) ?
    View_widthDc / 2 : View_heightDc / 2;
}

void AutoGL_GetVcOfDc 
(double *vcX_OUT, double *vcY_OUT, double *vcZ_OUT,
 int dcX, int dcY, int dcZ)
{
  double size = AutoGL_GetViewSize ();
  int sizeDc = AutoGL_GetViewSizeDc ();

  *vcX_OUT = size * dcX / sizeDc;
  *vcY_OUT = size * dcY / sizeDc;
  *vcZ_OUT = size * dcZ / sizeDc;
}

void AutoGL_GetPositionOfVc 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double vcX, double vcY, double vcZ)
{
  double centerX, centerY, centerZ;
  double zAxisX, zAxisY, zAxisZ;
  double upX, upY, upZ;
  double xAxisX, xAxisY, xAxisZ;
  double yAxisX, yAxisY, yAxisZ;

  AutoGL_GetViewCenter (&centerX, &centerY, &centerZ);
  AutoGL_GetViewDirection (&zAxisX, &zAxisY, &zAxisZ);
  AutoGL_GetViewUpVector (&upX, &upY, &upZ);

  AutoGL_GetVectorProduct3D (&xAxisX, &xAxisY, &xAxisZ,
			     upX, upY, upZ,
			     zAxisX, zAxisY, zAxisZ);
  AutoGL_NormalizeVector3D (&xAxisX, &xAxisY, &xAxisZ,
			    xAxisX, xAxisY, xAxisZ);
  
  AutoGL_GetVectorProduct3D (&yAxisX, &yAxisY, &yAxisZ,
			     zAxisX, zAxisY, zAxisZ, 
			     xAxisX, xAxisY, xAxisZ);
  AutoGL_NormalizeVector3D (&yAxisX, &yAxisY, &yAxisZ,
			    yAxisX, yAxisY, yAxisZ);
  
  AutoGL_AddVector3D (x_OUT, y_OUT, z_OUT,
		      centerX, centerY, centerZ,
		      xAxisX * vcX + yAxisX * vcY + zAxisX * vcZ,
		      xAxisY * vcX + yAxisY * vcY + zAxisY * vcZ,
		      xAxisZ * vcX + yAxisZ * vcY + zAxisZ * vcZ);
}

void AutoGL_GetPositionOfDc 
(double *x_OUT, double *y_OUT, double *z_OUT,
 int dcX, int dcY, int dcZ)
{
  double vcX, vcY, vcZ;

  AutoGL_GetVcOfDc (&vcX, &vcY, &vcZ,
		    dcX, dcY, dcZ);
  AutoGL_GetPositionOfVc (x_OUT, y_OUT, z_OUT,
			  vcX, vcY, vcZ);
}



/* Event Handling */

static int View_eventType;  /* AUTOGL_EVENT_ */

static int View_pointingDeviceDcX;
static int View_pointingDeviceDcY;
static int View_pointingDeviceDcZ;

static int View_keyChar;

void AutoGL_INSIDE_SetPointingDeviceEvent 
(int event,   /* AUTOGL_EVENT_ */
 int dcX, int dcY, int dcZ)
{
  View_eventType = event;
  View_pointingDeviceDcX = dcX;
  View_pointingDeviceDcY = dcY;
  View_pointingDeviceDcZ = dcZ;
}

void AutoGL_INSIDE_SetKeyEvent 
(int keyChar)
{
  View_eventType = AUTOGL_EVENT_KEY;
  View_keyChar = keyChar;
}

int AutoGL_GetViewEvent (void)  /* AUTOGL_EVENT_ */
{
  return View_eventType;
}

void AutoGL_GetPointingDevicePositionDc 
(int *dcX_OUT, int *dcY_OUT, int *dcZ_OUT)
{
  *dcX_OUT = View_pointingDeviceDcX;
  *dcY_OUT = View_pointingDeviceDcY;
  *dcZ_OUT = View_pointingDeviceDcZ;
}

int AutoGL_GetKeyChar (void)
{
  return View_keyChar;
}



/* Rendering */

static int View_DisplayListIsOpened;

typedef enum {
  PRIMITIVE_COLOR,
  PRIMITIVE_LINE,
  PRIMITIVE_TRIANGLE,
  PRIMITIVE_COLOR_TRIANGLE,
  PRIMITIVE_NORMAL_TRIANGLE,
  PRIMITIVE_STRING,
  PRIMITIVE_SELECTION
} PrimitiveType;

void AutoGL_SetColor 
(double red, double green, double blue)
{
  if (View_DisplayListIsOpened) {
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)PRIMITIVE_COLOR);

    AutoGL_INSIDE_AddPrimitiveBuffer ((float)red);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)green);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)blue);
  } else {
    AutoGL_SetColor2 
      ((float)red, (float)green, (float)blue);
  }
}

static int SetColor (float buffer[], int count)
{
  float red = buffer[count++];
  float green = buffer[count++];
  float blue = buffer[count++];
  
  AutoGL_SetColor2 (red, green, blue);
  return count;
}

void AutoGL_DrawLine 
(double x0, double y0, double z0,    
 double x1, double y1, double z1)
{
  if (View_DisplayListIsOpened) {
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)PRIMITIVE_LINE);

    AutoGL_INSIDE_AddPrimitiveBuffer ((float)x0);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)y0);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)z0);

    AutoGL_INSIDE_AddPrimitiveBuffer ((float)x1);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)y1);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)z1);
  } else {
    AutoGL_DrawLine2 
      ((float)x0, (float)y0, (float)z0, 
       (float)x1, (float)y1, (float)z1);
  }
}

static int DrawLine (float buffer[], int count)
{
  float x0 = buffer[count++];
  float y0 = buffer[count++];
  float z0 = buffer[count++];
  
  float x1 = buffer[count++];
  float y1 = buffer[count++];
  float z1 = buffer[count++];
  
  AutoGL_DrawLine2 
    (x0, y0, z0, 
     x1, y1, z1);
  return count;
}

void AutoGL_DrawTriangle 
(double x0, double y0, double z0,  
 double x1, double y1, double z1,  
 double x2, double y2, double z2)
{
  double nx, ny, nz;
  
  /* calculate the unit normal vector of the triangle */
  AutoGL_GetVectorProduct3D
    (&nx, &ny, &nz,
     x0 - x1, y0 - y1, z0 - z1,
     x1 - x2, y1 - y2, z1 - z2);
  AutoGL_NormalizeVector3D 
    (&nx, &ny, &nz,
     nx, ny, nz);
    
  if (View_DisplayListIsOpened) {
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)PRIMITIVE_TRIANGLE);

    AutoGL_INSIDE_AddPrimitiveBuffer ((float)x0);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)y0);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)z0);

    AutoGL_INSIDE_AddPrimitiveBuffer ((float)x1);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)y1);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)z1);

    AutoGL_INSIDE_AddPrimitiveBuffer ((float)x2);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)y2);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)z2);

    AutoGL_INSIDE_AddPrimitiveBuffer ((float)nx);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)ny);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)nz);
  } else {
    AutoGL_DrawTriangle2 
      ((float)x0, (float)y0, (float)z0, 
       (float)x1, (float)y1, (float)z1, 
       (float)x2, (float)y2, (float)z2,
       (float)nx, (float)ny, (float)nz);
  }
}

static int DrawTriangle (float buffer[], int count)
{
  float x0 = buffer[count++];
  float y0 = buffer[count++];
  float z0 = buffer[count++];
  
  float x1 = buffer[count++];
  float y1 = buffer[count++];
  float z1 = buffer[count++];
  
  float x2 = buffer[count++];
  float y2 = buffer[count++];
  float z2 = buffer[count++];
  
  float nx = buffer[count++];
  float ny = buffer[count++];
  float nz = buffer[count++];
  
  AutoGL_DrawTriangle2 
    (x0, y0, z0, 
     x1, y1, z1, 
     x2, y2, z2,
     nx, ny, nz);
  return count;
}

void AutoGL_DrawColorInterpolatedTriangle 
(double x0, double y0, double z0, double red0, double green0, double blue0,  
 double x1, double y1, double z1, double red1, double green1, double blue1,  
 double x2, double y2, double z2, double red2, double green2, double blue2)
{
  double nx, ny, nz;
  
  /* calculate the unit normal vector of the triangle */
  AutoGL_GetVectorProduct3D
    (&nx, &ny, &nz,
     x0 - x1, y0 - y1, z0 - z1,
     x1 - x2, y1 - y2, z1 - z2);
  AutoGL_NormalizeVector3D 
    (&nx, &ny, &nz,
     nx, ny, nz);
    
  if (View_DisplayListIsOpened) {
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)PRIMITIVE_COLOR_TRIANGLE);

    AutoGL_INSIDE_AddPrimitiveBuffer ((float)x0);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)y0);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)z0);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)red0);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)green0);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)blue0);

    AutoGL_INSIDE_AddPrimitiveBuffer ((float)x1);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)y1);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)z1);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)red1);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)green1);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)blue1);

    AutoGL_INSIDE_AddPrimitiveBuffer ((float)x2);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)y2);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)z2);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)red2);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)green2);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)blue2);

    AutoGL_INSIDE_AddPrimitiveBuffer ((float)nx);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)ny);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)nz);
  } else {
    AutoGL_DrawColorInterpolatedTriangle2 
      ((float)x0, (float)y0, (float)z0, 
       (float)red0, (float)green0, (float)blue0,
       (float)x1, (float)y1, (float)z1, 
       (float)red1, (float)green1, (float)blue1, 
       (float)x2, (float)y2, (float)z2, 
       (float)red2, (float)green2, (float)blue2,
       (float)nx, (float)ny, (float)nz);
  }
}

static int DrawColorInterpolatedTriangle (float buffer[], int count)
{
  float x0 = buffer[count++];
  float y0 = buffer[count++];
  float z0 = buffer[count++];
  
  float red0 = buffer[count++];
  float green0 = buffer[count++];
  float blue0 = buffer[count++];
  
  float x1 = buffer[count++];
  float y1 = buffer[count++];
  float z1 = buffer[count++];
  
  float red1 = buffer[count++];
  float green1 = buffer[count++];
  float blue1 = buffer[count++];
  
  float x2 = buffer[count++];
  float y2 = buffer[count++];
  float z2 = buffer[count++];
  
  float red2 = buffer[count++];
  float green2 = buffer[count++];
  float blue2 = buffer[count++];
  
  float nx = buffer[count++];
  float ny = buffer[count++];
  float nz = buffer[count++];
  
  AutoGL_DrawColorInterpolatedTriangle2 
    (x0, y0, z0, red0, green0, blue0,
     x1, y1, z1, red1, green1, blue1, 
     x2, y2, z2, red2, green2, blue2,
     nx, ny, nz);
  return count;
}

void AutoGL_DrawNormalInterpolatedTriangle 
(double x0, double y0, double z0, double nx0, double ny0, double nz0,  
 double x1, double y1, double z1, double nx1, double ny1, double nz1,  
 double x2, double y2, double z2, double nx2, double ny2, double nz2)
{
  double nx, ny, nz;
  double nnx0, nny0, nnz0;
  double nnx1, nny1, nnz1;
  double nnx2, nny2, nnz2;

  /* calculate the unit normal vector of the triangle */
  AutoGL_GetVectorProduct3D
    (&nx, &ny, &nz,
     x0 - x1, y0 - y1, z0 - z1,
     x1 - x2, y1 - y2, z1 - z2);
  AutoGL_NormalizeVector3D 
    (&nx, &ny, &nz,
     nx, ny, nz);

  /* normalize the normal vector of each vertex */
  AutoGL_NormalizeVector3D 
    (&nnx0, &nny0, &nnz0,
     nx0, ny0, nz0);
  AutoGL_NormalizeVector3D 
    (&nnx1, &nny1, &nnz1,
     nx1, ny1, nz1);
  AutoGL_NormalizeVector3D 
    (&nnx2, &nny2, &nnz2,
     nx2, ny2, nz2);

  /* (nx, ny, nz) must be the same direction as (nx0, ny0, nz0) */
  if (0.0 < AutoGL_GetScalarProduct3D (nx, ny, nz, 
				       nnx0, nny0, nnz0)) {
    /* nothing */
  } else {
    nnx0 = -nnx0;
    nny0 = -nny0;
    nnz0 = -nnz0;
    
    nnx1 = -nnx1;
    nny1 = -nny1;
    nnz1 = -nnz1;
    
    nnx2 = -nnx2;
    nny2 = -nny2;
    nnz2 = -nnz2;
  }

  if (View_DisplayListIsOpened) {
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)PRIMITIVE_NORMAL_TRIANGLE);

    AutoGL_INSIDE_AddPrimitiveBuffer ((float)x0);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)y0);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)z0);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)nnx0);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)nny0);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)nnz0);

    AutoGL_INSIDE_AddPrimitiveBuffer ((float)x1);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)y1);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)z1);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)nnx1);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)nny1);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)nnz1);

    AutoGL_INSIDE_AddPrimitiveBuffer ((float)x2);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)y2);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)z2);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)nnx2);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)nny2);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)nnz2);
  } else {
    AutoGL_DrawNormalInterpolatedTriangle2 
      ((float)x0, (float)y0, (float)z0, 
       (float)nnx0, (float)nny0, (float)nnz0,
       (float)x1, (float)y1, (float)z1, 
       (float)nnx1, (float)nny1, (float)nnz1, 
       (float)x2, (float)y2, (float)z2, 
       (float)nnx2, (float)nny2, (float)nnz2);
  }
}

static int DrawNormalInterpolatedTriangle (float buffer[], int count)
{
  float x0 = buffer[count++];
  float y0 = buffer[count++];
  float z0 = buffer[count++];
  
  float nx0 = buffer[count++];
  float ny0 = buffer[count++];
  float nz0 = buffer[count++];
  
  float x1 = buffer[count++];
  float y1 = buffer[count++];
  float z1 = buffer[count++];
  
  float nx1 = buffer[count++];
  float ny1 = buffer[count++];
  float nz1 = buffer[count++];
  
  float x2 = buffer[count++];
  float y2 = buffer[count++];
  float z2 = buffer[count++];
  
  float nx2 = buffer[count++];
  float ny2 = buffer[count++];
  float nz2 = buffer[count++];
  
  AutoGL_DrawNormalInterpolatedTriangle2 
    (x0, y0, z0, nx0, ny0, nz0,
     x1, y1, z1, nx1, ny1, nz1, 
     x2, y2, z2, nx2, ny2, nz2);
  return count;
}

void AutoGL_DrawString 
(double x, double y, double z,
 const char *str)
{
  if (View_DisplayListIsOpened) {
    int iCh;
    int length = strlen (str);
    
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)PRIMITIVE_STRING);
    
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)x);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)y);
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)z);

    AutoGL_INSIDE_AddPrimitiveBuffer ((float)length);

    for (iCh = 0; iCh < length; iCh++) {
      AutoGL_INSIDE_AddPrimitiveBuffer ((float)str[iCh]);
    }
  } else {
    AutoGL_DrawString2 ((float)x, (float)y, (float)z, 
			str);
  }
}

static int DrawString (float buffer[], int count)
{
  float x = buffer[count++];
  float y = buffer[count++];
  float z = buffer[count++];
  
  int length = (int)buffer[count++];
  int iCh;
  static char str[1024];
  
  assert(length < 1024);
  for (iCh = 0; iCh < length; iCh++) {
    str[iCh] = (char)buffer[count++];
  }
  str[length] = 0;
  
  AutoGL_DrawString2 (x, y, z, 
		      str);
  return count;
}

void AutoGL_SetSelectionId (int id) 
{
  if (View_DisplayListIsOpened) {
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)PRIMITIVE_SELECTION);
    
    AutoGL_INSIDE_AddPrimitiveBuffer ((float)id);
  } else {
    AutoGL_SetSelectionId2 (id);
  }
}

static int SetSelectionId (float buffer[], int count)
{
  int id = (int)buffer[count++];
  
  AutoGL_SetSelectionId2 (id);
  return count;
}

void AutoGL_DrawQuadrangle 
(double x0, double y0, double z0,
 double x1, double y1, double z1,
 double x2, double y2, double z2,
 double x3, double y3, double z3)
{
  AutoGL_DrawTriangle (x0, y0, z0,
		       x1, y1, z1,
		       x2, y2, z2);
  AutoGL_DrawTriangle (x2, y2, z2,
		       x3, y3, z3,
		       x0, y0, z0);
}

void AutoGL_OpenDisplayList (void)
{
  View_DisplayListIsOpened = 1;
  AutoGL_INSIDE_ClearPrimitiveBuffer ();
}

void AutoGL_CloseDisplayList (void)
{
  View_DisplayListIsOpened = 0;
}

void AutoGL_DrawDisplayList (void)
{
  int count;
  int size = AutoGL_INSIDE_PrimitiveBufferSize ();
  float *buffer = AutoGL_INSIDE_PrimitiveBuffer ();
    
  count = 0;
  for (;;) {
    PrimitiveType primitiveType;

    assert(count <= size);
    if (count == size) break;

    primitiveType = (PrimitiveType)((int)buffer[count++]);
    switch (primitiveType) {
    case PRIMITIVE_COLOR:
      count = SetColor (buffer, count);
      break;
    case PRIMITIVE_LINE:
      count = DrawLine (buffer, count);
      break;
    case PRIMITIVE_TRIANGLE:
      count = DrawTriangle (buffer, count);
      break;
    case PRIMITIVE_COLOR_TRIANGLE:
      count = DrawColorInterpolatedTriangle (buffer, count);
      break;
    case PRIMITIVE_NORMAL_TRIANGLE:
      count = DrawNormalInterpolatedTriangle (buffer, count);
      break;
    case PRIMITIVE_STRING:
      count = DrawString (buffer, count);
      break;
    case PRIMITIVE_SELECTION:
      count = SetSelectionId (buffer, count);
      break;
    default:
      assert(0);
      break;
    }
  }
}

void AutoGL_ReadMetafile 
(const char *fileName, int updateViewingParameterFlag)
{
  FILE *fp;
  double size;
  double centerX, centerY, centerZ;
  double directionX, directionY, directionZ;
  double upVectorX, upVectorY, upVectorZ;

  fp = fopen (fileName, "rb");
  assert(fp != NULL);

  fread (&size, sizeof (double), 1, fp);
  fread (&centerX, sizeof (double), 1, fp);
  fread (&centerY, sizeof (double), 1, fp);
  fread (&centerZ, sizeof (double), 1, fp);
  fread (&directionX, sizeof (double), 1, fp);
  fread (&directionY, sizeof (double), 1, fp);
  fread (&directionZ, sizeof (double), 1, fp);
  fread (&upVectorX, sizeof (double), 1, fp);
  fread (&upVectorY, sizeof (double), 1, fp);
  fread (&upVectorZ, sizeof (double), 1, fp);

  AutoGL_ReadDisplayList (fp);

  fclose (fp);

  if (updateViewingParameterFlag) {
    AutoGL_SetViewSize (size);
    AutoGL_SetViewCenter (centerX, centerY, centerZ);       
    AutoGL_SetViewDirection (directionX, directionY, directionZ);
    AutoGL_SetViewUpVector (upVectorX, upVectorY, upVectorZ);
  }
}

void AutoGL_WriteMetafile 
(const char *fileName)
{
  FILE *fp;
  double size;
  double centerX, centerY, centerZ;
  double directionX, directionY, directionZ;
  double upVectorX, upVectorY, upVectorZ;

  size = AutoGL_GetViewSize ();
  AutoGL_GetViewCenter (&centerX, &centerY, &centerZ);       
  AutoGL_GetViewDirection (&directionX, &directionY, &directionZ);
  AutoGL_GetViewUpVector (&upVectorX, &upVectorY, &upVectorZ);

  fp = fopen (fileName, "wb");
  assert(fp != NULL);

  fwrite (&size, sizeof (double), 1, fp);
  fwrite (&centerX, sizeof (double), 1, fp);
  fwrite (&centerY, sizeof (double), 1, fp);
  fwrite (&centerZ, sizeof (double), 1, fp);
  fwrite (&directionX, sizeof (double), 1, fp);
  fwrite (&directionY, sizeof (double), 1, fp);
  fwrite (&directionZ, sizeof (double), 1, fp);
  fwrite (&upVectorX, sizeof (double), 1, fp);
  fwrite (&upVectorY, sizeof (double), 1, fp);
  fwrite (&upVectorZ, sizeof (double), 1, fp);

  AutoGL_WriteDisplayList (fp);

  fclose (fp);
}



/* Image File */

#define MAX_IMAGE_FILE_NAME_LENGTH 1024
static char View_imageFileName[MAX_IMAGE_FILE_NAME_LENGTH] = 
  "result.ppm";

void AutoGL_SetImageFileName 
(const char *fileName)
{
  strcpy (View_imageFileName, fileName);
}

void AutoGL_GetImageFileName 
(char *fileName_OUT)
{
  strcpy (fileName_OUT, View_imageFileName);
}






