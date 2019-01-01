/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_OS__
#define __AUTOGL_OS__

/* OS$B0MB8=hM}(B */



/* Microsoft Visual C++$B$r;H$$$?$$>l9g$K$O!"$3$3$r(B0$B$+$i(B1$B$KJQ99$9$k!#(B*/
#if 0
#define VISUAL_CPP
#endif

#ifdef VISUAL_CPP
#include <windows.h>
#define M_PI 3.14159265358979323846
#endif



/* for FORTRAN binding */

/* Fortran$B$r;H$$$?$$>l9g$K$O!"$3$3$r(B0$B$+$i(B1$B$KJQ99$9$k!#(B*/
#if 0
#define AUTOGL_FORTRAN
void AutoGL_Main (void);
#endif

/* $B0J2<$N$&$A$I$l$+0l$D$,M-8z!#(B */
/* $B%G%U%)%k%H@_Dj$O(BGNU Fortran$B$H$J$C$F$$$k!#(B */
#if 1  /* for GNU Fortran (g77, g95) */
#define AUTOGL_FORTRAN_BINDING_DOUBLE_UNDERSCORE
#endif
#if 0  /* for Intel Fortran Compiler */
#define AUTOGL_FORTRAN_BINDING_UNDERSCORE
#endif



#endif  /* __AUTOGL_OS__ */



