/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_GRAPHICS__
#define __AUTOGL_GRAPHICS__

/* グラフィックス */



/* レンダリング */

/* ビューの背景色を設定する。 */
void AutoGL_SetBackgroundColor 
(double red, double green, double blue);  
/* red, green, blue : 赤、緑、青について、それぞれ0.0 - 1.0のレンジの強度。 */

/* 鏡面反射を有効にする。 */
void AutoGL_TurnOnSpecular (void);

/* 半透明処理を有効にする。 */
void AutoGL_TurnOnTransparency (void);

/* 現在の色を設定する。 */
void AutoGL_SetColor 
(double red, double green, double blue);  
/* red, green, blue : 赤、緑、青について、それぞれ0.0 - 1.0のレンジの強度。 */

/* 現在の不透明度を設定する。 */
void AutoGL_SetOpacity (double opacity);  
/* opacity : 不透明度で、0.0 - 1.0のレンジの強度。 */

/* 線分を現在の色で描画する。*/
void AutoGL_DrawLine 
(double x0, double y0, double z0,    
 double x1, double y1, double z1); 
/* (x0, y0, z0), (x1, y1, z1) : 二点の世界座標 */

/* 三角形を現在の色で塗り潰す。 */
void AutoGL_DrawTriangle 
(double x0, double y0, double z0,  
 double x1, double y1, double z1,  
 double x2, double y2, double z2); 
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) : 三点の世界座標 */

/* 三角形を色補間により塗り潰す。 */
void AutoGL_DrawColorInterpolatedTriangle 
(double x0, double y0, double z0, double red0, double green0, double blue0,  
 double x1, double y1, double z1, double red1, double green1, double blue1,  
 double x2, double y2, double z2, double red2, double green2, double blue2); 
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) : 三点の世界座標 */
/* (red0, green0, blue0), (red1, green1, blue1), (red2, green2, blue2) : 三点の色 */

/* 三角形を法線ベクトル補間により塗り潰す。 */
void AutoGL_DrawNormalInterpolatedTriangle 
(double x0, double y0, double z0, double nx0, double ny0, double nz0,
 double x1, double y1, double z1, double nx1, double ny1, double nz1,
 double x2, double y2, double z2, double nx2, double ny2, double nz2);
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) : 三点の世界座標 */
/* (nx0, ny0, nz0), (nx1, ny1, nz1), (nx2, ny2, nz2) : 三点の法線ベクトル */

/* 四辺形を現在の色で塗り潰す。 */
void AutoGL_DrawQuadrangle 
(double x0, double y0, double z0,
 double x1, double y1, double z1,
 double x2, double y2, double z2,
 double x3, double y3, double z3);
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2), (x3, y3, z3) : 四点の世界座標 */
/* 四点は同一平面上になくてもよい。 */

/* 文字列を現在の色で描画する。 */
void AutoGL_DrawString 
(double x, double y, double z,
 const char *str);
/* (x, y, z) : 文字列の原点の世界座標 */
/* str : 描画される文字列 */

/* ディスプレイリストを開き、これを空にする。*/
void AutoGL_OpenDisplayList (void);

/* ディスプレイリストを閉じる。 */
void AutoGL_CloseDisplayList (void);

/* ディスプレイリストを描画する。 */
void AutoGL_DrawDisplayList (void);

/* ディスプレイリストをファイルから読む。 */
void AutoGL_ReadDisplayList (FILE *fp);

/* ディスプレイリストをファイルに書く。 */
void AutoGL_WriteDisplayList (FILE *fp);

/* AutoGLメタファイルを読み出す。*/
void AutoGL_ReadMetafile 
(const char *fileName, int updateViewingParameterFlag);

/* AutoGLメタファイルに書き込む。*/
void AutoGL_WriteMetafile 
(const char *fileName);

/* AutoGLイメージファイル名を設定する。*/
void AutoGL_SetImageFileName 
(const char *fileName);

/* AutoGLイメージファイル名を取得する。*/
void AutoGL_GetImageFileName 
(char *fileName_OUT);

/* 上記の各種描画命令の高速版。 */
/* ディスプレイリストには登録されない。immediate mode用。*/
/* 法線方向をあらかじめ計算しておくこと。*/
/* 引数がすべてfloat型であることに注意。*/
void AutoGL_SetColor2 
(float red, float green, float blue);

void AutoGL_DrawLine2 
(float x0, float y0, float z0,
 float x1, float y1, float z1);

void AutoGL_DrawTriangle2 
(float x0, float y0, float z0,
 float x1, float y1, float z1,
 float x2, float y2, float z2,
 float nx, float ny, float nz);

void AutoGL_DrawColorInterpolatedTriangle2
(float x0, float y0, float z0, float red0, float green0, float blue0,  
 float x1, float y1, float z1, float red1, float green1, float blue1,  
 float x2, float y2, float z2, float red2, float green2, float blue2,
 float nx, float ny, float nz);

void AutoGL_DrawNormalInterpolatedTriangle2 
(float x0, float y0, float z0, float nx0, float ny0, float nz0,
 float x1, float y1, float z1, float nx1, float ny1, float nz1,
 float x2, float y2, float z2, float nx2, float ny2, float nz2);

void AutoGL_DrawString2
(float x, float y, float z,
 const char *str);

void AutoGL_SetSelectionId2 (int id);

/* OpenGLなどのネイティブなグラフィックスAPIを直接用いる際に、*/
/* ネイティブAPIコールの直前直後に呼び出す。*/
void AutoGL_BeginNativeCall (void);
void AutoGL_EndNativeCall (void);




/* ビューイングパラメータと座標変換 */

/* 世界座標系 : モデルが置かれている座標系。 */
/*   三次元の実数座標系。*/

/* 視点座標系 : 世界座標系が視野変換されたもの。 */
/*   三次元の実数座標系。*/
/*   画面の中心が(0, 0, 0)に対応する。*/
/*   右方向がX方向, 上がY方向, 奥行きの手前側にZ方向。*/

/* デバイス座標系 : 視点座標系が実際の画面サイズにスケールされたもの。三次元。*/
/*   三次元の整数座標系。*/
/*   画面の中心が(0, 0, 0)に対応する。*/
/*   右方向がX方向, 上がY方向, 奥行きの手前側にZ方向。*/
/*   X Window SystemやWindowsのデバイス座標系とは異なることに注意！！！ */

/* ビューのサイズを設定する。*/
void AutoGL_SetViewSize (double size);
/* size : 実際の画面の半分の大きさに対応する,世界座標におけるビューのサイズ。*/

/* ビューの注視点を設定する。*/
void AutoGL_SetViewCenter (double x, double y, double z);
/* (x, y, z) : 注視点の座標 */

/* ビューの視線方向を設定する。 */
void AutoGL_SetViewDirection (double x, double y, double z);
/* (x, y, z) : 視線方向ベクトル */

/* ビューの視線上向き方向を設定する。 */
void AutoGL_SetViewUpVector (double x, double y, double z);
/* (x, y, z) : 視線上向き方向ベクトル */

/* 投影法を透視投影にするか、しない(平行投影)かを設定する。*/
void AutoGL_SetPerspectiveViewFlag (int flag);

/* ポリゴンを裏表とも描画するかどうかを設定する。*/
void AutoGL_SetTwoSideFaceFlag (int flag);

/* ビューのサイズを得る。 */
double AutoGL_GetViewSize (void);

/* ビューの注視点を得る。 */
void AutoGL_GetViewCenter 
(double *x_OUT, double *y_OUT, double *z_OUT);
/* (x_OUT, y_OUT, z_OUT) : 注視点の座標 */

/* ビューの視線方向を得る。 */
void AutoGL_GetViewDirection 
(double *x_OUT, double *y_OUT, double *z_OUT);
/* (x_OUT, y_OUT, z_OUT) : 視線方向ベクトル */

/* ビューの視線上向き方向を得る。 */
void AutoGL_GetViewUpVector 
(double *x_OUT, double *y_OUT, double *z_OUT);
/* (x_OUT, y_OUT, z_OUT) : 視線上向き方向ベクトル */

/* ビューのデバイス座標系での高さと幅を得る。 */
void AutoGL_GetViewRangeDc 
(int *widthDc_OUT, int *heightDc_OUT);
/* widthDc_OUT : ビューの幅 */
/* heightDc_OUT : ビューの高さ */

/* ビューのデバイス座標系でのサイズを得る。 */
int AutoGL_GetViewSizeDc (void); 

/* デバイス座標から視点座標を得る。 */
void AutoGL_GetVcOfDc 
(double *vcX_OUT, double *vcY_OUT, double *vcZ_OUT,
 int dcX, int dcY, int dcZ);
/* (dcX, dcY, dcZ) : デバイス座標 */
/* (vcX_OUT, vcY_OUT, vcZ_OUT) : 視点座標 */

/* 視点座標から世界座標を得る。 */
void AutoGL_GetPositionOfVc 
(double *x_OUT, double *y_OUT, double *z_OUT,
 double vcX, double vcY, double vcZ);
/* (vcX, vcY, vcZ) : 視点座標 */
/* (x_OUT, y_OUT, z_OUT) : 世界座標 */

/* デバイス座標から世界座標を得る。 */
void AutoGL_GetPositionOfDc 
(double *x_OUT, double *y_OUT, double *z_OUT,
 int dcX, int dcY, int dcZ);
/* (dcX, dcY, dcZ) : デバイス座標 */
/* (x_OUT, y_OUT, z_OUT) : 世界座標 */

/* 投影法が透視投影かどうか。*/
int AutoGL_GetPerspectiveViewFlag (void);

/* ポリゴンを裏表とも描画するかどうか。*/
int AutoGL_GetTwoSideFaceFlag (void);



/* イメージ処理 */

/* ビューのイメージを得る。 */
void AutoGL_GetViewImage
(unsigned char image_OUT[]);
/* image_OUT : ビューの幅 x 高さ x 3 byte (24bit)の大きさの配列 */
/* 各ピクセルごとに、RGBの各成分が0-255で得られる。 */
/* unsigned char image_OUT[heightDc][widthDC][3] */




/* 選択 */

/* 選択処理を開始する。*/
void AutoGL_StartSelection (int x, int y, int range);

/* 現在の選択候補オブジェクトIDを設定する。*/
void AutoGL_SetSelectionId (int id);

/* 選択処理を終了する。*/
void AutoGL_EndSelection (void);

/* 実際に選択された選択候補オブジェクトIDを返す。*/
int AutoGL_GetSelectedId (void);




/* イベント処理 */

/* AUTOGL_EVENT_ イベントのタイプ */
/* ポインタのボタンが押された。 */
#define AUTOGL_EVENT_POINTING_DEVICE_PRESS 0
/* ポインタがドラッグされた。 */
#define AUTOGL_EVENT_POINTING_DEVICE_DRAG 1
/* ポインタのボタンが離された。 */
#define AUTOGL_EVENT_POINTING_DEVICE_RELEASE 2
/* キーボードのキーが押された。 */
#define AUTOGL_EVENT_KEY 3

/* イベントのタイプを得る。 */
int AutoGL_GetViewEvent (void);   /* AUTOGL_EVENT_ */

/* ポインティングデバイスの位置を得る。 */
void AutoGL_GetPointingDevicePositionDc 
(int *dcX_OUT, int *dcY_OUT, int *dcZ_OUT);
/* dcX_OUT, dcY_OUT, dcZ_OUT : ポインタのデバイス座標 */

/* キーボードキーを得る。 */
int AutoGL_GetKeyChar (void);






/* 古いAPI(次期バージョンでのサポート未定) */

/* 現在のテクスチャマップを設定する。*/
void AutoGL_SetTextureMap 
(int sizeLevel, const unsigned char image[]);
/* sizeLevel : テクスチャイメージのサイズのlog 2。 */
/* image[][][3] : テクスチャのRGBイメージ。 */

/* 三角形を現在のテクスチャマップで塗り潰す。*/
void AutoGL_DrawTexturedTriangle 
(double x0, double y0, double z0,
 double x1, double y1, double z1,
 double x2, double y2, double z2,
 double scaleFactor);
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) : 三点の世界座標 */
/* scaleFactor : テクスチャ座標から世界座標へのスケール */

/* イメージを描画する。 */
void AutoGL_DrawImage 
(double x, double y, double z,
 int widthDc, int heightDc, const unsigned char image[]);
/* (x, y, z) : 文字列の原点の世界座標 */
/* image : 幅 widthDc x 高さ heightDc x 3 byte (24bit)の大きさの配列 */
/* 各ピクセルごとに、RGBの各成分が0-255で得られる。 */
/* unsigned char image[heightDc][widthDC][3] */






/* test */

void AutoGL_OpenMacro (int macroId);

void AutoGL_CloseMacro (void);

void AutoGL_DrawMacro (int macroId);

void AutoGL_PushMatrix (void);

void AutoGL_PopMatrix (void);

void AutoGL_Translate (double x, double y, double z);

void AutoGL_Rotate 
(double angle,
 double axisX, double axisY, double axisZ);
/* angle is degree  0-360  */

void AutoGL_Scale (double ratio);



#endif  /* __AUTOGL_GRAPHICS__ */



