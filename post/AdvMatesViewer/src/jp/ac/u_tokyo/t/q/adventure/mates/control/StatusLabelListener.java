package jp.ac.u_tokyo.t.q.adventure.mates.control;

/**
 * ���C���E�B���h�E�����ɃX�e�[�^�X��\�����邽�߂̃��X�i�N���X
 */
public interface StatusLabelListener {
  //@formatter:off
  /**
   * �s�b�N�œ�������\�����郉�x���̃C���f�b�N�X
   */
  public static int LABEL_INDEX_PICK_INFO  = 0;
  /**
   * FPS��\�����郉�x���̃C���f�b�N�X
   */
  public static int LABEL_INDEX_FPS        = 1;
  //@formatter:on
  
  public void setStatusText(int index, String str);
  
  public void setIndexOfActive3DPanel(int index);
}
