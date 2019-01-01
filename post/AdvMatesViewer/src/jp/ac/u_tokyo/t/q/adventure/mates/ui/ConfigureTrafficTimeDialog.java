package jp.ac.u_tokyo.t.q.adventure.mates.ui;

import java.awt.Container;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.InputStream;
import java.util.Stack;
import java.util.logging.Logger;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JTextField;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import jp.ac.u_tokyo.t.q.adventure.mates.control.View2DEventListener;
import jp.ac.u_tokyo.t.q.adventure.mates.resource.ResourceManager;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;
import jp.ac.u_tokyo.t.q.adventure.mates.util.ExtensionFileFilter;
import net.miginfocom.swing.MigLayout;

public class ConfigureTrafficTimeDialog extends JDialog implements ActionListener, ChangeListener, Runnable {
  //@formatter:off
  private static final String   LABEL_PRE                    = "dialog.configureTrafficTime.";
  
  private static final int DEFAULT_TRAFFIC_TIME_MIN          = 0;
  private static final int DEFAULT_TRAFFIC_TIME_MAX          = 10;
  private static final int DEFAULT_TRAFFIC_TIME_INITIAL      = 0;
  
  private static final int DEFAULT_TRAFFIC_TIME_INCREMENT    = 1;
  private static final int DEFAULT_PLAY_INTERVAL             = 1;
  
  private static final int TIME_INCREMENT_NEXT_1             = 1;
  private static final int TIME_INCREMENT_NEXT_2             = 10;
  private static final int TIME_INCREMENT_NEXT_3             = 100;
  private static final int TIME_INCREMENT_PREVIOUS_1         = -1;
  private static final int TIME_INCREMENT_PREVIOUS_2         = -10;
  private static final int TIME_INCREMENT_PREVIOUS_3         = -100;
  
  private static final int SLEEP_TIME_FOR_IDLING             = 1;
  private static final int SLEEP_TIME_FOR_PLAYING            = 0;
  
  private static final String DEFAULT_OPTION_INPUT           = "-r 10";
  private static final String DEFAULT_OPTION_OUTPUT          = "-y -qscale 0";
  private static final String SAVE_IMAGE_PREFIX              = "Image_";
  private static final String SAVE_IMAGE_EXTENSION           = ".png";
  private static final String SAVE_IMAGE_TYPE                = Common.IMAGE_TYPE_PNG;
  private static final int LENGTH_OF_SAVE_IMAGE_FILE_COUNT   = 10;
  //@formatter:on
  
  /**
   * ログハンドリングクラス
   */
  private Logger _logger;
  /**
   * リソースマネージャ
   */
  private ResourceManager _rm;
  /**
   * GUI関連のイベントを処理するクラス。
   * MainControlクラスで実装している。
   */
  private View2DEventListener _eventListener;
  
  private Thread _thread;
  
  private boolean _isThreadEnabled;
  private boolean _isPlayEnabled;
  
  private boolean _toSaveImage;
  private boolean _toSaveMovie;
  
  private int _stepMin;
  private int _stepMax;
  private int _stepRangeMin;
  private int _stepRangeMax;
  private int _currentTimeStep;
  /**
   * 動画作成用の画像は、連番で1から始まる必要があるため、
   * 時間ステップの番号とは別の変数を用意しておく。
   */
  private int _currentTimeStepForMovie;
  
  private JTextField _tfTimeStep;
  
  private JLabel _lbTimeStepMin;
  private JLabel _lbTimeStepMax;
  private JTextField _tfTimeStepRangeMin;
  private JTextField _tfTimeStepRangeMax;
  private JSlider _slTrafficTime;
  private JButton _btNext1;
  private JButton _btNext2;
  private JButton _btNext3;
  private JButton _btPrevious1;
  private JButton _btPrevious2;
  private JButton _btPrevious3;
  
  /**
   * スライダーに値をセットするときに、イベントの発生を回避するためのフラグである。
   */
  private boolean _isTrafficTimeSliderEnabled;
  
  private JButton _btPlayAndPause;
  private String _strPlay;
  private String _strPause;
  private int _playTrafficTimeIncrement;
  private long _playCurrentTime;
  private int _playInterval;
  private JTextField _tfTrafficTimeIncrement;
  private JTextField _tfPlayInterval;
  private JButton _btChangeTrafficTimeIncrement;
  private JButton _btChangePlayInterval;
  
  private JCheckBox _cbSaveImage;
  private JTextField _tfSaveImageDirectoryPath;
  private JButton _btSelectSaveImageDirectoryPath;
  
  private JCheckBox _cbSaveMovie;
  private JTextField _tfSaveMoviePath;
  private JButton _btSelectSaveMoviePath;
  
  private JButton _btClose;
  
  public ConfigureTrafficTimeDialog(JFrame parent, View2DEventListener eventListener) {
    super(parent, false);
    _logger = Logger.getLogger(ConfigureTrafficTimeDialog.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    _rm = ResourceManager.getInstance();
    _eventListener = eventListener;
    
    _toSaveImage = false;
    _toSaveMovie = false;
    
    setDefaultCloseOperation(JDialog.HIDE_ON_CLOSE);
    
    buildUI();
    
    initThread();
  }
  
  private void buildUI() {
    String title = _rm.getString(LABEL_PRE + "title");
    setTitle(title);
    
    //手動で時刻ステップを変更するためのコンポーネント
    _lbTimeStepMin = new JLabel();
    _lbTimeStepMax = new JLabel();
    _tfTimeStepRangeMin = new JTextField();
    _tfTimeStepRangeMax = new JTextField();
    _isTrafficTimeSliderEnabled = true;
    _slTrafficTime = new JSlider();
    _slTrafficTime.setMinimum(DEFAULT_TRAFFIC_TIME_MIN);
    _slTrafficTime.setMaximum(DEFAULT_TRAFFIC_TIME_MAX);
    
    _lbTimeStepMin.setText(Integer.toString(DEFAULT_TRAFFIC_TIME_MIN));
    _lbTimeStepMax.setText(Integer.toString(DEFAULT_TRAFFIC_TIME_MAX));
    _tfTimeStepRangeMin.setText(Integer.toString(DEFAULT_TRAFFIC_TIME_MIN));
    _tfTimeStepRangeMax.setText(Integer.toString(DEFAULT_TRAFFIC_TIME_MAX));
    _tfTimeStep = new JTextField();
    _btPrevious3 = new JButton("<<<");
    _btPrevious2 = new JButton(" <<");
    _btPrevious1 = new JButton(" < ");
    _btNext1 = new JButton(" > ");
    _btNext2 = new JButton(">> ");
    _btNext3 = new JButton(">>>");
    _btPrevious1.setMargin(new Insets(0, 0, 0, 0));
    _btPrevious2.setMargin(new Insets(0, 0, 0, 0));
    _btPrevious3.setMargin(new Insets(0, 0, 0, 0));
    _btNext1.setMargin(new Insets(0, 0, 0, 0));
    _btNext2.setMargin(new Insets(0, 0, 0, 0));
    _btNext3.setMargin(new Insets(0, 0, 0, 0));
    
    //一定間隔で自動再生するためのコンポーネント
    _strPlay = _rm.getString("button.play");
    _strPause = _rm.getString("button.pause");
    _btPlayAndPause = new JButton(_strPlay);
    _tfTrafficTimeIncrement = new JTextField(Integer.toString(DEFAULT_TRAFFIC_TIME_INCREMENT));
    _tfPlayInterval = new JTextField(Integer.toString(DEFAULT_PLAY_INTERVAL));
    _btChangeTrafficTimeIncrement = new JButton(_rm.getString(LABEL_PRE + "button.changeTrafficTimeIncrement"));
    _btChangePlayInterval = new JButton(_rm.getString(LABEL_PRE + "button.changePlayInterval"));
    
    //描画内容を画像に保存する
    _cbSaveImage = new JCheckBox(_rm.getString(LABEL_PRE + "saveImage"));
    _tfSaveImageDirectoryPath = new JTextField();
    _tfSaveImageDirectoryPath.setEditable(false);
    _btSelectSaveImageDirectoryPath = new JButton(_rm.getString("button.select"));
    
    //描画内容を動画として保存する
    _cbSaveMovie = new JCheckBox(_rm.getString(LABEL_PRE + "saveMovie"));
    _tfSaveMoviePath = new JTextField();
    _tfSaveMoviePath.setEditable(false);
    _btSelectSaveMoviePath = new JButton(_rm.getString("button.select"));
    
    _btClose = new JButton(_rm.getString("button.close"));
    
    //#######################
    //ここからレイアウト
    String strLayoutSetting1 = "wrap 1";
    String strLayoutSetting2 = "[center, fill, grow]";
    
    //#####
    //ステップ番号の範囲のパネル
    JPanel plRange = new JPanel();
    plRange.setLayout(new MigLayout(strLayoutSetting1, strLayoutSetting2));
    String titlePanelRange = _rm.getString(LABEL_PRE + "setRange");
    plRange.setBorder(new TitledBorder(new EtchedBorder(), titlePanelRange));
    
    JPanel plRangeSub1 = new JPanel();
    plRangeSub1.setBorder(null);
    plRangeSub1.setLayout(new MigLayout("wrap 4", "[][fill, grow][][fill, grow]"));
    plRangeSub1.add(new JLabel(_rm.getString(LABEL_PRE + "stepMin")), "");
    plRangeSub1.add(_lbTimeStepMin, "grow");
    plRangeSub1.add(new JLabel(_rm.getString(LABEL_PRE + "stepMax")), "");
    plRangeSub1.add(_lbTimeStepMax, "grow");
    
    JPanel plRangeSub2 = new JPanel();
    plRangeSub2.setBorder(null);
    plRangeSub2.setLayout(new MigLayout("wrap 9", "fill, grow"));
    plRangeSub2.add(new JLabel(_rm.getString(LABEL_PRE + "rangeMin")), "");
    plRangeSub2.add(new JLabel(_rm.getString(LABEL_PRE + "previous"), JLabel.CENTER), "span 3");
    plRangeSub2.add(new JLabel(""), "");
    plRangeSub2.add(new JLabel(_rm.getString(LABEL_PRE + "next"), JLabel.CENTER), "span 3");
    plRangeSub2.add(new JLabel(_rm.getString(LABEL_PRE + "rangeMax")), "");
    plRangeSub2.add(_tfTimeStepRangeMin, "grow");
    plRangeSub2.add(_btPrevious3);
    plRangeSub2.add(_btPrevious2);
    plRangeSub2.add(_btPrevious1);
    plRangeSub2.add(_slTrafficTime);
    plRangeSub2.add(_btNext1);
    plRangeSub2.add(_btNext2);
    plRangeSub2.add(_btNext3);
    plRangeSub2.add(_tfTimeStepRangeMax, "grow");
    
    JPanel plRangeSub3 = new JPanel();
    plRangeSub3.setBorder(null);
    plRangeSub3.setLayout(new MigLayout("wrap 2", "[][fill, grow]"));
    plRangeSub3.add(new JLabel(_rm.getString(LABEL_PRE + "current")), "");
    plRangeSub3.add(_tfTimeStep, "");
    
    plRange.add(plRangeSub1, "grow");
    plRange.add(plRangeSub2, "grow");
    plRange.add(plRangeSub3, "grow");
    
    //#####
    //再生関係のパネル
    JPanel plPlay = new JPanel();
    plPlay.setLayout(new MigLayout("wrap 3", "[][fill, grow][]"));
    String titlePanelPlay = _rm.getString(LABEL_PRE + "playConfiguration");
    plPlay.setBorder(new TitledBorder(new EtchedBorder(), titlePanelPlay));
    
    plPlay.add(_btPlayAndPause, "span 3, grow");
    plPlay.add(new JLabel(_rm.getString(LABEL_PRE + "trafficTimeIncrement")), "");
    plPlay.add(_tfTrafficTimeIncrement, "");
    plPlay.add(_btChangeTrafficTimeIncrement, "");
    plPlay.add(new JLabel(_rm.getString(LABEL_PRE + "playInterval")), "");
    plPlay.add(_tfPlayInterval, "");
    plPlay.add(_btChangePlayInterval, "");
    
    plPlay.add(new JLabel(" "), "span 3");
    
    plPlay.add(_cbSaveImage, "span 3");
    plPlay.add(new JLabel(_rm.getString(LABEL_PRE + "saveImage.message"), JLabel.RIGHT), "");
    plPlay.add(_tfSaveImageDirectoryPath, "");
    plPlay.add(_btSelectSaveImageDirectoryPath, "");
    
    plPlay.add(_cbSaveMovie, "span 3");
    plPlay.add(new JLabel(_rm.getString(LABEL_PRE + "saveMovie.message"), JLabel.RIGHT), "");
    plPlay.add(_tfSaveMoviePath, "");
    plPlay.add(_btSelectSaveMoviePath, "");
    
    Container cp = getContentPane();
    cp.setLayout(new MigLayout(strLayoutSetting1, strLayoutSetting2));
    cp.add(plRange);
    cp.add(plPlay);
    
    cp.add(_btClose);
    
    init(DEFAULT_TRAFFIC_TIME_MIN, DEFAULT_TRAFFIC_TIME_MAX, DEFAULT_TRAFFIC_TIME_INITIAL);
    addListeners();
    pack();
  }
  
  public void init(int timeStepMin, int timeStepMax, int currentTimeStep) {
    _stepMin = timeStepMin;
    _stepMax = timeStepMax;
    _stepRangeMin = _stepMin;
    _stepRangeMax = _stepMax;
    _currentTimeStep = currentTimeStep;
    setValueToTrafficTimeSlideWithoutEvent(_currentTimeStep);
    _lbTimeStepMin.setText(Integer.toString(_stepMin));
    _lbTimeStepMax.setText(Integer.toString(_stepMax));
    applyTimeStepRange();
    setTimeStepToTrafficTimeTextField(_currentTimeStep);
  }
  
  private void applyTimeStepRange() {
    setMinMaxValueToTrafficTimeSlideWithoutEvent(_stepRangeMin, _stepRangeMax);
    _tfTimeStepRangeMin.setText(Integer.toString(_stepRangeMin));
    _tfTimeStepRangeMax.setText(Integer.toString(_stepRangeMax));
  }
  
  private void initThread() {
    _isThreadEnabled = true;
    stopPlaying();
    _thread = new Thread(this);
    _thread.start();
  }
  
  private void addListeners() {
    _tfTimeStep.addActionListener(this);
    _slTrafficTime.addChangeListener(this);
    _btNext1.addActionListener(this);
    _btNext2.addActionListener(this);
    _btNext3.addActionListener(this);
    _btPrevious1.addActionListener(this);
    _btPrevious2.addActionListener(this);
    _btPrevious3.addActionListener(this);
    _btClose.addActionListener(this);
    
    _btPlayAndPause.addActionListener(this);
    _btChangeTrafficTimeIncrement.addActionListener(this);
    _btChangePlayInterval.addActionListener(this);
    _tfTrafficTimeIncrement.addActionListener(this);
    _tfPlayInterval.addActionListener(this);
    
    _btSelectSaveImageDirectoryPath.addActionListener(this);
    _btSelectSaveMoviePath.addActionListener(this);
  }
  
  public void stopThread() {
    _isThreadEnabled = false;
  }
  
  private void startPlaying() {
    _playCurrentTime = System.currentTimeMillis();
    prepareToPlay();
    _isPlayEnabled = true;
    _btPlayAndPause.setText(_strPause);
  }
  
  private void stopPlaying() {
    _isPlayEnabled = false;
    _btPlayAndPause.setText(_strPlay);
  }
  
  private void prepareToPlay() {
    deleteDirectoryContents(Common.PATH_DIR_FOR_IMAGE);
    
    _toSaveImage = false;
    _toSaveMovie = false;
    if (_cbSaveImage.isSelected()) {
      _toSaveImage = true;
    }
    if (_cbSaveMovie.isSelected()) {
      _toSaveMovie = true;
    }
    
    if (_toSaveImage || _toSaveMovie) {
      _currentTimeStep = _stepRangeMin;
      if (_playTrafficTimeIncrement < 0) {
        _currentTimeStep = _stepRangeMax;
      }
      
      _currentTimeStepForMovie = 0;
      setTimeStep(_currentTimeStep, true);
      setValueToTrafficTimeSlideWithoutEvent(_currentTimeStep);
      setTimeStepToTrafficTimeTextField(_currentTimeStep);
    }
  }
  
  /**
   * ディレクトリを中身ごと消去する
   */
  private static boolean deleteDirectoryContents(String dirPath) {
    boolean isSuccess = true;
    File dir = new File(dirPath);
    if (dir.isDirectory() == false) {
    } else {
      Stack<File> stack = new Stack<File>();
      stack.add(dir);
      while (!stack.empty()) {
        File tmpfile = (File) stack.pop();
        if (!tmpfile.delete()) {
          if (tmpfile.isDirectory()) { //ディレトリーは空でないと消せない
            stack.push(tmpfile);
            File[] tmpsubfiles = tmpfile.listFiles();
            for (int i = 0; i < tmpsubfiles.length; i++) {
              stack.push(tmpsubfiles[i]);
            }
          } else {
            isSuccess = false;
            //sbError.append(tmpfile.getAbsolutePath());
            break;
          }
        }
      }
    }
    return isSuccess;
  }
  
  public void actionPerformed(ActionEvent e) {
    Object source = e.getSource();
    if (source == _btClose) {
      setVisible(false);
    } else if (source == _tfTimeStep) {
      setTimeStepByTextField();
    } else if (source == _btNext1) {
      increaseTimeStep(TIME_INCREMENT_NEXT_1);
    } else if (source == _btNext2) {
      increaseTimeStep(TIME_INCREMENT_NEXT_2);
    } else if (source == _btNext3) {
      increaseTimeStep(TIME_INCREMENT_NEXT_3);
    } else if (source == _btPrevious1) {
      increaseTimeStep(TIME_INCREMENT_PREVIOUS_1);
    } else if (source == _btPrevious2) {
      increaseTimeStep(TIME_INCREMENT_PREVIOUS_2);
    } else if (source == _btPrevious3) {
      increaseTimeStep(TIME_INCREMENT_PREVIOUS_3);
    } else if (source == _btPlayAndPause) {
      changePlayState();
    } else if (source == _btChangeTrafficTimeIncrement) {
      changeTrafficTimeIncrement();
    } else if (source == _btChangePlayInterval) {
      changePlayInterval();
    } else if (source == _tfTrafficTimeIncrement) {
      changeTrafficTimeIncrement();
    } else if (source == _tfPlayInterval) {
      changePlayInterval();
    } else if (source == _btSelectSaveImageDirectoryPath) {
      selectSaveImageDirectoryPath();
    } else if (source == _btSelectSaveMoviePath) {
      selectSaveMoviePath();
    }
  }
  
  private boolean checkTimeRange() {
    boolean isOK = true;
    
    int timeStepMin = 0;
    int timeStepMax = 0;
    if (isOK) {
      String strTimeStepMin = _tfTimeStepRangeMin.getText();
      String strTimeStepMax = _tfTimeStepRangeMax.getText();
      try {
        timeStepMin = Integer.parseInt(strTimeStepMin);
        timeStepMax = Integer.parseInt(strTimeStepMax);
      } catch (Exception e) {
        isOK = false;
      }
    }
    
    if (isOK) {
      if (timeStepMin >= timeStepMax) isOK = false;
      if (timeStepMin < _stepMin) isOK = false;
      if (timeStepMax > _stepMax) isOK = false;
    }
    
    if (isOK) {
      _stepRangeMin = timeStepMin;
      _stepRangeMax = timeStepMax;
      applyTimeStepRange();
      if (_currentTimeStep < _stepRangeMin) _currentTimeStep = _stepRangeMin;
      if (_currentTimeStep > _stepRangeMax) _currentTimeStep = _stepRangeMax;
    }
    
    if (isOK == false) {
      String message = _rm.getString(LABEL_PRE + "error.parse.trafficTimeRange.message");
      String title = _rm.getString(LABEL_PRE + "error.parse.trafficTimeRange.title");
      _eventListener.showErrorMessageDialog(message, title);
    }
    
    return isOK;
  }
  
  private void selectSaveImageDirectoryPath() {
    File tmpFile = new File(_tfSaveImageDirectoryPath.getText());
    File tmpDir = tmpFile.getParentFile();
    File file = _eventListener.showSelectDirectoryDialog(tmpDir, null);
    if (file != null) {
      String path = file.getAbsolutePath();
      _tfSaveImageDirectoryPath.setText(path);
    }
  }
  
  private void selectSaveMoviePath() {
    File tmpFile = new File(_tfSaveMoviePath.getText());
    File tmpDir = tmpFile.getParentFile();
    
    String extension = _rm.getString("extension.wmv.name");
    String description = _rm.getString("extension.wmv.description");
    ExtensionFileFilter[] filters = new ExtensionFileFilter[1];
    filters[0] = new ExtensionFileFilter(true);
    filters[0].setExtensions(extension, true);
    filters[0].setDescription(description);
    File file = _eventListener.showSaveFileDialog(tmpDir, filters);
    if (file != null) {
      String path = file.getAbsolutePath();
      _tfSaveMoviePath.setText(path);
    }
  }
  
  public void setVisible(boolean isVisible) {
    if (isVisible) {
      super.setVisible(isVisible);
    } else {
      //プレイ中はダイアログを閉じないようにする
      if (_isPlayEnabled) {
        String message = _rm.getString(LABEL_PRE + "warning.closeWhenPlaying.message");
        String title = _rm.getString(LABEL_PRE + "warning.closeWhenPlaying.title");
        _eventListener.showWarningMessageDialog(message, title);
      } else {
        //stopThread();
        super.setVisible(isVisible);
      }
    }
  }
  
  public void stateChanged(ChangeEvent e) {
    Object source = e.getSource();
    if (source == _slTrafficTime) {
      if (!_slTrafficTime.getValueIsAdjusting() && _isTrafficTimeSliderEnabled) {
        setTimeStepBySlider();
      }
    }
  }
  
  public void run() {
    while (_isThreadEnabled) {
      if (_isPlayEnabled) {
        long currentTime = System.currentTimeMillis();
        if (currentTime >= _playCurrentTime + _playInterval) {
          _playCurrentTime = currentTime;
          int tmpTimeStep = _currentTimeStep + _playTrafficTimeIncrement;
          _currentTimeStepForMovie++;
          boolean flgExceed = false;
          if (tmpTimeStep < _stepRangeMin) {
            tmpTimeStep = _stepRangeMin;
            flgExceed = true;
          }
          if (tmpTimeStep > _stepRangeMax) {
            tmpTimeStep = _stepRangeMax;
            flgExceed = true;
          }
          
          setTimeStep(tmpTimeStep, true);
          setValueToTrafficTimeSlideWithoutEvent(tmpTimeStep);
          setTimeStepToTrafficTimeTextField(tmpTimeStep);
          
          if (flgExceed) {
            stopPlaying();
            if (_toSaveMovie) {
              saveMovie(_tfSaveMoviePath.getText());
            }
          }
        }
        try {
          Thread.sleep(SLEEP_TIME_FOR_PLAYING);
        } catch (Exception e) {
        }
      } else {
        try {
          Thread.sleep(SLEEP_TIME_FOR_IDLING);
        } catch (Exception e) {
        }
      }
    }
    _logger.info("Finish run");
  }
  
  private void changePlayState() {
    if (_isPlayEnabled) {
      stopPlaying();
    } else {
      if (checkTimeRange()) {
        boolean isSuccessIncrement = getTrafficTimeIncrement();
        boolean isSuccessInterval = getPlayInterval();
        
        if (isSuccessIncrement && isSuccessInterval) {
          startPlaying();
        } else {
          StringBuffer sbError = new StringBuffer();
          String title = _rm.getString(LABEL_PRE + "error.parse.play.title");
          if (isSuccessIncrement == false) {
            String message = _rm.getString(LABEL_PRE + "error.parse.trafficTimeIncrement.message");
            if (sbError.length() > 0) sbError.append(Common.EOL);
            sbError.append(message);
          }
          if (isSuccessInterval == false) {
            String message = _rm.getString(LABEL_PRE + "error.parse.playInterval.message");
            if (sbError.length() > 0) sbError.append(Common.EOL);
            sbError.append(message);
          }
          
          _eventListener.showWarningMessageDialog(sbError.toString(), title);
        }
      }
    }
  }
  
  private void changeTrafficTimeIncrement() {
    boolean isSuccess = getTrafficTimeIncrement();
    if (isSuccess == false) {
      String message = _rm.getString(LABEL_PRE + "error.parse.trafficTimeIncrement.message");
      String title = _rm.getString(LABEL_PRE + "error.parse.trafficTimeIncrement.title");
      _eventListener.showWarningMessageDialog(message, title);
    }
  }
  
  private boolean getTrafficTimeIncrement() {
    boolean isSuccess = false;
    try {
      String strIncrement = _tfTrafficTimeIncrement.getText();
      _playTrafficTimeIncrement = Integer.parseInt(strIncrement);
      isSuccess = true;
    } catch (Exception e) {
    }
    return isSuccess;
  }
  
  private void changePlayInterval() {
    boolean isSuccess = getPlayInterval();
    if (isSuccess == false) {
      String message = _rm.getString(LABEL_PRE + "error.parse.playInterval.message");
      String title = _rm.getString(LABEL_PRE + "error.parse.playInterval.title");
      _eventListener.showWarningMessageDialog(message, title);
    }
  }
  
  private boolean getPlayInterval() {
    boolean isSuccess = false;
    try {
      String strInterval = _tfPlayInterval.getText();
      _playInterval = Integer.parseInt(strInterval);
      isSuccess = true;
    } catch (Exception e) {
    }
    return isSuccess;
  }
  
  private void setTimeStepByTextField() {
    if (checkTimeRange()) {
      boolean isSuccessParse = false;
      int timeStep = 0;
      try {
        timeStep = Integer.parseInt(_tfTimeStep.getText());
        isSuccessParse = true;
      } catch (Exception e) {
      }
      
      if (isSuccessParse) {
        if (timeStep < _stepRangeMin) isSuccessParse = false;
        if (timeStep > _stepRangeMax) isSuccessParse = false;
      }
      
      if (isSuccessParse) {
        setValueToTrafficTimeSlideWithoutEvent(timeStep);
        setTimeStep(timeStep, false);
      } else {
        String message = _rm.getString(LABEL_PRE + "error.parse.trafficTime.message");
        String title = _rm.getString(LABEL_PRE + "error.parse.trafficTime.title");
        _eventListener.showErrorMessageDialog(message, title);
      }
    }
  }
  
  private void setTimeStepBySlider() {
    if (checkTimeRange()) {
      int timeStep = _slTrafficTime.getValue();
      setTimeStepToTrafficTimeTextField(timeStep);
      setTimeStep(timeStep, false);
    }
  }
  
  private void increaseTimeStep(int increment) {
    if (checkTimeRange()) {
      int timeStep = _slTrafficTime.getValue();
      int tmpTimeStep = timeStep + increment;
      if (tmpTimeStep < _stepRangeMin) tmpTimeStep = _stepRangeMin;
      if (tmpTimeStep > _stepRangeMax) tmpTimeStep = _stepRangeMax;
      setValueToTrafficTimeSlideWithoutEvent(tmpTimeStep);
      setTimeStepToTrafficTimeTextField(tmpTimeStep);
      setTimeStep(tmpTimeStep, false);
    }
  }
  
  private void setValueToTrafficTimeSlideWithoutEvent(int timeStep) {
    //_slTrafficTime.removeChangeListener(this);
    _isTrafficTimeSliderEnabled = false;
    _slTrafficTime.setValue(timeStep);
    _isTrafficTimeSliderEnabled = true;
    //_slTrafficTime.addChangeListener(this);
  }
  
  private void setMinMaxValueToTrafficTimeSlideWithoutEvent(int timeStepMin, int timeStepMax) {
    //_slTrafficTime.removeChangeListener(this);
    _isTrafficTimeSliderEnabled = false;
    _slTrafficTime.setMinimum(timeStepMin);
    _slTrafficTime.setMaximum(timeStepMax);
    _isTrafficTimeSliderEnabled = true;
    //_slTrafficTime.addChangeListener(this);
  }
  
  private void setTimeStepToTrafficTimeTextField(int timeStep) {
    _tfTimeStep.setText(Integer.toString(timeStep));
  }
  
  private void setTimeStep(int timeStep, boolean toSaveImage) {
    boolean isSuccess = _eventListener.setTrafficTime(timeStep);
    if (isSuccess) {
      if (toSaveImage) {
        if (_toSaveImage) {
          String path = _tfSaveImageDirectoryPath.getText();
          String strTimeStep = formatInteger(timeStep, LENGTH_OF_SAVE_IMAGE_FILE_COUNT);
          path += Common.FILE_SEARATOR + "Image_" + strTimeStep + ".png";
          _eventListener.reserveToSaveScreenImage(path, Common.IMAGE_TYPE_PNG);
        }
        if (_toSaveMovie) {
          String path = Common.PATH_DIR_FOR_IMAGE;
          String strTimeStep = formatInteger(_currentTimeStepForMovie, LENGTH_OF_SAVE_IMAGE_FILE_COUNT);
          path += "Image_" + strTimeStep + ".png";
          _eventListener.reserveToSaveScreenImage(path, Common.IMAGE_TYPE_PNG);
        }
      }
      
      _currentTimeStep = timeStep;
      _eventListener.reDraw();
    } else {
      String message = _rm.getString(LABEL_PRE + "error.setTimeStep.message");
      String title = _rm.getString(LABEL_PRE + "error.setTimeStep.title");
      _eventListener.showErrorMessageDialog(message, title);
    }
  }
  
  private void saveMovie(String saveFilePath) {
    //$ avconv [input option] -i InputFiles [output option] OutputFileName
    //-r         : frame rate
    //-qscale    : specify quality
    //-i         : input files
    //-y         : overwrite output file
    //-pix_fmt   : format of color
    //-b         : bit rate
    //-vcodec    : video codec
    //TODO avconvの実行ファイルのパスを変更可能にする。
    String pathAVCONV = "bin" + System.getProperty("file.separator") + "avconv.exe";
    String optionInput = DEFAULT_OPTION_INPUT;
    String formatInputFiles = SAVE_IMAGE_PREFIX + "%" + LENGTH_OF_SAVE_IMAGE_FILE_COUNT + "d" + SAVE_IMAGE_EXTENSION;
    //String pathInput = "-i " + Common.DOUBLE_QUOTATION + Common.PATH_DIR_FOR_IMAGE + formatInputFiles + Common.DOUBLE_QUOTATION;
    String pathInput = "-i " + Common.PATH_DIR_FOR_IMAGE + formatInputFiles;
    //String optionOutput = "-y -r 10 -pix_fmt yuvj420p -b 1000k -minrate 100k -maxrate 12000k -bufsize 16000k -vcodec mjpeg";
    //String optionOutput = "-y -pix_fmt yuvj420p -vcodec mjpeg";
    //String optionOutput = "-f image2";
    String optionOutput = DEFAULT_OPTION_OUTPUT;
    //String pathOutput = Common.DOUBLE_QUOTATION + saveFilePath + Common.DOUBLE_QUOTATION;
    String pathOutput = saveFilePath;
    
    String command = pathAVCONV;
    command += Common.SPACE + optionInput;
    command += Common.SPACE + pathInput;
    command += Common.SPACE + optionOutput;
    command += Common.SPACE + pathOutput;
    _logger.info("command:" + command);
    
    InputStream is1 = null;
    InputStream is2 = null;
    
    StringBuffer sb = new StringBuffer();
    
    Runtime runTime = Runtime.getRuntime();
    Process process = null;
    try {
      process = runTime.exec(command);
      
      is1 = process.getInputStream();
      is2 = process.getErrorStream();
      
      StringBuffer sb1 = new StringBuffer();
      StringBuffer sb2 = new StringBuffer();
      TempExe exe1 = new TempExe(is1, sb1);
      TempExe exe2 = new TempExe(is2, sb2);
      
      int result = process.waitFor();
      sb.append("Process result : " + result);
      
      while (exe1.isAlive() || exe2.isAlive()) {
        try {
          Thread.sleep(1);
        } catch (Exception e) {
        }
      }
      sb.append(sb1).append(Common.EOL);
      sb.append(sb2).append(Common.EOL);
      
      _logger.info(sb.toString());
    } catch (Exception e) {
      e.printStackTrace();
    }
    if (is1 != null) {
      try {
        is1.close();
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
    if (is2 != null) {
      try {
        is2.close();
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
    _logger.info("Finish saving movie");
  }
  
  private static String formatInteger(int value, int length) {
    String str = Integer.toString(value);
    while (str.length() < length) {
      str = "0" + str;
    }
    return str;
  }
  
  class TempExe implements Runnable {
    Thread _thread;
    InputStream _is;
    StringBuffer _sb;
    boolean _toStop;
    
    public TempExe(InputStream is, StringBuffer sb) {
      _is = is;
      _sb = sb;
      _toStop = false;
      Thread _thThread = new Thread(this);
      _thThread.start();
    }
    
    public void run() {
      try {
        while (true) {
          if (_toStop) break;
          int readInt = _is.read();
          if (readInt == -1) break;
          _sb.append((char) readInt);
        }
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
    
    public boolean isAlive() {
      boolean isAlibe = false;
      if (_thread != null) {
        isAlibe = _thread.isAlive();
      }
      return isAlibe;
    }
    
    public void finish() {
      _toStop = true;
    }
  }
}
