/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_VISUALIZATION__
#define __AUTOGL_VISUALIZATION__

/* 可視化ユーティリティ(高レベル可視化API) */



/* 三角形をスムーズコンターで塗りつぶす。 */
void AutoGL_DrawSmoothContourTriangle 
(double minRange, double maxRange,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2);
/* minRange, maxRange : スカラー値の最大最小 */
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) : ３点の世界座標 */
/* value0, value1, value2 : ３点のスカラー値 */

/* 三角形を線コンターで塗りつぶす。 */
void AutoGL_DrawLineContourTriangle 
(double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2); 
/* minRange, maxRange : スカラー値の最大最小 */
/* nGrades : 線コンタ-の本数 (-1) */
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) : ３点の世界座標 */
/* value0, value1, value2 : ３点のスカラー値 */

/* 三角形をバンドコンターで塗りつぶす。 */
void AutoGL_DrawBandContourTriangle 
(double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2);
/* minRange, maxRange : スカラー値の最大最小 */
/* nGrades : バンドコンタ-の本数 (-1) */
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) : ３点の世界座標 */
/* value0, value1, value2 : ３点のスカラー値 */

/* 四辺形をスムーズコンターで塗りつぶす。 */
void AutoGL_DrawSmoothContourQuadrangle 
(double minRange, double maxRange,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3);
/* minRange, maxRange : スカラー値の最大最小 */
/* (x0, y0, z0) - (x3, y3, z3) : ４点の世界座標 */
/* value0 - value3 : ４点のスカラー値 */

/* 四辺形を線コンターで塗りつぶす。 */
void AutoGL_DrawLineContourQuadrangle 
(double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3);
/* minRange, maxRange : スカラー値の最大最小 */
/* nGrades : 線コンタ-の本数 (-1) */
/* (x0, y0, z0) - (x3, y3, z3) : ４点の世界座標 */
/* value0 - value3 : ４点のスカラー値 */

/* 四辺形をバンドコンターで塗りつぶす。 */
void AutoGL_DrawBandContourQuadrangle 
(double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3);
/* minRange, maxRange : スカラー値の最大最小 */
/* nGrades : バンドコンタ-の本数 (-1) */
/* (x0, y0, z0) - (x3, y3, z3) : ４点の世界座標 */
/* value0 - value3 : ４点のスカラー値 */

/* 四面体を等値面で塗りつぶす。 */
void AutoGL_DrawIsosurfaceTetrahedron
(double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3);
/* minRange, maxRange : スカラー値の最大最小 */
/* nGrades : 線コンタ-の本数 (-1) */
/* (x0, y0, z0) - (x3, y3, z3) : ４点の世界座標 */
/* value0 - value3 : ４点のスカラー値 */

/* 四面体と平面との交差断面をスムーズコンターで塗りつぶす。 */
void AutoGL_DrawSectionSmoothContourTetrahedron
(double a, double b, double c, double d,
 double minRange, double maxRange,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3);
/* a, b, c, d : 断面の方程式ax+by+cz+d=0の係数 */
/* minRange, maxRange : スカラー値の最大最小 */
/* (x0, y0, z0) - (x3, y3, z3) : ４点の世界座標 */
/* value0 - value3 : ４点のスカラー値 */

/* 四面体と平面との交差断面を線コンターで塗りつぶす。 */
void AutoGL_DrawSectionLineContourTetrahedron
(double a, double b, double c, double d,
 double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3);
/* a, b, c, d : 断面の方程式ax+by+cz+d=0の係数 */
/* minRange, maxRange : スカラー値の最大最小 */
/* nGrades : 線コンタ-の本数 (-1) */
/* (x0, y0, z0) - (x3, y3, z3) : ４点の世界座標 */
/* value0 - value3 : ４点のスカラー値 */

/* 四面体と平面との交差断面をバンドコンターで塗りつぶす。 */
void AutoGL_DrawSectionBandContourTetrahedron
(double a, double b, double c, double d,
 double minRange, double maxRange,
 int nGrades,
 double x0, double y0, double z0, double value0,  
 double x1, double y1, double z1, double value1,  
 double x2, double y2, double z2, double value2,  
 double x3, double y3, double z3, double value3);
/* a, b, c, d : 断面の方程式ax+by+cz+d=0の係数 */
/* minRange, maxRange : スカラー値の最大最小 */
/* nGrades : バンドコンタ-の本数 (-1) */
/* (x0, y0, z0) - (x3, y3, z3) : ４点の世界座標 */
/* value0 - value3 : ４点のスカラー値 */

/* 六面体を等値面で塗りつぶす。 */
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
/* minRange, maxRange : スカラー値の最大最小 */
/* nGrades : 線コンタ-の本数 (-1) */
/* (x0, y0, z0) - (x7, y7, z7) : ８点の世界座標 */
/* value0 - value7 : ８点のスカラー値 */

/* 六面体と平面との交差断面をスムーズコンターで塗りつぶす。 */
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
/* a, b, c, d : 断面の方程式ax+by+cz+d=0の係数 */
/* minRange, maxRange : スカラー値の最大最小 */
/* (x0, y0, z0) - (x7, y7, z7) : ８点の世界座標 */
/* value0 - value7 : ８点のスカラー値 */

/* 六面体と平面との交差断面を線コンターで塗りつぶす。 */
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
/* a, b, c, d : 断面の方程式ax+by+cz+d=0の係数 */
/* minRange, maxRange : スカラー値の最大最小 */
/* nGrades : 線コンタ-の本数 (-1) */
/* (x0, y0, z0) - (x7, y7, z7) : ８点の世界座標 */
/* value0 - value7 : ８点のスカラー値 */

/* 六面体と平面との交差断面をバンドコンターで塗りつぶす。 */
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
/* a, b, c, d : 断面の方程式ax+by+cz+d=0の係数 */
/* minRange, maxRange : スカラー値の最大最小 */
/* nGrades : バンドコンタ-の本数 (-1) */
/* (x0, y0, z0) - (x7, y7, z7) : ８点の世界座標 */
/* value0 - value7 : ８点のスカラー値 */



#endif  /* __AUTOGL_VISUALIZATION__ */






