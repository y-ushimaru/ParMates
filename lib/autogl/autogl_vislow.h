/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_VISLOW__
#define __AUTOGL_VISLOW__

/* 可視化ユーティリティ(低レベル可視化API) */



/* １つの六面体を６つの四面体に分割するさいの頂点対応表 */
extern int AutoGL_HexToTetraMap[6][4];

/* 三角形から等高線（等値線）データを計算する。 */
int AutoGL_GetIsoline 
(int vertexIds0_OUT[2],
 int vertexIds1_OUT[2],
 double ratios_OUT[2],
 double levelValue, 
 double triValues[3]);
/* vertexIds0_OUT, vertexIds0_OUT : 等高線の三角形における頂点番号 */
/* ratios_OUT : 上記２頂点間の補間比率 */
/* levelValue : 等高線のスカラー値 */
/* triValues : 三角形の各頂点のスカラー値 */

/* 三角形から等高線（等値線）の頂点を計算する。 */
void AutoGL_GetIsolineVertex 
(double points_OUT[2][3],
 double triPoints[3][3], 
 int vertexIds0[2],
 int vertexIds1[2],
 double ratios[2]);
/* points_OUT : 等高線の頂点座標 */
/* triPoints : 三角形の各頂点の座標 */
/* nVertexs : 等高線の頂点数 */
/* vertexIds0, vertexIds0 : 等高線の三角形における頂点番号 */
/* ratios : 上記２頂点間の補間比率 */

/* 三角形の等高線（等値線）を描画する。 */
void AutoGL_DrawTriangleIsoline 
(double levelValue,
 double triPoints[3][3],
 double triValues[3]);
/* levelValue : 等高線のスカラー値 */
/* triPoints : 三角形の各頂点の座標 */
/* triValues : 三角形の各頂点のスカラー値 */

/* 四面体から等高面（等値面）データを計算する。 */
int AutoGL_GetIsosurface 
(int *nVertexs_OUT,
 int vertexIds0_OUT[4],
 int vertexIds1_OUT[4],
 double ratios_OUT[4],
 double levelValue, 
 double tetraValues[4]);
/* nVertexs_OUT : 等値面の頂点数 */
/* vertexIds0_OUT, vertexIds0_OUT : 等値面の四面体における頂点番号 */
/* ratios_OUT : 上記２頂点間の補間比率 */
/* levelValue : 等高線のスカラー値 */
/* tetraValues : 四面体の各頂点のスカラー値 */

/* 四面体から等高面（等値面）の頂点を計算する。 */
void AutoGL_GetIsosurfaceVertex 
(double points_OUT[4][3],
 double tetraPoints[4][3], 
 int nVertexs, 
 int vertexIds0[4],
 int vertexIds1[4],
 double ratios[4]);
/* points_OUT : 等値面の頂点座標 */
/* tetraPoints : 四面体の各頂点の座標 */
/* nVertexs : 等値面の頂点数 */
/* vertexIds0, vertexIds0 : 等値面の四面体における頂点番号 */
/* ratios : 上記２頂点間の補間比率 */

/* 四面体の等高面（等値面）を描画する。 */
void AutoGL_DrawTetrahedronIsosurface 
(double levelValue,
 double tetraPoints[4][3],
 double tetraValues[4]);
/* levelValue : 等高線のスカラー値 */
/* tetraPoints : 四面体の各頂点の座標 */
/* tetraValues : 四面体の各頂点のスカラー値 */

/* 四面体と平面との交差断面の面コンタ-を描画する。 */
void AutoGL_DrawTetrahedronSectionContour 
(double a, double b, double c, double d,
 double tetraPoints[4][3],
 double tetraGrades[4]);
/* a, b, c, d : 断面の方程式ax+by+cz+d=0の係数 */
/* tetraPoints : 四面体の各頂点の座標 */
/* tetraGrades : 四面体の各頂点のスカラー値を0.0 - 1.0に正規化したもの */



#endif  /* __AUTOGL_VISLOW__ */









