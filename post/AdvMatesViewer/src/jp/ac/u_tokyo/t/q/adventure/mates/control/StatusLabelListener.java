package jp.ac.u_tokyo.t.q.adventure.mates.control;

/**
 * メインウィンドウ下部にステータスを表示するためのリスナクラス
 */
public interface StatusLabelListener {
  //@formatter:off
  /**
   * ピックで得た情報を表示するラベルのインデックス
   */
  public static int LABEL_INDEX_PICK_INFO  = 0;
  /**
   * FPSを表示するラベルのインデックス
   */
  public static int LABEL_INDEX_FPS        = 1;
  //@formatter:on
  
  public void setStatusText(int index, String str);
  
  public void setIndexOfActive3DPanel(int index);
}
