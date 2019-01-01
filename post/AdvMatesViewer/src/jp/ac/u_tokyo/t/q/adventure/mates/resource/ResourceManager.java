package jp.ac.u_tokyo.t.q.adventure.mates.resource;

import java.util.Locale;
import java.util.ResourceBundle;
import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * リソースを管理するクラス
 */
public class ResourceManager {
  /**
   * リソースファイルの置き場所を示す文字列
   */
  private static final String PATH_RESOURCE = "jp.ac.u_tokyo.t.q.adventure.mates.resource.resource";
  /**
   * ビルド番号を書いたファイルの置き場所を示す文字列
   */
  private static final String PATH_BUILD_NUMBER = "jp.ac.u_tokyo.t.q.adventure.mates.resource.buildNumber";
  /**
   * シングルトンのためのオブジェクト。
   */
  private static ResourceManager _instance;
  /**
   * リソースの読み込み・管理を行うオブジェクト(ビルド番号用)
   */
  private ResourceBundle _bundleBuildNumber;
  /**
   * リソースの読み込み・管理を行うオブジェクト
   */
  private ResourceBundle _bundle;
  /**
   * ログハンドリングクラス
   */
  private Logger _logger;
  
  /**
   * コンストラクタ。<br>
   * シングルトンのために、外部から呼べないようにしている
   */
  private ResourceManager() {
    _logger = Logger.getLogger(ResourceManager.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    Locale locale = Locale.getDefault();
    try {
      _bundle = ResourceBundle.getBundle(PATH_RESOURCE, locale);
    } catch (Exception e) {
      _logger.info("Bundle not found for base name " + PATH_RESOURCE + " , locale " + locale);
    }
    try {
      _bundleBuildNumber = ResourceBundle.getBundle(PATH_BUILD_NUMBER, locale);
    } catch (Exception e) {
      _logger.info("Bundle not found for base name " + PATH_BUILD_NUMBER + " , locale " + locale);
    }
  }
  
  /**
   * ResourceManagerオブジェクトを返す。初めて取得する際にはオブジェクトの生成も行う
   * @return ResourceManagerのシングルトンオブジェクト。
   */
  public static ResourceManager getInstance() {
    if (_instance == null) {
      _instance = new ResourceManager();
    }
    return _instance;
  }
  
  /**
   * 引数のキー文字列に対応するリソース文字列を返す。
   * キー文字列に対応するリソースが存在しない場合は、キー文字列そのものを返す。
   * @param key キー文字列。
   * @return 引数の文字列をキーとするリソース文字列。
   */
  public String getString(String key) {
    String value = null;
    if (key == null) {
      value = "";
    } else {
      boolean flgExistValue = false;
      if (flgExistValue == false) {
        try {
          value = _bundleBuildNumber.getString(key);
          flgExistValue = true;
        } catch (Exception e) {
          flgExistValue = false;
        }
      }
      if (flgExistValue == false) {
        try {
          value = _bundle.getString(key);
          flgExistValue = true;
        } catch (Exception e) {
          flgExistValue = false;
        }
      }
      if (flgExistValue == false) {
        if (value == null) {
          value = key;
        }
      }
    }
    return value;
  }
  
}