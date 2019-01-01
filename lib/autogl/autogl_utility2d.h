/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_UTILITY2D__
#define __AUTOGL_UTILITY2D__

/* 二次元ユーティリティ */



/* 二次元ビューイングパラメータ操作 */

/* ビューイングパラメータ設定機能をパネルに準備する。 */
void AutoGL_SetPanelInView2D (void);



/* 二次元描画プリミティブ */

/* 座標系にアラインされた箱を表示する。 */
void AutoGL_DrawBox2D 
(double p0X, double p0Y,
 double p1X, double p1Y, 
 double z);

/* 十字マークを表示する。 */
void AutoGL_DrawCrossMark2D 
(double pX, double pY, double pZ,
 int sizeDc);

/* ダイアモンドマークを表示する。 */
void AutoGL_DrawDiamondMark2D 
(double pX, double pY, double pZ,
 int sizeDc, int isFilled);

/* 正方形マークを表示する。 */
void AutoGL_DrawSquareMark2D 
(double pX, double pY, double pZ,
 int sizeDc, int isFilled);

/* 矢印を表示する。 */
void AutoGL_DrawArrow2D 
(double x0, double y0,
 double x1, double y1, 
 double z,
 double tipRatio);
/* tipRatio : 全体長さに対する矢印先端サイズの比 */



/* 二次元イベント処理 */

/*  AUTOGL_MODE_2D_  */
/* ユーザー定義イベント処理モード */
#define AUTOGL_MODE_2D_OTHER 0
/* 移動モード */
#define AUTOGL_MODE_2D_TRANSLATE 1
/* 拡大縮小モード */
#define AUTOGL_MODE_2D_SCALE 2

/* 移動拡大縮小機能をパネルに準備する。 */
void AutoGL_SetPanelInMode2D (void);

/* イベント処理モードを設定する。 */
void AutoGL_SetMode2D (int mode);   /*  AUTOGL_MODE_2D_  */

/* イベント処理モードを得る。 */
int AutoGL_GetMode2D (void);        /*  AUTOGL_MODE_2D_  */

/* ドラッグ中に描画を行う。 */
void AutoGL_EnableDragInMode2D (void);

/* ユーザー定義イベント処理コールバックを設定する。 */
void AutoGL_SetDefaultCallbackInMode2D 
(AutoGL_CallbackType otherCallback);
/* otherCallback : ユーザー定義イベント処理コールバック */
/* これはNULLでもよい。 */

/* デフォルトのキーイベント処理 */
/* ユーザー定義イベント処理コールバックから呼ぶ。 */
void AutoGL_HandleDefaultKeyEventInMode2D (void);

/* ポインティングデバイスがヒットしたかを調べる。 */
int AutoGL_PointingDeviceIsHit2D 
(double pX, double pY,
 int toleranceDc);
/* (pX, pY) : 世界座標, ただし二次元 */
/* toleranceDc : デバイス座標系での幅 */



#endif  /* __AUTOGL_UTILITY2D__ */



