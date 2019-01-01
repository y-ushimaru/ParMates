/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_CUI__
#define __AUTOGL_CUI__

/* $B%3%^%s%I%i%$%s$H%9%/%j%W%F%#%s%0=hM}(B */



/* $B%U%!%$%k$+$i%3%^%s%I$r2r<a!&=hM}$9$k!#(B */
void AutoGL_InterpretInBatchMode (FILE *in, FILE *out);
/* in : $B%3%^%s%I$NF~NO@h(B */
/* out : $B%3%^%s%I=hM}7k2L$N=PNO@h(B */

/* $BI8=`F~NO$+$i%3%^%s%I$r2r<a!&=hM}$9$k!#(B */
void AutoGL_InterpretInInteractiveMode (void);
/* $B%3%^%s%I$N=hM}7k2L$OI8=`=PNO$KI=<($5$l$k!#(B */

/* $B%9%/%j%W%H%U%!%$%k$r2r<a!&=hM}$9$k!#(B */
void AutoGL_InterpretScript (char *fileName);
/* fileName : $B%9%/%j%W%H%U%!%$%kL>(B */
/* $B%3%^%s%I$N=hM}7k2L$OI8=`=PNO$KI=<($5$l$k!#(B */



#endif  /* __AUTOGL_CUI__ */



