/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_UTILITY3D__
#define __AUTOGL_UTILITY3D__

/* $B;0<!85%f!<%F%#%j%F%#(B */



/* $B;0<!85%S%e!<%$%s%0%Q%i%a!<%?A`:n(B */

/* $B%S%e!<%$%s%0%Q%i%a!<%?@_Dj5!G=$r%Q%M%k$K=`Hw$9$k!#(B */
void AutoGL_SetPanelInView3D (void);

/* $B%S%e!<$rAjBPE*$K2sE>$9$k!#(B */
void AutoGL_RotateView 
(double offsetX, double offsetY, double offsetZ);
/* offsetX, offsetY, offsetZ     around -1.0 ... 1.0 */



/* $B;0<!85IA2h%W%j%_%F%#%V(B */

/* $B:BI87O$K%"%i%$%s$5$l$?H"$rI=<($9$k!#(B */
void AutoGL_DrawBox3D 
(double p0X, double p0Y, double p0Z,
 double p1X, double p1Y, double p1Z);

/* $B==;z%^!<%/$rI=<($9$k!#(B */
void AutoGL_DrawCrossMark3D 
(double pX, double pY, double pZ,
 int sizeDc);

/* $B%@%$%"%b%s%I%^!<%/$rI=<($9$k!#(B */
void AutoGL_DrawDiamondMark3D 
(double pX, double pY, double pZ,
 int sizeDc, int isFilled);
/* isFilled : $BEI$jDY$9!J%=%j%C%II=<(!K$+$I$&$+(B */

/* $BN)J}BN%^!<%/$rI=<($9$k!#(B */
void AutoGL_DrawCubeMark3D 
(double pX, double pY, double pZ,
 int sizeDc, int isFilled);
/* isFilled : $BEI$jDY$9!J%=%j%C%II=<(!K$+$I$&$+(B */

/* $BLp0u$rI=<($9$k!#(B */
void AutoGL_DrawArrow3D 
(double x0, double y0, double z0,
 double x1, double y1, double z1, 
 double tipRatio);
/* tipRatio : $BA4BND9$5$KBP$9$kLp0u@hC<%5%$%:$NHf(B */

/* $BLp0u$rI=<($9$k!#!J;M3Q?m!K(B */
void AutoGL_DrawConeArrow3D 
(double x0, double y0, double z0,
 double x1, double y1, double z1, 
 double tipRatio);
/* tipRatio : $BA4BND9$5$KBP$9$kLp0u@hC<%5%$%:$NHf(B */

/* $B:82<$K:BI87O$rI=<($9$k!#(B */
void AutoGL_DrawCoordinateSystem3D 
(int sizeDc);

/* $B5e$rI=<($9$k!#(B */
void AutoGL_DrawSphere3D 
(double centerX, double centerY, double centerZ,
 double radius, 
 int nDivisions);
/* centerX, centerY, centerZ : $BCf?4(B */
/* radius : $BH>7B(B */
/* nDivisions : $BJ,3d?t(B */

/* $B1_Cl$rI=<($9$k!#(B */
void AutoGL_DrawCylinder3D 
(double bottomX, double bottomY, double bottomZ,
 double topX, double topY, double topZ,
 double radius, 
 int nDivisions);
/* bottomX, bottomY, bottomZ : $B2<DlLL$NCf?4(B */
/* topX, topY, topZ : $B>eDlLL$NCf?4(B */
/* radius : $BH>7B(B */
/* nDivisions : $BJ,3d?t(B */

/* $B1_?m$rI=<($9$k!#(B */
void AutoGL_DrawCone3D 
(double bottomX, double bottomY, double bottomZ,
 double topX, double topY, double topZ,
 double radius, 
 int nDivisions);
/* bottomX, bottomY, bottomZ : $BDlLL$NCf?4(B */
/* topX, topY, topZ : $B1_?m$N@hE@(B */
/* radius : $BH>7B(B */
/* nDivisions : $BJ,3d?t(B */

/* $B1_$rI=<($9$k!#(B */
void AutoGL_DrawCircle3D 
(double centerX, double centerY, double centerZ,
 double normalX, double normalY, double normalZ,
 double radius, 
 int nDivisions);
/* centerX, centerY, centerZ : $BCf?4(B */
/* normalX, normalY, normalZ : $B1_$,B8:_$9$kJ?LL$NK!@~%Y%/%H%k(B */
/* radius : $BH>7B(B */
/* nDivisions : $BJ,3d?t(B */

/* $B:BI87O$K%"%i%$%s$5$l$?H"$r%=%j%C%II=<($9$k!#(B */
void AutoGL_FillBox3D 
(double x0, double y0, double z0,
 double x1, double y1, double z1);



/* $B;0<!85%$%Y%s%H=hM}(B */

/*  AUTOGL_MODE_3D_  */
/* $B%f!<%6!<Dj5A%$%Y%s%H=hM}%b!<%I(B */
#define AUTOGL_MODE_3D_OTHER 0
/* $B0\F0%b!<%I(B */
#define AUTOGL_MODE_3D_TRANSLATE 1
/* $B2sE>%b!<%I!J(BXY$B<4$^$o$j!K(B */
#define AUTOGL_MODE_3D_ROTATE_XY 2
/* $B2sE>%b!<%I!J(BZ$B<4$^$o$j!K(B */
#define AUTOGL_MODE_3D_ROTATE_Z 3
/* $B3HBg=L>.%b!<%I(B */
#define AUTOGL_MODE_3D_SCALE 4

/* $B0\F02sE>3HBg=L>.5!G=$r%Q%M%k$K=`Hw$9$k!#(B */
void AutoGL_SetPanelInMode3D (void);

/* $B%$%Y%s%H=hM}%b!<%I$r@_Dj$9$k!#(B */
void AutoGL_SetMode3D (int mode);    /*  AUTOGL_MODE_3D_  */

/* $B%$%Y%s%H=hM}%b!<%I$rF@$k!#(B */
int AutoGL_GetMode3D (void);         /*  AUTOGL_MODE_3D_  */

/* $B%I%i%C%0Cf$KIA2h$r9T$&!#(B */
void AutoGL_EnableDragInMode3D (void);

/* $B%f!<%6!<Dj5A%$%Y%s%H=hM}%3!<%k%P%C%/$r@_Dj$9$k!#(B */
void AutoGL_SetDefaultCallbackInMode3D 
(AutoGL_CallbackType otherCallback);
/* otherCallback : $B%f!<%6!<Dj5A%$%Y%s%H=hM}%3!<%k%P%C%/(B */
/* $B$3$l$O(BNULL$B$G$b$h$$!#(B */

/* $B%G%U%)%k%H$N%-!<%$%Y%s%H=hM}(B */
/* $B%f!<%6!<Dj5A%$%Y%s%H=hM}%3!<%k%P%C%/$+$i8F$V!#(B */
void AutoGL_HandleDefaultKeyEventInMode3D (void);

/* $B%]%$%s%F%#%s%0%G%P%$%9$,%R%C%H$7$?$+$rD4$Y$k!#(B */
int AutoGL_PointingDeviceIsHit3D 
(double *parameter_OUT,
 double pX, double pY, double pZ,
 int toleranceDc);
/* parameter_OUT : $B?<$5>pJs(B */
/* (pX, pY, pZ) : $B@$3&:BI8(B */
/* toleranceDc : $B%G%P%$%9:BI87O$G$NI}(B */



#endif  /* __AUTOGL_UTILITY3D__ */





