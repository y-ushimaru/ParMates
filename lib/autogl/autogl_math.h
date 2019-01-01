/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_MATH__
#define __AUTOGL_MATH__

/* 幾何演算 */



/* トレランス管理 */

/* トレランス値を設定する。*/
void AutoGL_SetTolerance (double tolerance);

/* トレランス値を得る。*/
double AutoGL_GetTolerance (void);



/* 二次元ベクトル演算 */

/* ベクトルをベクトルに代入する。*/
void AutoGL_AssignVector2D 
(double *x_OUT, double *y_OUT,
 double x, double y);

/* ２つのベクトルを加算する。*/
void AutoGL_AddVector2D 
(double *x_OUT, double *y_OUT,
 double x0, double y0,
 double x1, double y1);

/* ２つのベクトルを減算する。*/
void AutoGL_SubtractVector2D 
(double *x_OUT, double *y_OUT,
 double x0, double y0,
 double x1, double y1);

/* ベクトルとスカラーを乗算する。*/
void AutoGL_MultiplyVector2D 
(double *x_OUT, double *y_OUT,
 double x, double y,
 double scale);

/* ベクトルの長さを得る。*/
double AutoGL_GetVectorLength2D 
(double x, double y);

/* ベクトルを正規化する。*/
void AutoGL_NormalizeVector2D 
(double *x_OUT, double *y_OUT,
 double x, double y);

/* ２つのベクトルのスカラー積を得る。*/
double AutoGL_GetScalarProduct2D 
(double x0, double y0,
 double x1, double y1);

/* ２つのベクトルの距離を得る。*/
double AutoGL_GetVectorDistance2D 
(double x0, double y0,
 double x1, double y1);

/* ２つのベクトルの成分ごとの最小値を得る。*/
void AutoGL_GetMinVector2D 
(double *x_OUT, double *y_OUT,
 double x0, double y0,
 double x1, double y1);

/* ２つのベクトルの成分ごとの最大値を得る。*/
void AutoGL_GetMaxVector2D
(double *x_OUT, double *y_OUT,
 double x0, double y0,
 double x1, double y1);

/* ベクトルに垂直なベクトルを得る。*/
void AutoGL_GetAnyPerpendicularDirection2D 
(double *nx_OUT, double *ny_OUT,
 double x, double y);



/* 三次元ベクトル演算 */

/* ベクトルをベクトルに代入する。*/
void AutoGL_AssignVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x, double y, double z);

/* ２つのベクトルを加算する。*/
void AutoGL_AddVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x0, double y0, double z0,
 double x1, double y1, double z1);

/* ２つのベクトルを減算する。*/
void AutoGL_SubtractVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x0, double y0, double z0,
 double x1, double y1, double z1);

/* ベクトルとスカラーを乗算する。*/
void AutoGL_MultiplyVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x, double y, double z,
 double scale);

/* ベクトルの長さを得る。*/
double AutoGL_GetVectorLength3D 
(double x, double y, double z);

/* ベクトルを正規化する。*/
void AutoGL_NormalizeVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x, double y, double z);

/* ２つのベクトルのスカラー積を得る。*/
double AutoGL_GetScalarProduct3D 
(double x0, double y0, double z0,
 double x1, double y1, double z1);

/* ２つのベクトルのベクトル積を得る。*/
void AutoGL_GetVectorProduct3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x0, double y0, double z0,
 double x1, double y1, double z1);

/* ２つのベクトルの距離を得る。*/
double AutoGL_GetVectorDistance3D 
(double x0, double y0, double z0,
 double x1, double y1, double z1);

/* ２つのベクトルの成分ごとの最小値を得る。*/
void AutoGL_GetMinVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x0, double y0, double z0,
 double x1, double y1, double z1);

/* ２つのベクトルの成分ごとの最大値を得る。*/
void AutoGL_GetMaxVector3D
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x0, double y0, double z0,
 double x1, double y1, double z1);

/* ベクトルに垂直な２つのベクトルを得る。*/
void AutoGL_GetAnyPerpendicularDirection3D 
(double *nx0_OUT, double *ny0_OUT, double *nz0_OUT,
 double *nx1_OUT, double *ny1_OUT, double *nz1_OUT,
 double x, double y, double z);



#endif  /* __AUTOGL_MATH__ */















