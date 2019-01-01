/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_UTILITY__
#define __AUTOGL_UTILITY__

/* ユーティリティ */



/* コマンドライン処理とスクリプティング */

/* コマンドライン処理とスクリプティング機能をパネルに準備する。 */
void AutoGL_SetPanelForInterpreter (void);



/* イメージのセーブとロード */

/* PPMファイルにビューのイメージを保存する。*/
void AutoGL_SaveViewImageToPPMFile (const char *fileName);

/* PPMファイルからイメージを読み出す。*/
void AutoGL_ReadImageFromPPMFile 
(const char *fileName,
 int *widthDcPtr_OUT, int *heightDcPtr_OUT, unsigned char **imagePtr_OUT);

/* 画面セーブ機能をパネルに準備する。*/
void AutoGL_SetPanelForSave (void);



/* その他のユーティリティ */

/* ウィンドウ内の座標（左下隅(0.0, 0.0)、右上隅(1.0, 1.0)）で文字列を描画 */
void AutoGL_DrawStringAtScaledPosition
(double rx, double ry,
 const char *str);

/* 秒単位で指定された時間(time)を日・時間・分・秒単位に変換して表示 */
void AutoGL_DrawTimeDHMS (double time);



#endif  /* __AUTOGL_UTILITY__ */



