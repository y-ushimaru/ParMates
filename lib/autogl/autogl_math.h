/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_MATH__
#define __AUTOGL_MATH__

/* $B4v2?1i;;(B */



/* $B%H%l%i%s%94IM}(B */

/* $B%H%l%i%s%9CM$r@_Dj$9$k!#(B*/
void AutoGL_SetTolerance (double tolerance);

/* $B%H%l%i%s%9CM$rF@$k!#(B*/
double AutoGL_GetTolerance (void);



/* $BFs<!85%Y%/%H%k1i;;(B */

/* $B%Y%/%H%k$r%Y%/%H%k$KBeF~$9$k!#(B*/
void AutoGL_AssignVector2D 
(double *x_OUT, double *y_OUT,
 double x, double y);

/* $B#2$D$N%Y%/%H%k$r2C;;$9$k!#(B*/
void AutoGL_AddVector2D 
(double *x_OUT, double *y_OUT,
 double x0, double y0,
 double x1, double y1);

/* $B#2$D$N%Y%/%H%k$r8:;;$9$k!#(B*/
void AutoGL_SubtractVector2D 
(double *x_OUT, double *y_OUT,
 double x0, double y0,
 double x1, double y1);

/* $B%Y%/%H%k$H%9%+%i!<$r>h;;$9$k!#(B*/
void AutoGL_MultiplyVector2D 
(double *x_OUT, double *y_OUT,
 double x, double y,
 double scale);

/* $B%Y%/%H%k$ND9$5$rF@$k!#(B*/
double AutoGL_GetVectorLength2D 
(double x, double y);

/* $B%Y%/%H%k$r@55,2=$9$k!#(B*/
void AutoGL_NormalizeVector2D 
(double *x_OUT, double *y_OUT,
 double x, double y);

/* $B#2$D$N%Y%/%H%k$N%9%+%i!<@Q$rF@$k!#(B*/
double AutoGL_GetScalarProduct2D 
(double x0, double y0,
 double x1, double y1);

/* $B#2$D$N%Y%/%H%k$N5wN%$rF@$k!#(B*/
double AutoGL_GetVectorDistance2D 
(double x0, double y0,
 double x1, double y1);

/* $B#2$D$N%Y%/%H%k$N@.J,$4$H$N:G>.CM$rF@$k!#(B*/
void AutoGL_GetMinVector2D 
(double *x_OUT, double *y_OUT,
 double x0, double y0,
 double x1, double y1);

/* $B#2$D$N%Y%/%H%k$N@.J,$4$H$N:GBgCM$rF@$k!#(B*/
void AutoGL_GetMaxVector2D
(double *x_OUT, double *y_OUT,
 double x0, double y0,
 double x1, double y1);

/* $B%Y%/%H%k$K?bD>$J%Y%/%H%k$rF@$k!#(B*/
void AutoGL_GetAnyPerpendicularDirection2D 
(double *nx_OUT, double *ny_OUT,
 double x, double y);



/* $B;0<!85%Y%/%H%k1i;;(B */

/* $B%Y%/%H%k$r%Y%/%H%k$KBeF~$9$k!#(B*/
void AutoGL_AssignVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x, double y, double z);

/* $B#2$D$N%Y%/%H%k$r2C;;$9$k!#(B*/
void AutoGL_AddVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x0, double y0, double z0,
 double x1, double y1, double z1);

/* $B#2$D$N%Y%/%H%k$r8:;;$9$k!#(B*/
void AutoGL_SubtractVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x0, double y0, double z0,
 double x1, double y1, double z1);

/* $B%Y%/%H%k$H%9%+%i!<$r>h;;$9$k!#(B*/
void AutoGL_MultiplyVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x, double y, double z,
 double scale);

/* $B%Y%/%H%k$ND9$5$rF@$k!#(B*/
double AutoGL_GetVectorLength3D 
(double x, double y, double z);

/* $B%Y%/%H%k$r@55,2=$9$k!#(B*/
void AutoGL_NormalizeVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x, double y, double z);

/* $B#2$D$N%Y%/%H%k$N%9%+%i!<@Q$rF@$k!#(B*/
double AutoGL_GetScalarProduct3D 
(double x0, double y0, double z0,
 double x1, double y1, double z1);

/* $B#2$D$N%Y%/%H%k$N%Y%/%H%k@Q$rF@$k!#(B*/
void AutoGL_GetVectorProduct3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x0, double y0, double z0,
 double x1, double y1, double z1);

/* $B#2$D$N%Y%/%H%k$N5wN%$rF@$k!#(B*/
double AutoGL_GetVectorDistance3D 
(double x0, double y0, double z0,
 double x1, double y1, double z1);

/* $B#2$D$N%Y%/%H%k$N@.J,$4$H$N:G>.CM$rF@$k!#(B*/
void AutoGL_GetMinVector3D 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x0, double y0, double z0,
 double x1, double y1, double z1);

/* $B#2$D$N%Y%/%H%k$N@.J,$4$H$N:GBgCM$rF@$k!#(B*/
void AutoGL_GetMaxVector3D
(double *x_OUT, double *y_OUT, double *z_OUT,
 double x0, double y0, double z0,
 double x1, double y1, double z1);

/* $B%Y%/%H%k$K?bD>$J#2$D$N%Y%/%H%k$rF@$k!#(B*/
void AutoGL_GetAnyPerpendicularDirection3D 
(double *nx0_OUT, double *ny0_OUT, double *nz0_OUT,
 double *nx1_OUT, double *ny1_OUT, double *nz1_OUT,
 double x, double y, double z);



#endif  /* __AUTOGL_MATH__ */















