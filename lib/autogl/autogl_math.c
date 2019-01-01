/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_math.c : AutoGL mathematics */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "autogl_os.h"
#include "autogl_math.h"



/* Tolerance management */

static double Tolerance = 0.0000001;

void AutoGL_SetTolerance (double tolerance)
{
  Tolerance = tolerance;
}

double AutoGL_GetTolerance (void)
{
  return Tolerance;
}



/* 2-D vector operation */

void AutoGL_AssignVector2D 
(double *x_OUT, double *y_OUT,
 double x, double y)
{
  *x_OUT = x;
  *y_OUT = y;
}

void AutoGL_AddVector2D 
(double *x_OUT, double *y_OUT,
 double x0, double y0,
 double x1, double y1)
{
  *x_OUT = x0 + x1;
  *y_OUT = y0 + y1;
}

void AutoGL_SubtractVector2D 
(double *x_OUT, double *y_OUT,
 double x0, double y0,
 double x1, double y1)
{
  *x_OUT = x0 - x1;
  *y_OUT = y0 - y1;
}

void AutoGL_MultiplyVector2D 
(double *x_OUT, double *y_OUT,
 double x, double y,
 double scale)
{
  *x_OUT = x * scale;
  *y_OUT = y * scale;
}

double AutoGL_GetVectorLength2D 
(double x, double y)
{
  return sqrt (x * x + y * y);
}

void AutoGL_NormalizeVector2D 
(double *x_OUT, double *y_OUT,
 double x, double y)
{
  double length = AutoGL_GetVectorLength2D (x, y);

  if (length == 0.0) length = 1.0;
  *x_OUT = x / length;
  *y_OUT = y / length;
}

double AutoGL_GetScalarProduct2D 
(double x0, double y0,
 double x1, double y1)
{
  return x0 * x1 + y0 * y1;
}

double AutoGL_GetVectorDistance2D 
(double x0, double y0,
 double x1, double y1)
{
  double x, y;
  AutoGL_SubtractVector2D (&x, &y,
			   x0, y0,
			   x1, y1);
  return AutoGL_GetVectorLength2D (x, y);
}

void AutoGL_GetMinVector2D 
(double *x_OUT, double *y_OUT,
 double x0, double y0,
 double x1, double y1)
{
  *x_OUT = (x0 < x1) ? x0 : x1;
  *y_OUT = (y0 < y1) ? y0 : y1;
}

void AutoGL_GetMaxVector2D 
(double *x_OUT, double *y_OUT,
 double x0, double y0,
 double x1, double y1)
{
  *x_OUT = (x0 < x1) ? x1 : x0;
  *y_OUT = (y0 < y1) ? y1 : y0;
}

void AutoGL_GetAnyPerpendicularDirection2D 
(double *nx_OUT, double *ny_OUT,
 double x, double y)
{
  *nx_OUT = -y;
  *ny_OUT = x;
  AutoGL_NormalizeVector2D (nx_OUT, ny_OUT,
			    *nx_OUT, *ny_OUT);
}




/* 3-D vector operation */

void AutoGL_AssignVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x, double y, double z)
{
  *x_OUT = x;
  *y_OUT = y;
  *z_OUT = z;
}

void AutoGL_AddVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x0, double y0, double z0,
 double x1, double y1, double z1)
{
  *x_OUT = x0 + x1;
  *y_OUT = y0 + y1;
  *z_OUT = z0 + z1;
}

void AutoGL_SubtractVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x0, double y0, double z0,
 double x1, double y1, double z1)
{
  *x_OUT = x0 - x1;
  *y_OUT = y0 - y1;
  *z_OUT = z0 - z1;
}

void AutoGL_MultiplyVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x, double y, double z,
 double scale)
{
  *x_OUT = x * scale;
  *y_OUT = y * scale;
  *z_OUT = z * scale;
}

double AutoGL_GetVectorLength3D 
(double x, double y, double z)
{
  return sqrt (x * x + y * y + z * z);
}

void AutoGL_NormalizeVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x, double y, double z)
{
  double length = AutoGL_GetVectorLength3D (x, y, z);

  if (length == 0.0) length = 1.0;
  *x_OUT = x / length;
  *y_OUT = y / length;
  *z_OUT = z / length;
}

double AutoGL_GetScalarProduct3D 
(double x0, double y0, double z0,
 double x1, double y1, double z1)
{
  return x0 * x1 + y0 * y1 + z0 * z1;
}

void AutoGL_GetVectorProduct3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x0, double y0, double z0,
 double x1, double y1, double z1)
{
  *x_OUT = y0 * z1 - z0 * y1;
  *y_OUT = z0 * x1 - x0 * z1;
  *z_OUT = x0 * y1 - y0 * x1;
}

double AutoGL_GetVectorDistance3D 
(double x0, double y0, double z0,
 double x1, double y1, double z1)
{
  double x, y, z;
  AutoGL_SubtractVector3D (&x, &y, &z,
			   x0, y0, z0,
			   x1, y1, z1);
  return AutoGL_GetVectorLength3D (x, y, z);
}

void AutoGL_GetMinVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x0, double y0, double z0,
 double x1, double y1, double z1)
{
  *x_OUT = (x0 < x1) ? x0 : x1;
  *y_OUT = (y0 < y1) ? y0 : y1;
  *z_OUT = (z0 < z1) ? z0 : z1;
}

void AutoGL_GetMaxVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x0, double y0, double z0,
 double x1, double y1, double z1)
{
  *x_OUT = (x0 < x1) ? x1 : x0;
  *y_OUT = (y0 < y1) ? y1 : y0;
  *z_OUT = (z0 < z1) ? z1 : z0;
}

void AutoGL_GetAnyPerpendicularDirection3D 
(double *nx0_OUT, double *ny0_OUT, double *nz0_OUT,
 double *nx1_OUT, double *ny1_OUT, double *nz1_OUT,
 double x, double y, double z)
{
  double length;

  AutoGL_GetVectorProduct3D (nx0_OUT, ny0_OUT, nz0_OUT,
			     x, y, z,
			     1, 0, 0);
  length = AutoGL_GetVectorLength3D (*nx0_OUT, *ny0_OUT, *nz0_OUT);
  if (length < Tolerance) {
    AutoGL_GetVectorProduct3D (nx0_OUT, ny0_OUT, nz0_OUT,
			       x, y, z,
			       0, 1, 0);
  }
  AutoGL_NormalizeVector3D (nx0_OUT, ny0_OUT, nz0_OUT,
			    *nx0_OUT, *ny0_OUT, *nz0_OUT);

  AutoGL_GetVectorProduct3D (nx1_OUT, ny1_OUT, nz1_OUT,
			     x, y, z,
			     *nx0_OUT, *ny0_OUT, *nz0_OUT);
  AutoGL_NormalizeVector3D (nx1_OUT, ny1_OUT, nz1_OUT,
			    *nx1_OUT, *ny1_OUT, *nz1_OUT);
}




