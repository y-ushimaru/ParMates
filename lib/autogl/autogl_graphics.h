/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_GRAPHICS__
#define __AUTOGL_GRAPHICS__

/* $B%0%i%U%#%C%/%9(B */



/* $B%l%s%@%j%s%0(B */

/* $B%S%e!<$NGX7J?'$r@_Dj$9$k!#(B */
void AutoGL_SetBackgroundColor 
(double red, double green, double blue);  
/* red, green, blue : $B@V!"NP!"@D$K$D$$$F!"$=$l$>$l(B0.0 - 1.0$B$N%l%s%8$N6/EY!#(B */

/* $B6@LLH?<M$rM-8z$K$9$k!#(B */
void AutoGL_TurnOnSpecular (void);

/* $BH>F)L@=hM}$rM-8z$K$9$k!#(B */
void AutoGL_TurnOnTransparency (void);

/* $B8=:_$N?'$r@_Dj$9$k!#(B */
void AutoGL_SetColor 
(double red, double green, double blue);  
/* red, green, blue : $B@V!"NP!"@D$K$D$$$F!"$=$l$>$l(B0.0 - 1.0$B$N%l%s%8$N6/EY!#(B */

/* $B8=:_$NITF)L@EY$r@_Dj$9$k!#(B */
void AutoGL_SetOpacity (double opacity);  
/* opacity : $BITF)L@EY$G!"(B0.0 - 1.0$B$N%l%s%8$N6/EY!#(B */

/* $B@~J,$r8=:_$N?'$GIA2h$9$k!#(B*/
void AutoGL_DrawLine 
(double x0, double y0, double z0,    
 double x1, double y1, double z1); 
/* (x0, y0, z0), (x1, y1, z1) : $BFsE@$N@$3&:BI8(B */

/* $B;03Q7A$r8=:_$N?'$GEI$jDY$9!#(B */
void AutoGL_DrawTriangle 
(double x0, double y0, double z0,  
 double x1, double y1, double z1,  
 double x2, double y2, double z2); 
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) : $B;0E@$N@$3&:BI8(B */

/* $B;03Q7A$r?'Jd4V$K$h$jEI$jDY$9!#(B */
void AutoGL_DrawColorInterpolatedTriangle 
(double x0, double y0, double z0, double red0, double green0, double blue0,  
 double x1, double y1, double z1, double red1, double green1, double blue1,  
 double x2, double y2, double z2, double red2, double green2, double blue2); 
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) : $B;0E@$N@$3&:BI8(B */
/* (red0, green0, blue0), (red1, green1, blue1), (red2, green2, blue2) : $B;0E@$N?'(B */

/* $B;03Q7A$rK!@~%Y%/%H%kJd4V$K$h$jEI$jDY$9!#(B */
void AutoGL_DrawNormalInterpolatedTriangle 
(double x0, double y0, double z0, double nx0, double ny0, double nz0,
 double x1, double y1, double z1, double nx1, double ny1, double nz1,
 double x2, double y2, double z2, double nx2, double ny2, double nz2);
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) : $B;0E@$N@$3&:BI8(B */
/* (nx0, ny0, nz0), (nx1, ny1, nz1), (nx2, ny2, nz2) : $B;0E@$NK!@~%Y%/%H%k(B */

/* $B;MJU7A$r8=:_$N?'$GEI$jDY$9!#(B */
void AutoGL_DrawQuadrangle 
(double x0, double y0, double z0,
 double x1, double y1, double z1,
 double x2, double y2, double z2,
 double x3, double y3, double z3);
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2), (x3, y3, z3) : $B;ME@$N@$3&:BI8(B */
/* $B;ME@$OF10lJ?LL>e$K$J$/$F$b$h$$!#(B */

/* $BJ8;zNs$r8=:_$N?'$GIA2h$9$k!#(B */
void AutoGL_DrawString 
(double x, double y, double z,
 const char *str);
/* (x, y, z) : $BJ8;zNs$N86E@$N@$3&:BI8(B */
/* str : $BIA2h$5$l$kJ8;zNs(B */

/* $B%G%#%9%W%l%$%j%9%H$r3+$-!"$3$l$r6u$K$9$k!#(B*/
void AutoGL_OpenDisplayList (void);

/* $B%G%#%9%W%l%$%j%9%H$rJD$8$k!#(B */
void AutoGL_CloseDisplayList (void);

/* $B%G%#%9%W%l%$%j%9%H$rIA2h$9$k!#(B */
void AutoGL_DrawDisplayList (void);

/* $B%G%#%9%W%l%$%j%9%H$r%U%!%$%k$+$iFI$`!#(B */
void AutoGL_ReadDisplayList (FILE *fp);

/* $B%G%#%9%W%l%$%j%9%H$r%U%!%$%k$K=q$/!#(B */
void AutoGL_WriteDisplayList (FILE *fp);

/* AutoGL$B%a%?%U%!%$%k$rFI$_=P$9!#(B*/
void AutoGL_ReadMetafile 
(const char *fileName, int updateViewingParameterFlag);

/* AutoGL$B%a%?%U%!%$%k$K=q$-9~$`!#(B*/
void AutoGL_WriteMetafile 
(const char *fileName);

/* AutoGL$B%$%a!<%8%U%!%$%kL>$r@_Dj$9$k!#(B*/
void AutoGL_SetImageFileName 
(const char *fileName);

/* AutoGL$B%$%a!<%8%U%!%$%kL>$r<hF@$9$k!#(B*/
void AutoGL_GetImageFileName 
(char *fileName_OUT);

/* $B>e5-$N3F<oIA2hL?Na$N9bB.HG!#(B */
/* $B%G%#%9%W%l%$%j%9%H$K$OEPO?$5$l$J$$!#(Bimmediate mode$BMQ!#(B*/
/* $BK!@~J}8~$r$"$i$+$8$a7W;;$7$F$*$/$3$H!#(B*/
/* $B0z?t$,$9$Y$F(Bfloat$B7?$G$"$k$3$H$KCm0U!#(B*/
void AutoGL_SetColor2 
(float red, float green, float blue);

void AutoGL_DrawLine2 
(float x0, float y0, float z0,
 float x1, float y1, float z1);

void AutoGL_DrawTriangle2 
(float x0, float y0, float z0,
 float x1, float y1, float z1,
 float x2, float y2, float z2,
 float nx, float ny, float nz);

void AutoGL_DrawColorInterpolatedTriangle2
(float x0, float y0, float z0, float red0, float green0, float blue0,  
 float x1, float y1, float z1, float red1, float green1, float blue1,  
 float x2, float y2, float z2, float red2, float green2, float blue2,
 float nx, float ny, float nz);

void AutoGL_DrawNormalInterpolatedTriangle2 
(float x0, float y0, float z0, float nx0, float ny0, float nz0,
 float x1, float y1, float z1, float nx1, float ny1, float nz1,
 float x2, float y2, float z2, float nx2, float ny2, float nz2);

void AutoGL_DrawString2
(float x, float y, float z,
 const char *str);

void AutoGL_SetSelectionId2 (int id);

/* OpenGL$B$J$I$N%M%$%F%#%V$J%0%i%U%#%C%/%9(BAPI$B$rD>@\MQ$$$k:]$K!"(B*/
/* $B%M%$%F%#%V(BAPI$B%3!<%k$ND>A0D>8e$K8F$S=P$9!#(B*/
void AutoGL_BeginNativeCall (void);
void AutoGL_EndNativeCall (void);




/* $B%S%e!<%$%s%0%Q%i%a!<%?$H:BI8JQ49(B */

/* $B@$3&:BI87O(B : $B%b%G%k$,CV$+$l$F$$$k:BI87O!#(B */
/*   $B;0<!85$N<B?t:BI87O!#(B*/

/* $B;kE@:BI87O(B : $B@$3&:BI87O$,;kLnJQ49$5$l$?$b$N!#(B */
/*   $B;0<!85$N<B?t:BI87O!#(B*/
/*   $B2hLL$NCf?4$,(B(0, 0, 0)$B$KBP1~$9$k!#(B*/
/*   $B1&J}8~$,(BX$BJ}8~(B, $B>e$,(BY$BJ}8~(B, $B1|9T$-$N<jA0B&$K(BZ$BJ}8~!#(B*/

/* $B%G%P%$%9:BI87O(B : $B;kE@:BI87O$,<B:]$N2hLL%5%$%:$K%9%1!<%k$5$l$?$b$N!#;0<!85!#(B*/
/*   $B;0<!85$N@0?t:BI87O!#(B*/
/*   $B2hLL$NCf?4$,(B(0, 0, 0)$B$KBP1~$9$k!#(B*/
/*   $B1&J}8~$,(BX$BJ}8~(B, $B>e$,(BY$BJ}8~(B, $B1|9T$-$N<jA0B&$K(BZ$BJ}8~!#(B*/
/*   X Window System$B$d(BWindows$B$N%G%P%$%9:BI87O$H$O0[$J$k$3$H$KCm0U!*!*!*(B */

/* $B%S%e!<$N%5%$%:$r@_Dj$9$k!#(B*/
void AutoGL_SetViewSize (double size);
/* size : $B<B:]$N2hLL$NH>J,$NBg$-$5$KBP1~$9$k(B,$B@$3&:BI8$K$*$1$k%S%e!<$N%5%$%:!#(B*/

/* $B%S%e!<$NCm;kE@$r@_Dj$9$k!#(B*/
void AutoGL_SetViewCenter (double x, double y, double z);
/* (x, y, z) : $BCm;kE@$N:BI8(B */

/* $B%S%e!<$N;k@~J}8~$r@_Dj$9$k!#(B */
void AutoGL_SetViewDirection (double x, double y, double z);
/* (x, y, z) : $B;k@~J}8~%Y%/%H%k(B */

/* $B%S%e!<$N;k@~>e8~$-J}8~$r@_Dj$9$k!#(B */
void AutoGL_SetViewUpVector (double x, double y, double z);
/* (x, y, z) : $B;k@~>e8~$-J}8~%Y%/%H%k(B */

/* $BEj1FK!$rF);kEj1F$K$9$k$+!"$7$J$$(B($BJ?9TEj1F(B)$B$+$r@_Dj$9$k!#(B*/
void AutoGL_SetPerspectiveViewFlag (int flag);

/* $B%]%j%4%s$rN"I=$H$bIA2h$9$k$+$I$&$+$r@_Dj$9$k!#(B*/
void AutoGL_SetTwoSideFaceFlag (int flag);

/* $B%S%e!<$N%5%$%:$rF@$k!#(B */
double AutoGL_GetViewSize (void);

/* $B%S%e!<$NCm;kE@$rF@$k!#(B */
void AutoGL_GetViewCenter 
(double *x_OUT, double *y_OUT, double *z_OUT);
/* (x_OUT, y_OUT, z_OUT) : $BCm;kE@$N:BI8(B */

/* $B%S%e!<$N;k@~J}8~$rF@$k!#(B */
void AutoGL_GetViewDirection 
(double *x_OUT, double *y_OUT, double *z_OUT);
/* (x_OUT, y_OUT, z_OUT) : $B;k@~J}8~%Y%/%H%k(B */

/* $B%S%e!<$N;k@~>e8~$-J}8~$rF@$k!#(B */
void AutoGL_GetViewUpVector 
(double *x_OUT, double *y_OUT, double *z_OUT);
/* (x_OUT, y_OUT, z_OUT) : $B;k@~>e8~$-J}8~%Y%/%H%k(B */

/* $B%S%e!<$N%G%P%$%9:BI87O$G$N9b$5$HI}$rF@$k!#(B */
void AutoGL_GetViewRangeDc 
(int *widthDc_OUT, int *heightDc_OUT);
/* widthDc_OUT : $B%S%e!<$NI}(B */
/* heightDc_OUT : $B%S%e!<$N9b$5(B */

/* $B%S%e!<$N%G%P%$%9:BI87O$G$N%5%$%:$rF@$k!#(B */
int AutoGL_GetViewSizeDc (void); 

/* $B%G%P%$%9:BI8$+$i;kE@:BI8$rF@$k!#(B */
void AutoGL_GetVcOfDc 
(double *vcX_OUT, double *vcY_OUT, double *vcZ_OUT,
 int dcX, int dcY, int dcZ);
/* (dcX, dcY, dcZ) : $B%G%P%$%9:BI8(B */
/* (vcX_OUT, vcY_OUT, vcZ_OUT) : $B;kE@:BI8(B */

/* $B;kE@:BI8$+$i@$3&:BI8$rF@$k!#(B */
void AutoGL_GetPositionOfVc 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double vcX, double vcY, double vcZ);
/* (vcX, vcY, vcZ) : $B;kE@:BI8(B */
/* (x_OUT, y_OUT, z_OUT) : $B@$3&:BI8(B */

/* $B%G%P%$%9:BI8$+$i@$3&:BI8$rF@$k!#(B */
void AutoGL_GetPositionOfDc 
(double *x_OUT, double *y_OUT, double *z_OUT,
 int dcX, int dcY, int dcZ);
/* (dcX, dcY, dcZ) : $B%G%P%$%9:BI8(B */
/* (x_OUT, y_OUT, z_OUT) : $B@$3&:BI8(B */

/* $BEj1FK!$,F);kEj1F$+$I$&$+!#(B*/
int AutoGL_GetPerspectiveViewFlag (void);

/* $B%]%j%4%s$rN"I=$H$bIA2h$9$k$+$I$&$+!#(B*/
int AutoGL_GetTwoSideFaceFlag (void);



/* $B%$%a!<%8=hM}(B */

/* $B%S%e!<$N%$%a!<%8$rF@$k!#(B */
void AutoGL_GetViewImage
(unsigned char image_OUT[]);
/* image_OUT : $B%S%e!<$NI}(B x $B9b$5(B x 3 byte (24bit)$B$NBg$-$5$NG[Ns(B */
/* $B3F%T%/%;%k$4$H$K!"(BRGB$B$N3F@.J,$,(B0-255$B$GF@$i$l$k!#(B */
/* unsigned char image_OUT[heightDc][widthDC][3] */




/* $BA*Br(B */

/* $BA*Br=hM}$r3+;O$9$k!#(B*/
void AutoGL_StartSelection (int x, int y, int range);

/* $B8=:_$NA*Br8uJd%*%V%8%'%/%H(BID$B$r@_Dj$9$k!#(B*/
void AutoGL_SetSelectionId (int id);

/* $BA*Br=hM}$r=*N;$9$k!#(B*/
void AutoGL_EndSelection (void);

/* $B<B:]$KA*Br$5$l$?A*Br8uJd%*%V%8%'%/%H(BID$B$rJV$9!#(B*/
int AutoGL_GetSelectedId (void);




/* $B%$%Y%s%H=hM}(B */

/* AUTOGL_EVENT_ $B%$%Y%s%H$N%?%$%W(B */
/* $B%]%$%s%?$N%\%?%s$,2!$5$l$?!#(B */
#define AUTOGL_EVENT_POINTING_DEVICE_PRESS 0
/* $B%]%$%s%?$,%I%i%C%0$5$l$?!#(B */
#define AUTOGL_EVENT_POINTING_DEVICE_DRAG 1
/* $B%]%$%s%?$N%\%?%s$,N%$5$l$?!#(B */
#define AUTOGL_EVENT_POINTING_DEVICE_RELEASE 2
/* $B%-!<%\!<%I$N%-!<$,2!$5$l$?!#(B */
#define AUTOGL_EVENT_KEY 3

/* $B%$%Y%s%H$N%?%$%W$rF@$k!#(B */
int AutoGL_GetViewEvent (void);   /* AUTOGL_EVENT_ */

/* $B%]%$%s%F%#%s%0%G%P%$%9$N0LCV$rF@$k!#(B */
void AutoGL_GetPointingDevicePositionDc 
(int *dcX_OUT, int *dcY_OUT, int *dcZ_OUT);
/* dcX_OUT, dcY_OUT, dcZ_OUT : $B%]%$%s%?$N%G%P%$%9:BI8(B */

/* $B%-!<%\!<%I%-!<$rF@$k!#(B */
int AutoGL_GetKeyChar (void);






/* $B8E$$(BAPI($B<!4|%P!<%8%g%s$G$N%5%]!<%HL$Dj(B) */

/* $B8=:_$N%F%/%9%A%c%^%C%W$r@_Dj$9$k!#(B*/
void AutoGL_SetTextureMap 
(int sizeLevel, const unsigned char image[]);
/* sizeLevel : $B%F%/%9%A%c%$%a!<%8$N%5%$%:$N(Blog 2$B!#(B */
/* image[][][3] : $B%F%/%9%A%c$N(BRGB$B%$%a!<%8!#(B */

/* $B;03Q7A$r8=:_$N%F%/%9%A%c%^%C%W$GEI$jDY$9!#(B*/
void AutoGL_DrawTexturedTriangle 
(double x0, double y0, double z0,
 double x1, double y1, double z1,
 double x2, double y2, double z2,
 double scaleFactor);
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) : $B;0E@$N@$3&:BI8(B */
/* scaleFactor : $B%F%/%9%A%c:BI8$+$i@$3&:BI8$X$N%9%1!<%k(B */

/* $B%$%a!<%8$rIA2h$9$k!#(B */
void AutoGL_DrawImage 
(double x, double y, double z,
 int widthDc, int heightDc, const unsigned char image[]);
/* (x, y, z) : $BJ8;zNs$N86E@$N@$3&:BI8(B */
/* image : $BI}(B widthDc x $B9b$5(B heightDc x 3 byte (24bit)$B$NBg$-$5$NG[Ns(B */
/* $B3F%T%/%;%k$4$H$K!"(BRGB$B$N3F@.J,$,(B0-255$B$GF@$i$l$k!#(B */
/* unsigned char image[heightDc][widthDC][3] */






/* test */

void AutoGL_OpenMacro (int macroId);

void AutoGL_CloseMacro (void);

void AutoGL_DrawMacro (int macroId);

void AutoGL_PushMatrix (void);

void AutoGL_PopMatrix (void);

void AutoGL_Translate (double x, double y, double z);

void AutoGL_Rotate 
(double angle,
 double axisX, double axisY, double axisZ);
/* angle is degree  0-360  */

void AutoGL_Scale (double ratio);



#endif  /* __AUTOGL_GRAPHICS__ */



