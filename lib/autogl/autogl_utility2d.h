/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_UTILITY2D__
#define __AUTOGL_UTILITY2D__

/* $BFs<!85%f!<%F%#%j%F%#(B */



/* $BFs<!85%S%e!<%$%s%0%Q%i%a!<%?A`:n(B */

/* $B%S%e!<%$%s%0%Q%i%a!<%?@_Dj5!G=$r%Q%M%k$K=`Hw$9$k!#(B */
void AutoGL_SetPanelInView2D (void);



/* $BFs<!85IA2h%W%j%_%F%#%V(B */

/* $B:BI87O$K%"%i%$%s$5$l$?H"$rI=<($9$k!#(B */
void AutoGL_DrawBox2D 
(double p0X, double p0Y,
 double p1X, double p1Y, 
 double z);

/* $B==;z%^!<%/$rI=<($9$k!#(B */
void AutoGL_DrawCrossMark2D 
(double pX, double pY, double pZ,
 int sizeDc);

/* $B%@%$%"%b%s%I%^!<%/$rI=<($9$k!#(B */
void AutoGL_DrawDiamondMark2D 
(double pX, double pY, double pZ,
 int sizeDc, int isFilled);

/* $B@5J}7A%^!<%/$rI=<($9$k!#(B */
void AutoGL_DrawSquareMark2D 
(double pX, double pY, double pZ,
 int sizeDc, int isFilled);

/* $BLp0u$rI=<($9$k!#(B */
void AutoGL_DrawArrow2D 
(double x0, double y0,
 double x1, double y1, 
 double z,
 double tipRatio);
/* tipRatio : $BA4BND9$5$KBP$9$kLp0u@hC<%5%$%:$NHf(B */



/* $BFs<!85%$%Y%s%H=hM}(B */

/*  AUTOGL_MODE_2D_  */
/* $B%f!<%6!<Dj5A%$%Y%s%H=hM}%b!<%I(B */
#define AUTOGL_MODE_2D_OTHER 0
/* $B0\F0%b!<%I(B */
#define AUTOGL_MODE_2D_TRANSLATE 1
/* $B3HBg=L>.%b!<%I(B */
#define AUTOGL_MODE_2D_SCALE 2

/* $B0\F03HBg=L>.5!G=$r%Q%M%k$K=`Hw$9$k!#(B */
void AutoGL_SetPanelInMode2D (void);

/* $B%$%Y%s%H=hM}%b!<%I$r@_Dj$9$k!#(B */
void AutoGL_SetMode2D (int mode);   /*  AUTOGL_MODE_2D_  */

/* $B%$%Y%s%H=hM}%b!<%I$rF@$k!#(B */
int AutoGL_GetMode2D (void);        /*  AUTOGL_MODE_2D_  */

/* $B%I%i%C%0Cf$KIA2h$r9T$&!#(B */
void AutoGL_EnableDragInMode2D (void);

/* $B%f!<%6!<Dj5A%$%Y%s%H=hM}%3!<%k%P%C%/$r@_Dj$9$k!#(B */
void AutoGL_SetDefaultCallbackInMode2D 
(AutoGL_CallbackType otherCallback);
/* otherCallback : $B%f!<%6!<Dj5A%$%Y%s%H=hM}%3!<%k%P%C%/(B */
/* $B$3$l$O(BNULL$B$G$b$h$$!#(B */

/* $B%G%U%)%k%H$N%-!<%$%Y%s%H=hM}(B */
/* $B%f!<%6!<Dj5A%$%Y%s%H=hM}%3!<%k%P%C%/$+$i8F$V!#(B */
void AutoGL_HandleDefaultKeyEventInMode2D (void);

/* $B%]%$%s%F%#%s%0%G%P%$%9$,%R%C%H$7$?$+$rD4$Y$k!#(B */
int AutoGL_PointingDeviceIsHit2D 
(double pX, double pY,
 int toleranceDc);
/* (pX, pY) : $B@$3&:BI8(B, $B$?$@$7Fs<!85(B */
/* toleranceDc : $B%G%P%$%9:BI87O$G$NI}(B */



#endif  /* __AUTOGL_UTILITY2D__ */



