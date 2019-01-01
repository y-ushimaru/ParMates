package jp.ac.u_tokyo.t.q.adventure.mates.control;

import java.awt.Font;
import java.io.File;
import java.util.logging.Logger;

import javax.swing.JPanel;

import jp.ac.u_tokyo.t.q.adventure.mates.model.TrafficModel;
import jp.ac.u_tokyo.t.q.adventure.mates.resource.ResourceManager;
import jp.ac.u_tokyo.t.q.adventure.mates.ui.SingleBarMonitorDialog;
import jp.ac.u_tokyo.t.q.adventure.mates.ui.View2D;
import jp.ac.u_tokyo.t.q.adventure.mates.ui.render.Drawable2DPanel;
import jp.ac.u_tokyo.t.q.adventure.mates.ui.render.Drawable3DPanel;
import jp.ac.u_tokyo.t.q.adventure.mates.ui.render.ViewConfiguration;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;
import jp.ac.u_tokyo.t.q.adventure.mates.util.ExtensionFileFilter;

/**
 * �S�̂��Ǘ�����N���X
 */
public class MainControl implements View2DEventListener, ViewControlListener, StatusLabelListener {
  /**
   * ���O�n���h�����O�N���X
   */
  private Logger _logger;
  /**
   * GUI�N���X
   */
  private View2D _view2D;
  /**
   * 3D��ʃN���X
   */
  private Drawable3DPanel _drawable3DPanel;
  /**
   * IO�֘A���Ǘ�����N���X
   */
  private IOControl _ioControl;
  /**
   * ���\�[�X�}�l�[�W��
   */
  private ResourceManager _rm;
  
  /**
   * �R���X�g���N�^
   */
  public MainControl() {
    _logger = Logger.getLogger(MainControl.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    
    _rm = ResourceManager.getInstance();
    _ioControl = new IOControl();
    Drawable2DPanel drawable2DPanel = new Drawable2DPanel(this);
    JPanel panel3D = new JPanel();
    _drawable3DPanel = new Drawable3DPanel(this, panel3D, drawable2DPanel);
    _view2D = new View2D(this, panel3D, drawable2DPanel);
    _view2D.setVisible(true);
    showMainComponent(View2D.PANEL_NAME_3D);
  }
  
  /**
   * Exit
   */
  public void exit() {
    _logger.info("Exit program.");
    
    //�E�B���h�E��j������B
    _view2D.exit();
  }
  
  /**
   * 3D��ʂ��ĕ`�悷��
   */
  public void reDraw() {
    _drawable3DPanel.reDraw();
  }
  
  /**
   * �^����ꂽ������ɑΉ����鏈�����s��
   * @param command ����������e��\��������
   */
  public void action(String command) {
    _logger.info("Action on command : " + command);
    
    // ################################################################################
    // COMMAND_FILE_**
    // ################################################################################
    if (command.equalsIgnoreCase(View2D.COMMAND_FILE_LOAD)) {
      loadData();
    } else if (command.equalsIgnoreCase(View2D.COMMAND_FILE_CONVERT)) {
      convertData();
    } else if (command.equalsIgnoreCase(View2D.COMMAND_FILE_EXIT)) {
      exit();
    }
    // ################################################################################
    // COMMAND_VIEW_**
    // ################################################################################
    else if (command.equalsIgnoreCase(View2D.COMMAND_VIEW_CONFIGURE_VIEW)) {
      showConfigureViewDialog();
    } else if (command.equalsIgnoreCase(View2D.COMMAND_VIEW_CONFIGURE_TRAFFIC_TIME)) {
      showConfigureTrafficTimeDialog();
    }
    // ################################################################################
    // COMMAND_VIEW_**
    // ################################################################################
    else if (command.equalsIgnoreCase(View2D.COMMAND_TOOL_SAVE_SCREEN_SHOT)) {
      saveScreenImage();
    } else {
      _logger.info("!!! Action Not found : " + command);
    }
  }
  
  /**
   * �^����ꂽ�������boolean�l�ɑΉ����鏈�����s��
   * @param command ����������e��\��������
   * @param state ��Ԃ�\���l
   */
  public void action(String command, boolean state) {
    _logger.info("Action on command : " + command + " with state \"" + state + "\"");
  }
  
  public void loadData() {
    File selectedDir = showSelectDirectoryDialog(null, null);
    
    if (selectedDir != null) {
      _logger.info("Load data from " + selectedDir.getAbsolutePath());
      
      LoadDataExecutor1 executor = new LoadDataExecutor1(this, selectedDir);
    }
  }
  
  public void loadDataImpl(File dir) {
    SingleBarMonitorDialog dialogMonitor = _view2D.showSingleBarMonitorDialog();
    dialogMonitor.setIndeterminate(true);
    dialogMonitor.setVisible(true);
    
    LoadDataExecutor2 executor = new LoadDataExecutor2(_ioControl, dir);
    boolean isCanceld = false;
    while (executor.isAlive()) {
      if (dialogMonitor.isCanceled()) {
        isCanceld = true;
        executor.cancel();
      }
      
      //float progress = executor.getProgress();
      String message1 = executor.getMessage1();
      String message2 = executor.getMessage2();
      
      dialogMonitor.setMessage1(message1);
      dialogMonitor.setMessage2(message2);
      
      try {
        Thread.sleep(10);
      } catch (Exception e) {
      }
    }
    
    TrafficModel model = executor.getModel();
    if (model != null) {
      _drawable3DPanel.setTrafficModel(model);
      _drawable3DPanel.reDraw();
    } else {
      if (isCanceld) {
        _logger.info("Loading canceled");
      } else {
        String message = _rm.getString("error.load.message") + Common.EOL + "directory=" + dir.getAbsolutePath();
        String title = _rm.getString("error.load.title");
        showErrorMessageDialog(message, title);
      }
    }
    
    dialogMonitor.setVisible(false);
    _view2D.hideDialogs();
    
    _logger.info("Finish load data");
  }
  
  public void convertData() {
    File selectedDir = showSelectDirectoryDialog(null, null);
    
    if (selectedDir != null) {
      _logger.info("Load data from " + selectedDir.getAbsolutePath());
      
      ConvertDataExecutor1 executor = new ConvertDataExecutor1(this, selectedDir);
    }
  }
  
  public void convertDataImpl(File dir) {
    SingleBarMonitorDialog dialogMonitor = _view2D.showSingleBarMonitorDialog();
    dialogMonitor.setIndeterminate(true);
    dialogMonitor.setVisible(true);
    
    ConvertDataExecutor2 executor = new ConvertDataExecutor2(_ioControl, dir);
    boolean isCanceld = false;
    while (executor.isAlive()) {
      if (dialogMonitor.isCanceled()) {
        isCanceld = true;
        executor.cancel();
      }
      
      //float progress = executor.getProgress();
      String message1 = executor.getMessage1();
      String message2 = executor.getMessage2();
      
      dialogMonitor.setMessage1(message1);
      dialogMonitor.setMessage2(message2);
      
      try {
        Thread.sleep(10);
      } catch (Exception e) {
      }
    }
    
    boolean isSuccess = executor.isSuccess();
    if (isSuccess) {
      String message = _rm.getString("confirm.convert.message") + Common.EOL + "directory=" + dir.getAbsolutePath();
      String title = _rm.getString("confirm.convert.title");
      showInfoMessageDialog(message, title);
    } else {
      if (isCanceld) {
        _logger.info("Converting canceled");
      } else {
        String message = _rm.getString("error.convert.message") + Common.EOL + "directory=" + dir.getAbsolutePath();
        String title = _rm.getString("error.convert.title");
        showErrorMessageDialog(message, title);
      }
    }
    
    dialogMonitor.setVisible(false);
    _view2D.hideDialogs();
    
    _logger.info("Finish convert data");
  }
  
  /**
   * false�̂Ƃ����C����ʂւ̑�����󂯕t���Ȃ�����
   * @param isEnabled �I���I�t�̃t���O
   */
  public void setEnabled(boolean isEnabled) {
    _view2D.setEnabled(isEnabled);
  }
  
  /**
   * �t�H���g���Z�b�g����B
   * @param font �t�H���g
   */
  public void setFont(Font font) {
    _drawable3DPanel.setFont(font);
  }
  
  /**
   * �`����e���t�@�C���ɕۑ�����
   */
  public void saveScreenImage() {
    String extensionPNG = _rm.getString("extension.image.png.name");
    String descriptionPNG = _rm.getString("extension.image.png.description");
    String extensionJPG = _rm.getString("extension.image.jpg.name");
    String descriptionJPG = _rm.getString("extension.image.jpg.description");
    ExtensionFileFilter[] filters = new ExtensionFileFilter[2];
    filters[0] = new ExtensionFileFilter(true);
    filters[0].setExtensions(extensionPNG, true);
    filters[0].setDescription(descriptionPNG);
    filters[1] = new ExtensionFileFilter(true);
    filters[1].setExtensions(extensionJPG, true);
    filters[1].setDescription(descriptionJPG);
    File file = showSaveFileDialog(null, filters);
    if (file != null) {
      String imageType = Common.IMAGE_TYPE_PNG;
      String path = file.getAbsolutePath();
      if (path.toUpperCase().endsWith(extensionJPG.toUpperCase())) {
        imageType = Common.IMAGE_TYPE_JPG;
      }
      saveScreenImage(path, imageType);
    }
  }
  
  /**
   * �`����e���摜�t�@�C���Ƃ��ĕۑ�����B
   * @param filePath �ۑ���̃t�@�C���p�X
   * @param imageType �摜�̎��
   */
  public void saveScreenImage(String filePath, String imageType) {
    File file = new File(filePath);
    file.mkdirs();
    _drawable3DPanel.saveScreenImage(filePath, imageType);
  }
  
  /**
   * �`����e�̕ۑ���̃t�@�C���p�X��ݒ肷��B
   * ���ۂɃt�@�C���ւ̕ۑ��́Adisplay()�̍Ō�Ŏ��s����B
   * @param filePath �ۑ���̃t�@�C���p�X
   * @param imageType �摜�̎��
   */
  public void reserveToSaveScreenImage(String filePath, String imageType) {
    _drawable3DPanel.reserveToSaveScreenImage(filePath, imageType);
  }
  
  /**
   * 3D,2D�̂ǂ����\�����邩���w�肷��B
   * @param name �p�l����\��������
   */
  public void showMainComponent(String name) {
    _view2D.showMainComponent(name);
    if (name != null) {
      if (name.equals(View2D.PANEL_NAME_3D)) {
        _drawable3DPanel.enable3D();
      } else if (name.equals(View2D.PANEL_NAME_2D)) {
        _drawable3DPanel.enable2D();
      }
    }
    reDraw();
  }
  
  /**
   * ���b�Z�[�W��\������
   * @param message ���b�Z�[�W�̕�����
   * @param title �^�C�g���̕�����
   */
  public void showInfoMessageDialog(String message, String title) {
    _view2D.showInfoMessageDialog(message, title);
  }
  
  /**
   * �G���[���b�Z�[�W��\������
   * @param message ���b�Z�[�W�̕�����
   * @param title �^�C�g���̕�����
   */
  public void showErrorMessageDialog(String message, String title) {
    _view2D.showErrorMessageDialog(message, title);
  }
  
  /**
   * �x�����b�Z�[�W��\������
   * @param message ���b�Z�[�W�̕�����
   * @param title �^�C�g���̕�����
   */
  public void showWarningMessageDialog(String message, String title) {
    _view2D.showWarningMessageDialog(message, title);
  }
  
  /**
   * �P�ƃt�@�C���p�̑I���_�C�A���O���J��
   * @param dir �f�B���N�g��
   * @param filters �t�@�C���t�B���^
   * @return �I�������t�@�C��
   */
  public File showSelectSingleFileDialog(File dir, ExtensionFileFilter[] filters) {
    return _view2D.showSelectSingleFileDialog(dir, filters);
  }
  
  /**
   * �f�B���N�g���I���_�C�A���O���J��
   * @param dir �f�B���N�g��
   * @param filters �t�@�C���t�B���^
   * @return �I�������f�B���N�g��
   */
  public File showSelectDirectoryDialog(File dir, ExtensionFileFilter[] filters) {
    return _view2D.showSelectDirectoryDialog(dir, filters);
  }
  
  /**
   * �t�@�C���ۑ��_�C�A���O���J��
   * @param dir �f�B���N�g��
   * @param filters �t�@�C���t�B���^
   * @return �I�������t�@�C��
   */
  public File showSaveFileDialog(File dir, ExtensionFileFilter[] filters) {
    return _view2D.showSaveFileDialog(dir, filters);
  }
  
  public void showConfigureViewDialog() {
    ViewConfiguration viewConfiguration = _drawable3DPanel.getViewConfiguration();
    _view2D.showConfigureViewDialog(viewConfiguration);
  }
  
  private void showConfigureTrafficTimeDialog() {
    int nTimeSteps = 1;
    if (_drawable3DPanel.getTrafficDrawer() != null) {
      nTimeSteps = _drawable3DPanel.getTrafficDrawer().getTrafficModel().getNumTimeSteps();
    }
    int currentTimeStep = _drawable3DPanel.getCurrentTrafficTime();
    int timeStepMin = 0;
    int timeStepMax = nTimeSteps - 1;
    _view2D.showConfigureTrafficTimeDialog(timeStepMin, timeStepMax, currentTimeStep);
  }
  
  public void setStatusText(int index, String str) {
    _view2D.setStatusText(index, str);
  }
  
  public void setIndexOfActive3DPanel(int index) {
    if (_drawable3DPanel != null && _view2D != null) {
      int[] modeViewPointControl = _drawable3DPanel.getModeForViewPointControl();
      int[] vehicleIDForViewPoint = _drawable3DPanel.getVehicleIDForViewPoint();
      _view2D.setIndexOfActive3DPanel(index, modeViewPointControl, vehicleIDForViewPoint);
    }
  }
  
  /**
   * ���_�̐ݒ���@�̃��[�h���w�肷��
   * @param mode ���_�̃��[�h
   * @param vehicleID �^�]�莋�_�̏ꍇ�̎ԗ��ԍ�
   * @return �w�肵���ԍ��̎ԗ������݂��Ȃ��ꍇ�Ȃǂ�false��Ԃ�
   */
  public boolean setModeForViewPointControl(int mode, int vehicleID) {
    return _drawable3DPanel.setModeForViewPointControl(mode, vehicleID);
  }
  
  /**
   * �����X�e�b�v���w�肷��B
   * @param trafficTime �����X�e�b�v
   * @return �s���Ȏ����X�e�b�v���n���ꂽ�ꍇ��false��Ԃ�
   */
  public boolean setTrafficTime(int trafficTime) {
    return _drawable3DPanel.setTrafficTime(trafficTime);
  }
  
  public void nextTrafficTime() {
    _drawable3DPanel.nextTime();
  }
  
  public void previousTrafficTime() {
    _drawable3DPanel.previousTime();
  }
  
  /**
   * �\���̐ݒ���Z�b�g����B
   * @param viewConfiguration �\���̐ݒ�
   */
  public void setViewConfiguration(ViewConfiguration viewConfiguration) {
    _drawable3DPanel.setViewConfiguration(viewConfiguration);
  }
  
  /**
   * �\������3D��ʂ̌���ݒ肷��B
   * @param nPanels �p�l���̌�
   */
  public void setNumberOfActive3DPanels(int nPanels) {
    _drawable3DPanel.setNumberOfActive3DPanels(nPanels);
  }
  
  /**
   * ���쒆��3D�p�l���̎��_�̐ݒ���擾����B
   * @return ���_�̐ݒ��\��������z��
   */
  public String[] getViewPointConfigurationOfActive3DPanel() {
    return _drawable3DPanel.getViewPointConfigurationOfActive3DPanel();
  }
  
  /**
   * ���쒆��3D�p�l���̎��_�̐ݒ��ݒ肷��B
   * @param configuration ���_�̐ݒ�
   */
  public void setViewPointConfigurationOfActive3DPanel(String[] configuration) {
    _drawable3DPanel.setViewPointConfigurationOfActive3DPanel(configuration);
  }
}
