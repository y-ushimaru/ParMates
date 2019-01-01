/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_UIMS__
#define __AUTOGL_UIMS__

/* ユーザーインターフェイス管理 */



/* UIを作る。*/
/* このルーチンは必ずユーザープログラム側に一つ存在しなければならない。*/
void AutoGL_SetUp (int argc, char *argv[]);



/* コールバック関数（引数なし、戻り値なし）へのポインタ型 */
typedef void (*AutoGL_CallbackType) (void);

/* グループを登録する。 （パネルに対応する。） */
void AutoGL_AddGroup 
(const char *name);
/* name : グループ名 */

/* コメントを登録する。 （ラベルに対応する。） */
void AutoGL_AddComment (void);
/* ラベル文字列はAutoGL_SetLabelで設定する。 */

/* コールバック関数を登録する。 （ボタンに対応する。） */
void AutoGL_AddCallback 
(AutoGL_CallbackType callback, 
 const char *name);
/* callback : 関数名 */
/* name : コールバック名 */

/* 論理変数を登録する。 （トグルに対応する。） */
void AutoGL_AddBoolean 
(int *value_IO, 
 const char *name);
/* value_IO : int型変数へのポインタ */
/* C++では、bool型変数からキャストしても良い。 */
/* name : 論理変数名 */

/* 文字列変数を登録する。 （フィールドに対応する。） */
void AutoGL_AddString 
(char *value_IO, 
 const char *name, int length);
/* value_IO : char型変数配列 */
/* name : 文字列変数名 */
/* length : 配列長 */

/* 整数変数を登録する。 （フィールドまたはチョイスに対応する。） */
void AutoGL_AddInteger 
(int *value_IO, 
 const char *name);
/* value_IO : int型変数へのポインタ */
/* name : 整数変数名 */
/* C++では、enum型変数からキャストしても良い。 */

/* 整数変数の最大最小レンジを登録する。 */
void AutoGL_SetIntegerRange 
(int minValue, int maxValue);

/* 整数変数の列挙値を加える。 （チョイスに対応する。）*/
void AutoGL_AddIntegerItem 
(const char *label);
/* label : 整数変数の値のラベル */

/* 実数変数を登録する。 （フィールドに対応する。）*/
void AutoGL_AddReal 
(double *value_IO, 
 const char *name);
/* value_IO : double型変数へのポインタ */
/* name : 実数変数名 */

/* 実数変数の最大最小レンジを登録する。 */
void AutoGL_SetRealRange 
(double minValue, double maxValue);

/* 現在のコールバック関数・制御変数について、ラベルを設定する。 */
void AutoGL_SetLabel 
(const char *label);
/* label : 関数・変数のラベル */

/* ビューの再描画用コールバック関数を登録する。 */
void AutoGL_SetViewRedrawCallback 
(AutoGL_CallbackType callback);
/* callback : 関数名 */

/* ビューのイベント処理コールバック関数を登録する。 */
void AutoGL_SetViewEventCallback 
(AutoGL_CallbackType callback);
/* callback : 関数名 */

/* アイドルイベント処理を有効にする。 */
void AutoGL_EnableIdleEvent (void);

/* アイドルイベント処理コールバック関数を登録する。 */
void AutoGL_SetIdleEventCallback 
(AutoGL_CallbackType callback);
/* callback : 関数名 */

/* バッチ処理コールバック関数を登録する。 */
void AutoGL_SetBatchProcessCallback 
(AutoGL_CallbackType callback);
/* callback : 関数名 */



#endif  /* __AUTOGL_UIMS__ */




