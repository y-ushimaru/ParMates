/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_callback.c : hook for callback */

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
#include "autogl_gui.h"
#include "autogl_gui_inside.h"

#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
#include "autogl_com_inside.h"
#endif

#include "autogl_callback_inside.h"



int AutoGL_INSIDE_IsMaster (void)
{
#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
  return AutoNOSim_MyTaskId () == AUTONOSIM_MASTER_TASK_ID;
#else
  return 1;
#endif
}

void AutoGL_INSIDE_InvokeCallback 
(AutoGL_Property *property)
{
#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
  AutoGL_INSIDE_SendCallbackEvent (property);
#else
  AutoGL_INSIDE_CallCallback (property);
#endif
}

void AutoGL_INSIDE_InvokeViewRedrawCallback (void)
{
#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
  if (AutoGL_INSIDE_IsMaster ()) {
    AutoGL_INSIDE_SendRedrawEvent ();
  } else {
    AutoGL_INSIDE_CallViewRedrawCallback ();
  }
#else
  AutoGL_INSIDE_CallViewRedrawCallback ();
#endif
}

void AutoGL_INSIDE_InvokeViewEventCallback (void)
{
#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
  if (AutoGL_INSIDE_GetViewEventCallback () != NULL) {
    switch (AutoGL_GetViewEvent ()) {
    case AUTOGL_EVENT_POINTING_DEVICE_PRESS:
    case AUTOGL_EVENT_POINTING_DEVICE_DRAG:
    case AUTOGL_EVENT_POINTING_DEVICE_RELEASE:
      AutoGL_INSIDE_SendMouseEvent ();
      break;
    case AUTOGL_EVENT_KEY:
      AutoGL_INSIDE_SendKeyEvent ();
      break;
    }
  }
#else
  AutoGL_INSIDE_CallViewEventCallback ();
#endif
}

void AutoGL_INSIDE_InvokeIdleEventCallback (void)
{
#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
  if (AutoGL_INSIDE_IdleEventIsEnabled ()) {
    if (AutoGL_INSIDE_GetIdleEventCallback () != NULL) {
      AutoGL_INSIDE_SendIdleEvent ();
    }
  }
#else
  AutoGL_INSIDE_CallIdleEventCallback ();
#endif
}





