package jp.ac.u_tokyo.t.q.adventure.mates.control;

/**
 * ModelControlからMainControlなどのメソッドを呼ぶためのリスナクラス
 */
public interface ViewControlListener {
  /**
   * エラーメッセージを表示する
   * @param message メッセージの文字列
   * @param title タイトルの文字列
   */
  public void showErrorMessageDialog(String message, String title);
}
