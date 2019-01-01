/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __FILE_MANAGER_H__
#define __FILE_MANAGER_H__

#include <string>
#include <map>
#include <unordered_map>
#include <iostream>
#include <fstream>

/// シミュレーション内で大域的に利用するファイルを管理するクラス
/**
 * 主に、計算の開始から終了まで開いておいて追記したいファイルの管理に用いる
 *
 * @attention シミュレーション終了時に全てのファイルストリームを閉じること
 * @note Managerは静的クラスとする
 * @ingroup Manager
 */
class FileManager
{
public:
    /// 指定のファイル名のファイルストリームを返す
    /**
     * @param filename ファイル名
     * @return 該当するファイルストリーム
     */
    static std::ofstream& getOFStream(const std::string& filename);

    /// 指定のファイル名のファイルストリームを閉じる
    /**
     * @param filename ファイル名
     */
    static void closeOFStream(const std::string& filename);

    /// 保持する全てのファイルストリームを閉じる
    /**
     * @note シミュレーションを終えるときに用いる
     */
    static void deleteAllOFStreams();

private:
    FileManager(){};
    ~FileManager(){};

private:
    /// ファイル名と書きこみ用ファイルオブジェクトを保持するマップ
    static std::unordered_map<std::string, std::ofstream*> _ofstreams;
};

#endif //__FILE_MANAGER_H__
