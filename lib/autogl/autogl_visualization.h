/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_VISUALIZATION__
#define __AUTOGL_VISUALIZATION__

/* $B2D;k2=%f!<%F%#%j%F%#(B($B9b%l%Y%k2D;k2=(BAPI) */



/* $B;03Q7A$r%9%`!<%:%3%s%?!<$GEI$j$D$V$9!#(B */
void AutoGL_DrawSmoothContourTriangle 
(double minRange, double maxRange,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2);
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) : $B#3E@$N@$3&:BI8(B */
/* value0, value1, value2 : $B#3E@$N%9%+%i!<CM(B */

/* $B;03Q7A$r@~%3%s%?!<$GEI$j$D$V$9!#(B */
void AutoGL_DrawLineContourTriangle 
(double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2); 
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */
/* nGrades : $B@~%3%s%?(B-$B$NK\?t(B (-1) */
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) : $B#3E@$N@$3&:BI8(B */
/* value0, value1, value2 : $B#3E@$N%9%+%i!<CM(B */

/* $B;03Q7A$r%P%s%I%3%s%?!<$GEI$j$D$V$9!#(B */
void AutoGL_DrawBandContourTriangle 
(double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2);
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */
/* nGrades : $B%P%s%I%3%s%?(B-$B$NK\?t(B (-1) */
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) : $B#3E@$N@$3&:BI8(B */
/* value0, value1, value2 : $B#3E@$N%9%+%i!<CM(B */

/* $B;MJU7A$r%9%`!<%:%3%s%?!<$GEI$j$D$V$9!#(B */
void AutoGL_DrawSmoothContourQuadrangle 
(double minRange, double maxRange,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3);
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */
/* (x0, y0, z0) - (x3, y3, z3) : $B#4E@$N@$3&:BI8(B */
/* value0 - value3 : $B#4E@$N%9%+%i!<CM(B */

/* $B;MJU7A$r@~%3%s%?!<$GEI$j$D$V$9!#(B */
void AutoGL_DrawLineContourQuadrangle 
(double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3);
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */
/* nGrades : $B@~%3%s%?(B-$B$NK\?t(B (-1) */
/* (x0, y0, z0) - (x3, y3, z3) : $B#4E@$N@$3&:BI8(B */
/* value0 - value3 : $B#4E@$N%9%+%i!<CM(B */

/* $B;MJU7A$r%P%s%I%3%s%?!<$GEI$j$D$V$9!#(B */
void AutoGL_DrawBandContourQuadrangle 
(double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3);
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */
/* nGrades : $B%P%s%I%3%s%?(B-$B$NK\?t(B (-1) */
/* (x0, y0, z0) - (x3, y3, z3) : $B#4E@$N@$3&:BI8(B */
/* value0 - value3 : $B#4E@$N%9%+%i!<CM(B */

/* $B;MLLBN$rEyCMLL$GEI$j$D$V$9!#(B */
void AutoGL_DrawIsosurfaceTetrahedron
(double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3);
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */
/* nGrades : $B@~%3%s%?(B-$B$NK\?t(B (-1) */
/* (x0, y0, z0) - (x3, y3, z3) : $B#4E@$N@$3&:BI8(B */
/* value0 - value3 : $B#4E@$N%9%+%i!<CM(B */

/* $B;MLLBN$HJ?LL$H$N8r:9CGLL$r%9%`!<%:%3%s%?!<$GEI$j$D$V$9!#(B */
void AutoGL_DrawSectionSmoothContourTetrahedron
(double a, double b, double c, double d,
 double minRange, double maxRange,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3);
/* a, b, c, d : $BCGLL$NJ}Dx<0(Bax+by+cz+d=0$B$N78?t(B */
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */
/* (x0, y0, z0) - (x3, y3, z3) : $B#4E@$N@$3&:BI8(B */
/* value0 - value3 : $B#4E@$N%9%+%i!<CM(B */

/* $B;MLLBN$HJ?LL$H$N8r:9CGLL$r@~%3%s%?!<$GEI$j$D$V$9!#(B */
void AutoGL_DrawSectionLineContourTetrahedron
(double a, double b, double c, double d,
 double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3);
/* a, b, c, d : $BCGLL$NJ}Dx<0(Bax+by+cz+d=0$B$N78?t(B */
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */
/* nGrades : $B@~%3%s%?(B-$B$NK\?t(B (-1) */
/* (x0, y0, z0) - (x3, y3, z3) : $B#4E@$N@$3&:BI8(B */
/* value0 - value3 : $B#4E@$N%9%+%i!<CM(B */

/* $B;MLLBN$HJ?LL$H$N8r:9CGLL$r%P%s%I%3%s%?!<$GEI$j$D$V$9!#(B */
void AutoGL_DrawSectionBandContourTetrahedron
(double a, double b, double c, double d,
 double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3);
/* a, b, c, d : $BCGLL$NJ}Dx<0(Bax+by+cz+d=0$B$N78?t(B */
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */
/* nGrades : $B%P%s%I%3%s%?(B-$B$NK\?t(B (-1) */
/* (x0, y0, z0) - (x3, y3, z3) : $B#4E@$N@$3&:BI8(B */
/* value0 - value3 : $B#4E@$N%9%+%i!<CM(B */

/* $BO;LLBN$rEyCMLL$GEI$j$D$V$9!#(B */
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
 double x7, double y7, double z7, double value7);
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */
/* nGrades : $B@~%3%s%?(B-$B$NK\?t(B (-1) */
/* (x0, y0, z0) - (x7, y7, z7) : $B#8E@$N@$3&:BI8(B */
/* value0 - value7 : $B#8E@$N%9%+%i!<CM(B */

/* $BO;LLBN$HJ?LL$H$N8r:9CGLL$r%9%`!<%:%3%s%?!<$GEI$j$D$V$9!#(B */
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
 double x7, double y7, double z7, double value7);
/* a, b, c, d : $BCGLL$NJ}Dx<0(Bax+by+cz+d=0$B$N78?t(B */
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */
/* (x0, y0, z0) - (x7, y7, z7) : $B#8E@$N@$3&:BI8(B */
/* value0 - value7 : $B#8E@$N%9%+%i!<CM(B */

/* $BO;LLBN$HJ?LL$H$N8r:9CGLL$r@~%3%s%?!<$GEI$j$D$V$9!#(B */
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
 double x7, double y7, double z7, double value7);
/* a, b, c, d : $BCGLL$NJ}Dx<0(Bax+by+cz+d=0$B$N78?t(B */
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */
/* nGrades : $B@~%3%s%?(B-$B$NK\?t(B (-1) */
/* (x0, y0, z0) - (x7, y7, z7) : $B#8E@$N@$3&:BI8(B */
/* value0 - value7 : $B#8E@$N%9%+%i!<CM(B */

/* $BO;LLBN$HJ?LL$H$N8r:9CGLL$r%P%s%I%3%s%?!<$GEI$j$D$V$9!#(B */
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
 double x7, double y7, double z7, double value7);
/* a, b, c, d : $BCGLL$NJ}Dx<0(Bax+by+cz+d=0$B$N78?t(B */
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */
/* nGrades : $B%P%s%I%3%s%?(B-$B$NK\?t(B (-1) */
/* (x0, y0, z0) - (x7, y7, z7) : $B#8E@$N@$3&:BI8(B */
/* value0 - value7 : $B#8E@$N%9%+%i!<CM(B */



#endif  /* __AUTOGL_VISUALIZATION__ */






