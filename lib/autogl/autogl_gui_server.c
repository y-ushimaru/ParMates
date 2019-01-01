/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_gui_server.c : software emulation on server */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

/* usleep */
#include <unistd.h>

#include "autogl_os.h"
#include "autogl_math.h"
#include "autogl_uims.h"
#include "autogl_uims_inside.h"
#include "autogl_graphics.h"
#include "autogl_graphics_inside.h"
#include "autogl_gui.h"
#include "autogl_gui_inside.h"
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

  AutoGL_INSIDE_ResizeView (widthDc, heightDc);
}

void AutoGL_DrawView (void)
{
  if (!AutoGL_INSIDE_IsMaster ()) return;

  AutoGL_INSIDE_UpdateView ();
}

void AutoGL_Terminate (void)
{
  if (!AutoGL_INSIDE_IsMaster ()) return;

  /* do nothing */
}



#define COPY "/bin/cp"
#define DELETE "/bin/rm"
#define RENAME "/bin/mv"
#define CHANGE_MODE "/bin/chmod og+w"

#if 0
/* NEED TO REDEFINE */
/* portability problem of convert command */
/* in some distribution of Linux */
#define CONVERT "/usr/bin/convert"
#else  
#define CONVERT "/usr/X11R6/bin/convert"
#endif

#define INFO_COMMAND "autogl_info_command_file"
#define INFO_COMMAND_OUTPUT "autogl_info_command_output_file"

#define VALUE_COMMAND "autogl_value_command_file"
#define VALUE_COMMAND_OUTPUT "autogl_value_command_output_file"

#define DRAW_COMMAND "autogl_draw_command_file"
#define DRAW_COMMAND_OUTPUT "autogl_draw_command_output_file"

#define POST_COMMAND "autogl_post_command_file"
#define POST_COMMAND_OUTPUT "autogl_post_command_output_file"

#define VIEW_IMAGE_SRC "result.ppm"
#define VIEW_IMAGE "autogl_view_image"
#define VIEW_IMAGE_COUNT "autogl_view_image_count_file"

#if 0
#define VIEW_IMAGE_FILE_TYPE "gif"
#else
#define VIEW_IMAGE_FILE_TYPE "jpeg"
#endif

static int ViewImageCount = 0;

static void CreateViewImageFile (void)
{
  char buf[1024];
  char fileName[256];
  FILE *out;

#if 0
  /* NEED TO REDEFINE */
  /* convert command is not available at web server */
  sprintf (buf, "rsh euler /usr/bin/convert public_html/%s public_html/%s.%s",
	   VIEW_IMAGE_SRC, 
	   VIEW_IMAGE,
	   VIEW_IMAGE_FILE_TYPE);
#else
  sprintf (buf, "%s %s %s.%s",
	   CONVERT,
	   VIEW_IMAGE_SRC, 
	   VIEW_IMAGE,
	   VIEW_IMAGE_FILE_TYPE);
#endif
  system (buf);

  if (0 < ViewImageCount) {
    sprintf (fileName, "%s%d.%s",
	     VIEW_IMAGE, 
	     ViewImageCount - 1,
	     VIEW_IMAGE_FILE_TYPE);
    sprintf (buf, "%s %s",
	     DELETE,
	     fileName);
    system (buf);
  }
  
  sprintf (fileName, "%s%d.%s",
	   VIEW_IMAGE, 
	   ViewImageCount,
	   VIEW_IMAGE_FILE_TYPE);
  sprintf (buf, "%s %s.%s %s",
	   COPY,
	   VIEW_IMAGE,
	   VIEW_IMAGE_FILE_TYPE,
	   fileName);
  system (buf);

  out = fopen (VIEW_IMAGE_COUNT, "w");
  fprintf (out, "%d\n", ViewImageCount);
  fclose (out);

  ViewImageCount++;
}

static void ExecuteInfoCommand (void)
{
  char buf[1024];
  FILE *in;
  FILE *out;

  out = fopen (INFO_COMMAND, "w");
  fprintf (out, "info\n");
  fclose (out);

  in = fopen (INFO_COMMAND, "r");
  out = fopen (INFO_COMMAND_OUTPUT, "w");
  AutoGL_InterpretInBatchMode (in, out);
  fclose (in);
  fclose (out);
  
  sprintf (buf, "%s %s",
	   DELETE,
	   INFO_COMMAND);
  system (buf);
}

static void ExecuteValueCommand (void)
{
  char buf[1024];
  FILE *in;
  FILE *out;

  out = fopen (VALUE_COMMAND, "w");
  fprintf (out, "value\n");
  fclose (out);

  in = fopen (VALUE_COMMAND, "r");
  out = fopen (VALUE_COMMAND_OUTPUT, "w");
  AutoGL_InterpretInBatchMode (in, out);
  fclose (in);
  fclose (out);

  sprintf (buf, "%s %s",
	   DELETE,
	   VALUE_COMMAND);
  system (buf);
}

static void ExecuteDrawCommand (void)
{
  char buf[1024];
  FILE *in;
  FILE *out;

  out = fopen (DRAW_COMMAND, "w");
  fprintf (out, "draw\n");
  fclose (out);

  in = fopen (DRAW_COMMAND, "r");
  out = fopen (DRAW_COMMAND_OUTPUT, "w");
  AutoGL_InterpretInBatchMode (in, out);
  fclose (in);
  fclose (out);

  sprintf (buf, "%s %s",
	   DELETE,
	   DRAW_COMMAND);
  system (buf);

  sprintf (buf, "%s %s",
	   DELETE,
	   DRAW_COMMAND_OUTPUT);
  system (buf);

  CreateViewImageFile ();
}

static void CgiEventLoop (void)
{
  char buf[1024];
  FILE *in;
  FILE *out;



#if 0
  /* NFS environment */
  for (;;) {
    system ("/bin/ls > /dev/null");
    in = fopen (POST_COMMAND, "r");
    if (in != NULL) break;
    usleep (500000);
  }
#else
  /* stand alone environment */
  for (;;) {
    in = fopen (POST_COMMAND, "r");
    if (in != NULL) break;
    usleep (100000);
  }
#endif



  sprintf (buf, "%s_TMP",
	   POST_COMMAND_OUTPUT);
  out = fopen (buf, "w");
  AutoGL_InterpretInBatchMode (in, out);
  fclose (in);
  fclose (out);
  
  sprintf (buf, "%s %s",
	   DELETE,
	   POST_COMMAND);
  system (buf);

  CreateViewImageFile ();
  
  sprintf (buf, "%s %s_TMP %s_BAK",
	   COPY,
	   POST_COMMAND_OUTPUT, 
	   POST_COMMAND_OUTPUT);
  system (buf);

  sprintf (buf, "%s %s_TMP",
	   CHANGE_MODE,
	   POST_COMMAND_OUTPUT);
  system (buf);

  sprintf (buf, "%s %s_TMP %s",
	   RENAME,
	   POST_COMMAND_OUTPUT, 
	   POST_COMMAND_OUTPUT);
  system (buf);
}






/* Main Routine */

static void CommonMain (void)
{
  ExecuteInfoCommand ();
  ExecuteValueCommand ();
  ExecuteDrawCommand ();

  for (;;) {
    CgiEventLoop ();
  }
}

void AutoGL_MainLoop (void) 
{
}

#ifdef AUTOGL_FORTRAN

void AutoGL_Main (void) 
{
  CommonMain ();
}

#else

int main (int argc, char *argv[]) 
{
  /* call the user-side set-up routine */
  AutoGL_SetUp (argc, argv);

  CommonMain ();

  /* never return */
  return 0;
}

#endif
