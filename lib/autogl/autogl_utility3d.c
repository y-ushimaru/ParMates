/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_utility3d.c : AutoGL utility for 3-D application */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "autogl_os.h"
#include "autogl_math.h"
#include "autogl_uims.h"
#include "autogl_graphics.h"
#include "autogl_gui.h"
#include "autogl_cui.h"

#include "autogl_utility3d.h"



/* 3-D Viewing Parameter Handling */

static double Size = 100.0;

static double CenterX = 0.0;
static double CenterY = 0.0;
static double CenterZ = 0.0;

static double DirectionX = 0.0;
static double DirectionY = 0.0;
static double DirectionZ = 1.0;

static double UpVectorX = 0.0;
static double UpVectorY = 1.0;
static double UpVectorZ = 0.0;

static void UpdateView (void) 
{
  AutoGL_SetViewSize (Size);
  AutoGL_SetViewCenter (CenterX, CenterY, CenterZ);       
  AutoGL_SetViewDirection (DirectionX, DirectionY, DirectionZ);
  AutoGL_SetViewUpVector (UpVectorX, UpVectorY, UpVectorZ);
  AutoGL_DrawView ();  
}

static void GetViewParameter (void) 
{
  Size = AutoGL_GetViewSize ();
  AutoGL_GetViewCenter (&CenterX, &CenterY, &CenterZ);       
  AutoGL_GetViewDirection (&DirectionX, &DirectionY, &DirectionZ);
  AutoGL_GetViewUpVector (&UpVectorX, &UpVectorY, &UpVectorZ);

  /* NEED TO REDEFINE */
  /* update panel */
}

void AutoGL_SetPanelInView3D (void)
{
  GetViewParameter ();

  AutoGL_AddComment ();
  AutoGL_SetLabel ("VIEW");

  AutoGL_AddReal (&Size, "AutoGL_Size");               
  AutoGL_SetLabel ("size");               
  AutoGL_AddReal (&CenterX, "AutoGL_CenterX");        
  AutoGL_SetLabel ("center x");               
  AutoGL_AddReal (&CenterY, "AutoGL_CenterY");      
  AutoGL_SetLabel ("center y");               
  AutoGL_AddReal (&CenterZ, "AutoGL_CenterZ"); 
  AutoGL_SetLabel ("center z");               
  AutoGL_AddReal (&DirectionX, "AutoGL_DirectionX");  
  AutoGL_SetLabel ("dir x");               
  AutoGL_AddReal (&DirectionY, "AutoGL_DirectionY");
  AutoGL_SetLabel ("dir y");               
  AutoGL_AddReal (&DirectionZ, "AutoGL_DirectionZ");
  AutoGL_SetLabel ("dir z");               
  AutoGL_AddReal (&UpVectorX, "AutoGL_UpVectorX");   
  AutoGL_SetLabel ("up x");               
  AutoGL_AddReal (&UpVectorY, "AutoGL_UpVectorY");
  AutoGL_SetLabel ("up y");               
  AutoGL_AddReal (&UpVectorZ, "AutoGL_UpVectorZ");
  AutoGL_SetLabel ("up z");               

  AutoGL_AddCallback (UpdateView, "AutoGL_UpdateView");
  AutoGL_SetLabel ("update view");               
  AutoGL_AddCallback (GetViewParameter, "AutoGL_GetViewParameter");
  AutoGL_SetLabel ("get params");               
}

void AutoGL_RotateView 
(double offsetX, double offsetY, double offsetZ)
{
  double upX, upY, upZ;
  double xAxisX, xAxisY, xAxisZ;
  double yAxisX, yAxisY, yAxisZ;
  double zAxisX, zAxisY, zAxisZ;

  AutoGL_GetViewUpVector (&upX, &upY, &upZ);
  AutoGL_GetViewDirection (&zAxisX, &zAxisY, &zAxisZ);

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

  {
    double x, y, z;
    AutoGL_SubtractVector3D (&x, &y, &z,
			     zAxisX, zAxisY, zAxisZ,
			     xAxisX * offsetX + yAxisX * offsetY,
			     xAxisY * offsetX + yAxisY * offsetY,
			     xAxisZ * offsetX + yAxisZ * offsetY);
    AutoGL_SetViewDirection (x, y, z);
  }
  
  {
    double x, y, z;
    AutoGL_AddVector3D (&x, &y, &z,
			yAxisX, yAxisY, yAxisZ,
			zAxisX * offsetY + xAxisX * offsetZ, 
			zAxisY * offsetY + xAxisY * offsetZ, 
			zAxisZ * offsetY + xAxisZ * offsetZ);
    AutoGL_SetViewUpVector (x, y, z);
  }

}



/* 3-D Drawing Primitive */

void AutoGL_DrawBox3D 
(double p0X, double p0Y, double p0Z,
 double p1X, double p1Y, double p1Z)
{
  AutoGL_DrawLine (p0X, p0Y, p0Z,
		   p1X, p0Y, p0Z);
  AutoGL_DrawLine (p1X, p0Y, p0Z,
		   p1X, p1Y, p0Z);
  AutoGL_DrawLine (p1X, p1Y, p0Z,
		   p0X, p1Y, p0Z);
  AutoGL_DrawLine (p0X, p1Y, p0Z,
		   p0X, p0Y, p0Z);

  AutoGL_DrawLine (p0X, p0Y, p1Z,
		   p1X, p0Y, p1Z);
  AutoGL_DrawLine (p1X, p0Y, p1Z,
		   p1X, p1Y, p1Z);
  AutoGL_DrawLine (p1X, p1Y, p1Z,
		   p0X, p1Y, p1Z);
  AutoGL_DrawLine (p0X, p1Y, p1Z,
		   p0X, p0Y, p1Z);

  AutoGL_DrawLine (p0X, p0Y, p0Z,
		   p0X, p0Y, p1Z);
  AutoGL_DrawLine (p1X, p0Y, p0Z,
		   p1X, p0Y, p1Z);
  AutoGL_DrawLine (p1X, p1Y, p0Z,
		   p1X, p1Y, p1Z);
  AutoGL_DrawLine (p0X, p1Y, p0Z,
		   p0X, p1Y, p1Z);
}

void AutoGL_DrawCrossMark3D 
(double pX, double pY, double pZ,
 int sizeDc)
{
  double size = sizeDc * AutoGL_GetViewSize () / AutoGL_GetViewSizeDc ();

  AutoGL_DrawLine (pX - size, pY, pZ,
		   pX + size, pY, pZ);
  AutoGL_DrawLine (pX, pY - size, pZ,
		   pX, pY + size, pZ);
  AutoGL_DrawLine (pX, pY, pZ - size,
		   pX, pY, pZ + size);
}

void AutoGL_DrawDiamondMark3D 
(double pX, double pY, double pZ,
 int sizeDc, int isFilled)
{
  double size = sizeDc * AutoGL_GetViewSize () / AutoGL_GetViewSizeDc ();

  if (isFilled) {

    AutoGL_DrawTriangle (pX + size, pY, pZ,
			 pX, pY + size, pZ,
			 pX, pY, pZ + size);
    AutoGL_DrawTriangle (pX, pY + size, pZ,
			 pX - size, pY, pZ,
			 pX, pY, pZ + size);
    AutoGL_DrawTriangle (pX - size, pY, pZ,
			 pX, pY - size, pZ,
			 pX, pY, pZ + size);
    AutoGL_DrawTriangle (pX, pY - size, pZ,
			 pX + size, pY, pZ,
			 pX, pY, pZ + size);

    AutoGL_DrawTriangle (pX + size, pY, pZ,
			 pX, pY + size, pZ,
			 pX, pY, pZ - size);
    AutoGL_DrawTriangle (pX, pY + size, pZ,
			 pX - size, pY, pZ,
			 pX, pY, pZ - size);
    AutoGL_DrawTriangle (pX - size, pY, pZ,
			 pX, pY - size, pZ,
			 pX, pY, pZ - size);
    AutoGL_DrawTriangle (pX, pY - size, pZ,
			 pX + size, pY, pZ,
			 pX, pY, pZ - size);

  } else {

    AutoGL_DrawLine (pX, pY + size, pZ,
		     pX, pY, pZ + size);
    AutoGL_DrawLine (pX, pY, pZ + size,
		     pX, pY - size, pZ);
    AutoGL_DrawLine (pX, pY - size, pZ,
		     pX, pY, pZ - size);
    AutoGL_DrawLine (pX, pY, pZ - size,
		     pX, pY + size, pZ);
    
    AutoGL_DrawLine (pX, pY, pZ + size,
		     pX + size, pY, pZ);
    AutoGL_DrawLine (pX + size, pY, pZ,
		     pX, pY, pZ - size);
    AutoGL_DrawLine (pX, pY, pZ - size,
		     pX - size, pY, pZ);
    AutoGL_DrawLine (pX - size, pY, pZ,
		     pX, pY, pZ + size);
    
    AutoGL_DrawLine (pX + size, pY, pZ,
		     pX, pY + size, pZ);
    AutoGL_DrawLine (pX, pY + size, pZ,
		     pX - size, pY, pZ);
    AutoGL_DrawLine (pX - size, pY, pZ,
		     pX, pY - size, pZ);
    AutoGL_DrawLine (pX, pY - size, pZ,
		     pX + size, pY, pZ);

  }
}

void AutoGL_DrawCubeMark3D 
(double pX, double pY, double pZ,
 int sizeDc, int isFilled)
{
  double size = sizeDc * AutoGL_GetViewSize () / AutoGL_GetViewSizeDc ();

  if (isFilled) {

    AutoGL_DrawQuadrangle (pX - size, pY - size, pZ - size,
			   pX + size, pY - size, pZ - size,
			   pX + size, pY + size, pZ - size,
			   pX - size, pY + size, pZ - size);
    AutoGL_DrawQuadrangle (pX - size, pY - size, pZ + size,
			   pX + size, pY - size, pZ + size,
			   pX + size, pY + size, pZ + size,
			   pX - size, pY + size, pZ + size);
    AutoGL_DrawQuadrangle (pX - size, pY - size, pZ - size,
			   pX - size, pY + size, pZ - size,
			   pX - size, pY + size, pZ + size,
			   pX - size, pY - size, pZ + size);
    AutoGL_DrawQuadrangle (pX + size, pY - size, pZ - size,
			   pX + size, pY + size, pZ - size,
			   pX + size, pY + size, pZ + size,
			   pX + size, pY - size, pZ + size);
    AutoGL_DrawQuadrangle (pX - size, pY - size, pZ - size,
			   pX + size, pY - size, pZ - size,
			   pX + size, pY - size, pZ + size,
			   pX - size, pY - size, pZ + size);
    AutoGL_DrawQuadrangle (pX - size, pY + size, pZ - size,
			   pX + size, pY + size, pZ - size,
			   pX + size, pY + size, pZ + size,
			   pX - size, pY + size, pZ + size);

  } else {

    AutoGL_DrawLine (pX - size, pY - size, pZ - size,
		     pX + size, pY - size, pZ - size);
    AutoGL_DrawLine (pX - size, pY + size, pZ - size,
		     pX + size, pY + size, pZ - size);
    AutoGL_DrawLine (pX - size, pY + size, pZ + size,
		     pX + size, pY + size, pZ + size);
    AutoGL_DrawLine (pX - size, pY - size, pZ + size,
		     pX + size, pY - size, pZ + size);
    
    AutoGL_DrawLine (pX - size, pY - size, pZ - size,
		     pX - size, pY + size, pZ - size);
    AutoGL_DrawLine (pX - size, pY - size, pZ + size,
		     pX - size, pY + size, pZ + size);
    AutoGL_DrawLine (pX + size, pY - size, pZ + size,
		     pX + size, pY + size, pZ + size);
    AutoGL_DrawLine (pX + size, pY - size, pZ - size,
		     pX + size, pY + size, pZ - size);
    
    AutoGL_DrawLine (pX - size, pY - size, pZ - size,
		     pX - size, pY - size, pZ + size);
    AutoGL_DrawLine (pX + size, pY - size, pZ - size,
		     pX + size, pY - size, pZ + size);
    AutoGL_DrawLine (pX + size, pY + size, pZ - size,
		     pX + size, pY + size, pZ + size);
    AutoGL_DrawLine (pX - size, pY + size, pZ - size,
		     pX - size, pY + size, pZ + size);

  }
}

void AutoGL_DrawArrow3D 
(double x0, double y0, double z0,
 double x1, double y1, double z1, 
 double tipRatio)
{
  double length = AutoGL_GetVectorLength3D 
    (x1 - x0, y1 - y0, z1 - z0);
  double tipSize = length * tipRatio;
  double nx, ny, nz;
  double px, py, pz;
  double qx, qy, qz;
  
  AutoGL_DrawLine (x0, y0, z0,
		   x1, y1, z1);
  
  if (length == 0.0) {
    return;
  } 
  
  AutoGL_NormalizeVector3D 
    (&nx, &ny, &nz,
     x1 - x0, y1 - y0, z1 - z0);
  AutoGL_GetAnyPerpendicularDirection3D 
    (&px, &py, &pz,
     &qx, &qy, &qz,
     nx, ny, nz);

  AutoGL_DrawTriangle 
    (x1, y1, z1,
     x1 - nx * tipSize - px * tipSize * 0.5, 
     y1 - ny * tipSize - py * tipSize * 0.5, 
     z1 - nz * tipSize - pz * tipSize * 0.5,
     x1 - nx * tipSize + px * tipSize * 0.5, 
     y1 - ny * tipSize + py * tipSize * 0.5, 
     z1 - nz * tipSize + pz * tipSize * 0.5);
  AutoGL_DrawTriangle 
    (x1, y1, z1,
     x1 - nx * tipSize - qx * tipSize * 0.5, 
     y1 - ny * tipSize - qy * tipSize * 0.5, 
     z1 - nz * tipSize - qz * tipSize * 0.5,
     x1 - nx * tipSize + qx * tipSize * 0.5, 
     y1 - ny * tipSize + qy * tipSize * 0.5, 
     z1 - nz * tipSize + qz * tipSize * 0.5);
}

void AutoGL_DrawConeArrow3D 
(double x0, double y0, double z0,
 double x1, double y1, double z1, 
 double tipRatio)
{
  double length = AutoGL_GetVectorLength3D 
    (x1 - x0, y1 - y0, z1 - z0);
  double tipSize = length * tipRatio;
  double nx, ny, nz;
  
  AutoGL_DrawLine (x0, y0, z0,
		   x1, y1, z1);
  
  if (length == 0.0) {
    return;
  } 
  
  AutoGL_NormalizeVector3D 
    (&nx, &ny, &nz,
     x1 - x0, y1 - y0, z1 - z0);

  AutoGL_DrawCone3D 
    (x1 - nx * tipSize, 
     y1 - ny * tipSize, 
     z1 - nz * tipSize,
     x1, y1, z1,
     tipSize * 0.5, 
     2);
}

void AutoGL_DrawCoordinateSystem3D 
(int sizeDc)
{
  int widthDc, heightDc;
  int centerXDc, centerYDc;
  double centerX, centerY, centerZ;
  double axisLength;
  
  AutoGL_GetViewRangeDc (&widthDc, &heightDc);
  
  centerXDc = -widthDc / 2 + sizeDc;
  centerYDc = -heightDc / 2 + sizeDc;

  AutoGL_GetPositionOfDc (&centerX, &centerY, &centerZ,
			  centerXDc, centerYDc, 0);
  axisLength = sizeDc * AutoGL_GetViewSize () / AutoGL_GetViewSizeDc ();

  AutoGL_SetColor (1, 1, 1);
  AutoGL_DrawLine (centerX, centerY, centerZ,
		   centerX + axisLength, centerY, centerZ); 
  AutoGL_DrawString (centerX + axisLength, centerY, centerZ, "X");  
  AutoGL_DrawLine (centerX, centerY, centerZ,
		   centerX, centerY + axisLength, centerZ); 
  AutoGL_DrawString (centerX, centerY + axisLength, centerZ, "Y");  
  AutoGL_DrawLine (centerX, centerY, centerZ,
		   centerX, centerY, centerZ + axisLength); 
  AutoGL_DrawString (centerX, centerY, centerZ + axisLength, "Z");  
}

void AutoGL_DrawSphere3D 
(double centerX, double centerY, double centerZ,
 double radius, 
 int nDivisions)
{
  int i, j;
  
  for (i = 0; i < nDivisions * 4; i++) {
    double s0 = M_PI * 2 / (nDivisions * 4) * i;
    double s1 = M_PI * 2 / (nDivisions * 4) * (i + 1);
    
    for (j = 0; j < nDivisions * 2; j++) {
      double t0 = M_PI / (nDivisions * 2) * j;
      double t1 = M_PI / (nDivisions * 2) * (j + 1);

      double cos_s0 = cos (s0);
      double sin_s0 = sin (s0); 
      double cos_s1 = cos (s1);
      double sin_s1 = sin (s1); 
      double cos_t0 = cos (t0); 
      double sin_t0 = sin (t0);
      double cos_t1 = cos (t1); 
      double sin_t1 = sin (t1);
      
      double nx00 = cos_s0 * sin_t0;
      double ny00 = sin_s0 * sin_t0;
      double nz00 = cos_t0;
      double x00 = centerX + radius * nx00;
      double y00 = centerY + radius * ny00;
      double z00 = centerZ + radius * nz00;
      
      double nx01 = cos_s0 * sin_t1;
      double ny01 = sin_s0 * sin_t1;
      double nz01 = cos_t1;
      double x01 = centerX + radius * nx01;
      double y01 = centerY + radius * ny01;
      double z01 = centerZ + radius * nz01;

      double nx10 = cos_s1 * sin_t0;
      double ny10 = sin_s1 * sin_t0;
      double nz10 = cos_t0;
      double x10 = centerX + radius * nx10;
      double y10 = centerY + radius * ny10;
      double z10 = centerZ + radius * nz10;

      double nx11 = cos_s1 * sin_t1;
      double ny11 = sin_s1 * sin_t1;
      double nz11 = cos_t1;
      double x11 = centerX + radius * nx11;
      double y11 = centerY + radius * ny11;
      double z11 = centerZ + radius * nz11;

      AutoGL_DrawNormalInterpolatedTriangle 
	(x00, y00, z00, nx00, ny00, nz00,
	 x10, y10, z10, nx10, ny10, nz10,
	 x11, y11, z11, nx11, ny11, nz11);
      AutoGL_DrawNormalInterpolatedTriangle 
	(x11, y11, z11, nx11, ny11, nz11,
	 x01, y01, z01, nx01, ny01, nz01,
	 x00, y00, z00, nx00, ny00, nz00);
    }
  }
}

void AutoGL_DrawCylinder3D 
(double bottomX, double bottomY, double bottomZ,
 double topX, double topY, double topZ,
 double radius, 
 int nDivisions)
{
  double zOffsetX, zOffsetY, zOffsetZ;
  double xDirX, xDirY, xDirZ;
  double yDirX, yDirY, yDirZ;
  double xOffsetX, xOffsetY, xOffsetZ;
  double yOffsetX, yOffsetY, yOffsetZ;
  int i;

  AutoGL_SubtractVector3D
    (&zOffsetX, &zOffsetY, &zOffsetZ,
     topX, topY, topZ,
     bottomX, bottomY, bottomZ);
  AutoGL_GetAnyPerpendicularDirection3D 
    (&xDirX, &xDirY, &xDirZ,
     &yDirX, &yDirY, &yDirZ,
     zOffsetX, zOffsetY, zOffsetZ);
  AutoGL_MultiplyVector3D
    (&xOffsetX, &xOffsetY, &xOffsetZ,
     xDirX, xDirY, xDirZ,
     radius);
  AutoGL_MultiplyVector3D
    (&yOffsetX, &yOffsetY, &yOffsetZ,
     yDirX, yDirY, yDirZ,
     radius);

  for (i = 0; i < nDivisions * 4; i++) {
    double t0 = M_PI * 2 / (nDivisions * 4) * i;
    double t1 = M_PI * 2 / (nDivisions * 4) * (i + 1);
    double sin_t0 = sin (t0);
    double sin_t1 = sin (t1);
    double cos_t0 = cos (t0);
    double cos_t1 = cos (t1);
    double ox0, oy0, oz0;
    double ox1, oy1, oz1;
    double bx0, by0, bz0;
    double bx1, by1, bz1;
    double tx0, ty0, tz0;
    double tx1, ty1, tz1;

    ox0 = xOffsetX * cos_t0 + yOffsetX * sin_t0;
    oy0 = xOffsetY * cos_t0 + yOffsetY * sin_t0;
    oz0 = xOffsetZ * cos_t0 + yOffsetZ * sin_t0;
    ox1 = xOffsetX * cos_t1 + yOffsetX * sin_t1;
    oy1 = xOffsetY * cos_t1 + yOffsetY * sin_t1;
    oz1 = xOffsetZ * cos_t1 + yOffsetZ * sin_t1;
    AutoGL_AddVector3D
      (&bx0, &by0, &bz0,
       bottomX, bottomY, bottomZ,
       ox0, oy0, oz0);
    AutoGL_AddVector3D
      (&bx1, &by1, &bz1,
       bottomX, bottomY, bottomZ,
       ox1, oy1, oz1);
    AutoGL_AddVector3D
      (&tx0, &ty0, &tz0,
       topX, topY, topZ,
       ox0, oy0, oz0);
    AutoGL_AddVector3D
      (&tx1, &ty1, &tz1,
       topX, topY, topZ,
       ox1, oy1, oz1);

    AutoGL_DrawNormalInterpolatedTriangle 
      (bx0, by0, bz0, ox0, oy0, oz0, 
       bx1, by1, bz1, ox1, oy1, oz1, 
       tx1, ty1, tz1, ox1, oy1, oz1);
    AutoGL_DrawNormalInterpolatedTriangle 
      (tx1, ty1, tz1, ox1, oy1, oz1, 
       tx0, ty0, tz0, ox0, oy0, oz0,
       bx0, by0, bz0, ox0, oy0, oz0);
  }
}

void AutoGL_DrawCone3D 
(double bottomX, double bottomY, double bottomZ,
 double topX, double topY, double topZ,
 double radius, 
 int nDivisions)
{
  double zOffsetX, zOffsetY, zOffsetZ;
  double xDirX, xDirY, xDirZ;
  double yDirX, yDirY, yDirZ;
  double xOffsetX, xOffsetY, xOffsetZ;
  double yOffsetX, yOffsetY, yOffsetZ;
  int i;

  AutoGL_SubtractVector3D
    (&zOffsetX, &zOffsetY, &zOffsetZ,
     topX, topY, topZ,
     bottomX, bottomY, bottomZ);
  AutoGL_GetAnyPerpendicularDirection3D 
    (&xDirX, &xDirY, &xDirZ,
     &yDirX, &yDirY, &yDirZ,
     zOffsetX, zOffsetY, zOffsetZ);
  AutoGL_MultiplyVector3D
    (&xOffsetX, &xOffsetY, &xOffsetZ,
     xDirX, xDirY, xDirZ,
     radius);
  AutoGL_MultiplyVector3D
    (&yOffsetX, &yOffsetY, &yOffsetZ,
     yDirX, yDirY, yDirZ,
     radius);

  for (i = 0; i < nDivisions * 4; i++) {
    double t0 = M_PI * 2 / (nDivisions * 4) * i;
    double t1 = M_PI * 2 / (nDivisions * 4) * (i + 1);
    double sin_t0 = sin (t0);
    double sin_t1 = sin (t1);
    double cos_t0 = cos (t0);
    double cos_t1 = cos (t1);
    double ox0 = xOffsetX * cos_t0 + yOffsetX * sin_t0;
    double oy0 = xOffsetY * cos_t0 + yOffsetY * sin_t0;
    double oz0 = xOffsetZ * cos_t0 + yOffsetZ * sin_t0;
    double ox1 = xOffsetX * cos_t1 + yOffsetX * sin_t1;
    double oy1 = xOffsetY * cos_t1 + yOffsetY * sin_t1;
    double oz1 = xOffsetZ * cos_t1 + yOffsetZ * sin_t1;
    double bx0, by0, bz0;
    double bx1, by1, bz1;
    double vpx0, vpy0, vpz0;
    double vpx1, vpy1, vpz1;
    double nx0, ny0, nz0;
    double nx1, ny1, nz1;
    double nxt, nyt, nzt;

    AutoGL_AddVector3D
      (&bx0, &by0, &bz0,
       bottomX, bottomY, bottomZ,
       ox0, oy0, oz0);
    AutoGL_AddVector3D
      (&bx1, &by1, &bz1,
       bottomX, bottomY, bottomZ,
       ox1, oy1, oz1);

    AutoGL_GetVectorProduct3D
      (&vpx0, &vpy0, &vpz0, 
       ox0, oy0, oz0,
       zOffsetX, zOffsetY, zOffsetZ);
    AutoGL_GetVectorProduct3D
      (&nx0, &ny0, &nz0, 
       zOffsetX - ox0, zOffsetY - oy0, zOffsetZ - oz0, 
       vpx0, vpy0, vpz0);

    AutoGL_GetVectorProduct3D
      (&vpx1, &vpy1, &vpz1, 
       ox1, oy1, oz1,
       zOffsetX, zOffsetY, zOffsetZ);
    AutoGL_GetVectorProduct3D
      (&nx1, &ny1, &nz1, 
       zOffsetX - ox1, zOffsetY - oy1, zOffsetZ - oz1, 
       vpx1, vpy1, vpz1);

    nxt = (nx0 + nx1) / 2;
    nyt = (ny0 + ny1) / 2;
    nzt = (nz0 + nz1) / 2;

    AutoGL_DrawNormalInterpolatedTriangle 
      (bx0, by0, bz0, nx0, ny0, nz0, 
       bx1, by1, bz1, nx1, ny1, nz1, 
       topX, topY, topZ, nxt, nyt, nzt);
  }
}

void AutoGL_DrawCircle3D 
(double centerX, double centerY, double centerZ,
 double normalX, double normalY, double normalZ,
 double radius, 
 int nDivisions)
{
  double xDirX, xDirY, xDirZ;
  double yDirX, yDirY, yDirZ;
  double xOffsetX, xOffsetY, xOffsetZ;
  double yOffsetX, yOffsetY, yOffsetZ;
  int i;

  AutoGL_GetAnyPerpendicularDirection3D 
    (&xDirX, &xDirY, &xDirZ,
     &yDirX, &yDirY, &yDirZ,
     normalX, normalY, normalZ);
  AutoGL_MultiplyVector3D
    (&xOffsetX, &xOffsetY, &xOffsetZ,
     xDirX, xDirY, xDirZ,
     radius);
  AutoGL_MultiplyVector3D
    (&yOffsetX, &yOffsetY, &yOffsetZ,
     yDirX, yDirY, yDirZ,
     radius);

  for (i = 0; i < nDivisions * 4; i++) {
    double t0 = M_PI * 2 / (nDivisions * 4) * i;
    double t1 = M_PI * 2 / (nDivisions * 4) * (i + 1);
    double ox0, oy0, oz0;
    double ox1, oy1, oz1;
    double x0, y0, z0;
    double x1, y1, z1;

    ox0 = xOffsetX * cos (t0) + yOffsetX * sin (t0);
    oy0 = xOffsetY * cos (t0) + yOffsetY * sin (t0);
    oz0 = xOffsetZ * cos (t0) + yOffsetZ * sin (t0);
    ox1 = xOffsetX * cos (t1) + yOffsetX * sin (t1);
    oy1 = xOffsetY * cos (t1) + yOffsetY * sin (t1);
    oz1 = xOffsetZ * cos (t1) + yOffsetZ * sin (t1);
    AutoGL_AddVector3D
      (&x0, &y0, &z0,
       centerX, centerY, centerZ,
       ox0, oy0, oz0);
    AutoGL_AddVector3D
      (&x1, &y1, &z1,
       centerX, centerY, centerZ,
       ox1, oy1, oz1);

    AutoGL_DrawTriangle (x0, y0, z0,
			 x1, y1, z1,
			 centerX, centerY, centerZ);
  }
}

void AutoGL_FillBox3D 
(double x0, double y0, double z0,
 double x1, double y1, double z1)
{
  AutoGL_DrawQuadrangle 
    (x0, y0, z0, 
     x1, y0, z0, 
     x1, y1, z0, 
     x0, y1, z0);
  AutoGL_DrawQuadrangle 
    (x0, y0, z1, 
     x1, y0, z1, 
     x1, y1, z1, 
     x0, y1, z1);

  AutoGL_DrawQuadrangle 
    (x0, y0, z0, 
     x0, y1, z0, 
     x0, y1, z1, 
     x0, y0, z1);
  AutoGL_DrawQuadrangle 
    (x1, y0, z0, 
     x1, y1, z0, 
     x1, y1, z1, 
     x1, y0, z1);
  
  AutoGL_DrawQuadrangle 
    (x0, y0, z0, 
     x0, y0, z1, 
     x1, y0, z1, 
     x1, y0, z0);
  AutoGL_DrawQuadrangle 
    (x0, y1, z0, 
     x0, y1, z1, 
     x1, y1, z1, 
     x1, y1, z0);
}



/* 3-D Event Handler */

static int Mode = AUTOGL_MODE_3D_ROTATE_XY;    /*  AUTOGL_MODE_3D_  */
static AutoGL_CallbackType OtherCallback;
static int DragIsOn;

void AutoGL_SetMode3D (int mode)    /*  AUTOGL_MODE_3D_  */
{
  Mode = mode;
}

int AutoGL_GetMode3D (void)         /*  AUTOGL_MODE_3D_  */
{
  return Mode;
}

void AutoGL_EnableDragInMode3D (void)
{
  DragIsOn = 1;
}

void AutoGL_HandleDefaultKeyEventInMode3D (void)
{
  int keyChar = AutoGL_GetKeyChar ();
  
  switch (keyChar) {
  case 'o':
  case 'O':
    AutoGL_SetMode3D (AUTOGL_MODE_3D_OTHER);
    break;
  case 't':
  case 'T':
    /* translate */
    AutoGL_SetMode3D (AUTOGL_MODE_3D_TRANSLATE);
    break;
  case 'r':
  case 'R':
    /* rotate direction */
    AutoGL_SetMode3D (AUTOGL_MODE_3D_ROTATE_XY);
    break;
  case 'u':
  case 'U':
    /* rotate up vector */
    AutoGL_SetMode3D (AUTOGL_MODE_3D_ROTATE_Z);
    break;
  case 's':
  case 'S':
    /* scale */
    AutoGL_SetMode3D (AUTOGL_MODE_3D_SCALE);
    break;
  default:
    break;
  }
}

static void Translate (void)
{
  static double oldX, oldY, oldZ;
  static double oldCenterX, oldCenterY, oldCenterZ;
  int event = AutoGL_GetViewEvent ();

  switch (event) {
  case AUTOGL_EVENT_POINTING_DEVICE_PRESS:
    {
      int dcX, dcY, dcZ;

      AutoGL_GetPointingDevicePositionDc (&dcX, &dcY, &dcZ);
      AutoGL_GetPositionOfDc (&oldX, &oldY, &oldZ,
			      dcX, dcY, dcZ);
      AutoGL_GetViewCenter (&oldCenterX, &oldCenterY, &oldCenterZ);
    }
    break;
  case AUTOGL_EVENT_POINTING_DEVICE_DRAG:
  case AUTOGL_EVENT_POINTING_DEVICE_RELEASE:
    if ((DragIsOn && event == AUTOGL_EVENT_POINTING_DEVICE_DRAG)
	|| (!DragIsOn && event == AUTOGL_EVENT_POINTING_DEVICE_RELEASE)) {
      int dcX, dcY, dcZ;
      double x, y, z;
      double offsetX, offsetY, offsetZ;
      double centerX, centerY, centerZ;

      AutoGL_GetPointingDevicePositionDc (&dcX, &dcY, &dcZ);
      AutoGL_SetViewCenter (oldCenterX, oldCenterY, oldCenterZ);
      AutoGL_GetPositionOfDc (&x, &y, &z,
			      dcX, dcY, dcZ);
      AutoGL_SubtractVector3D (&offsetX, &offsetY, &offsetZ,
			       x, y, z,
			       oldX, oldY, oldZ);
      AutoGL_SubtractVector3D (&centerX, &centerY, &centerZ,
			       oldCenterX, oldCenterY, oldCenterZ,
			       offsetX, offsetY, offsetZ);
      AutoGL_SetViewCenter (centerX, centerY, centerZ);
      AutoGL_DrawView ();
    }
    break;
  case AUTOGL_EVENT_KEY:
    AutoGL_HandleDefaultKeyEventInMode3D ();
    break;
  default:
    break;
  }
}

static void RotateXY (void)
{
  static int oldDcX, oldDcY, oldDcZ;
  int event = AutoGL_GetViewEvent ();

  switch (event) {
  case AUTOGL_EVENT_POINTING_DEVICE_PRESS:
    AutoGL_GetPointingDevicePositionDc (&oldDcX, &oldDcY, &oldDcZ);
    break;
  case AUTOGL_EVENT_POINTING_DEVICE_DRAG:
  case AUTOGL_EVENT_POINTING_DEVICE_RELEASE:
    if ((DragIsOn && event == AUTOGL_EVENT_POINTING_DEVICE_DRAG)
	|| (!DragIsOn && event == AUTOGL_EVENT_POINTING_DEVICE_RELEASE)) {
      int dcX, dcY, dcZ;
      double offsetX, offsetY;

      AutoGL_GetPointingDevicePositionDc (&dcX, &dcY, &dcZ);
      offsetX = (dcX - oldDcX) / 200.0;
      offsetY = (dcY - oldDcY) / 200.0;
      AutoGL_RotateView (offsetX, offsetY, 0.0);
      AutoGL_DrawView ();
      oldDcX = dcX;
      oldDcY = dcY;
      oldDcZ = dcZ;
    }
    break;
  case AUTOGL_EVENT_KEY:
    AutoGL_HandleDefaultKeyEventInMode3D ();
    break;
  default:
    break;
  }
}

static void RotateZ (void)
{
  static int oldDcX, oldDcY, oldDcZ;
  int event = AutoGL_GetViewEvent ();

  switch (event) {
  case AUTOGL_EVENT_POINTING_DEVICE_PRESS:
    {
      AutoGL_GetPointingDevicePositionDc (&oldDcX, &oldDcY, &oldDcZ);
    }
    break;
  case AUTOGL_EVENT_POINTING_DEVICE_DRAG:
  case AUTOGL_EVENT_POINTING_DEVICE_RELEASE:
    if ((DragIsOn && event == AUTOGL_EVENT_POINTING_DEVICE_DRAG)
	|| (!DragIsOn && event == AUTOGL_EVENT_POINTING_DEVICE_RELEASE)) {
      int dcX, dcY, dcZ;
      double offsetX, offsetY, offsetZ;
      double offset;

      AutoGL_GetPointingDevicePositionDc (&dcX, &dcY, &dcZ);
      offsetX = (dcX - oldDcX) / 200.0;
      offsetY = (dcY - oldDcY) / 200.0;
      offsetZ = (dcZ - oldDcZ) / 200.0;
      offset = offsetX;
      if (fabs (offset) < fabs (offsetY)) offset = offsetY;
      if (fabs (offset) < fabs (offsetZ)) offset = offsetZ;
      AutoGL_RotateView (0, 0, offset);
      AutoGL_DrawView ();
      oldDcX = dcX;
      oldDcY = dcY;
      oldDcZ = dcZ;
    }
    break;
  case AUTOGL_EVENT_KEY:
    AutoGL_HandleDefaultKeyEventInMode3D ();
    break;
  default:
    break;
  }
}

static void Scale (void)
{
  static int oldDcX, oldDcY, oldDcZ;
  static double oldViewSize;
  int event = AutoGL_GetViewEvent ();

  switch (event) {
  case AUTOGL_EVENT_POINTING_DEVICE_PRESS:
    {
      AutoGL_GetPointingDevicePositionDc (&oldDcX, &oldDcY, &oldDcZ);
      oldViewSize = AutoGL_GetViewSize ();
    }
    break;
  case AUTOGL_EVENT_POINTING_DEVICE_DRAG:
  case AUTOGL_EVENT_POINTING_DEVICE_RELEASE:
    if ((DragIsOn && event == AUTOGL_EVENT_POINTING_DEVICE_DRAG)
	|| (!DragIsOn && event == AUTOGL_EVENT_POINTING_DEVICE_RELEASE)) {
      int dcX, dcY, dcZ;
      double scaleX, scaleY, scaleZ;
      double scale;

      AutoGL_GetPointingDevicePositionDc (&dcX, &dcY, &dcZ);
      scaleX = (dcX - oldDcX) / 500.0;
      scaleY = (dcY - oldDcY) / 500.0;
      scaleZ = (dcZ - oldDcZ) / 500.0;
      scale = scaleX;
      if (fabs (scale) < fabs (scaleY)) scale = scaleY;
      if (fabs (scale) < fabs (scaleZ)) scale = scaleZ;
      scale += 1.0;
      if (scale < 0.1) scale = 0.1;
      AutoGL_SetViewSize (oldViewSize / scale);
      AutoGL_DrawView ();
    }
    break;
  case AUTOGL_EVENT_KEY:
    AutoGL_HandleDefaultKeyEventInMode3D ();
    break;
  default:
    break;
  }
}

static void DefaultViewEventCallback (void) 
{
  switch (Mode) {
  case AUTOGL_MODE_3D_OTHER:
    if (OtherCallback != 0) {
      (*OtherCallback) ();
    }
    break;
  case AUTOGL_MODE_3D_TRANSLATE:
    Translate ();
    break;
  case AUTOGL_MODE_3D_ROTATE_XY:
    RotateXY ();
    break;
  case AUTOGL_MODE_3D_ROTATE_Z:
    RotateZ ();
    break;
  case AUTOGL_MODE_3D_SCALE:
    Scale ();
    break;
  default:
    assert(0);
    break;
  }
}

void AutoGL_SetPanelInMode3D (void)
{
  AutoGL_AddInteger (&Mode, "AutoGL_Mode3D"); 
  AutoGL_SetLabel ("mode");
  AutoGL_AddIntegerItem ("Other");              
  AutoGL_AddIntegerItem ("Translate");             
  AutoGL_AddIntegerItem ("Rotate_direction");         
  AutoGL_AddIntegerItem ("rotate_Up_vector");              
  AutoGL_AddIntegerItem ("Scale");              
}

static void DefaultOtherCallback (void)
{
  int event = AutoGL_GetViewEvent ();

  switch (event) {
  case AUTOGL_EVENT_KEY:
    AutoGL_HandleDefaultKeyEventInMode3D ();
    break;
  default:
    break;
  }
}

void AutoGL_SetDefaultCallbackInMode3D 
(AutoGL_CallbackType otherCallback)
{
  if (otherCallback == NULL) {
    OtherCallback = DefaultOtherCallback;
  } else {
    OtherCallback = otherCallback;
  }
  AutoGL_SetViewEventCallback (DefaultViewEventCallback);
}

int AutoGL_PointingDeviceIsHit3D 
(double *parameter_OUT,
 double pX, double pY, double pZ,
 int toleranceDc)
{
  int dcX, dcY, dcZ;
  double originX, originY, originZ;
  double dirX, dirY, dirZ;
  double x, y, z;
  double ptX, ptY, ptZ;
  double t;
  double distance;
  double tolerance;

  AutoGL_GetPointingDevicePositionDc (&dcX, &dcY, &dcZ);
  AutoGL_GetPositionOfDc (&originX, &originY, &originZ,
			  dcX, dcY, dcZ);
  AutoGL_GetViewDirection (&dirX, &dirY, &dirZ);
  
  AutoGL_SubtractVector3D (&x, &y, &z,
			   pX, pY, pZ,
			   originX, originY, originZ);
  t = AutoGL_GetScalarProduct3D (x, y, z,
				 dirX, dirY, dirZ);
  *parameter_OUT = t;

  AutoGL_AddVector3D (&ptX, &ptY, &ptZ,
		      originX, originY, originZ,
		      dirX * t, dirY * t, dirZ * t);
  distance = AutoGL_GetVectorDistance3D (pX, pY, pZ,
					 ptX, ptY, ptZ);
  tolerance = toleranceDc 
    * AutoGL_GetViewSize () / AutoGL_GetViewSizeDc ();
  return (distance <= tolerance);
}



