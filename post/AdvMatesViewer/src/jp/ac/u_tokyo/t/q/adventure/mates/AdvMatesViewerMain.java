package jp.ac.u_tokyo.t.q.adventure.mates;

import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.control.MainControl;
import jp.ac.u_tokyo.t.q.adventure.mates.resource.ResourceManager;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * �N���N���X
 */
public class AdvMatesViewerMain {
  protected static final String OPTION_VERSION = "-version";
  
  /**
   * ���O�n���h�����O�N���X
   */
  private Logger _logger;
  /**
   * ���\�[�X�}�l�[�W��
   */
  private ResourceManager _rm;
  
  /**
   * ���C�����\�b�h
   * @param args ���s������
   */
  public static void main(String[] args) {
    new AdvMatesViewerMain(args);
  }
  
  /**
   * �R���X�g���N�^
   * @param args ���s������
   */
  public AdvMatesViewerMain(String[] args) {
    _logger = Logger.getLogger(AdvMatesViewerMain.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    _rm = ResourceManager.getInstance();
    if (args.length == 1 && (args[0].equals(OPTION_VERSION))) {
      StringBuffer strbuff = new StringBuffer();
      strbuff.append(Common.EOL);
      strbuff.append("Product Name : ADVENTURE MATES").append(Common.EOL);
      strbuff.append("     Version : Ver1.0").append(Common.EOL);
      _logger.info(strbuff.toString());
      System.exit(0);
    }
    
    MainControl mainControl = new MainControl();
    mainControl.setEnabled(true);
  }
  
  public static String getBuildNumber() {
    ResourceManager rm = ResourceManager.getInstance();
    return rm.getString(Common.RESOURCE_KEY_BUILD_NUMBER);
  }
}
