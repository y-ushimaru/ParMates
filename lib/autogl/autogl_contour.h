/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_CONTOUR__
#define __AUTOGL_CONTOUR__

/* $B2D;k2=%f!<%F%#%j%F%#(B($B%3%s%?(B-) */



/* $B%3%s%?(B-$B%^%C%W$N%+%i!<%F!<%V%k$r%/%j%"$9$k!#(B */
void AutoGL_ClearContourColor (void);

/* $B%3%s%?(B-$B%^%C%W$N%+%i!<%F!<%V%k$K%0%l!<%I$4$H$N?'$r@_Dj$9$k!#(B */
void AutoGL_AddContourColorOfGrade 
(double grade, 
 double red, double green, double blue);

/* $B%0%l%$%9%1!<%k$N%3%s%?(B-$B%^%C%W$r@_Dj$9$k!#(B */
void AutoGL_SetGrayScaleContourMap (void);

/* $B@D!"?e?'!"NP!"2+?'!"@V!";g$N=g$GFz?'$N%3%s%?(B-$B%^%C%W$r@_Dj$9$k!#(B */
void AutoGL_SetContourMap_BCGYRM (void);

/* $B;g!"@D!"?e?'!"NP!"2+?'!"@V$N=g$GFz?'$N%3%s%?(B-$B%^%C%W$r@_Dj$9$k!#(B */
void AutoGL_SetContourMap_MBCGYR (void);

/* $B@D!"?e?'!"NP!"2+?'!"@V$N=g$GFz?'$N%3%s%?(B-$B%^%C%W$r@_Dj$9$k!#(B */
void AutoGL_SetContourMap_BCGYR (void);

/* $B%3%s%?(B-$B$NCM$KBP1~$9$k?'$rF@$k!#(B */
void AutoGL_GetContourColor 
(double *red_OUT, double *green_OUT, double *blue_OUT,
 double grade);
/* red_OUT, green_OUT, blue_OUT : $B@V!"NP!"@D$K$D$$$F!"(B
                                  $B$=$l$>$l(B0.0 - 1.0$B$N%l%s%8$N6/EY!#(B */
/* grade : 0.0 - 1.0$B$K5,3J2=$5$l$?CM!#(B */

/* $B;03Q7A$r%3%s%?(B-$B$GEI$jDY$9!#(B */
void AutoGL_DrawContourTriangle 
(double x0, double y0, double z0, double grade0,  
 double x1, double y1, double z1, double grade1,  
 double x2, double y2, double z2, double grade2); 
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) : $B;0E@$N@$3&:BI8(B */
/* grade0, grade1, grade2 : $B;0E@$N%9%+%i!<CM!J(B0.0 - 1.0$B$K5,3J2=!K(B */

/* $B2hLL1&$KLL%3%s%?(B-$B$G$N?'$HCM$H$NBP1~I=$r%P!<I=<($9$k!#(B */
void AutoGL_DrawContourMap 
(int sizeDc,
 double minRange, double maxRange);
/* sizeDc : $B2hLL$K@j$a$k%G%P%$%9:BI87O$G$NI}(B */
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */

/* $B2hLL1&$K@~%3%s%?(B-$B$G$N?'$HCM$H$NBP1~I=$r%P!<I=<($9$k!#(B */
void AutoGL_DrawLineContourMap 
(int sizeDc,
 double minRange, double maxRange,
 int nGrades);
/* sizeDc : $B2hLL$K@j$a$k%G%P%$%9:BI87O$G$NI}(B */
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */
/* nGrades : $B@~%3%s%?(B-$B$NK\?t(B (-1) */

/* $B2hLL1&$K%P%s%I%3%s%?(B-$B$G$N?'$HCM$H$NBP1~I=$r%P!<I=<($9$k!#(B */
void AutoGL_DrawBandContourMap 
(int sizeDc,
 double minRange, double maxRange,
 int nGrades);
/* sizeDc : $B2hLL$K@j$a$k%G%P%$%9:BI87O$G$NI}(B */
/* minRange, maxRange : $B%9%+%i!<CM$N:GBg:G>.(B */
/* nGrades : $B@~%3%s%?(B-$B$NK\?t(B (-1) */



#endif  /* __AUTOGL_CONTOUR__ */











