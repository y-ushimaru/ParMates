/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* header file for autogl_graphics.c */

#ifndef __AUTOGL_GRAPHICS_INSIDE__
#define __AUTOGL_GRAPHICS_INSIDE__



void AutoGL_INSIDE_ClearPrimitiveBuffer (void);

void AutoGL_INSIDE_AddPrimitiveBuffer (float value);

int AutoGL_INSIDE_PrimitiveBufferSize (void);

float *AutoGL_INSIDE_PrimitiveBuffer (void);

#define MAX_IMAGE_WIDTH 2048
#define MAX_IMAGE_HEIGHT 2048
#define MAX_PIXEL (MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT)

void AutoGL_INSIDE_SetViewRangeDc (int widthDc, int heightDc);

void AutoGL_INSIDE_SetPointingDeviceEvent 
(int event,   /* AUTOGL_EVENT_ */
 int dcX, int dcY, int dcZ);

void AutoGL_INSIDE_SetKeyEvent (int keyChar);



#endif  /* __AUTOGL_GRAPHICS_INSIDE__ */



