/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_CUI__
#define __AUTOGL_CUI__

/* コマンドラインとスクリプティング処理 */



/* ファイルからコマンドを解釈・処理する。 */
void AutoGL_InterpretInBatchMode (FILE *in, FILE *out);
/* in : コマンドの入力先 */
/* out : コマンド処理結果の出力先 */

/* 標準入力からコマンドを解釈・処理する。 */
void AutoGL_InterpretInInteractiveMode (void);
/* コマンドの処理結果は標準出力に表示される。 */

/* スクリプトファイルを解釈・処理する。 */
void AutoGL_InterpretScript (char *fileName);
/* fileName : スクリプトファイル名 */
/* コマンドの処理結果は標準出力に表示される。 */



#endif  /* __AUTOGL_CUI__ */



