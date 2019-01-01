/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL__
#define __AUTOGL__

#ifdef __cplusplus
#include <cstdio>
extern "C" {
#endif



/* OS依存処理 */
#include "autogl_os.h"

/* 幾何演算 */
#include "autogl_math.h"

/* ユーザーインターフェイス管理 */
#include "autogl_uims.h"

/* 三次元グラフィックス処理 */
#include "autogl_graphics.h"

/* グラフィカルユーザーインターフェイス処理 */
#include "autogl_gui.h"

/* コマンドラインとスクリプティング処理 */
#include "autogl_cui.h"



/* ユーティリティ */
#include "autogl_utility.h"

/* 可視化ユーティリティ(コンター) */
#include "autogl_contour.h"

/* 可視化ユーティリティ(低レベルAPI) */
#include "autogl_vislow.h"

/* 可視化ユーティリティ(高レベルAPI) */
#include "autogl_visualization.h"

/* 二次元アプリケーション用ユーティリティ */
#include "autogl_utility2d.h"

/* 三次元アプリケーション用ユーティリティ */
#include "autogl_utility3d.h"



#ifdef __cplusplus
}
#endif

#endif  /* __AUTOGL__ */

