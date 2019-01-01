package jp.ac.u_tokyo.t.q.adventure.mates.ui;

import java.awt.BorderLayout;
import java.awt.CardLayout;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.HashSet;

import javax.imageio.ImageIO;
import javax.swing.AbstractButton;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.UIManager;
import javax.swing.filechooser.FileFilter;

import jp.ac.u_tokyo.t.q.adventure.mates.control.StatusLabelListener;
import jp.ac.u_tokyo.t.q.adventure.mates.control.View2DEventListener;
import jp.ac.u_tokyo.t.q.adventure.mates.image.ImageLoader;
import jp.ac.u_tokyo.t.q.adventure.mates.resource.ResourceManager;
import jp.ac.u_tokyo.t.q.adventure.mates.ui.render.ViewConfiguration;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;
import jp.ac.u_tokyo.t.q.adventure.mates.util.ExtensionFileFilter;

/**
 * ���C���E�B���h�E
 */
public class View2D extends JFrame implements ActionListener, WindowListener {
  //@formatter:off
  public static final String COMMAND_FILE                                  = "menu.file";
  public static final String COMMAND_FILE_LOAD                             = "menu.file.load";
  public static final String COMMAND_FILE_CONVERT                          = "menu.file.convert";
  public static final String COMMAND_FILE_EXIT                             = "menu.file.exit";
  
  public static final String COMMAND_VIEW                                  = "menu.view";
  public static final String COMMAND_VIEW_CONFIGURE_VIEW                   = "menu.view.configureView";
  public static final String COMMAND_VIEW_CONFIGURE_TRAFFIC_TIME           = "menu.view.configureTrafficTime";
  
  public static final String COMMAND_TOOL                                  = "menu.tool";
  public static final String COMMAND_TOOL_SAVE_SCREEN_SHOT                 = "menu.tool.saveScreenShot";
  
  public static final String COMMAND_HELP                                  = "menu.help";
  public static final String COMMAND_HELP_ABOUT                            = "menu.help.about";
  
  private static final String[] LIST_ALPHABETS = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
                                                   "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
                                                   "U", "V", "W", "X", "Y", "Z" };
  private static final int[] LIST_KEY_CODES = { 
      KeyEvent.VK_A, KeyEvent.VK_B, KeyEvent.VK_C, KeyEvent.VK_D, KeyEvent.VK_E,
      KeyEvent.VK_F, KeyEvent.VK_G, KeyEvent.VK_H, KeyEvent.VK_I, KeyEvent.VK_J,
      KeyEvent.VK_K, KeyEvent.VK_L, KeyEvent.VK_M, KeyEvent.VK_N, KeyEvent.VK_O,
      KeyEvent.VK_P, KeyEvent.VK_Q, KeyEvent.VK_R, KeyEvent.VK_S, KeyEvent.VK_T,
      KeyEvent.VK_U, KeyEvent.VK_V, KeyEvent.VK_W, KeyEvent.VK_X, KeyEvent.VK_Y,
      KeyEvent.VK_Z };
  
  public static final String PANEL_NAME_3D = "panel3D";
  public static final String PANEL_NAME_2D = "panel2D";
  
  private static final int NUM_STATUS_LABEL = 2;
  
  private static final int DEFAULT_WINDOW_WIDTH  = 800;
  private static final int DEFAULT_WINDOW_HEIGHT = 600;
  //@formatter:on
  
  /**
   * ���\�[�X�}�l�[�W��
   */
  private ResourceManager _rm;
  /**
   * GUI�֘A�̃C�x���g����������N���X�B
   * MainControl�N���X�Ŏ������Ă���B
   */
  private View2DEventListener _eventListener;
  /**
   * JMenuItem�ȂǁA�����ꂽ�Ƃ��̏�Ԃ���Ȃ��{�^����ActionCommand���i�[����
   */
  private HashSet<String> _setCommandsOfButton;
  /**
   * JRadioButtonMenuItem�ȂǁA�����ꂽ�Ƃ��̏�Ԃ��擾����K�v������{�^����ActionCommand���i�[����
   */
  private HashSet<String> _setCommandsOfSelectableButton;
  /**
   * ���C���̃R���|�[�l���g�B���ۂ�3D�\���̈�ł���GLJPanel�̃I�u�W�F�N�g
   */
  private JComponent _mainComponent1;
  private JComponent _mainComponent2;
  private JPanel _plMain;
  private CardLayout _mainCardLayout;
  
  private String _productName;
  
  /**
   * �X�e�[�^�X�\���p�̃��x���̔z��B<br>
   * JLabel�ł́A��������Z�b�g����ƃ��T�C�Y���s�����悤�ŁA
   * ���̂��߂�GLJPanel���e�����󂯂�GLEventListener.reshape()���Ă΂��B
   * ����ł�FPS��\�������肷��ƕ`�悷�邽�т�reshape()���Ă΂�邱�ƂɂȂ�̂ŁA
   * JTextField��ҏW�s�ɂ���JLabel�̑�p�Ƃ���B
   */
  private JTextField[] _lbStatus;
  /**
   * �t�@�C���I���_�C�A���O
   */
  private JFileChooser _fileChooser;
  
  /**
   * �_�C�A���O�Q
   */
  private ConfigureViewDialog _dialogConfigureView;
  private ConfigureTrafficTimeDialog _dialogConfigureTrafficTime;
  private SingleBarMonitorDialog _dialogSingleBarMonitor;
  
  /**
   * �R���X�g���N�^
   * @param eventListener �C�x���g�����̖{�̕�����S������
   * @param component1 3D�\���p�̃R���|�[�l���g
   * @param component2 2D�\���p�̃R���|�[�l���g
   */
  public View2D(View2DEventListener eventListener, JComponent component1, JComponent component2) {
    super();
    addWindowListener(this);
    _rm = ResourceManager.getInstance();
    _eventListener = eventListener;
    _mainComponent1 = component1;
    _mainComponent2 = component2;
    //_mainComponent.setToolTipText(_rm.getString("tooltip.view3D"));
    
    _setCommandsOfButton = new HashSet<String>();
    _setCommandsOfSelectableButton = new HashSet<String>();
    
    // �^�C�g��
    _productName = _rm.getString(Common.RESOURCE_KEY_PRODUCT_NAME) + " " + _rm.getString(Common.RESOURCE_KEY_PRODUCT_VERSION);
    super.setTitle(_productName);
    
    setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
    buildUI();
    
    //�͂��߂���ԗ��ԍ����_�C�A���O�ɏo����悤�ɁA�����Ń_�C�A���O���쐬���Ă���
    _dialogConfigureView = new ConfigureViewDialog(this, _eventListener);
    
    setSize(new Dimension(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT));
    setLocationRelativeTo(null);
  }
  
  /**
   * ���j���[��c�[���o�[�̍쐬�A��ʔz�u���s���܂��B
   */
  private void buildUI() {
    try {
      UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
    } catch (Exception e) {
      e.printStackTrace();
    }
    { //�t�H���g�̐ݒ�
      JLabel tmpLabel = new JLabel();
      Font font = tmpLabel.getFont();
      _eventListener.setFont(font);
    }
    
    ImageLoader imageLoader = new ImageLoader("advkun.png");
    setIconImage(imageLoader.getImage(0));
    
    JMenuBar menubar = new JMenuBar();
    setJMenuBar(menubar);
    
    //@formatter:off
    // ################################################################################
    // ���j���[[�t�@�C��]
    // ################################################################################
    JMenu     menuFile              = getMenu(             COMMAND_FILE,                          "F");
    JMenuItem miFileLoad            = getMenuItem(         COMMAND_FILE_LOAD,                     "L");
    JMenuItem miFileConvert         = getMenuItem(         COMMAND_FILE_CONVERT,                  "C");
    JMenuItem miExit                = getMenuItem(         COMMAND_FILE_EXIT,                     "X");
    // ################################################################################
    // ���j���[[�\��]
    // ################################################################################
    JMenu     menuView              = getMenu(             COMMAND_VIEW,                          "V");
    JMenuItem miConfigView          = getMenuItem(         COMMAND_VIEW_CONFIGURE_VIEW,           "V");
    JMenuItem miConfigTrafficTime   = getMenuItem(         COMMAND_VIEW_CONFIGURE_TRAFFIC_TIME,   "T");
    // ################################################################################
    // ���j���[[�c�[��]
    // ################################################################################
    JMenu     menuTool              = getMenu(             COMMAND_TOOL,                          "T");
    JMenuItem miSaveScreenShot      = getMenuItem(         COMMAND_TOOL_SAVE_SCREEN_SHOT,         "S");
    // ################################################################################
    // ���j���[[�w���v]
    // ################################################################################
    JMenu     menuHelp              = getMenu(             COMMAND_HELP,                          "H");
    JMenuItem miHelpAbout           = getMenuItem(         COMMAND_HELP_ABOUT,                    "A");
    //@formatter:on
    
    _lbStatus = new JTextField[NUM_STATUS_LABEL];
    for (int i = 0; i < NUM_STATUS_LABEL; i++) {
      _lbStatus[i] = new JTextField(Common.SPACE);
      _lbStatus[i].setEditable(false);
    }
    _lbStatus[NUM_STATUS_LABEL - 1].setHorizontalAlignment(JTextField.RIGHT);
    
    // ################################################################################
    // ���j���[[�t�@�C��]
    // ################################################################################
    menubar.add(menuFile);
    menuFile.add(miFileLoad);
    menuFile.addSeparator();
    menuFile.add(miFileConvert);
    menuFile.addSeparator();
    menuFile.add(miExit);
    
    // ################################################################################
    // ���j���[[�\��]
    // ################################################################################
    menubar.add(menuView);
    menuView.add(miConfigView);
    menuView.add(miConfigTrafficTime);
    
    // ################################################################################
    // ���j���[[�c�[��]
    // ################################################################################
    menubar.add(menuTool);
    menuTool.add(miSaveScreenShot);
    
    // ################################################################################
    // ���j���[[�w���v]
    // ################################################################################
    menubar.add(menuHelp);
    //menuHelp.add(miHelpAbout);
    
    JPanel plStatus = new JPanel();
    plStatus.setLayout(new GridLayout(1, NUM_STATUS_LABEL));
    for (int i = 0; i < NUM_STATUS_LABEL; i++) {
      plStatus.add(_lbStatus[i]);
    }
    
    _plMain = new JPanel();
    _mainCardLayout = new CardLayout();
    _plMain.setLayout(_mainCardLayout);
    _plMain.add(_mainComponent1, PANEL_NAME_3D);
    _plMain.add(_mainComponent2, PANEL_NAME_2D);
    showMainComponent(PANEL_NAME_3D);
    
    Container cp = getContentPane();
    cp.setLayout(new BorderLayout());
    cp.add(_plMain, BorderLayout.CENTER);
    cp.add(plStatus, BorderLayout.SOUTH);
    //cp.add(toolBar, BorderLayout.NORTH);
  }
  
  /**
   * JMenu���쐬���ĕԂ�
   * @param command
   * @return
   */
  private JMenu getMenu(String command, String mnemonic) {
    //JMenu tmpMenu = new JMenu(_rm.getString(command));
    JMenu tmpMenu = new JMenu();
    setMnemonic(tmpMenu, _rm.getString(command), mnemonic);
    return tmpMenu;
  }
  
  /**
   * JMenuItem���쐬���A���X�i�[��ǉ����Ă���Ԃ�
   * @param command
   * @return
   */
  private JMenuItem getMenuItem(String command, String mnemonic) {
    //JMenuItem tmpMenuItem = new JMenuItem(_rm.getString(command));
    JMenuItem tmpMenuItem = new JMenuItem();
    setMnemonic(tmpMenuItem, _rm.getString(command), mnemonic);
    addActionListenerForButton(tmpMenuItem, command);
    return tmpMenuItem;
  }
  
  private JFileChooser getFileChooser() {
    if (_fileChooser == null) {
      _fileChooser = new JFileChooser(Common.USER_DIR);
    }
    return _fileChooser;
  }
  
  /**
   * AbstractButton�ɁA���̃{�^���ɑ΂���A�N�V������\���������^���A���X�i�[��ǉ�����B
   * @param button
   * @param actionCommand
   */
  private void addActionListenerForButton(AbstractButton button, String actionCommand) {
    button.addActionListener(this);
    button.setActionCommand(actionCommand);
    _setCommandsOfButton.add(actionCommand);
  }
  
  private void setMnemonic(AbstractButton button, String label, String mnemonic) {
    String labelMnemonic = null;
    int keyMnemonic = -1;
    if (mnemonic != null) {
      for (int i = 0; i < LIST_ALPHABETS.length; i++) {
        if (mnemonic.equalsIgnoreCase(LIST_ALPHABETS[i])) {
          labelMnemonic = LIST_ALPHABETS[i];
          keyMnemonic = LIST_KEY_CODES[i];
        }
      }
    }
    if (labelMnemonic == null) {
      labelMnemonic = "";
    } else {
      labelMnemonic = "(" + labelMnemonic + ")";
      button.setMnemonic(keyMnemonic);
    }
    button.setText(label + labelMnemonic);
  }
  
  public void actionPerformed(ActionEvent e) {
    AbstractButton source = (AbstractButton) (e.getSource());
    String command = e.getActionCommand();
    if (_setCommandsOfButton.contains(command)) {
      _eventListener.action(command);
    } else if (_setCommandsOfSelectableButton.contains(command)) {
      boolean state = source.isSelected();
      _eventListener.action(command, state);
    }
  }
  
  public void windowActivated(WindowEvent e) {
  }
  
  public void windowClosed(WindowEvent e) {
  }
  
  public void windowClosing(WindowEvent e) {
    _eventListener.action(COMMAND_FILE_EXIT);
  }
  
  public void windowDeactivated(WindowEvent e) {
  }
  
  public void windowDeiconified(WindowEvent e) {
  }
  
  public void windowIconified(WindowEvent e) {
  }
  
  public void windowOpened(WindowEvent e) {
  }
  
  /**
   * �R���|�[�l���g�̓��e���摜�ɕۑ�����
   * @param filePath �ۑ���t�@�C���p�X
   * @param component �R���|�[�l���g
   * @param imageType �摜�̎��
   * @return ����ɕۑ��ł����ꍇ��true��Ԃ�
   */
  public static boolean saveComponentToImageFile(String filePath, Component component, String imageType) {
    boolean isSuccess = false;
    if (filePath != null && component != null) {
      File outputFile = new File(filePath);
      File dir = outputFile.getParentFile();
      dir.mkdirs();
      BufferedImage bufferedImage = null;
      {
        bufferedImage = new BufferedImage(component.getWidth(), component.getHeight(), BufferedImage.TYPE_INT_RGB);
        component.paintAll(bufferedImage.getGraphics());
      }
      
      try {
        ImageIO.write(bufferedImage, imageType, outputFile);
        isSuccess = true;
      } catch (IOException e) {
        e.printStackTrace();
      }
    }
    return isSuccess;
  }
  
  public void setEnabled(boolean isEnabled) {
    super.setEnabled(isEnabled);
    setEnabled(this, isEnabled);
  }
  
  /**
   * Exit
   */
  public void exit() {
    if (_dialogConfigureTrafficTime != null) {
      _dialogConfigureTrafficTime.stopThread();
    }
    dispose();
  }
  
  /**
   * Component�̗L���E�������Z�b�g����B
   * Container�̏ꍇ�́A���̒���Component�ɂ��ċA�I�ɓK�p����B
   * �������A�������[�v������邽��View2D�͓��ʈ����Ƃ���B
   * @param component
   * @param isEnabled
   */
  private static void setEnabled(Component component, boolean isEnabled) {
    if ((component instanceof View2D) == false) {
      component.setEnabled(isEnabled);
    }
    if (component instanceof Container) {
      Component[] subComponents = ((Container) component).getComponents();
      for (int i = 0; i < subComponents.length; i++) {
        setEnabled(subComponents[i], isEnabled);
      }
    }
  }
  
  public void setTitle(String title) {
    super.setTitle(_productName + " - " + title);
  }
  
  public void setWindowSize(int width, int height) {
    super.setSize(width, height);
  }
  
  public void setStatusText(int index, String str) {
    if (index >= 0 && index < NUM_STATUS_LABEL) {
      if (str != null && str.length() != 0) {
        _lbStatus[index].setText(str);
      } else {
        _lbStatus[index].setText(Common.SPACE);
      }
      if (index == StatusLabelListener.LABEL_INDEX_PICK_INFO) {
        if (_dialogConfigureView != null) {
          _dialogConfigureView.setVehicleIDList(str);
        }
      }
    }
  }
  
  public void hideDialogs() {
    hideDialog(_dialogConfigureView);
    hideDialog(_dialogConfigureTrafficTime);
    hideDialog(_dialogSingleBarMonitor);
  }
  
  private void hideDialog(JDialog dialog) {
    if (dialog != null) {
      dialog.setVisible(false);
    }
  }
  
  public void showMainComponent(String name) {
    _mainCardLayout.show(_plMain, name);
    if (name != null) {
      //�L�[�C�x���g�ɔ����ł���悤�ɁA�R���|�[�l���g�Ƀt�H�[�J�X�𓖂Ă�B
      if (name.equals(PANEL_NAME_3D) && _mainComponent1 != null) {
        _mainComponent1.requestFocusInWindow();
      }
      if (name.equals(PANEL_NAME_2D) && _mainComponent2 != null) {
        _mainComponent2.requestFocusInWindow();
      }
    }
  }
  
  /**
   * ���b�Z�[�W��\������
   * @param message ���b�Z�[�W�̕�����
   * @param title �^�C�g���̕�����
   */
  public void showInfoMessageDialog(String message, String title) {
    JOptionPane.showMessageDialog(this, message, title, JOptionPane.INFORMATION_MESSAGE);
  }
  
  /**
   * �G���[���b�Z�[�W��\������
   * @param message ���b�Z�[�W�̕�����
   * @param title �^�C�g���̕�����
   */
  public void showErrorMessageDialog(String message, String title) {
    JOptionPane.showMessageDialog(this, message, title, JOptionPane.ERROR_MESSAGE);
  }
  
  /**
   * �x�����b�Z�[�W��\������
   * @param message ���b�Z�[�W�̕�����
   * @param title �^�C�g���̕�����
   */
  public void showWarningMessageDialog(String message, String title) {
    JOptionPane.showMessageDialog(this, message, title, JOptionPane.WARNING_MESSAGE);
  }
  
  /**
   * �m�F�_�C�A���O���J��
   * @param message ���b�Z�[�W�̕�����
   * @param title �^�C�g���̕�����
   * @return ���[�U�[�����b�Z�[�W�̓��e�𗹏������ꍇ��true��Ԃ�
   */
  public boolean showConfirmationDialog(String message, String title) {
    int result = JOptionPane.showConfirmDialog(this, message, title, JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE);
    return result == JOptionPane.YES_OPTION;
  }
  
  /**
   * �P�ƃt�@�C���p�̑I���_�C�A���O���J��
   * @param dir �f�B���N�g��
   * @param filters �t�@�C���t�B���^
   * @return �I�������t�@�C��
   */
  public File showSelectSingleFileDialog(File dir, ExtensionFileFilter[] filters) {
    File file = null;
    
    JFileChooser fileChooser = getFileChooser();
    fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
    
    if (dir != null && dir.isDirectory()) {
      fileChooser.setCurrentDirectory(dir);
    }
    fileChooser.setMultiSelectionEnabled(false);
    
    fileChooser.resetChoosableFileFilters();
    if (filters != null) {
      for (int i = 0; i < filters.length; i++) {
        fileChooser.addChoosableFileFilter(filters[i]);
      }
      fileChooser.setFileFilter(filters[0]);
    }
    int intent = fileChooser.showOpenDialog(this);
    if (intent == JFileChooser.APPROVE_OPTION) {
      file = fileChooser.getSelectedFile();
    }
    
    return file;
  }
  
  /**
   * �f�B���N�g���I���_�C�A���O���J��
   * @param dir �f�B���N�g��
   * @param filters �t�@�C���t�B���^
   * @return �I�������f�B���N�g��
   */
  public File showSelectDirectoryDialog(File dir, ExtensionFileFilter[] filters) {
    File selectedDir = null;
    
    JFileChooser fileChooser = getFileChooser();
    fileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
    
    if (dir != null && dir.isDirectory()) {
      fileChooser.setCurrentDirectory(dir);
    }
    fileChooser.setMultiSelectionEnabled(false);
    
    fileChooser.resetChoosableFileFilters();
    if (filters != null) {
      for (int i = 0; i < filters.length; i++) {
        fileChooser.addChoosableFileFilter(filters[i]);
      }
    }
    int intent = fileChooser.showOpenDialog(this);
    if (intent == JFileChooser.APPROVE_OPTION) {
      selectedDir = fileChooser.getSelectedFile();
    }
    
    return selectedDir;
  }
  
  /**
   * �t�@�C���ۑ��_�C�A���O���J��
   * @param dir �f�B���N�g��
   * @param filters �t�@�C���t�B���^
   * @return �I�������t�@�C��
   */
  public File showSaveFileDialog(File dir, ExtensionFileFilter[] filters) {
    File file = null;
    
    JFileChooser fileChooser = getFileChooser();
    fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
    
    if (dir != null && dir.isDirectory()) {
      fileChooser.setCurrentDirectory(dir);
    }
    fileChooser.setMultiSelectionEnabled(false);
    
    fileChooser.resetChoosableFileFilters();
    if (filters != null && filters.length > 0) {
      for (int i = 0; i < filters.length; i++) {
        fileChooser.addChoosableFileFilter(filters[i]);
      }
      fileChooser.setFileFilter(filters[0]);
    }
    
    while (true) {
      file = null;
      int intent = fileChooser.showSaveDialog(this);
      if (intent == JFileChooser.APPROVE_OPTION) {
        file = fileChooser.getSelectedFile();
        //�g���q���Ȃ��ꍇ�͕₤
        FileFilter filter = fileChooser.getFileFilter();
        if (filters != null && filter != null) {
          for (int i = 0; i < filters.length; i++) {
            if (filter == filters[i]) {
              String path = file.getAbsolutePath();
              String extension = filters[i].getExtension();
              if (extension != null) {
                if (path.toUpperCase().endsWith(extension.toUpperCase()) == false) {
                  file = new File(path + Common.DOT + extension);
                }
              }
            }
          }
        }
        //�ۑ���Ƀt�@�C�������݂���ꍇ�͏㏑���m�F
        if (file.exists()) {
          String message = _rm.getString("dialoge.confirm.fileOverwrite.message");
          String title = _rm.getString("dialoge.confirm.fileOverwrite.title");
          boolean toOverwrite = showConfirmationDialog(message, title);
          if (toOverwrite == false) {
            continue;
          }
        }
      }
      break;
    }
    
    return file;
  }
  
  public void showConfigureViewDialog(ViewConfiguration viewConfiguration) {
    if (_dialogConfigureView == null) {
      _dialogConfigureView = new ConfigureViewDialog(this, _eventListener);
    }
    _dialogConfigureView.init(viewConfiguration);
    _dialogConfigureView.setLocationRelativeTo(this);
    _dialogConfigureView.setVisible(true);
  }
  
  public void showConfigureTrafficTimeDialog(int timeStepMin, int timeStepMax, int currentTimeStep) {
    if (_dialogConfigureTrafficTime == null) {
      _dialogConfigureTrafficTime = new ConfigureTrafficTimeDialog(this, _eventListener);
    }
    _dialogConfigureTrafficTime.init(timeStepMin, timeStepMax, currentTimeStep);
    _dialogConfigureTrafficTime.setLocationRelativeTo(this);
    _dialogConfigureTrafficTime.setVisible(true);
  }
  
  /**
   * �i���_�C�A���O��\������
   * @return �i���_�C�A���O
   */
  public SingleBarMonitorDialog showSingleBarMonitorDialog() {
    if (_dialogSingleBarMonitor == null) {
      _dialogSingleBarMonitor = new SingleBarMonitorDialog(this);
    }
    _dialogSingleBarMonitor.init();
    _dialogSingleBarMonitor.setLocationRelativeTo(this);
    return _dialogSingleBarMonitor;
  }
  
  public void setIndexOfActive3DPanel(int index, int[] modeViewPointControl, int[] vehicleIDForViewPoint) {
    if (_dialogConfigureView != null) {
      _dialogConfigureView.setIndexOfActive3DPanel(index, modeViewPointControl, vehicleIDForViewPoint);
    }
  }
}