/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_UTILITY3D__
#define __AUTOGL_UTILITY3D__

/* 三次元ユーティリティ */



/* 三次元ビューイングパラメータ操作 */

/* ビューイングパラメータ設定機能をパネルに準備する。 */
void AutoGL_SetPanelInView3D (void);

/* ビューを相対的に回転する。 */
void AutoGL_RotateView 
(double offsetX, double offsetY, double offsetZ);
/* offsetX, offsetY, offsetZ     around -1.0 ... 1.0 */



/* 三次元描画プリミティブ */

/* 座標系にアラインされた箱を表示する。 */
void AutoGL_DrawBox3D 
(double p0X, double p0Y, double p0Z,
 double p1X, double p1Y, double p1Z);

/* 十字マークを表示する。 */
void AutoGL_DrawCrossMark3D 
(double pX, double pY, double pZ,
 int sizeDc);

/* ダイアモンドマークを表示する。 */
void AutoGL_DrawDiamondMark3D 
(double pX, double pY, double pZ,
 int sizeDc, int isFilled);
/* isFilled : 塗り潰す（ソリッド表示）かどうか */

/* 立方体マークを表示する。 */
void AutoGL_DrawCubeMark3D 
(double pX, double pY, double pZ,
 int sizeDc, int isFilled);
/* isFilled : 塗り潰す（ソリッド表示）かどうか */

/* 矢印を表示する。 */
void AutoGL_DrawArrow3D 
(double x0, double y0, double z0,
 double x1, double y1, double z1, 
 double tipRatio);
/* tipRatio : 全体長さに対する矢印先端サイズの比 */

/* 矢印を表示する。（四角錐） */
void AutoGL_DrawConeArrow3D 
(double x0, double y0, double z0,
 double x1, double y1, double z1, 
 double tipRatio);
/* tipRatio : 全体長さに対する矢印先端サイズの比 */

/* 左下に座標系を表示する。 */
void AutoGL_DrawCoordinateSystem3D 
(int sizeDc);

/* 球を表示する。 */
void AutoGL_DrawSphere3D 
(double centerX, double centerY, double centerZ,
 double radius, 
 int nDivisions);
/* centerX, centerY, centerZ : 中心 */
/* radius : 半径 */
/* nDivisions : 分割数 */

/* 円柱を表示する。 */
void AutoGL_DrawCylinder3D 
(double bottomX, double bottomY, double bottomZ,
 double topX, double topY, double topZ,
 double radius, 
 int nDivisions);
/* bottomX, bottomY, bottomZ : 下底面の中心 */
/* topX, topY, topZ : 上底面の中心 */
/* radius : 半径 */
/* nDivisions : 分割数 */

/* 円錐を表示する。 */
void AutoGL_DrawCone3D 
(double bottomX, double bottomY, double bottomZ,
 double topX, double topY, double topZ,
 double radius, 
 int nDivisions);
/* bottomX, bottomY, bottomZ : 底面の中心 */
/* topX, topY, topZ : 円錐の先点 */
/* radius : 半径 */
/* nDivisions : 分割数 */

/* 円を表示する。 */
void AutoGL_DrawCircle3D 
(double centerX, double centerY, double centerZ,
 double normalX, double normalY, double normalZ,
 double radius, 
 int nDivisions);
/* centerX, centerY, centerZ : 中心 */
/* normalX, normalY, normalZ : 円が存在する平面の法線ベクトル */
/* radius : 半径 */
/* nDivisions : 分割数 */

/* 座標系にアラインされた箱をソリッド表示する。 */
void AutoGL_FillBox3D 
(double x0, double y0, double z0,
 double x1, double y1, double z1);



/* 三次元イベント処理 */

/*  AUTOGL_MODE_3D_  */
/* ユーザー定義イベント処理モード */
#define AUTOGL_MODE_3D_OTHER 0
/* 移動モード */
#define AUTOGL_MODE_3D_TRANSLATE 1
/* 回転モード（XY軸まわり） */
#define AUTOGL_MODE_3D_ROTATE_XY 2
/* 回転モード（Z軸まわり） */
#define AUTOGL_MODE_3D_ROTATE_Z 3
/* 拡大縮小モード */
#define AUTOGL_MODE_3D_SCALE 4

/* 移動回転拡大縮小機能をパネルに準備する。 */
void AutoGL_SetPanelInMode3D (void);

/* イベント処理モードを設定する。 */
void AutoGL_SetMode3D (int mode);    /*  AUTOGL_MODE_3D_  */

/* イベント処理モードを得る。 */
int AutoGL_GetMode3D (void);         /*  AUTOGL_MODE_3D_  */

/* ドラッグ中に描画を行う。 */
void AutoGL_EnableDragInMode3D (void);

/* ユーザー定義イベント処理コールバックを設定する。 */
void AutoGL_SetDefaultCallbackInMode3D 
(AutoGL_CallbackType otherCallback);
/* otherCallback : ユーザー定義イベント処理コールバック */
/* これはNULLでもよい。 */

/* デフォルトのキーイベント処理 */
/* ユーザー定義イベント処理コールバックから呼ぶ。 */
void AutoGL_HandleDefaultKeyEventInMode3D (void);

/* ポインティングデバイスがヒットしたかを調べる。 */
int AutoGL_PointingDeviceIsHit3D 
(double *parameter_OUT,
 double pX, double pY, double pZ,
 int toleranceDc);
/* parameter_OUT : 深さ情報 */
/* (pX, pY, pZ) : 世界座標 */
/* toleranceDc : デバイス座標系での幅 */



#endif  /* __AUTOGL_UTILITY3D__ */





