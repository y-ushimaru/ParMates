/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_VISLOW__
#define __AUTOGL_VISLOW__

/* $B2D;k2=%f!<%F%#%j%F%#(B($BDc%l%Y%k2D;k2=(BAPI) */



/* $B#1$D$NO;LLBN$r#6$D$N;MLLBN$KJ,3d$9$k$5$$$ND:E@BP1~I=(B */
extern int AutoGL_HexToTetraMap[6][4];

/* $B;03Q7A$+$iEy9b@~!JEyCM@~!K%G!<%?$r7W;;$9$k!#(B */
int AutoGL_GetIsoline 
(int vertexIds0_OUT[2],
 int vertexIds1_OUT[2],
 double ratios_OUT[2],
 double levelValue, 
 double triValues[3]);
/* vertexIds0_OUT, vertexIds0_OUT : $BEy9b@~$N;03Q7A$K$*$1$kD:E@HV9f(B */
/* ratios_OUT : $B>e5-#2D:E@4V$NJd4VHfN((B */
/* levelValue : $BEy9b@~$N%9%+%i!<CM(B */
/* triValues : $B;03Q7A$N3FD:E@$N%9%+%i!<CM(B */

/* $B;03Q7A$+$iEy9b@~!JEyCM@~!K$ND:E@$r7W;;$9$k!#(B */
void AutoGL_GetIsolineVertex 
(double points_OUT[2][3],
 double triPoints[3][3], 
 int vertexIds0[2],
 int vertexIds1[2],
 double ratios[2]);
/* points_OUT : $BEy9b@~$ND:E@:BI8(B */
/* triPoints : $B;03Q7A$N3FD:E@$N:BI8(B */
/* nVertexs : $BEy9b@~$ND:E@?t(B */
/* vertexIds0, vertexIds0 : $BEy9b@~$N;03Q7A$K$*$1$kD:E@HV9f(B */
/* ratios : $B>e5-#2D:E@4V$NJd4VHfN((B */

/* $B;03Q7A$NEy9b@~!JEyCM@~!K$rIA2h$9$k!#(B */
void AutoGL_DrawTriangleIsoline 
(double levelValue,
 double triPoints[3][3],
 double triValues[3]);
/* levelValue : $BEy9b@~$N%9%+%i!<CM(B */
/* triPoints : $B;03Q7A$N3FD:E@$N:BI8(B */
/* triValues : $B;03Q7A$N3FD:E@$N%9%+%i!<CM(B */

/* $B;MLLBN$+$iEy9bLL!JEyCMLL!K%G!<%?$r7W;;$9$k!#(B */
int AutoGL_GetIsosurface 
(int *nVertexs_OUT,
 int vertexIds0_OUT[4],
 int vertexIds1_OUT[4],
 double ratios_OUT[4],
 double levelValue, 
 double tetraValues[4]);
/* nVertexs_OUT : $BEyCMLL$ND:E@?t(B */
/* vertexIds0_OUT, vertexIds0_OUT : $BEyCMLL$N;MLLBN$K$*$1$kD:E@HV9f(B */
/* ratios_OUT : $B>e5-#2D:E@4V$NJd4VHfN((B */
/* levelValue : $BEy9b@~$N%9%+%i!<CM(B */
/* tetraValues : $B;MLLBN$N3FD:E@$N%9%+%i!<CM(B */

/* $B;MLLBN$+$iEy9bLL!JEyCMLL!K$ND:E@$r7W;;$9$k!#(B */
void AutoGL_GetIsosurfaceVertex 
(double points_OUT[4][3],
 double tetraPoints[4][3], 
 int nVertexs, 
 int vertexIds0[4],
 int vertexIds1[4],
 double ratios[4]);
/* points_OUT : $BEyCMLL$ND:E@:BI8(B */
/* tetraPoints : $B;MLLBN$N3FD:E@$N:BI8(B */
/* nVertexs : $BEyCMLL$ND:E@?t(B */
/* vertexIds0, vertexIds0 : $BEyCMLL$N;MLLBN$K$*$1$kD:E@HV9f(B */
/* ratios : $B>e5-#2D:E@4V$NJd4VHfN((B */

/* $B;MLLBN$NEy9bLL!JEyCMLL!K$rIA2h$9$k!#(B */
void AutoGL_DrawTetrahedronIsosurface 
(double levelValue,
 double tetraPoints[4][3],
 double tetraValues[4]);
/* levelValue : $BEy9b@~$N%9%+%i!<CM(B */
/* tetraPoints : $B;MLLBN$N3FD:E@$N:BI8(B */
/* tetraValues : $B;MLLBN$N3FD:E@$N%9%+%i!<CM(B */

/* $B;MLLBN$HJ?LL$H$N8r:9CGLL$NLL%3%s%?(B-$B$rIA2h$9$k!#(B */
void AutoGL_DrawTetrahedronSectionContour 
(double a, double b, double c, double d,
 double tetraPoints[4][3],
 double tetraGrades[4]);
/* a, b, c, d : $BCGLL$NJ}Dx<0(Bax+by+cz+d=0$B$N78?t(B */
/* tetraPoints : $B;MLLBN$N3FD:E@$N:BI8(B */
/* tetraGrades : $B;MLLBN$N3FD:E@$N%9%+%i!<CM$r(B0.0 - 1.0$B$K@55,2=$7$?$b$N(B */



#endif  /* __AUTOGL_VISLOW__ */









