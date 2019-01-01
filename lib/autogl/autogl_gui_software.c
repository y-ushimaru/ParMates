/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_gui_software.c : software emulation */

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
#include "autogl_gui.h"
#include "autogl_gui_inside.h"

#ifdef AUTOGL_MP
#include "autogl_com_inside.h"
#endif

#include "autogl_callback_inside.h"
#include "autogl_cui.h"
#include "autogl_graphics_software_inside.h"



/* Module Variables */



void AutoGL_INSIDE_ResizeView (int widthDc, int heightDc)
{
  AutoGL_INSIDE_ResizeOffScreenWindow (widthDc, heightDc);
  AutoGL_INSIDE_SetViewRangeDc (widthDc, heightDc);
}

void AutoGL_INSIDE_UpdateView (void)
{
  AutoGL_INSIDE_StartDrawingOffScreenWindow ();
  AutoGL_INSIDE_InvokeViewRedrawCallback ();
  AutoGL_INSIDE_EndDrawingOffScreenWindow ();
}



void AutoGL_ResizeView (int widthDc, int heightDc)
{
  AutoGL_INSIDE_SetViewRangeDc (widthDc, heightDc);

  if (!AutoGL_INSIDE_IsMaster ()) return;

#ifdef AUTOGL_MP
  AutoGL_INSIDE_SendResizeEvent (widthDc, heightDc);
#else
  AutoGL_INSIDE_ResizeView (widthDc, heightDc);
#endif
}

void AutoGL_DrawView (void)
{
  if (!AutoGL_INSIDE_IsMaster ()) return;

  AutoGL_INSIDE_UpdateView ();
}

void AutoGL_Terminate (void)
{
  if (!AutoGL_INSIDE_IsMaster ()) return;

#ifdef AUTOGL_MP
  AutoGL_INSIDE_SendQuitEvent ();
#else
  /* do nothing */
#endif
}






/* Main Routine */

static void CommonMain (void)
{
  if (AutoGL_INSIDE_GetBatchProcessCallback () != NULL) {
    AutoGL_INSIDE_CallBatchProcessCallback ();
  } else {
    AutoGL_InterpretInInteractiveMode ();
  }

  AutoGL_Terminate ();
}

void AutoGL_MainLoop (void) 
{
}

#ifdef AUTOGL_FORTRAN

void AutoGL_Main (void) 
{
  /* NEED TO REDEFINE */
  /* no argc, argv */
  /* how to initialize AutoNOSim ??? */

  CommonMain ();
}

#else

int main (int argc, char *argv[]) 
{
#ifdef AUTOGL_MP
  AutoGL_INSIDE_InitializeAutoNOSim (&argc, &argv);
#endif

  /* call the user-side set-up routine */
  AutoGL_SetUp (argc, argv);

#ifdef AUTOGL_MP
  AutoGL_INSIDE_Synchronize ();
#endif

  assert(AutoGL_INSIDE_IsMaster ());

  CommonMain ();

  return 0;
}

#endif
