/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_GUI__
#define __AUTOGL_GUI__

/* $B%0%i%U%#%+%k%f!<%6!<%$%s%?!<%U%'%$%9=hM}(B */



/* $B%S%e!<$N%5%$%:$rJQ99$9$k!#(B */
void AutoGL_ResizeView (int widthDc, int heightDc);

/* $B%S%e!<$r:FIA2h$9$k!#(B */
void AutoGL_DrawView (void);
/* $B%S%e!<$N:FIA2h4X?t$,8F$S=P$5$l$k!#(B */

/* AutoGL$B$N=*N;=hM}$r$9$k!#(B */
void AutoGL_Terminate (void);
/* $B<+J,$GMQ0U$7$?(BQuit$B%\%?%s$J$I$+$i(B, exit()$B$ND>A0$K8F$S=P$9!#(B */

void AutoGL_MainLoop (void);



#endif  /* __AUTOGL_GUI__ */



