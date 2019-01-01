/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __AUTOGL_MATES__
#define __AUTOGL_MATES_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /// 2次元の線分を描画する
    void AutoGL_DrawBoldLine2D(
        double x0, double y0, double z0,    
        double x1, double y1, double z1,
        double width);

    /// 3次元の線分を描画する
    void AutoGL_DrawBoldLine(
        double x0, double y0, double z0,    
        double x1, double y1, double z1,
        double width);
  
    /// 2次元の矢印を描画する
    void AutoGL_DrawArrow2D(
        double x0, double y0, double z0,
        double x1, double y1, double z1);

    /// 2次元の矢印を描画 太さを指定できる
    void AutoGL_DrawBoldArrow2D(
        double x0, double y0, double z0,
        double x1, double y1, double z1,
        double width);

    /// 3次元の矢印を描画 太さを指定できる
    void AutoGL_DrawBoldArrow(
        double x0, double y0, double z0,
        double x1, double y1, double z1,
        double width
        );
  
    /// 画面上の定位置に文字列を描画する
    /**
     * @param sizeDc デバイス座標系でどの位のマージンをとるか 
     * @param str 描画される文字列
     */
    void AutoGL_DrawStickyString(
        int sizeDc,
        const char *str);

    /// 画面上の定位置に四角形を描画する
    void AutoGL_DrawSticyQuadrangle2D(
        int margin, double xWidth, double yWidth);

    /// 赤，青の順でコンターマップを設定する
    void AutoGL_SetContourMap_RB();

    // 地面を描画する
    void AutoGL_DrawBackground2D();

#ifdef __cplusplus
}
#endif
#endif /*__AUTOGL_MATES__*/
