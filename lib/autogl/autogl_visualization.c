/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_visualization.c : AutoGL utility for visualization (high level API) */

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
#include "autogl_vislow.h"
#include "autogl_visualization.h"



/* Visualization : high level API */

void AutoGL_DrawSmoothContourTriangle 
(double minRange, double maxRange,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2)
{
  double grade0 = (value0 - minRange) / (maxRange - minRange);
  double grade1 = (value1 - minRange) / (maxRange - minRange);
  double grade2 = (value2 - minRange) / (maxRange - minRange);

  AutoGL_DrawContourTriangle (x0, y0, z0, grade0,
			      x1, y1, z1, grade1,
			      x2, y2, z2, grade2);
}

void AutoGL_DrawLineContourTriangle 
(double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2)
{
  double triPoints[3][3];
  double triValues[3];
  int iGrade;

  triPoints[0][0] = x0;
  triPoints[0][1] = y0;
  triPoints[0][2] = z0;
  triValues[0] = value0;

  triPoints[1][0] = x1;
  triPoints[1][1] = y1;
  triPoints[1][2] = z1;
  triValues[1] = value1;

  triPoints[2][0] = x2;
  triPoints[2][1] = y2;
  triPoints[2][2] = z2;
  triValues[2] = value2;

  for (iGrade = 0; iGrade <= nGrades; iGrade++) {
    double levelValue;
    int vertexIds0[2], vertexIds1[2];
    double ratios[2];

    if (0 < nGrades) {
      levelValue = minRange 
	+ (maxRange - minRange) / nGrades * iGrade;
    } else {
      levelValue = minRange;
    }

    if (AutoGL_GetIsoline
	(vertexIds0, vertexIds1, ratios,
	 levelValue, triValues)) {
      double points[2][3];
      
      if (0 < nGrades) {
	double grade = 1.0 / nGrades * iGrade;
	double red, green, blue;

	AutoGL_GetContourColor (&red, &green, &blue,
				grade);
	AutoGL_SetColor (red, green, blue);
      }

      AutoGL_GetIsolineVertex 
	(points,
	 triPoints, vertexIds0, vertexIds1, ratios);
      AutoGL_DrawLine 
	(points[0][0], points[0][1], points[0][2],
	 points[1][0], points[1][1], points[1][2]);
    }
  }
}

void AutoGL_DrawBandContourTriangle 
(double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2)
{
  double tx0, ty0, tz0;
  double tx1, ty1, tz1;
  double tx2, ty2, tz2;
  double tv0, tv1, tv2;
  int iGrade;

  assert(0 < nGrades);

  if (value0 < value1) {
    if (value1 < value2) {
      tx0 = x0;
      ty0 = y0;
      tz0 = z0;
      tv0 = value0;

      tx1 = x1;
      ty1 = y1;
      tz1 = z1;
      tv1 = value1;

      tx2 = x2;
      ty2 = y2;
      tz2 = z2;
      tv2 = value2;
    } else {
      /* value2 <= value1 */
      if (value0 < value2) {
	tx0 = x0;
	ty0 = y0;
	tz0 = z0;
	tv0 = value0;
	
	tx1 = x2;
	ty1 = y2;
	tz1 = z2;
	tv1 = value2;
	
	tx2 = x1;
	ty2 = y1;
	tz2 = z1;
	tv2 = value1;
      } else {
	tx0 = x2;
	ty0 = y2;
	tz0 = z2;
	tv0 = value2;
	
	tx1 = x0;
	ty1 = y0;
	tz1 = z0;
	tv1 = value0;
	
	tx2 = x1;
	ty2 = y1;
	tz2 = z1;
	tv2 = value1;
      }
    }
  } else {
    /* value1 <= value0 */ 
    if (value0 < value2) {
      tx0 = x1;
      ty0 = y1;
      tz0 = z1;
      tv0 = value1;
      
      tx1 = x0;
      ty1 = y0;
      tz1 = z0;
      tv1 = value0;
      
      tx2 = x2;
      ty2 = y2;
      tz2 = z2;
      tv2 = value2;
    } else {
      /* value2 <= value0 */
      if (value1 < value2) {
	tx0 = x1;
	ty0 = y1;
	tz0 = z1;
	tv0 = value1;
	
	tx1 = x2;
	ty1 = y2;
	tz1 = z2;
	tv1 = value2;
	
	tx2 = x0;
	ty2 = y0;
	tz2 = z0;
	tv2 = value0;
      } else {
	tx0 = x2;
	ty0 = y2;
	tz0 = z2;
	tv0 = value2;
	
	tx1 = x1;
	ty1 = y1;
	tz1 = z1;
	tv1 = value1;
	
	tx2 = x0;
	ty2 = y0;
	tz2 = z0;
	tv2 = value0;
      }
    }
  }

  /* NEED TO REDEFINE */
  /* in case tv0 == tv1, or tv1 == tv2 */
  
  for (iGrade = 0; iGrade < nGrades; iGrade++) {
    double prevLevelValue = minRange 
      + (maxRange - minRange) / nGrades * iGrade;
    double currentLevelValue = minRange 
      + (maxRange - minRange) / nGrades * (iGrade + 1);

    {
      double grade;
      double red, green, blue;
      
      grade = 1.0 / nGrades * iGrade;
      AutoGL_GetContourColor (&red, &green, &blue,
			      grade);
      AutoGL_SetColor (red, green, blue);
    }
    
    if (currentLevelValue <= tv0) {
      /* current */
      /* below vertex 0 */

      if (iGrade == nGrades - 1) {
	/* the last grade */

	AutoGL_DrawTriangle 
	  (tx0, ty0, tz0,
	   tx1, ty1, tz1,
	   tx2, ty2, tz2);
	
	break;
      } else {
	continue;
      }
    }

    /* tv0 < currentLevelValue */

    if (currentLevelValue <= tv1) {
      /* current */
      /* between vertex 0 and vertex 1 */

      /* current */
      /* on edge between vertex 0 and vertex 1 */
      /* on edge between vertex 0 and vertex 2 */

      double cr01 = (currentLevelValue - tv0) / (tv1 - tv0);
      double cr02 = (currentLevelValue - tv0) / (tv2 - tv0);

      double cx01 = tx0 + (tx1 - tx0) * cr01;
      double cy01 = ty0 + (ty1 - ty0) * cr01;
      double cz01 = tz0 + (tz1 - tz0) * cr01;
      
      double cx02 = tx0 + (tx2 - tx0) * cr02;
      double cy02 = ty0 + (ty2 - ty0) * cr02;
      double cz02 = tz0 + (tz2 - tz0) * cr02;

      if (iGrade == 0) {
	/* the first grade */
	
	AutoGL_DrawTriangle 
	  (tx0, ty0, tz0,
	   cx01, cy01, cz01,
	   cx02, cy02, cz02);

	continue;
      }

      if (prevLevelValue <= tv0) {
	/* prev */
	/* below vertex 0 */

	/* prev */
	/* vertex 0 */

	AutoGL_DrawTriangle 
	  (tx0, ty0, tz0,
	   cx01, cy01, cz01,
	   cx02, cy02, cz02);

      } else {
	/* tv0 < prevLevelValue */

	/* prev */
	/* on edge between vertex 0 and vertex 1 */
	/* on edge between vertex 0 and vertex 2 */

	double pr01 = (prevLevelValue - tv0) / (tv1 - tv0);
	double pr02 = (prevLevelValue - tv0) / (tv2 - tv0);
	
	double px01 = tx0 + (tx1 - tx0) * pr01;
	double py01 = ty0 + (ty1 - ty0) * pr01;
	double pz01 = tz0 + (tz1 - tz0) * pr01;
	
	double px02 = tx0 + (tx2 - tx0) * pr02;
	double py02 = ty0 + (ty2 - ty0) * pr02;
	double pz02 = tz0 + (tz2 - tz0) * pr02;
	
	AutoGL_DrawQuadrangle 
	  (px02, py02, pz02,
	   px01, py01, pz01,
	   cx01, cy01, cz01,
	   cx02, cy02, cz02);
      }

      if (iGrade == nGrades - 1) {
	/* the last grade */
	
	AutoGL_DrawQuadrangle 
	  (cx02, cy02, cz02,
	   cx01, cy01, cz01,
	   tx1, ty1, tz1,
	   tx2, ty2, tz2);
	
	break;
      }

    } else {
      /* tv1 < currentLevelValue */

      if (currentLevelValue <= tv2) {
	/* current */
	/* between vertex 1 and vertex 2 */

	/* current */
	/* on edge between vertex 1 and vertex 2 */
	/* on edge between vertex 0 and vertex 2 */

	double cr12 = (currentLevelValue - tv1) / (tv2 - tv1);
	double cr02 = (currentLevelValue - tv0) / (tv2 - tv0);
	
	double cx12 = tx1 + (tx2 - tx1) * cr12;
	double cy12 = ty1 + (ty2 - ty1) * cr12;
	double cz12 = tz1 + (tz2 - tz1) * cr12;
	
	double cx02 = tx0 + (tx2 - tx0) * cr02;
	double cy02 = ty0 + (ty2 - ty0) * cr02;
	double cz02 = tz0 + (tz2 - tz0) * cr02;

	if (iGrade == 0) {
	  /* the first grade */
	  
	  AutoGL_DrawQuadrangle 
	    (tx0, ty0, tz0,
	     tx1, ty1, tz1,
	     cx12, cy12, cz12,
	     cx02, cy02, cz02);
	  
	  continue;
	}

	if (prevLevelValue <= tv0) {
	  /* prev */
	  /* below vertex 0 */
	  
	  /* prev */
	  /* vertex 0 */
	  
	  AutoGL_DrawQuadrangle 
	    (tx0, ty0, tz0,
	     tx1, ty1, tz1,
	     cx12, cy12, cz12,
	     cx02, cy02, cz02);

	} else {
	  /* tv0 < prevLevelValue */

	  if (prevLevelValue <= tv1) {
	    /* prev */
	    /* between vertex 0 and vertex 1 */

	    /* prev */
	    /* on edge between vertex 0 and vertex 1 */
	    /* on edge between vertex 0 and vertex 2 */
	    
	    double pr01 = (prevLevelValue - tv0) / (tv1 - tv0);
	    double pr02 = (prevLevelValue - tv0) / (tv2 - tv0);
	    
	    double px01 = tx0 + (tx1 - tx0) * pr01;
	    double py01 = ty0 + (ty1 - ty0) * pr01;
	    double pz01 = tz0 + (tz1 - tz0) * pr01;
	    
	    double px02 = tx0 + (tx2 - tx0) * pr02;
	    double py02 = ty0 + (ty2 - ty0) * pr02;
	    double pz02 = tz0 + (tz2 - tz0) * pr02;

	    AutoGL_DrawTriangle 
	      (px02, py02, pz02,
	       px01, py01, pz01,
	       tx1, ty1, tz1);
	    AutoGL_DrawTriangle 
	      (tx1, ty1, tz1,
	       cx12, cy12, cz12,
	       cx02, cy02, cz02);
	    AutoGL_DrawTriangle 
	      (cx02, cy02, cz02,
	       px02, py02, pz02,
	       tx1, ty1, tz1);

	  } else {
	    /* tv1 < prevLevelValue */

	    /* prev */
	    /* between vertex 1 and vertex 2 */

	    /* prev */
	    /* on edge between vertex 1 and vertex 2 */
	    /* on edge between vertex 0 and vertex 2 */
	    
	    double pr12 = (prevLevelValue - tv1) / (tv2 - tv1);
	    double pr02 = (prevLevelValue - tv0) / (tv2 - tv0);
	    
	    double px12 = tx1 + (tx2 - tx1) * pr12;
	    double py12 = ty1 + (ty2 - ty1) * pr12;
	    double pz12 = tz1 + (tz2 - tz1) * pr12;
	    
	    double px02 = tx0 + (tx2 - tx0) * pr02;
	    double py02 = ty0 + (ty2 - ty0) * pr02;
	    double pz02 = tz0 + (tz2 - tz0) * pr02;

	    AutoGL_DrawQuadrangle 
	      (px02, py02, pz02,
	       px12, py12, pz12,
	       cx12, cy12, cz12,
	       cx02, cy02, cz02);
	  }

	}

	if (iGrade == nGrades - 1) {
	  /* the last grade */
	  
	  AutoGL_DrawTriangle 
	    (cx02, cy02, cz02,
	     cx12, cy12, cz12,
	     tx2, ty2, tz2);
	  
	  break;
	}

      } else {
	/* tv2 < currentLevelValue */

	/* current */
	/* above vertex 2 */

	if (iGrade == 0) {
	  /* the first grade */
	  
	  AutoGL_DrawTriangle 
	    (tx0, ty0, tz0,
	     tx1, ty1, tz1,
	     tx2, ty2, tz2);
	  
	  continue;
	}

	if (prevLevelValue <= tv0) {
	  /* prev */
	  /* below vertex 0 */
	  
	  /* prev */
	  /* vertex 0 */
	  
	  AutoGL_DrawTriangle 
	    (tx0, ty0, tz0,
	     tx1, ty1, tz1,
	     tx2, ty2, tz2);

	} else {
	  /* tv0 < prevLevelValue */

	  if (prevLevelValue <= tv1) {
	    /* prev */
	    /* between vertex 0 and vertex 1 */

	    /* prev */
	    /* on edge between vertex 0 and vertex 1 */
	    /* on edge between vertex 0 and vertex 2 */
	    
	    double pr01 = (prevLevelValue - tv0) / (tv1 - tv0);
	    double pr02 = (prevLevelValue - tv0) / (tv2 - tv0);
	    
	    double px01 = tx0 + (tx1 - tx0) * pr01;
	    double py01 = ty0 + (ty1 - ty0) * pr01;
	    double pz01 = tz0 + (tz1 - tz0) * pr01;
	    
	    double px02 = tx0 + (tx2 - tx0) * pr02;
	    double py02 = ty0 + (ty2 - ty0) * pr02;
	    double pz02 = tz0 + (tz2 - tz0) * pr02;

	    AutoGL_DrawQuadrangle 
	      (px02, py02, pz02,
	       px01, py01, pz01,
	       tx1, ty1, tz1,
	       tx2, ty2, tz2);

	  } else {
	    /* tv1 < prevLevelValue */

	    if (prevLevelValue <= tv2) {

	      /* prev */
	      /* between vertex 1 and vertex 2 */
	      
	      /* prev */
	      /* on edge between vertex 1 and vertex 2 */
	      /* on edge between vertex 0 and vertex 2 */
	      
	      double pr12 = (prevLevelValue - tv1) / (tv2 - tv1);
	      double pr02 = (prevLevelValue - tv0) / (tv2 - tv0);
	      
	      double px12 = tx1 + (tx2 - tx1) * pr12;
	      double py12 = ty1 + (ty2 - ty1) * pr12;
	      double pz12 = tz1 + (tz2 - tz1) * pr12;
	      
	      double px02 = tx0 + (tx2 - tx0) * pr02;
	      double py02 = ty0 + (ty2 - ty0) * pr02;
	      double pz02 = tz0 + (tz2 - tz0) * pr02;
	      
	      AutoGL_DrawTriangle 
		(px02, py02, pz02,
		 px12, py12, pz12,
		 tx2, ty2, tz2);
	      
	    } else {
	      /* tv2 < prevLevelValue */

	      /* prev */
	      /* above vertex 2 */
	      
	      AutoGL_DrawTriangle 
		(tx0, ty0, tz0,
		 tx1, ty1, tz1,
		 tx2, ty2, tz2);
	    }
	  }
	}

	break;
      }
    }
  }
}

void AutoGL_DrawSmoothContourQuadrangle 
(double minRange, double maxRange,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3)
{
  AutoGL_DrawSmoothContourTriangle 
    (minRange, maxRange,
     x0, y0, z0, value0,  
     x1, y1, z1, value1,  
     x2, y2, z2, value2);
  AutoGL_DrawSmoothContourTriangle 
    (minRange, maxRange,
     x2, y2, z2, value2,  
     x3, y3, z3, value3,  
     x0, y0, z0, value0);
}

void AutoGL_DrawLineContourQuadrangle 
(double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3)
{
  AutoGL_DrawLineContourTriangle 
    (minRange, maxRange, nGrades,
     x0, y0, z0, value0,  
     x1, y1, z1, value1,  
     x2, y2, z2, value2);
  AutoGL_DrawLineContourTriangle 
    (minRange, maxRange, nGrades,
     x2, y2, z2, value2,  
     x3, y3, z3, value3,  
     x0, y0, z0, value0);
}

void AutoGL_DrawBandContourQuadrangle 
(double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3)
{
  AutoGL_DrawBandContourTriangle 
    (minRange, maxRange, nGrades,
     x0, y0, z0, value0,  
     x1, y1, z1, value1,  
     x2, y2, z2, value2);
  AutoGL_DrawBandContourTriangle 
    (minRange, maxRange, nGrades,
     x2, y2, z2, value2,  
     x3, y3, z3, value3,  
     x0, y0, z0, value0);
}

void AutoGL_DrawIsosurfaceTetrahedron
(double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3)
{
  double tetraPoints[4][3];
  double tetraValues[4];
  int iGrade;

  tetraPoints[0][0] = x0;
  tetraPoints[0][1] = y0;
  tetraPoints[0][2] = z0;
  tetraValues[0] = value0;

  tetraPoints[1][0] = x1;
  tetraPoints[1][1] = y1;
  tetraPoints[1][2] = z1;
  tetraValues[1] = value1;

  tetraPoints[2][0] = x2;
  tetraPoints[2][1] = y2;
  tetraPoints[2][2] = z2;
  tetraValues[2] = value2;

  tetraPoints[3][0] = x3;
  tetraPoints[3][1] = y3;
  tetraPoints[3][2] = z3;
  tetraValues[3] = value3;

  for (iGrade = 0; iGrade <= nGrades; iGrade++) {
    double levelValue;
    int nVertexs;
    int vertexIds0[4], vertexIds1[4];
    double ratios[4];

    if (0 < nGrades) {
      levelValue = minRange 
	+ (maxRange - minRange) / nGrades * iGrade;
    } else {
      levelValue = minRange;
    }

    if (AutoGL_GetIsosurface
	(&nVertexs, vertexIds0, vertexIds1, ratios,
	 levelValue, tetraValues)) {
      double points[4][3];
      
      if (0 < nGrades) {
	double grade = 1.0 / nGrades * iGrade;
	double red, green, blue;

	AutoGL_GetContourColor (&red, &green, &blue,
				grade);
	AutoGL_SetColor (red, green, blue);
      }

      AutoGL_GetIsosurfaceVertex 
	(points, 
	 tetraPoints, 
	 nVertexs, vertexIds0, vertexIds1, ratios);
      
      assert (nVertexs == 3 || nVertexs == 4);
      AutoGL_DrawTriangle (points[0][0], points[0][1], points[0][2],
			   points[1][0], points[1][1], points[1][2],
			   points[2][0], points[2][1], points[2][2]);
      if (nVertexs == 4) {
	AutoGL_DrawTriangle (points[2][0], points[2][1], points[2][2],
			     points[3][0], points[3][1], points[3][2],
			     points[0][0], points[0][1], points[0][2]);
      }
    }
  }
}

void AutoGL_DrawSectionSmoothContourTetrahedron
(double a, double b, double c, double d,
 double minRange, double maxRange,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3)
{
  double tetraPoints[4][3];
  double tetraGrades[4];

  tetraPoints[0][0] = x0;
  tetraPoints[0][1] = y0;
  tetraPoints[0][2] = z0;
  tetraGrades[0] = (value0 - minRange) / (maxRange - minRange);

  tetraPoints[1][0] = x1;
  tetraPoints[1][1] = y1;
  tetraPoints[1][2] = z1;
  tetraGrades[1] = (value1 - minRange) / (maxRange - minRange);

  tetraPoints[2][0] = x2;
  tetraPoints[2][1] = y2;
  tetraPoints[2][2] = z2;
  tetraGrades[2] = (value2 - minRange) / (maxRange - minRange);

  tetraPoints[3][0] = x3;
  tetraPoints[3][1] = y3;
  tetraPoints[3][2] = z3;
  tetraGrades[3] = (value3 - minRange) / (maxRange - minRange);

  AutoGL_DrawTetrahedronSectionContour 
    (a, b, c, d,
     tetraPoints,
     tetraGrades);
}

void AutoGL_DrawSectionLineContourTetrahedron
(double a, double b, double c, double d,
 double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3)
{
  double tetraPoints[4][3];
  double tetraValues[4];
  double tetraEquationValues[4];
  int iVertex;
  int nVertexs;
  int vertexIds0[4], vertexIds1[4];
  double ratios[4];

  tetraPoints[0][0] = x0;
  tetraPoints[0][1] = y0;
  tetraPoints[0][2] = z0;
  tetraValues[0] = value0;

  tetraPoints[1][0] = x1;
  tetraPoints[1][1] = y1;
  tetraPoints[1][2] = z1;
  tetraValues[1] = value1;

  tetraPoints[2][0] = x2;
  tetraPoints[2][1] = y2;
  tetraPoints[2][2] = z2;
  tetraValues[2] = value2;

  tetraPoints[3][0] = x3;
  tetraPoints[3][1] = y3;
  tetraPoints[3][2] = z3;
  tetraValues[3] = value3;

  for (iVertex = 0; iVertex < 4; iVertex++) {
    double x = tetraPoints[iVertex][0];
    double y = tetraPoints[iVertex][1];
    double z = tetraPoints[iVertex][2];
    tetraEquationValues[iVertex] = a * x + b * y + c * z + d;
  }
  
  if (AutoGL_GetIsosurface
      (&nVertexs, vertexIds0, vertexIds1, ratios,
       0.0, tetraEquationValues)) {
    double points[4][3];
    double values[4];
    
    AutoGL_GetIsosurfaceVertex 
      (points, 
       tetraPoints, 
       nVertexs, vertexIds0, vertexIds1, ratios);
    
    for (iVertex = 0; iVertex < nVertexs; iVertex++) {
      int v0 = vertexIds0[iVertex];
      int v1 = vertexIds1[iVertex];
      double ratio = ratios[iVertex];
      
      values[iVertex] = tetraValues[v0] 
	+ (tetraValues[v1] - tetraValues[v0]) * ratio;
    }

    assert (nVertexs == 3 || nVertexs == 4);
    AutoGL_DrawLineContourTriangle
      (minRange, maxRange, nGrades,
       points[0][0], points[0][1], points[0][2], values[0], 
       points[1][0], points[1][1], points[1][2], values[1],
       points[2][0], points[2][1], points[2][2], values[2]);
    if (nVertexs == 4) {
      AutoGL_DrawLineContourTriangle
	(minRange, maxRange, nGrades,
	 points[2][0], points[2][1], points[2][2], values[2],
	 points[3][0], points[3][1], points[3][2], values[3],
	 points[0][0], points[0][1], points[0][2], values[0]);
    }
  }
}

void AutoGL_DrawSectionBandContourTetrahedron
(double a, double b, double c, double d,
 double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3)
{
  double tetraPoints[4][3];
  double tetraValues[4];
  double tetraEquationValues[4];
  int iVertex;
  int nVertexs;
  int vertexIds0[4], vertexIds1[4];
  double ratios[4];

  tetraPoints[0][0] = x0;
  tetraPoints[0][1] = y0;
  tetraPoints[0][2] = z0;
  tetraValues[0] = value0;

  tetraPoints[1][0] = x1;
  tetraPoints[1][1] = y1;
  tetraPoints[1][2] = z1;
  tetraValues[1] = value1;

  tetraPoints[2][0] = x2;
  tetraPoints[2][1] = y2;
  tetraPoints[2][2] = z2;
  tetraValues[2] = value2;

  tetraPoints[3][0] = x3;
  tetraPoints[3][1] = y3;
  tetraPoints[3][2] = z3;
  tetraValues[3] = value3;

  for (iVertex = 0; iVertex < 4; iVertex++) {
    double x = tetraPoints[iVertex][0];
    double y = tetraPoints[iVertex][1];
    double z = tetraPoints[iVertex][2];
    tetraEquationValues[iVertex] = a * x + b * y + c * z + d;
  }
  
  if (AutoGL_GetIsosurface
      (&nVertexs, vertexIds0, vertexIds1, ratios,
       0.0, tetraEquationValues)) {
    double points[4][3];
    double values[4];
    
    AutoGL_GetIsosurfaceVertex 
      (points, 
       tetraPoints, 
       nVertexs, vertexIds0, vertexIds1, ratios);
    
    for (iVertex = 0; iVertex < nVertexs; iVertex++) {
      int v0 = vertexIds0[iVertex];
      int v1 = vertexIds1[iVertex];
      double ratio = ratios[iVertex];
      
      values[iVertex] = tetraValues[v0] 
	+ (tetraValues[v1] - tetraValues[v0]) * ratio;
    }

    assert (nVertexs == 3 || nVertexs == 4);
    AutoGL_DrawBandContourTriangle
      (minRange, maxRange, nGrades,
       points[0][0], points[0][1], points[0][2], values[0], 
       points[1][0], points[1][1], points[1][2], values[1],
       points[2][0], points[2][1], points[2][2], values[2]);
    if (nVertexs == 4) {
      AutoGL_DrawBandContourTriangle
	(minRange, maxRange, nGrades,
	 points[2][0], points[2][1], points[2][2], values[2],
	 points[3][0], points[3][1], points[3][2], values[3],
	 points[0][0], points[0][1], points[0][2], values[0]);
    }
  }
}

void AutoGL_DrawIsosurfaceHexahedron
(double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3,
 double x4, double y4, double z4, double value4,  
 double x5, double y5, double z5, double value5,  
 double x6, double y6, double z6, double value6,  
 double x7, double y7, double z7, double value7)
{
  AutoGL_DrawIsosurfaceTetrahedron
    (minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x1, y1, z1, value1,
     x2, y2, z2, value2,
     x5, y5, z5, value5);
  AutoGL_DrawIsosurfaceTetrahedron
    (minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x5, y5, z5, value5,
     x2, y2, z2, value2,
     x6, y6, z6, value6);
  AutoGL_DrawIsosurfaceTetrahedron
    (minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x5, y5, z5, value5,
     x6, y6, z6, value6,
     x4, y4, z4, value4);
  AutoGL_DrawIsosurfaceTetrahedron
    (minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x2, y2, z2, value2,
     x3, y3, z3, value3,
     x6, y6, z6, value6);
  AutoGL_DrawIsosurfaceTetrahedron
    (minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x4, y4, z4, value4,
     x6, y6, z6, value6,
     x7, y7, z7, value7);
  AutoGL_DrawIsosurfaceTetrahedron
    (minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x6, y6, z6, value6,
     x3, y3, z3, value3,
     x7, y7, z7, value7);
}

void AutoGL_DrawSectionSmoothContourHexahedron
(double a, double b, double c, double d,
 double minRange, double maxRange,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3,
 double x4, double y4, double z4, double value4,  
 double x5, double y5, double z5, double value5,  
 double x6, double y6, double z6, double value6,  
 double x7, double y7, double z7, double value7)
{
  AutoGL_DrawSectionSmoothContourTetrahedron
    (a, b, c, d, minRange, maxRange,
     x0, y0, z0, value0,
     x1, y1, z1, value1,
     x2, y2, z2, value2,
     x5, y5, z5, value5);
  AutoGL_DrawSectionSmoothContourTetrahedron
    (a, b, c, d, minRange, maxRange,
     x0, y0, z0, value0,
     x5, y5, z5, value5,
     x2, y2, z2, value2,
     x6, y6, z6, value6);
  AutoGL_DrawSectionSmoothContourTetrahedron
    (a, b, c, d, minRange, maxRange,
     x0, y0, z0, value0,
     x5, y5, z5, value5,
     x6, y6, z6, value6,
     x4, y4, z4, value4);
  AutoGL_DrawSectionSmoothContourTetrahedron
    (a, b, c, d, minRange, maxRange,
     x0, y0, z0, value0,
     x2, y2, z2, value2,
     x3, y3, z3, value3,
     x6, y6, z6, value6);
  AutoGL_DrawSectionSmoothContourTetrahedron
    (a, b, c, d, minRange, maxRange,
     x0, y0, z0, value0,
     x4, y4, z4, value4,
     x6, y6, z6, value6,
     x7, y7, z7, value7);
  AutoGL_DrawSectionSmoothContourTetrahedron
    (a, b, c, d, minRange, maxRange,
     x0, y0, z0, value0,
     x6, y6, z6, value6,
     x3, y3, z3, value3,
     x7, y7, z7, value7);
}

void AutoGL_DrawSectionLineContourHexahedron
(double a, double b, double c, double d,
 double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3,
 double x4, double y4, double z4, double value4,  
 double x5, double y5, double z5, double value5,  
 double x6, double y6, double z6, double value6,  
 double x7, double y7, double z7, double value7)
{
  AutoGL_DrawSectionLineContourTetrahedron
    (a, b, c, d, minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x1, y1, z1, value1,
     x2, y2, z2, value2,
     x5, y5, z5, value5);
  AutoGL_DrawSectionLineContourTetrahedron
    (a, b, c, d, minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x5, y5, z5, value5,
     x2, y2, z2, value2,
     x6, y6, z6, value6);
  AutoGL_DrawSectionLineContourTetrahedron
    (a, b, c, d, minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x5, y5, z5, value5,
     x6, y6, z6, value6,
     x4, y4, z4, value4);
  AutoGL_DrawSectionLineContourTetrahedron
    (a, b, c, d, minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x2, y2, z2, value2,
     x3, y3, z3, value3,
     x6, y6, z6, value6);
  AutoGL_DrawSectionLineContourTetrahedron
    (a, b, c, d, minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x4, y4, z4, value4,
     x6, y6, z6, value6,
     x7, y7, z7, value7);
  AutoGL_DrawSectionLineContourTetrahedron
    (a, b, c, d, minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x6, y6, z6, value6,
     x3, y3, z3, value3,
     x7, y7, z7, value7);
}

void AutoGL_DrawSectionBandContourHexahedron
(double a, double b, double c, double d,
 double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3,
 double x4, double y4, double z4, double value4,  
 double x5, double y5, double z5, double value5,  
 double x6, double y6, double z6, double value6,  
 double x7, double y7, double z7, double value7)
{
  AutoGL_DrawSectionBandContourTetrahedron
    (a, b, c, d, minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x1, y1, z1, value1,
     x2, y2, z2, value2,
     x5, y5, z5, value5);
  AutoGL_DrawSectionBandContourTetrahedron
    (a, b, c, d, minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x5, y5, z5, value5,
     x2, y2, z2, value2,
     x6, y6, z6, value6);
  AutoGL_DrawSectionBandContourTetrahedron
    (a, b, c, d, minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x5, y5, z5, value5,
     x6, y6, z6, value6,
     x4, y4, z4, value4);
  AutoGL_DrawSectionBandContourTetrahedron
    (a, b, c, d, minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x2, y2, z2, value2,
     x3, y3, z3, value3,
     x6, y6, z6, value6);
  AutoGL_DrawSectionBandContourTetrahedron
    (a, b, c, d, minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x4, y4, z4, value4,
     x6, y6, z6, value6,
     x7, y7, z7, value7);
  AutoGL_DrawSectionBandContourTetrahedron
    (a, b, c, d, minRange, maxRange, nGrades,
     x0, y0, z0, value0,
     x6, y6, z6, value6,
     x3, y3, z3, value3,
     x7, y7, z7, value7);
}



