package jp.ac.u_tokyo.t.q.adventure.mates.control;

/**
 * ModelControl����MainControl�Ȃǂ̃��\�b�h���ĂԂ��߂̃��X�i�N���X
 */
public interface ViewControlListener {
  /**
   * �G���[���b�Z�[�W��\������
   * @param message ���b�Z�[�W�̕�����
   * @param title �^�C�g���̕�����
   */
  public void showErrorMessageDialog(String message, String title);
}
