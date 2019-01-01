/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_utility.c : AutoGL utility */

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

#include "autogl_utility.h"



/* Command Line Interface and Scripting */

static char ScriptFileName[256];

static void GoToCommandLineMode (void)
{
  AutoGL_InterpretInInteractiveMode ();
}

static void RunScript (void)
{
  AutoGL_InterpretScript (ScriptFileName);
}

void AutoGL_SetPanelForInterpreter (void)
{
  AutoGL_AddComment ();
  AutoGL_SetLabel ("INTERPRETER");
  AutoGL_AddCallback (GoToCommandLineMode, 
		      "AutoGL_GoToCommandLineMode"); 
  AutoGL_SetLabel ("command line"); 
  AutoGL_AddCallback (RunScript, "AutoGL_RunScript"); 
  AutoGL_SetLabel ("run script"); 
  AutoGL_AddString (ScriptFileName, "AutoGL_ScriptFileName", 256); 
  AutoGL_SetLabel ("file"); 
}



/* File I/O */

static char ImageFileName[256] = "image.ppm";

void AutoGL_SaveViewImageToPPMFile (const char *fileName)
{
  FILE *fp;
  int widthDc, heightDc;
  unsigned char *image;
  int iRow;

  AutoGL_GetViewRangeDc (&widthDc, &heightDc);
  image = (unsigned char *)malloc (widthDc * heightDc * 3);
  assert(image != NULL);
  AutoGL_GetViewImage (image);

  fp = fopen (fileName, "wb");
  assert(fp != NULL);
  fprintf (fp, "P6\n%d %d\n255\n", 
	   widthDc, heightDc);
  for (iRow = heightDc - 1; 0 <= iRow; iRow--) {
    fwrite (&image[widthDc * 3 * iRow], widthDc * 3, 1, fp);
  }
  fclose (fp);

  free (image);
}

void AutoGL_ReadImageFromPPMFile 
(const char *fileName,
 int *widthDcPtr_OUT, int *heightDcPtr_OUT, unsigned char **imagePtr_OUT)
{
  FILE *fp;
  int widthDc, heightDc;
  unsigned char *image;
  int iRow;
  char line[256];

  /* NEED TO REDEFINE */
  /* PPM file is half ascii, half binary */
  /* is it OK ?   or, rb ??? */
  fp = fopen (fileName, "r");
  assert(fp != NULL);

  fgets (line,  256, fp);
  assert(strcmp (line, "P6\n") == 0);

  fgets (line,  256, fp);
  sscanf (line, "%d %d\n", 
	  &widthDc, &heightDc);
  assert(0 < widthDc);
  assert(0 < heightDc);

  fgets (line,  256, fp);
  assert(strcmp (line, "255\n") == 0);

  image = (unsigned char *)malloc (widthDc * heightDc * 3);
  assert(image != NULL);
  for (iRow = heightDc - 1; 0 <= iRow; iRow--) {
    fread (&image[widthDc * 3 * iRow], widthDc * 3, 1, fp);
  }

  fclose (fp);

  *widthDcPtr_OUT = widthDc;
  *heightDcPtr_OUT = heightDc;
  *imagePtr_OUT = image;
}

static void SaveViewImage (void) 
{
  AutoGL_SaveViewImageToPPMFile (ImageFileName);
}

void AutoGL_SetPanelForSave (void)
{
  AutoGL_AddComment ();
  AutoGL_SetLabel ("SAVE");
  AutoGL_AddCallback (SaveViewImage, "AutoGL_SaveViewImage");
  AutoGL_SetLabel ("save image");
  AutoGL_AddString (ImageFileName, "AutoGL_ImageFileName", 256); 
  AutoGL_SetLabel ("file");
}



/* Other Utility Routines */

void AutoGL_DrawStringAtScaledPosition
(double rx, double ry,
 const char *str)
{
  int widthDc, heightDc;
  int xDc, yDc;
  double pX, pY, pZ;
  
  AutoGL_GetViewRangeDc (&widthDc, &heightDc);
  
  xDc = (double)widthDc * (rx - 0.5);
  yDc = (double)heightDc * (ry - 0.5);
  
  AutoGL_GetPositionOfDc (&pX, &pY, &pZ,
			  xDc, yDc, 0);
  
  AutoGL_DrawString (pX, pY, pZ,
		     str);
}

void AutoGL_DrawTimeDHMS (double time)
{
  char buf[1024];
  int days, hours, mins, secs;
  
  days  = (int)(time / (3600.0 * 24.0));
  hours = (int)((time - days * (3600.0 * 24.0)) / 3600.0);
  mins  = (int)(time - days * (3600.0 * 24.0) - hours * 3600.0) / 60.0;
  secs  = (int)(time - days * (3600.0 * 24.0) - hours * 3600.0
		- mins * 60.0);
  sprintf (buf, "%3dd %2dh %2dm %2ds",
	   days, hours, mins, secs);
  AutoGL_DrawStringAtScaledPosition (0.1, 0.9, buf);
}




