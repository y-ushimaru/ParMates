/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_OS__
#define __AUTOGL_OS__

/* OS依存処理 */



/* Microsoft Visual C++を使いたい場合には、ここを0から1に変更する。*/
#if 0
#define VISUAL_CPP
#endif

#ifdef VISUAL_CPP
#include <windows.h>
#define M_PI 3.14159265358979323846
#endif



/* for FORTRAN binding */

/* Fortranを使いたい場合には、ここを0から1に変更する。*/
#if 0
#define AUTOGL_FORTRAN
void AutoGL_Main (void);
#endif

/* 以下のうちどれか一つが有効。 */
/* デフォルト設定はGNU Fortranとなっている。 */
#if 1  /* for GNU Fortran (g77, g95) */
#define AUTOGL_FORTRAN_BINDING_DOUBLE_UNDERSCORE
#endif
#if 0  /* for Intel Fortran Compiler */
#define AUTOGL_FORTRAN_BINDING_UNDERSCORE
#endif



#endif  /* __AUTOGL_OS__ */



