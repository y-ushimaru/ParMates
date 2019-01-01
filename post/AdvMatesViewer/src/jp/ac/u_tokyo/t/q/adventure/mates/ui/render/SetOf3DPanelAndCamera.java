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
   * OpenGL�̏��������\�b�h
   */
  public void init(GLAutoDrawable glAutoDrawable) {
    _drawable3DPanel.init(_panelIndex, glAutoDrawable);
  }
  
  /**
  * �`��̈�̃T�C�Y�ύX���ɌĂ΂�郁�\�b�h
  */
  public void reshape(GLAutoDrawable glAutoDrawable, int x, int y, int width, int height) {
    _drawable3DPanel.reshape(_panelIndex, glAutoDrawable, x, y, width, height);
  }
  
  /**
   * JOGL�ł͎�������ĂȂ��̂ŁA��ŗǂ��B
   */
  public void displayChanged(GLAutoDrawable glAutoDrawable, boolean modeChanged, boolean deviceChanged) {
    _drawable3DPanel.displayChanged(_panelIndex, glAutoDrawable, modeChanged, deviceChanged);
  }
  
  /**
   * OpengGL�̕`�惁�\�b�h
   */
  public void display(GLAutoDrawable glAutoDrawable) {
    _drawable3DPanel.display(_panelIndex, glAutoDrawable);
  }
}
