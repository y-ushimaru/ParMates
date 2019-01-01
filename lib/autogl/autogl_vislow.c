/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_vislow.c : AutoGL utility for visualization (low level API) */

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



/* Visualization : low level API */

int AutoGL_HexToTetraMap[6][4] = {
  { 0, 1, 2, 5 }, 
  { 0, 5, 2, 6 }, 
  { 0, 5, 6, 4 }, 
  { 0, 2, 3, 6 }, 
  { 0, 4, 6, 7 }, 
  { 0, 6, 3, 7 }
};

static int TriangleEdgeVertexMap[3][2] = {
  { 0, 1 }, { 1, 2 }, { 2, 0 }
};

typedef struct {
  int nEdges;
  int edges[2];
} TriangleEdgeCase;

static TriangleEdgeCase TriangleEdgeCases[4] = {
  { 0, { -1, -1 } },
  { 2, { 0, 2 } },
  { 2, { 0, 1 } },
  { 2, { 1, 2 } }
};

static int TetrahedronEdgeVertexMap[6][2] = {
  { 0, 1 }, { 1, 2 }, { 2, 0 }, 
  { 0, 3 }, { 1, 3 }, { 2, 3 }
};

typedef struct {
  int nEdges;
  int edges[4];
} TetrahedronEdgeCase;

static TetrahedronEdgeCase TetrahedronEdgeCases[8] = {
  { 0, { -1, -1, -1, -1 } },
  { 3, { 0, 2, 3, -1 } },
  { 3, { 0, 1, 4, -1 } },
  { 4, { 1, 2, 3, 4 } },
  { 3, { 1, 2, 5, -1 } },
  { 4, { 0, 1, 5, 3 } },
  { 4, { 0, 4, 5, 2 } },
  { 3, { 3, 4, 5, -1 } }
};

int AutoGL_GetIsoline 
(int vertexIds0_OUT[2],
 int vertexIds1_OUT[2],
 double ratios_OUT[2],
 double levelValue, 
 double triValues[3])
{
  unsigned int mask;
  int nEdges;
  int iEdge;

  mask = 0;
  if (levelValue < triValues[0]) mask |= 1;
  if (levelValue < triValues[1]) mask |= 2;
  if (levelValue < triValues[2]) mask |= 4;
  if ((mask & 4) != 0) mask = (~mask) & 7;

  nEdges = TriangleEdgeCases[mask].nEdges;
  if (nEdges == 0) return 0;
  
  assert(nEdges == 2);

  for (iEdge = 0; iEdge < nEdges; iEdge++) {
    int edgeId = TriangleEdgeCases[mask].edges[iEdge];
    int v0 = TriangleEdgeVertexMap[edgeId][0];
    int v1 = TriangleEdgeVertexMap[edgeId][1];
    double ratio = (levelValue - triValues[v0]) 
      / (triValues[v1] - triValues[v0]);
    
    vertexIds0_OUT[iEdge] = v0; 
    vertexIds1_OUT[iEdge] = v1;
    ratios_OUT[iEdge] = ratio;
  }    

  return 1;
}

void AutoGL_GetIsolineVertex 
(double points_OUT[2][3],
 double triPoints[3][3], 
 int vertexIds0[2],
 int vertexIds1[2],
 double ratios[2])
{
  int iVertex;

  for (iVertex = 0; iVertex < 2; iVertex++) {
    int v0 = vertexIds0[iVertex];
    int v1 = vertexIds1[iVertex];
    double ratio = ratios[iVertex];

    points_OUT[iVertex][0] = triPoints[v0][0] 
      + (triPoints[v1][0] - triPoints[v0][0]) * ratio;
    points_OUT[iVertex][1] = triPoints[v0][1] 
      + (triPoints[v1][1] - triPoints[v0][1]) * ratio;
    points_OUT[iVertex][2] = triPoints[v0][2] 
      + (triPoints[v1][2] - triPoints[v0][2]) * ratio;
  }
}

void AutoGL_DrawTriangleIsoline 
(double levelValue,
 double triPoints[3][3],
 double triValues[3])
{
  int vertexIds0[2], vertexIds1[2];
  double ratios[2];
      
  if (AutoGL_GetIsoline
      (vertexIds0, vertexIds1, ratios,
       levelValue, triValues)) {
    double points[2][3];
    
    AutoGL_GetIsolineVertex 
      (points,
       triPoints, vertexIds0, vertexIds1, ratios);
    AutoGL_DrawLine 
      (points[0][0], points[0][1], points[0][2],
       points[1][0], points[1][1], points[1][2]);
  }
}

int AutoGL_GetIsosurface 
(int *nVertexs_OUT,
 int vertexIds0_OUT[4],
 int vertexIds1_OUT[4],
 double ratios_OUT[4],
 double levelValue, 
 double tetraValues[4])
{
  unsigned int mask;
  int nEdges;
  int iEdge;

  mask = 0;
  if (levelValue < tetraValues[0]) mask |= 1;
  if (levelValue < tetraValues[1]) mask |= 2;
  if (levelValue < tetraValues[2]) mask |= 4;
  if (levelValue < tetraValues[3]) mask |= 8;
  if ((mask & 8) != 0) mask = (~mask) & 15;

  nEdges = TetrahedronEdgeCases[mask].nEdges;
  if (nEdges <= 2) return 0;
  assert(nEdges == 3 || nEdges == 4);

  *nVertexs_OUT = nEdges;
  for (iEdge = 0; iEdge < nEdges; iEdge++) {
    int edgeId = TetrahedronEdgeCases[mask].edges[iEdge];
    int v0 = TetrahedronEdgeVertexMap[edgeId][0];
    int v1 = TetrahedronEdgeVertexMap[edgeId][1];
    double ratio = (levelValue - tetraValues[v0]) 
      / (tetraValues[v1] - tetraValues[v0]);

    vertexIds0_OUT[iEdge] = v0; 
    vertexIds1_OUT[iEdge] = v1;
    ratios_OUT[iEdge] = ratio;
  }

  return 1;
}

void AutoGL_GetIsosurfaceVertex 
(double points_OUT[4][3],
 double tetraPoints[4][3], 
 int nVertexs, 
 int vertexIds0[4],
 int vertexIds1[4],
 double ratios[4])
{
  int iVertex;

  assert(nVertexs == 3 || nVertexs == 4);

  for (iVertex = 0; iVertex < nVertexs; iVertex++) {
    int v0 = vertexIds0[iVertex];
    int v1 = vertexIds1[iVertex];
    double ratio = ratios[iVertex];

    points_OUT[iVertex][0] = tetraPoints[v0][0] 
      + (tetraPoints[v1][0] - tetraPoints[v0][0]) * ratio;
    points_OUT[iVertex][1] = tetraPoints[v0][1] 
      + (tetraPoints[v1][1] - tetraPoints[v0][1]) * ratio;
    points_OUT[iVertex][2] = tetraPoints[v0][2] 
      + (tetraPoints[v1][2] - tetraPoints[v0][2]) * ratio;
  }
}

void AutoGL_DrawTetrahedronIsosurface 
(double levelValue,
 double tetraPoints[4][3],
 double tetraValues[4])
{
  int nVertexs;
  int vertexIds0[4], vertexIds1[4];
  double ratios[4];
  
  if (AutoGL_GetIsosurface
      (&nVertexs, vertexIds0, vertexIds1, ratios,
       levelValue, tetraValues)) {
    double points[4][3];
    
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

void AutoGL_DrawTetrahedronSectionContour 
(double a, double b, double c, double d,
 double tetraPoints[4][3],
 double tetraGrades[4])
{
  int iVertex;
  double tetraEquationValues[4];
  int nVertexs;
  int vertexIds0[4], vertexIds1[4];
  double ratios[4];

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
    double grades[4];
    
    AutoGL_GetIsosurfaceVertex 
      (points, 
       tetraPoints, 
       nVertexs, vertexIds0, vertexIds1, ratios);
    
    for (iVertex = 0; iVertex < nVertexs; iVertex++) {
      int v0 = vertexIds0[iVertex];
      int v1 = vertexIds1[iVertex];
      double ratio = ratios[iVertex];
      
      grades[iVertex] = tetraGrades[v0] 
	+ (tetraGrades[v1] - tetraGrades[v0]) * ratio;
    }

    assert (nVertexs == 3 || nVertexs == 4);
    AutoGL_DrawContourTriangle 
      (points[0][0], points[0][1], points[0][2], grades[0], 
       points[1][0], points[1][1], points[1][2], grades[1],
       points[2][0], points[2][1], points[2][2], grades[2]);
    if (nVertexs == 4) {
      AutoGL_DrawContourTriangle 
	(points[2][0], points[2][1], points[2][2], grades[2],
	 points[3][0], points[3][1], points[3][2], grades[3],
	 points[0][0], points[0][1], points[0][2], grades[0]);
    }
  }
}



