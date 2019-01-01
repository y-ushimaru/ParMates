package jp.ac.u_tokyo.t.q.adventure.mates.resource;

import java.util.Locale;
import java.util.ResourceBundle;
import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * ���\�[�X���Ǘ�����N���X
 */
public class ResourceManager {
  /**
   * ���\�[�X�t�@�C���̒u���ꏊ������������
   */
  private static final String PATH_RESOURCE = "jp.ac.u_tokyo.t.q.adventure.mates.resource.resource";
  /**
   * �r���h�ԍ����������t�@�C���̒u���ꏊ������������
   */
  private static final String PATH_BUILD_NUMBER = "jp.ac.u_tokyo.t.q.adventure.mates.resource.buildNumber";
  /**
   * �V���O���g���̂��߂̃I�u�W�F�N�g�B
   */
  private static ResourceManager _instance;
  /**
   * ���\�[�X�̓ǂݍ��݁E�Ǘ����s���I�u�W�F�N�g(�r���h�ԍ��p)
   */
  private ResourceBundle _bundleBuildNumber;
  /**
   * ���\�[�X�̓ǂݍ��݁E�Ǘ����s���I�u�W�F�N�g
   */
  private ResourceBundle _bundle;
  /**
   * ���O�n���h�����O�N���X
   */
  private Logger _logger;
  
  /**
   * �R���X�g���N�^�B<br>
   * �V���O���g���̂��߂ɁA�O������ĂׂȂ��悤�ɂ��Ă���
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
   * ResourceManager�I�u�W�F�N�g��Ԃ��B���߂Ď擾����ۂɂ̓I�u�W�F�N�g�̐������s��
   * @return ResourceManager�̃V���O���g���I�u�W�F�N�g�B
   */
  public static ResourceManager getInstance() {
    if (_instance == null) {
      _instance = new ResourceManager();
    }
    return _instance;
  }
  
  /**
   * �����̃L�[������ɑΉ����郊�\�[�X�������Ԃ��B
   * �L�[������ɑΉ����郊�\�[�X�����݂��Ȃ��ꍇ�́A�L�[�����񂻂̂��̂�Ԃ��B
   * @param key �L�[������B
   * @return �����̕�������L�[�Ƃ��郊�\�[�X������B
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