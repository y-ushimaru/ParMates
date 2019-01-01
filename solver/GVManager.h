/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __GV_MANAGER_H__
#define __GV_MANAGER_H__

#include <string>
#include <map>
#include <unordered_map>

/// シミュレーション内で大域的に利用する変数を管理するクラス
/**
 * 変数を登録・管理する
 *
 * @note Managerは静的クラスとする
 * @ingroup Manager IO
 */
class GVManager
{
public:
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** @name 値を返す関数 */
    //@{

    /// 指定のシンボルに関連付けられた文字列を返す
    static bool getVariable(const std::string& key, std::string* value);

    /// 指定のシンボルに関連付けられた文字列を返す
    static std::string getString(const std::string& key);

    /// 指定のシンボルに関連付けられた数値を返す
    static bool getVariable(const std::string& key, double* value);

    /// 指定のシンボルに関連付けられた数値を返す
    static double getNumeric(const std::string& key);

    /// 指定のシンボルに関連付けられた真偽値を返す
    static bool getFlag(const std::string& key);

    /// _maxTimeを返す
    static unsigned long getMaxTime();

    //@}

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** @name 値をセットする関数 */
    //@{

    /// 指定のシンボルに文字列を設定する
    static bool setNewString(const std::string& key,
                             const std::string& value);

    /// 指定のシンボルに数値を設定する
    static bool setNewNumeric(const std::string& key,
                              const double value);

    /// 指定のシンボルに数値を設定する
    /**
     * @note
     * 内部でdoubleに変換する
     */
    static bool setNewNumeric(const std::string& key,
                              const int value);
    
    /// 指定のシンボルに真偽値を設定する
    static bool setNewFlag(const std::string& key,
                           const bool value);

    //@}

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** @name 値をリセットする関数 */
    //@{
    
    /// 指定のシンボルに文字列を設定し直す
    static bool resetString(const std::string& key,
                            const std::string& value);

    /// 指定のシンボルに数値を設定し直す
    static bool resetNumeric(const std::string& key,
                             const double value);

    /// 指定のシンボルに数値を設定し直す
    /**
     * @note
     * 内部でdoubleに変換する
     */
    static bool resetNumeric(const std::string& key,
                             const int value);
    
    /// 指定のシンボルに真偽値を設定し直す
    static bool resetFlag(const std::string& key,
                          const bool value);

    /// _maxTimeを設定し直す
    /**
     * @note
     * _maxTimeは初期値0が与えられているため，setではなくreset関数
     */
    static bool resetMaxTime(const unsigned long maxTime);
    //@}

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    /// ファイルから変数を読み込む
    /**
     * 1行につき1変数．形式は symbol=value とする．
     */
    static bool setVariablesFromFile(const std::string& fileName);

    /// 全ての変数を出力する
    static void print();

protected:
    /// 文字列として保持される変数
    static std::unordered_map<std::string, std::string> _strings;

    /// 数値として保持される変数
    static std::unordered_map<std::string, double> _numerics;

    /// 真偽値として保持される変数
    static std::unordered_map<std::string, bool> _flags;

    /// _maxTimeのみ特別に持つ
    static unsigned long _maxTime;

private:
    GVManager(){};
    ~GVManager(){};

    /// 文字列のキーが登録されているかどうかチェックする
    static bool _isStringKeyFound(const std::string& key);

    /// 数値のキーが登録されているかどうかチェックする
    static bool _isNumericKeyFound(const std::string& key);

    /// 真偽値のキーが登録されているかどうかチェックする
    static bool _isFlagKeyFound(const std::string& key);
};

#endif //__GV_MANAGER_H__
