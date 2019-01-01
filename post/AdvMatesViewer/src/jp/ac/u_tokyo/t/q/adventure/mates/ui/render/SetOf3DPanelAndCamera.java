package jp.ac.u_tokyo.t.q.adventure.mates.ui.render;

import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLEventListener;

public class SetOf3DPanelAndCamera implements GLEventListener {
  private Drawable3DPanel _drawable3DPanel;
  private int _panelIndex;
  
  public SetOf3DPanelAndCamera(Drawable3DPanel drawable3DPanel, int panelIndex) {
    _drawable3DPanel = drawable3DPanel;
    _panelIndex = panelIndex;
  }
  
  /**
   * OpenGLの初期化メソッド
   */
  public void init(GLAutoDrawable glAutoDrawable) {
    _drawable3DPanel.init(_panelIndex, glAutoDrawable);
  }
  
  /**
  * 描画領域のサイズ変更時に呼ばれるメソッド
  */
  public void reshape(GLAutoDrawable glAutoDrawable, int x, int y, int width, int height) {
    _drawable3DPanel.reshape(_panelIndex, glAutoDrawable, x, y, width, height);
  }
  
  /**
   * JOGLでは実装されてないので、空で良い。
   */
  public void displayChanged(GLAutoDrawable glAutoDrawable, boolean modeChanged, boolean deviceChanged) {
    _drawable3DPanel.displayChanged(_panelIndex, glAutoDrawable, modeChanged, deviceChanged);
  }
  
  /**
   * OpengGLの描画メソッド
   */
  public void display(GLAutoDrawable glAutoDrawable) {
    _drawable3DPanel.display(_panelIndex, glAutoDrawable);
  }
}
