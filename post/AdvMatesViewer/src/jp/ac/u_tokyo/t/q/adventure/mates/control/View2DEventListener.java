package jp.ac.u_tokyo.t.q.adventure.mates.control;

import java.awt.Font;
import java.io.File;

import jp.ac.u_tokyo.t.q.adventure.mates.ui.render.ViewConfiguration;
import jp.ac.u_tokyo.t.q.adventure.mates.util.ExtensionFileFilter;

/**
 * View2D�ɑ΂���A�N�V�������������邽�߂̃��X�i�N���X
 */
public interface View2DEventListener {
  /**
   * �^����ꂽ������ɑΉ����鏈�����s��
   * @param command ����������e��\��������
   */
  public void action(String command);
  
  /**
   * �^����ꂽ�������boolean�l�ɑΉ����鏈�����s��
   * @param command ����������e��\��������
   * @param state ��Ԃ�\���l
   */
  public void action(String command, boolean state);
  
  /**
   * false�̂Ƃ����C����ʂւ̑�����󂯕t���Ȃ�����
   * @param isEnabled �I���I�t�̃t���O
   */
  public void setEnabled(boolean isEnabled);
  
  /**
   * �t�H���g���Z�b�g����B
   * @param font �t�H���g
   */
  public void setFont(Font font);
  
  /**
   * 3D��ʂ��ĕ`�悷��
   */
  public void reDraw();
  
  /**
   * �`����e���摜�t�@�C���Ƃ��ĕۑ�����B
   * @param filePath �ۑ���̃t�@�C���p�X
   * @param imageType �摜�̎��
   */
  public void saveScreenImage(String filePath, String imageType);
  
  /**
   * �`����e�̕ۑ���̃t�@�C���p�X��ݒ肷��B
   * ���ۂɃt�@�C���ւ̕ۑ��́Adisplay()�̍Ō�Ŏ��s����B
   * @param filePath �ۑ���̃t�@�C���p�X
   * @param imageType �摜�̎��
   */
  public void reserveToSaveScreenImage(String filePath, String imageType);
  
  /**
   * 3D,2D�̂ǂ����\�����邩���w�肷��B
   * @param name �p�l����\��������
   */
  public void showMainComponent(String name);
  
  /**
   * ���b�Z�[�W��\������
   * @param message ���b�Z�[�W�̕�����
   * @param title �^�C�g���̕�����
   */
  public void showInfoMessageDialog(String message, String title);
  
  /**
   * �G���[���b�Z�[�W��\������
   * @param message ���b�Z�[�W�̕�����
   * @param title �^�C�g���̕�����
   */
  public void showErrorMessageDialog(String message, String title);
  
  /**
   * �x�����b�Z�[�W��\������
   * @param message ���b�Z�[�W�̕�����
   * @param title �^�C�g���̕�����
   */
  public void showWarningMessageDialog(String message, String title);
  
  /**
   * �P�ƃt�@�C���p�̑I���_�C�A���O���J��
   * @param dir �f�B���N�g��
   * @param filters �t�@�C���t�B���^
   * @return �I�������t�@�C��
   */
  public File showSelectSingleFileDialog(File dir, ExtensionFileFilter[] filters);
  
  /**
   * �f�B���N�g���I���_�C�A���O���J��
   * @param dir �f�B���N�g��
   * @param filters �t�@�C���t�B���^
   * @return �I�������f�B���N�g��
   */
  public File showSelectDirectoryDialog(File dir, ExtensionFileFilter[] filters);
  
  /**
   * �t�@�C���ۑ��_�C�A���O���J��
   * @param dir �f�B���N�g��
   * @param filters �t�@�C���t�B���^
   * @return �I�������t�@�C��
   */
  public File showSaveFileDialog(File dir, ExtensionFileFilter[] filters);
  
  /**
   * ���_�̐ݒ���@�̃��[�h���w�肷��
   * @param mode ���_�̃��[�h
   * @param vehicleID �^�]�莋�_�̏ꍇ�̎ԗ��ԍ�
   * @return �w�肵���ԍ��̎ԗ������݂��Ȃ��ꍇ�Ȃǂ�false��Ԃ�
   */
  public boolean setModeForViewPointControl(int mode, int vehicleID);
  
  /**
   * �����X�e�b�v���w�肷��B
   * @param trafficTime �����X�e�b�v
   * @return �s���Ȏ����X�e�b�v���n���ꂽ�ꍇ��false��Ԃ�
   */
  public boolean setTrafficTime(int trafficTime);
  
  /**
   * �����X�e�b�v��1�i�߂�B
   */
  public void nextTrafficTime();
  
  /**
   * �����X�e�b�v��1�߂��B
   */
  public void previousTrafficTime();
  
  /**
   * �\���̐ݒ���Z�b�g����B
   * @param viewConfiguration �\���̐ݒ�
   */
  public void setViewConfiguration(ViewConfiguration viewConfiguration);
  
  /**
   * �\������3D��ʂ̌���ݒ肷��B
   * @param nPanels �p�l���̌�
   */
  public void setNumberOfActive3DPanels(int nPanels);
  
  /**
   * ���쒆��3D�p�l���̎��_�̐ݒ���擾����B
   * @return ���_�̐ݒ��\��������z��
   */
  public String[] getViewPointConfigurationOfActive3DPanel();
  
  /**
   * ���쒆��3D�p�l���̎��_�̐ݒ��ݒ肷��B
   * @param configuration ���_�̐ݒ�
   */
  public void setViewPointConfigurationOfActive3DPanel(String[] configuration);
}
