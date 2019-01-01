package jp.ac.u_tokyo.t.q.adventure.mates.control;

import java.awt.Font;
import java.io.File;

import jp.ac.u_tokyo.t.q.adventure.mates.ui.render.ViewConfiguration;
import jp.ac.u_tokyo.t.q.adventure.mates.util.ExtensionFileFilter;

/**
 * View2Dに対するアクションを処理するためのリスナクラス
 */
public interface View2DEventListener {
  /**
   * 与えられた文字列に対応する処理を行う
   * @param command 処理する内容を表す文字列
   */
  public void action(String command);
  
  /**
   * 与えられた文字列とboolean値に対応する処理を行う
   * @param command 処理する内容を表す文字列
   * @param state 状態を表す値
   */
  public void action(String command, boolean state);
  
  /**
   * falseのときメイン画面への操作を受け付けなくする
   * @param isEnabled オンオフのフラグ
   */
  public void setEnabled(boolean isEnabled);
  
  /**
   * フォントをセットする。
   * @param font フォント
   */
  public void setFont(Font font);
  
  /**
   * 3D画面を再描画する
   */
  public void reDraw();
  
  /**
   * 描画内容を画像ファイルとして保存する。
   * @param filePath 保存先のファイルパス
   * @param imageType 画像の種類
   */
  public void saveScreenImage(String filePath, String imageType);
  
  /**
   * 描画内容の保存先のファイルパスを設定する。
   * 実際にファイルへの保存は、display()の最後で実行する。
   * @param filePath 保存先のファイルパス
   * @param imageType 画像の種類
   */
  public void reserveToSaveScreenImage(String filePath, String imageType);
  
  /**
   * 3D,2Dのどちらを表示するかを指定する。
   * @param name パネルを表す文字列
   */
  public void showMainComponent(String name);
  
  /**
   * メッセージを表示する
   * @param message メッセージの文字列
   * @param title タイトルの文字列
   */
  public void showInfoMessageDialog(String message, String title);
  
  /**
   * エラーメッセージを表示する
   * @param message メッセージの文字列
   * @param title タイトルの文字列
   */
  public void showErrorMessageDialog(String message, String title);
  
  /**
   * 警告メッセージを表示する
   * @param message メッセージの文字列
   * @param title タイトルの文字列
   */
  public void showWarningMessageDialog(String message, String title);
  
  /**
   * 単独ファイル用の選択ダイアログを開く
   * @param dir ディレクトリ
   * @param filters ファイルフィルタ
   * @return 選択したファイル
   */
  public File showSelectSingleFileDialog(File dir, ExtensionFileFilter[] filters);
  
  /**
   * ディレクトリ選択ダイアログを開く
   * @param dir ディレクトリ
   * @param filters ファイルフィルタ
   * @return 選択したディレクトリ
   */
  public File showSelectDirectoryDialog(File dir, ExtensionFileFilter[] filters);
  
  /**
   * ファイル保存ダイアログを開く
   * @param dir ディレクトリ
   * @param filters ファイルフィルタ
   * @return 選択したファイル
   */
  public File showSaveFileDialog(File dir, ExtensionFileFilter[] filters);
  
  /**
   * 視点の設定方法のモードを指定する
   * @param mode 視点のモード
   * @param vehicleID 運転手視点の場合の車両番号
   * @return 指定した番号の車両が存在しない場合などにfalseを返す
   */
  public boolean setModeForViewPointControl(int mode, int vehicleID);
  
  /**
   * 時刻ステップを指定する。
   * @param trafficTime 時刻ステップ
   * @return 不正な時刻ステップが渡された場合にfalseを返す
   */
  public boolean setTrafficTime(int trafficTime);
  
  /**
   * 時刻ステップを1つ進める。
   */
  public void nextTrafficTime();
  
  /**
   * 時刻ステップを1つ戻す。
   */
  public void previousTrafficTime();
  
  /**
   * 表示の設定をセットする。
   * @param viewConfiguration 表示の設定
   */
  public void setViewConfiguration(ViewConfiguration viewConfiguration);
  
  /**
   * 表示する3D画面の個数を設定する。
   * @param nPanels パネルの個数
   */
  public void setNumberOfActive3DPanels(int nPanels);
  
  /**
   * 操作中の3Dパネルの視点の設定を取得する。
   * @return 視点の設定を表す文字列配列
   */
  public String[] getViewPointConfigurationOfActive3DPanel();
  
  /**
   * 操作中の3Dパネルの視点の設定を設定する。
   * @param configuration 視点の設定
   */
  public void setViewPointConfigurationOfActive3DPanel(String[] configuration);
}
