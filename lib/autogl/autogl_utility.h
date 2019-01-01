/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_UTILITY__
#define __AUTOGL_UTILITY__

/* $B%f!<%F%#%j%F%#(B */



/* $B%3%^%s%I%i%$%s=hM}$H%9%/%j%W%F%#%s%0(B */

/* $B%3%^%s%I%i%$%s=hM}$H%9%/%j%W%F%#%s%05!G=$r%Q%M%k$K=`Hw$9$k!#(B */
void AutoGL_SetPanelForInterpreter (void);



/* $B%$%a!<%8$N%;!<%V$H%m!<%I(B */

/* PPM$B%U%!%$%k$K%S%e!<$N%$%a!<%8$rJ]B8$9$k!#(B*/
void AutoGL_SaveViewImageToPPMFile (const char *fileName);

/* PPM$B%U%!%$%k$+$i%$%a!<%8$rFI$_=P$9!#(B*/
void AutoGL_ReadImageFromPPMFile 
(const char *fileName,
 int *widthDcPtr_OUT, int *heightDcPtr_OUT, unsigned char **imagePtr_OUT);

/* $B2hLL%;!<%V5!G=$r%Q%M%k$K=`Hw$9$k!#(B*/
void AutoGL_SetPanelForSave (void);



/* $B$=$NB>$N%f!<%F%#%j%F%#(B */

/* $B%&%#%s%I%&Fb$N:BI8!J:82<6y(B(0.0, 0.0)$B!"1&>e6y(B(1.0, 1.0)$B!K$GJ8;zNs$rIA2h(B */
void AutoGL_DrawStringAtScaledPosition
(double rx, double ry,
 const char *str);

/* $BICC10L$G;XDj$5$l$?;~4V(B(time)$B$rF|!&;~4V!&J,!&ICC10L$KJQ49$7$FI=<((B */
void AutoGL_DrawTimeDHMS (double time);



#endif  /* __AUTOGL_UTILITY__ */



