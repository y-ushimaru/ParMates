/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_GUI__
#define __AUTOGL_GUI__

/* グラフィカルユーザーインターフェイス処理 */



/* ビューのサイズを変更する。 */
void AutoGL_ResizeView (int widthDc, int heightDc);

/* ビューを再描画する。 */
void AutoGL_DrawView (void);
/* ビューの再描画関数が呼び出される。 */

/* AutoGLの終了処理をする。 */
void AutoGL_Terminate (void);
/* 自分で用意したQuitボタンなどから, exit()の直前に呼び出す。 */

void AutoGL_MainLoop (void);



#endif  /* __AUTOGL_GUI__ */



