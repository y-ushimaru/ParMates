/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* header file for autogl_com.c */

#ifndef __AUTOGL_COM_INSIDE__
#define __AUTOGL_COM_INSIDE__



void AutoGL_INSIDE_InitializeAutoNOSim (int *argc, char ***argv);

void AutoGL_INSIDE_TerminateAutoNOSim (void);

void AutoGL_INSIDE_Synchronize (void);

int AutoGL_INSIDE_IsSynchronized (void);

void AutoGL_INSIDE_SendRedrawEvent (void);

void AutoGL_INSIDE_SendCallbackEvent (AutoGL_Property *property);

void AutoGL_INSIDE_SendIdleEvent (void);

void AutoGL_INSIDE_SendMouseEvent (void);

void AutoGL_INSIDE_SendKeyEvent (void);

void AutoGL_INSIDE_SendQuitEvent (void);

void AutoGL_INSIDE_SendResizeEvent (int widthDc, int heightDc);

void AutoGL_INSIDE_SendTaskImage 
(int imageWidthDc, int imageHeightDc,
 unsigned char imageColors[], unsigned int imageDepths[]);

void AutoGL_INSIDE_GatherAndComposeImage 
(int imageWidthDc, int imageHeightDc,
 unsigned char imageColors[], unsigned int imageDepths[]);

int AutoGL_INSIDE_GatherSelectedId (int selectedId, unsigned int minZ);



#endif  /* __AUTOGL_COM_INSIDE__ */



