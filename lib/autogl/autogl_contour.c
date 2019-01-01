/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_contour.c : AutoGL utility for contour plot */

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

#include "autogl_contour.h"



/* Contour */

#define MAX_CONTOUR_COLORS 100
static int NContourColors = 6;
static double ContourGrades[MAX_CONTOUR_COLORS] = {
  0.0, 
  1.0 / 21 * 5,
  1.0 / 21 * 8,
  1.0 / 21 * 11,
  1.0 / 21 * 16,
  1.0
};

static double ContourColors[MAX_CONTOUR_COLORS][3] = {
  { 0.0, 0.0, 1.0 },
  { 0.0, 1.0, 1.0 },
  { 0.0, 1.0, 0.0 },
  { 1.0, 1.0, 0.0 },
  { 1.0, 0.0, 0.0 },
  { 1.0, 0.0, 1.0 }
};

void AutoGL_ClearContourColor (void)
{
  NContourColors = 0;
}

void AutoGL_AddContourColorOfGrade 
(double grade, 
 double red, double green, double blue)
{
  assert(NContourColors < MAX_CONTOUR_COLORS);

  ContourGrades[NContourColors] = grade;
  ContourColors[NContourColors][0] = red;
  ContourColors[NContourColors][1] = green;
  ContourColors[NContourColors][2] = blue;
  NContourColors++;
}

void AutoGL_SetGrayScaleContourMap (void)
{
  AutoGL_ClearContourColor ();
  AutoGL_AddContourColorOfGrade (0.0, 
				 0.0, 0.0, 0.0);
  AutoGL_AddContourColorOfGrade (1.0, 
				 1.0, 1.0, 1.0);
}

void AutoGL_SetContourMap_BCGYRM (void)
{
  AutoGL_ClearContourColor ();
  AutoGL_AddContourColorOfGrade (0.0, 
				 0.0, 0.0, 1.0);
  AutoGL_AddContourColorOfGrade (1.0 / 21 * 5, 
				 0.0, 1.0, 1.0);
  AutoGL_AddContourColorOfGrade (1.0 / 21 * 8, 
				 0.0, 1.0, 0.0);
  AutoGL_AddContourColorOfGrade (1.0 / 21 * 11, 
				 1.0, 1.0, 0.0);
  AutoGL_AddContourColorOfGrade (1.0 / 21 * 16, 
				 1.0, 0.0, 0.0);
  AutoGL_AddContourColorOfGrade (1.0, 
				 1.0, 0.0, 1.0);
}

void AutoGL_SetContourMap_MBCGYR (void)
{
  AutoGL_ClearContourColor ();
  AutoGL_AddContourColorOfGrade (0.0, 
				 1.0, 0.0, 1.0);
  AutoGL_AddContourColorOfGrade (1.0 / 21 * 5, 
				 0.0, 0.0, 1.0);
  AutoGL_AddContourColorOfGrade (1.0 / 21 * 8, 
				 0.0, 1.0, 1.0);
  AutoGL_AddContourColorOfGrade (1.0 / 21 * 11, 
				 0.0, 1.0, 0.0);
  AutoGL_AddContourColorOfGrade (1.0 / 21 * 16, 
				 1.0, 1.0, 0.0);
  AutoGL_AddContourColorOfGrade (1.0,
				 1.0, 0.0, 0.0);
}

void AutoGL_SetContourMap_BCGYR (void)
{
  AutoGL_ClearContourColor ();
  AutoGL_AddContourColorOfGrade (0.0, 
				 0.0, 0.0, 1.0);
  AutoGL_AddContourColorOfGrade (1.0 / 21 * 5, 
				 0.0, 1.0, 1.0);
  AutoGL_AddContourColorOfGrade (1.0 / 21 * 8, 
				 0.0, 1.0, 0.0);
  AutoGL_AddContourColorOfGrade (1.0 / 16 * 11, 
				 1.0, 1.0, 0.0);
  AutoGL_AddContourColorOfGrade (1.0, 
				 1.0, 0.0, 0.0);
}

void AutoGL_GetContourColor 
(double *red_OUT, double *green_OUT, double *blue_OUT,
 double grade)
{
  int iColor;

  if (grade < 0.0) {
    *red_OUT = ContourColors[0][0];
    *green_OUT = ContourColors[0][1];
    *blue_OUT = ContourColors[0][2];
  } else if (1.0 <= grade) {
    *red_OUT = ContourColors[NContourColors - 1][0];
    *green_OUT = ContourColors[NContourColors - 1][1];
    *blue_OUT = ContourColors[NContourColors - 1][2];
  } else {
    for (iColor = 0; 
	 iColor < NContourColors - 1; 
	 iColor++) {
      if (grade < ContourGrades[iColor + 1]) {
	double ratio = (grade - ContourGrades[iColor]) 
	  / (ContourGrades[iColor + 1] - ContourGrades[iColor]);
	
	*red_OUT = ContourColors[iColor][0]
	  + (ContourColors[iColor + 1][0] - ContourColors[iColor][0]) * ratio;
	*green_OUT = ContourColors[iColor][1]
	  + (ContourColors[iColor + 1][1] - ContourColors[iColor][1]) * ratio;
	*blue_OUT = ContourColors[iColor][2]
	  + (ContourColors[iColor + 1][2] - ContourColors[iColor][2]) * ratio;
	
	break;
      }
    }
  }
}

void AutoGL_DrawContourTriangle 
(double x0, double y0, double z0, double grade0, 
 double x1, double y1, double z1, double grade1, 
 double x2, double y2, double z2, double grade2)
{
  double red0, green0, blue0;
  double red1, green1, blue1;
  double red2, green2, blue2;

  AutoGL_GetContourColor (&red0, &green0, &blue0,
			  grade0);
  AutoGL_GetContourColor (&red1, &green1, &blue1,
			  grade1);
  AutoGL_GetContourColor (&red2, &green2, &blue2,
			  grade2);
  AutoGL_DrawColorInterpolatedTriangle 
    (x0, y0, z0, red0, green0, blue0,
     x1, y1, z1, red1, green1, blue1,
     x2, y2, z2, red2, green2, blue2);
}

void AutoGL_DrawContourMap 
(int sizeDc,
 double minRange, double maxRange)
{
  int widthDc, heightDc;
  int corner0XDc, corner0YDc;
  int corner1XDc, corner1YDc;
  int nGrades = 10;
  int iGrade;
  char buf[256];
  
  AutoGL_GetViewRangeDc (&widthDc, &heightDc);
  
  corner0XDc = widthDc / 2 - sizeDc;
  corner1XDc = widthDc / 2 - sizeDc / 4 * 3;
  corner0YDc = -heightDc / 2 + sizeDc / 4;
  corner1YDc = heightDc / 2 - sizeDc / 4;

  for (iGrade = 0; iGrade < nGrades; iGrade++) {
    double grade0 = 1.0 / nGrades * iGrade;
    double grade1 = 1.0 / nGrades * (iGrade + 1);
    int p0YDc = corner0YDc 
      + (corner1YDc - corner0YDc) * iGrade / nGrades;
    int p1YDc = corner0YDc 
      + (corner1YDc - corner0YDc) * (iGrade + 1) / nGrades;
    double p00X, p00Y, p00Z;
    double p01X, p01Y, p01Z;
    double p11X, p11Y, p11Z;
    double p10X, p10Y, p10Z;
    
    AutoGL_GetPositionOfDc (&p00X, &p00Y, &p00Z,
			    corner0XDc, p0YDc, 0);
    AutoGL_GetPositionOfDc (&p01X, &p01Y, &p01Z,
			    corner1XDc, p0YDc, 0);
    AutoGL_GetPositionOfDc (&p11X, &p11Y, &p11Z,
			    corner1XDc, p1YDc, 0);
    AutoGL_GetPositionOfDc (&p10X, &p10Y, &p10Z,
			    corner0XDc, p1YDc, 0);
    AutoGL_DrawContourTriangle 
      (p00X, p00Y, p00Z, grade0,
       p01X, p01Y, p01Z, grade0,
       p11X, p11Y, p11Z, grade1);
    AutoGL_DrawContourTriangle 
      (p11X, p11Y, p11Z, grade1,
       p10X, p10Y, p10Z, grade1,
       p00X, p00Y, p00Z, grade0);

    sprintf (buf, "%5.2e", 
	     minRange + (maxRange - minRange) / nGrades * iGrade);
    AutoGL_DrawString (p01X, p01Y, p01Z, 
		       buf);  

    if (iGrade == nGrades - 1) {
      sprintf (buf, "%5.2e", maxRange);
      AutoGL_DrawString (p11X, p11Y, p11Z, 
			 buf);  
    }
  }
}

void AutoGL_DrawLineContourMap 
(int sizeDc,
 double minRange, double maxRange,
 int nGrades)
{
  int widthDc, heightDc;
  int corner0XDc, corner0YDc;
  int corner1XDc, corner1YDc;
  int iGrade;
  char buf[256];
  
  AutoGL_GetViewRangeDc (&widthDc, &heightDc);
  
  corner0XDc = widthDc / 2 - sizeDc;
  corner1XDc = widthDc / 2 - sizeDc / 4 * 3;
  corner0YDc = -heightDc / 2 + sizeDc / 4;
  corner1YDc = heightDc / 2 - sizeDc / 4;

  {
    double p0X, p0Y, p0Z;
    double p1X, p1Y, p1Z;

    AutoGL_SetColor (1, 1, 1);
    AutoGL_GetPositionOfDc (&p0X, &p0Y, &p0Z,
			    corner0XDc, corner0YDc, 0);
    AutoGL_GetPositionOfDc (&p1X, &p1Y, &p1Z,
			    corner0XDc, corner1YDc, 0);
    AutoGL_DrawLine 
      (p0X, p0Y, p0Z,
       p1X, p1Y, p1Z);
    AutoGL_GetPositionOfDc (&p0X, &p0Y, &p0Z,
			    corner1XDc, corner0YDc, 0);
    AutoGL_GetPositionOfDc (&p1X, &p1Y, &p1Z,
			    corner1XDc, corner1YDc, 0);
    AutoGL_DrawLine 
      (p0X, p0Y, p0Z,
       p1X, p1Y, p1Z);
  }

  for (iGrade = 0; iGrade <= nGrades; iGrade++) {
    double grade = 1.0 / nGrades * iGrade;
    double red, green, blue;
    int pYDc = corner0YDc 
      + (corner1YDc - corner0YDc) * iGrade / nGrades;
    double p0X, p0Y, p0Z;
    double p1X, p1Y, p1Z;
    
    AutoGL_GetContourColor (&red, &green, &blue,
			    grade);
    AutoGL_SetColor (red, green, blue);
    AutoGL_GetPositionOfDc (&p0X, &p0Y, &p0Z,
			    corner0XDc, pYDc, 0);
    AutoGL_GetPositionOfDc (&p1X, &p1Y, &p1Z,
			    corner1XDc, pYDc, 0);
    AutoGL_DrawLine 
      (p0X, p0Y, p0Z,
       p1X, p1Y, p1Z);

    sprintf (buf, "%5.2e", 
	     minRange + (maxRange - minRange) / nGrades * iGrade);
    AutoGL_DrawString (p1X, p1Y, p1Z, 
		       buf);  
  }
}

void AutoGL_DrawBandContourMap 
(int sizeDc,
 double minRange, double maxRange,
 int nGrades)
{
  int widthDc, heightDc;
  int corner0XDc, corner0YDc;
  int corner1XDc, corner1YDc;
  int iGrade;
  char buf[256];
  
  AutoGL_GetViewRangeDc (&widthDc, &heightDc);
  
  corner0XDc = widthDc / 2 - sizeDc;
  corner1XDc = widthDc / 2 - sizeDc / 4 * 3;
  corner0YDc = -heightDc / 2 + sizeDc / 4;
  corner1YDc = heightDc / 2 - sizeDc / 4;

  for (iGrade = 0; iGrade < nGrades; iGrade++) {
    double grade = 1.0 / nGrades * iGrade;
    double red, green, blue;
    int p0YDc = corner0YDc 
      + (corner1YDc - corner0YDc) * iGrade / nGrades;
    int p1YDc = corner0YDc 
      + (corner1YDc - corner0YDc) * (iGrade + 1) / nGrades;
    double p00X, p00Y, p00Z;
    double p01X, p01Y, p01Z;
    double p11X, p11Y, p11Z;
    double p10X, p10Y, p10Z;
    
    AutoGL_GetContourColor (&red, &green, &blue,
			    grade);
    AutoGL_SetColor (red, green, blue);
    AutoGL_GetPositionOfDc (&p00X, &p00Y, &p00Z,
			    corner0XDc, p0YDc, 0);
    AutoGL_GetPositionOfDc (&p01X, &p01Y, &p01Z,
			    corner1XDc, p0YDc, 0);
    AutoGL_GetPositionOfDc (&p11X, &p11Y, &p11Z,
			    corner1XDc, p1YDc, 0);
    AutoGL_GetPositionOfDc (&p10X, &p10Y, &p10Z,
			    corner0XDc, p1YDc, 0);
    AutoGL_DrawQuadrangle 
      (p00X, p00Y, p00Z,
       p01X, p01Y, p01Z,
       p11X, p11Y, p11Z,
       p10X, p10Y, p10Z);

    sprintf (buf, "%5.2e", 
	     minRange + (maxRange - minRange) / nGrades * iGrade);
    AutoGL_DrawString (p01X, p01Y, p01Z, 
		       buf);  

    if (iGrade == nGrades - 1) {
      sprintf (buf, "%5.2e", maxRange);
      AutoGL_DrawString (p11X, p11Y, p11Z, 
			 buf);  
    }
  }
}






