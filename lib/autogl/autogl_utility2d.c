/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_utility2d.c : AutoGL utility for 2-D application */

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

#include "autogl_utility2d.h"



/* 2-D Viewing Parameter Handling */

static double Size = 100.0;

static double CenterX = 0.0;
static double CenterY = 0.0;

static void UpdateView (void) 
{
  AutoGL_SetViewSize (Size);
  AutoGL_SetViewCenter (CenterX, CenterY, 0);       
  AutoGL_DrawView ();  
}

static void GetViewParameter (void) 
{
  double dummy;

  Size = AutoGL_GetViewSize ();
  AutoGL_GetViewCenter (&CenterX, &CenterY, &dummy);       

  /* NEED TO REDEFINE */
  /* update panel */
}

void AutoGL_SetPanelInView2D (void)
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

  AutoGL_AddCallback (UpdateView, "AutoGL_UpdateView");
  AutoGL_SetLabel ("update view");               
  AutoGL_AddCallback (GetViewParameter, "AutoGL_GetViewParameter");
  AutoGL_SetLabel ("get params");               
}



/* 2-D Drawing Primitive */

void AutoGL_DrawBox2D 
(double p0X, double p0Y,
 double p1X, double p1Y, 
 double z)
{
  AutoGL_DrawLine (p0X, p0Y, z,
		   p1X, p0Y, z);
  AutoGL_DrawLine (p1X, p0Y, z,
		   p1X, p1Y, z);
  AutoGL_DrawLine (p1X, p1Y, z,
		   p0X, p1Y, z);
  AutoGL_DrawLine (p0X, p1Y, z,
		   p0X, p0Y, z);
}

void AutoGL_DrawCrossMark2D 
(double pX, double pY, double pZ,
 int sizeDc)
{
  double size = sizeDc * AutoGL_GetViewSize () / AutoGL_GetViewSizeDc ();

  AutoGL_DrawLine (pX - size, pY, pZ,
		   pX + size, pY, pZ);
  AutoGL_DrawLine (pX, pY - size, pZ,
		   pX, pY + size, pZ);
}

void AutoGL_DrawDiamondMark2D 
(double pX, double pY, double pZ,
 int sizeDc, int isFilled)
{
  double size = sizeDc * AutoGL_GetViewSize () / AutoGL_GetViewSizeDc ();

  if (isFilled) {

    AutoGL_DrawQuadrangle (pX + size, pY, pZ,
			   pX, pY + size, pZ,
			   pX - size, pY, pZ,
			   pX, pY - size, pZ);

  } else {

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

void AutoGL_DrawSquareMark2D 
(double pX, double pY, double pZ,
 int sizeDc, int isFilled)
{
  double size = sizeDc * AutoGL_GetViewSize () / AutoGL_GetViewSizeDc ();

  if (isFilled) {

    AutoGL_DrawQuadrangle (pX - size, pY - size, pZ,
			   pX + size, pY - size, pZ,
			   pX + size, pY + size, pZ,
			   pX - size, pY + size, pZ);

  } else {

    AutoGL_DrawLine (pX - size, pY - size, pZ,
		     pX + size, pY - size, pZ);
    AutoGL_DrawLine (pX + size, pY - size, pZ,
		     pX + size, pY + size, pZ);
    AutoGL_DrawLine (pX + size, pY + size, pZ,
		     pX - size, pY + size, pZ);
    AutoGL_DrawLine (pX - size, pY + size, pZ,
		     pX - size, pY - size, pZ);

  }
}

void AutoGL_DrawArrow2D 
(double x0, double y0,
 double x1, double y1, 
 double z,
 double tipRatio)
{
  double length = AutoGL_GetVectorLength2D (x1 - x0, y1 - y0);
  double tipSize = length * tipRatio;
  double nx, ny;
  double px, py;
  
  AutoGL_DrawLine (x0, y0, z,
		   x1, y1, z);
  
  if (length == 0.0) {
    return;
  } 
  
  AutoGL_NormalizeVector2D 
    (&nx, &ny,
     x1 - x0, y1 - y0);
  AutoGL_GetAnyPerpendicularDirection2D 
    (&px, &py,
     nx, ny);
  
  AutoGL_DrawTriangle 
    (x1, y1, z,
     x1 - nx * tipSize - px * tipSize * 0.5, 
     y1 - ny * tipSize - py * tipSize * 0.5, 
     z,
     x1 - nx * tipSize + px * tipSize * 0.5, 
     y1 - ny * tipSize + py * tipSize * 0.5, 
     z);
}



/* 2-D Event Handler */

static int Mode = AUTOGL_MODE_2D_TRANSLATE;   /*  AUTOGL_MODE_2D_  */
static AutoGL_CallbackType OtherCallback;
static int DragIsOn;

void AutoGL_SetMode2D (int mode)   /*  AUTOGL_MODE_2D_  */
{
  Mode = mode;
}

int AutoGL_GetMode2D (void)        /*  AUTOGL_MODE_2D_  */
{
  return Mode;
}

void AutoGL_EnableDragInMode2D (void)
{
  DragIsOn = 1;
}

void AutoGL_HandleDefaultKeyEventInMode2D (void)
{
  int keyChar = AutoGL_GetKeyChar ();
  
  switch (keyChar) {
  case 'o':
  case 'O':
    AutoGL_SetMode2D (AUTOGL_MODE_2D_OTHER);
    break;
  case 't':
  case 'T':
    /* translate */
    AutoGL_SetMode2D (AUTOGL_MODE_2D_TRANSLATE);
    break;
  case 's':
  case 'S':
    /* scale */
    AutoGL_SetMode2D (AUTOGL_MODE_2D_SCALE);
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
    AutoGL_HandleDefaultKeyEventInMode2D ();
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
    AutoGL_HandleDefaultKeyEventInMode2D ();
    break;
  default:
    break;
  }
}

static void DefaultViewEventCallback (void) 
{
  switch (Mode) {
  case AUTOGL_MODE_2D_OTHER:
    if (OtherCallback != NULL) {
      (*OtherCallback) ();
    }
    break;
  case AUTOGL_MODE_2D_TRANSLATE:
    Translate ();
    break;
  case AUTOGL_MODE_2D_SCALE:
    Scale ();
    break;
  default:
    assert(0);
    break;
  }
}

void AutoGL_SetPanelInMode2D (void)
{
  AutoGL_AddInteger (&Mode, "AutoGL_Mode2D"); 
  AutoGL_SetLabel ("mode");
  AutoGL_AddIntegerItem ("Other");              
  AutoGL_AddIntegerItem ("Translate");             
  AutoGL_AddIntegerItem ("Scale");              
}

static void DefaultOtherCallback (void)
{
  int event = AutoGL_GetViewEvent ();

  switch (event) {
  case AUTOGL_EVENT_KEY:
    AutoGL_HandleDefaultKeyEventInMode2D ();
    break;
  default:
    break;
  }
}

void AutoGL_SetDefaultCallbackInMode2D 
(AutoGL_CallbackType otherCallback)
{
  if (otherCallback == NULL) {
    OtherCallback = DefaultOtherCallback;
  } else {
    OtherCallback = otherCallback;
  }
  AutoGL_SetViewEventCallback (DefaultViewEventCallback);
}

int AutoGL_PointingDeviceIsHit2D 
(double pX, double pY,
 int toleranceDc)
{
  int dcX, dcY, dcZ;
  double originX, originY, originZ;
  double distance;
  double tolerance;

  AutoGL_GetPointingDevicePositionDc (&dcX, &dcY, &dcZ);
  AutoGL_GetPositionOfDc (&originX, &originY, &originZ,
			  dcX, dcY, dcZ);
  distance = AutoGL_GetVectorDistance3D (pX, pY, 0, 
					 originX, originY, 0);
  tolerance = toleranceDc 
    * AutoGL_GetViewSize () / AutoGL_GetViewSizeDc ();
  return (distance <= tolerance);
}



