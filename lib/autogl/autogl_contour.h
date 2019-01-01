/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_CONTOUR__
#define __AUTOGL_CONTOUR__

/* 可視化ユーティリティ(コンタ-) */



/* コンタ-マップのカラーテーブルをクリアする。 */
void AutoGL_ClearContourColor (void);

/* コンタ-マップのカラーテーブルにグレードごとの色を設定する。 */
void AutoGL_AddContourColorOfGrade 
(double grade, 
 double red, double green, double blue);

/* グレイスケールのコンタ-マップを設定する。 */
void AutoGL_SetGrayScaleContourMap (void);

/* 青、水色、緑、黄色、赤、紫の順で虹色のコンタ-マップを設定する。 */
void AutoGL_SetContourMap_BCGYRM (void);

/* 紫、青、水色、緑、黄色、赤の順で虹色のコンタ-マップを設定する。 */
void AutoGL_SetContourMap_MBCGYR (void);

/* 青、水色、緑、黄色、赤の順で虹色のコンタ-マップを設定する。 */
void AutoGL_SetContourMap_BCGYR (void);

/* コンタ-の値に対応する色を得る。 */
void AutoGL_GetContourColor 
(double *red_OUT, double *green_OUT, double *blue_OUT,
 double grade);
/* red_OUT, green_OUT, blue_OUT : 赤、緑、青について、
                                  それぞれ0.0 - 1.0のレンジの強度。 */
/* grade : 0.0 - 1.0に規格化された値。 */

/* 三角形をコンタ-で塗り潰す。 */
void AutoGL_DrawContourTriangle 
(double x0, double y0, double z0, double grade0,  
 double x1, double y1, double z1, double grade1,  
 double x2, double y2, double z2, double grade2); 
/* (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) : 三点の世界座標 */
/* grade0, grade1, grade2 : 三点のスカラー値（0.0 - 1.0に規格化） */

/* 画面右に面コンタ-での色と値との対応表をバー表示する。 */
void AutoGL_DrawContourMap 
(int sizeDc,
 double minRange, double maxRange);
/* sizeDc : 画面に占めるデバイス座標系での幅 */
/* minRange, maxRange : スカラー値の最大最小 */

/* 画面右に線コンタ-での色と値との対応表をバー表示する。 */
void AutoGL_DrawLineContourMap 
(int sizeDc,
 double minRange, double maxRange,
 int nGrades);
/* sizeDc : 画面に占めるデバイス座標系での幅 */
/* minRange, maxRange : スカラー値の最大最小 */
/* nGrades : 線コンタ-の本数 (-1) */

/* 画面右にバンドコンタ-での色と値との対応表をバー表示する。 */
void AutoGL_DrawBandContourMap 
(int sizeDc,
 double minRange, double maxRange,
 int nGrades);
/* sizeDc : 画面に占めるデバイス座標系での幅 */
/* minRange, maxRange : スカラー値の最大最小 */
/* nGrades : 線コンタ-の本数 (-1) */



#endif  /* __AUTOGL_CONTOUR__ */











