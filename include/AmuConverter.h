/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __AMU_CONVERTER_H__
#define __AMU_CONVERTER_H__

#include <string>

/**
 * @addtogroup Utility
 * @brief シミュレータ全般から利用される機能を提供するモジュール
 */

/// 型の変換を行うクラス
/**
 * @note このクラスはインスタンスを作成しない
 * @ingroup Utility
 */
class AmuConverter
{
private:
    AmuConverter(){};
    ~AmuConverter(){};

public:
    /// int型の数値をstring型に変換する
    /**
     * @param num 変換したい数字
     * @param size 桁数
     * @return 変換された文字列
     */
    static std::string itos(int num, int size);

    /// string型の文字列を実数値に変換する
    /**
     * @param str 変換したい文字列
     * @return 変換された実数値
     */
    static double strtod(const std::string& str);

    /// string型の文字列をunsigned long型に変換する
    /**
     * @param str 変換したい文字列
     * @return 変換されたunsigned long型変数
     */
    static unsigned long strtoul(const std::string& str);

    /// ファイルから読み込んだ文字列を識別番号として使えるように書式化する
    /**
     * 桁数が少ない場合は先頭を'0'で埋め、
     * 多すぎる場合は末尾から指定の桁数の文字列を返す。
     * string型からstring型への変換である。
     *
     * @param src 変換元文字列（10進数値）
     * @param size 桁数
     * @return フォーマット済み文字列  
     */
    static const std::string formatId
    (const std::string& src, 
     const std::string::size_type size);

    /// formatIdの拡張版
    /**
     * GenerateTableの"******"にも対応
     *
     * @param src 変換元文字列（10進数値もしくは"******"）
     * @param size 桁数
     * @return フォーマット済み文字列
     */
    static const std::string formatIdExt
    (const std::string& src,
     const std::string::size_type size);
};

#endif //__AMU_CONVERTER_H__
