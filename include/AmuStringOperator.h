/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __AMU_STRINGOPERATOR_H__
#define __AMU_STRINGOPERATOR_H__

#include <string>
#include <vector>

/// 文字列操作をまとめたクラス
/** 
 * @note このクラスはインスタンスを生成しない
 * @ingroup Utility
 */
class AmuStringOperator
{
public:
    //====================================================================
    /// 文字列から指定した文字を削除する
    /**
     * @param[in,out] result_str 変換前後の文字列
     * @param mark 取り除く文字
     */
    static void getStrippedString(std::string* result_str, const char mark);

    /// 文字列@から指定した文字列を削除する
    /**
     * @param[in,out] result_str 変換前後の文字列
     * @param mark 取り除く文字列
     */
    static void getStrippedString(std::string* result_str, const std::string& mark);

    /// 文字列から指定した文字を削除する
    /**
     * @p marksで指定した文字列に含まれる文字のうち
     * どれか1つにマッチしたらその文字を読み飛ばす
     *
     * @param[in,out] result_str 変換前後の文字列
     * @param marks 取り除く文字
     */
    static void getStrippedStringForEach(std::string* result_str,
                                         const std::string& marks);

    //====================================================================
    /// 指定した文字で始まるコメントを読み飛ばす
    /**
     * @note  改行なしが前提になっている
     *
     * @param[in,out] result_str 変換前後の文字列
     * @param comment_mark コメントの開始を表す文字
     */
    static void getSubstantialString(std::string* result_str, 
                                     const char comment_mark);

    /// 指定した文字列で始まるコメントを読み飛ばす
    /**
     * @note  改行なしが前提になっている
     *
     * @param[in,out] result_str 変換前後の文字列
     * @param comment_mark コメントの開始を表す文字列
     */
    static void getSubstantialString(std::string* result_str, 
                                     const std::string& comment_mark);

    //====================================================================
    /// 文字列を指定した文字で分割する
    /**
     * @param[out] result_str 分割後の複数の文字列が格納されたvector
     * @param str 分割する文字列
     * @param delimiter 分割する文字
     */
    static void getTokens(std::vector<std::string>* result_str, 
                          const std::string& str,
                          const char delimiter);

    /// 文字列を指定した文字列で分割する
    /**
     * @param[out] result_str 分割後の複数の文字列が格納されたvector
     * @param str 分割する文字列
     * @param delimiter 分割する文字列
     */
    static void getTokens(std::vector<std::string>* result_str, 
                          const std::string& str,
                          const std::string& delimiter);
  
    //====================================================================
    /// 文字列の中から不要な部分を削除する
    /**
     * \#で始まるコメント行と空白を読み飛ばす
     *
     * @attention 読み飛ばすルールは固定されているので，必要があれば場合は関数の中を変更する
     *
     * @param[in,out] result_str 変換前後の文字列
     */
    static void getAdjustString(std::string* result_str);

private:
    AmuStringOperator(){};
    ~AmuStringOperator(){};
};

#endif //__AMU_STRINGOPERATOR_H__
