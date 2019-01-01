/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_UIMS__
#define __AUTOGL_UIMS__

/* $B%f!<%6!<%$%s%?!<%U%'%$%94IM}(B */



/* UI$B$r:n$k!#(B*/
/* $B$3$N%k!<%A%s$OI,$:%f!<%6!<%W%m%0%i%`B&$K0l$DB8:_$7$J$1$l$P$J$i$J$$!#(B*/
void AutoGL_SetUp (int argc, char *argv[]);



/* $B%3!<%k%P%C%/4X?t!J0z?t$J$7!"La$jCM$J$7!K$X$N%]%$%s%?7?(B */
typedef void (*AutoGL_CallbackType) (void);

/* $B%0%k!<%W$rEPO?$9$k!#(B $B!J%Q%M%k$KBP1~$9$k!#!K(B */
void AutoGL_AddGroup 
(const char *name);
/* name : $B%0%k!<%WL>(B */

/* $B%3%a%s%H$rEPO?$9$k!#(B $B!J%i%Y%k$KBP1~$9$k!#!K(B */
void AutoGL_AddComment (void);
/* $B%i%Y%kJ8;zNs$O(BAutoGL_SetLabel$B$G@_Dj$9$k!#(B */

/* $B%3!<%k%P%C%/4X?t$rEPO?$9$k!#(B $B!J%\%?%s$KBP1~$9$k!#!K(B */
void AutoGL_AddCallback 
(AutoGL_CallbackType callback, 
 const char *name);
/* callback : $B4X?tL>(B */
/* name : $B%3!<%k%P%C%/L>(B */

/* $BO@M}JQ?t$rEPO?$9$k!#(B $B!J%H%0%k$KBP1~$9$k!#!K(B */
void AutoGL_AddBoolean 
(int *value_IO, 
 const char *name);
/* value_IO : int$B7?JQ?t$X$N%]%$%s%?(B */
/* C++$B$G$O!"(Bbool$B7?JQ?t$+$i%-%c%9%H$7$F$bNI$$!#(B */
/* name : $BO@M}JQ?tL>(B */

/* $BJ8;zNsJQ?t$rEPO?$9$k!#(B $B!J%U%#!<%k%I$KBP1~$9$k!#!K(B */
void AutoGL_AddString 
(char *value_IO, 
 const char *name, int length);
/* value_IO : char$B7?JQ?tG[Ns(B */
/* name : $BJ8;zNsJQ?tL>(B */
/* length : $BG[NsD9(B */

/* $B@0?tJQ?t$rEPO?$9$k!#(B $B!J%U%#!<%k%I$^$?$O%A%g%$%9$KBP1~$9$k!#!K(B */
void AutoGL_AddInteger 
(int *value_IO, 
 const char *name);
/* value_IO : int$B7?JQ?t$X$N%]%$%s%?(B */
/* name : $B@0?tJQ?tL>(B */
/* C++$B$G$O!"(Benum$B7?JQ?t$+$i%-%c%9%H$7$F$bNI$$!#(B */

/* $B@0?tJQ?t$N:GBg:G>.%l%s%8$rEPO?$9$k!#(B */
void AutoGL_SetIntegerRange 
(int minValue, int maxValue);

/* $B@0?tJQ?t$NNs5sCM$r2C$($k!#(B $B!J%A%g%$%9$KBP1~$9$k!#!K(B*/
void AutoGL_AddIntegerItem 
(const char *label);
/* label : $B@0?tJQ?t$NCM$N%i%Y%k(B */

/* $B<B?tJQ?t$rEPO?$9$k!#(B $B!J%U%#!<%k%I$KBP1~$9$k!#!K(B*/
void AutoGL_AddReal 
(double *value_IO, 
 const char *name);
/* value_IO : double$B7?JQ?t$X$N%]%$%s%?(B */
/* name : $B<B?tJQ?tL>(B */

/* $B<B?tJQ?t$N:GBg:G>.%l%s%8$rEPO?$9$k!#(B */
void AutoGL_SetRealRange 
(double minValue, double maxValue);

/* $B8=:_$N%3!<%k%P%C%/4X?t!&@)8fJQ?t$K$D$$$F!"%i%Y%k$r@_Dj$9$k!#(B */
void AutoGL_SetLabel 
(const char *label);
/* label : $B4X?t!&JQ?t$N%i%Y%k(B */

/* $B%S%e!<$N:FIA2hMQ%3!<%k%P%C%/4X?t$rEPO?$9$k!#(B */
void AutoGL_SetViewRedrawCallback 
(AutoGL_CallbackType callback);
/* callback : $B4X?tL>(B */

/* $B%S%e!<$N%$%Y%s%H=hM}%3!<%k%P%C%/4X?t$rEPO?$9$k!#(B */
void AutoGL_SetViewEventCallback 
(AutoGL_CallbackType callback);
/* callback : $B4X?tL>(B */

/* $B%"%$%I%k%$%Y%s%H=hM}$rM-8z$K$9$k!#(B */
void AutoGL_EnableIdleEvent (void);

/* $B%"%$%I%k%$%Y%s%H=hM}%3!<%k%P%C%/4X?t$rEPO?$9$k!#(B */
void AutoGL_SetIdleEventCallback 
(AutoGL_CallbackType callback);
/* callback : $B4X?tL>(B */

/* $B%P%C%A=hM}%3!<%k%P%C%/4X?t$rEPO?$9$k!#(B */
void AutoGL_SetBatchProcessCallback 
(AutoGL_CallbackType callback);
/* callback : $B4X?tL>(B */



#endif  /* __AUTOGL_UIMS__ */




