package jp.ac.u_tokyo.t.q.adventure.mates.ui;

import java.awt.Container;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.logging.Logger;

import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;

import jp.ac.u_tokyo.t.q.adventure.mates.control.View2DEventListener;
import jp.ac.u_tokyo.t.q.adventure.mates.resource.ResourceManager;
import jp.ac.u_tokyo.t.q.adventure.mates.ui.render.Drawable3DPanel;
import jp.ac.u_tokyo.t.q.adventure.mates.ui.render.ViewConfiguration;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;
import net.miginfocom.swing.MigLayout;

public class ConfigureViewDialog extends JDialog implements ActionListener, ItemListener {
  //@formatter:off
  private static final String   LABEL_PRE                                     = "dialog.configureView.";
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
  
  private ViewConfiguration _viewConfiguration;
  
  private JCheckBox _cbRoad;
  private JCheckBox _cbRoadLine;
  private JCheckBox _cbRoadSide;
  private JCheckBox _cbSideWalk;
  private JCheckBox _cbIntersectionID;
  private JCheckBox _cbVehicle;
  private JCheckBox _cbVehicleID;
  private JCheckBox _cbSignal;
  private JCheckBox _cbGround;
  private JCheckBox _cbAxis;
  private JCheckBox _cbSimpleView;
  
  private JRadioButton _rbViewPointBirdEye;
  private JRadioButton _rbViewPointWalkThrough;
  private JRadioButton _rbViewPointOnVehicle;
  
  private JTextField _tfVehicleID;
  private JTextArea _taVehicleIDList;
  
  private JComboBox<Integer> _comboboxNumActive;
  private JLabel _lbIndexOfActive3DPanel;
  
  private JTextArea _taViewPointConfiguration;
  private JButton _btGetViewPointConfiguration;
  private JButton _btSetViewPointConfiguration;
  private JButton _btLoadViewPointConfiguration;
  private JButton _btSaveViewPointConfiguration;
  
  private JButton _btClose;
  
  public ConfigureViewDialog(JFrame parent, View2DEventListener eventListener) {
    super(parent, false);
    _logger = Logger.getLogger(ConfigureViewDialog.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    _rm = ResourceManager.getInstance();
    _eventListener = eventListener;
    
    setDefaultCloseOperation(JDialog.HIDE_ON_CLOSE);
    
    buildUI();
  }
  
  private void buildUI() {
    String title = _rm.getString(LABEL_PRE + "title");
    setTitle(title);
    
    //@formatter:off
    _cbRoad            = new JCheckBox(_rm.getString(LABEL_PRE + "viewEnable.road"));
    _cbRoadLine        = new JCheckBox(_rm.getString(LABEL_PRE + "viewEnable.roadLine"));
    _cbRoadSide        = new JCheckBox(_rm.getString(LABEL_PRE + "viewEnable.roadSide"));
    _cbSideWalk        = new JCheckBox(_rm.getString(LABEL_PRE + "viewEnable.sideWalk"));
    _cbIntersectionID  = new JCheckBox(_rm.getString(LABEL_PRE + "viewEnable.intersectionID"));
    _cbVehicle         = new JCheckBox(_rm.getString(LABEL_PRE + "viewEnable.vehicle"));
    _cbVehicleID       = new JCheckBox(_rm.getString(LABEL_PRE + "viewEnable.vehicleID"));
    _cbSignal          = new JCheckBox(_rm.getString(LABEL_PRE + "viewEnable.signal"));
    _cbGround          = new JCheckBox(_rm.getString(LABEL_PRE + "viewEnable.ground"));
    _cbAxis            = new JCheckBox(_rm.getString(LABEL_PRE + "viewEnable.axis"));
    _cbSimpleView      = new JCheckBox(_rm.getString(LABEL_PRE + "viewEnable.simpleView"));
    //@formatter:on
    
    _comboboxNumActive = new JComboBox<Integer>();
    for (int i = 1; i <= Drawable3DPanel.MAX_NUM_PANELS; i++) {
      _comboboxNumActive.addItem(new Integer(i));
    }
    _comboboxNumActive.addItemListener(this);
    
    _lbIndexOfActive3DPanel = new JLabel(Integer.toString(Drawable3DPanel.DEFAULT_INDEX_OF_ACTIVE_PANEL));
    
    _rbViewPointBirdEye = new JRadioButton(_rm.getString(LABEL_PRE + "viewPoint.birdEye"));
    _rbViewPointWalkThrough = new JRadioButton(_rm.getString(LABEL_PRE + "viewPoint.walkThrough"));
    _rbViewPointOnVehicle = new JRadioButton(_rm.getString(LABEL_PRE + "viewPoint.onVehicle"));
    ButtonGroup buttonGroup = new ButtonGroup();
    buttonGroup.add(_rbViewPointBirdEye);
    buttonGroup.add(_rbViewPointWalkThrough);
    buttonGroup.add(_rbViewPointOnVehicle);
    _rbViewPointBirdEye.setSelected(true);
    
    _tfVehicleID = new JTextField();
    _taVehicleIDList = new JTextArea(2, 10);
    _taVehicleIDList.setLineWrap(true);
    JScrollPane spVehicleIDList = new JScrollPane(_taVehicleIDList, JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,
        JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
    JPanel plVehicleIDList = new JPanel();
    plVehicleIDList.setLayout(new GridLayout(1, 1));
    plVehicleIDList.add(spVehicleIDList);
    plVehicleIDList.setBorder(new TitledBorder(new EtchedBorder(), _rm.getString(LABEL_PRE
        + "viewPoint.vehicleIDList.title")));
    
    _taViewPointConfiguration = new JTextArea(2, 10);
    _taViewPointConfiguration.setLineWrap(true);
    _btGetViewPointConfiguration = new JButton(_rm.getString(LABEL_PRE + "viewPointConfiguration.get"));
    _btSetViewPointConfiguration = new JButton(_rm.getString(LABEL_PRE + "viewPointConfiguration.set"));
    _btLoadViewPointConfiguration = new JButton(_rm.getString(LABEL_PRE + "viewPointConfiguration.load"));
    _btSaveViewPointConfiguration = new JButton(_rm.getString(LABEL_PRE + "viewPointConfiguration.save"));
    JScrollPane spViewPointConfiguration = new JScrollPane(_taViewPointConfiguration,
        JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED, JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
    
    _btClose = new JButton(_rm.getString("button.close"));
    
    //###############################
    //表示のON/OFFのパネル
    JPanel plViewEnable = new JPanel();
    plViewEnable.setLayout(new GridLayout(6, 2));
    //plViewEnable.setBorder(new TitledBorder(new EtchedBorder(), _rm.getString(LABEL_PRE + "viewEnable.title")));
    plViewEnable.add(_cbRoad);
    plViewEnable.add(_cbRoadLine);
    plViewEnable.add(_cbRoadSide);
    plViewEnable.add(_cbSideWalk);
    plViewEnable.add(_cbIntersectionID);
    plViewEnable.add(_cbVehicle);
    plViewEnable.add(_cbVehicleID);
    plViewEnable.add(_cbSignal);
    plViewEnable.add(_cbGround);
    plViewEnable.add(_cbAxis);
    plViewEnable.add(_cbSimpleView);
    
    //###############################
    //視点の設定のパネル
    JPanel plViewPoint = new JPanel();
    {
      String str1 = "wrap 3";
      String str2 = "[center,fill,grow]";
      String str3 = "[][][][][][][fill,grow]"; //走行車両IDの欄が可変サイズになるようにする
      plViewPoint.setLayout(new MigLayout(str1, str2, str3));
    }
    //plViewPoint.setBorder(new TitledBorder(new EtchedBorder(), _rm.getString(LABEL_PRE + "viewPoint.title")));
    plViewPoint.add(new JLabel(_rm.getString(LABEL_PRE + "numActive3DPanels")), "span 2");
    plViewPoint.add(_comboboxNumActive);
    plViewPoint.add(new JLabel(_rm.getString(LABEL_PRE + "indexOfActive3DPanel")), "span 2");
    plViewPoint.add(_lbIndexOfActive3DPanel);
    
    plViewPoint.add(_rbViewPointBirdEye, "span 3");
    plViewPoint.add(_rbViewPointWalkThrough, "span 3");
    plViewPoint.add(_rbViewPointOnVehicle, "span 3");
    plViewPoint.add(new JLabel(_rm.getString(LABEL_PRE + "viewPoint.vehicleID")));
    plViewPoint.add(_tfVehicleID, "span 2");
    plViewPoint.add(plVehicleIDList, "span 3");
    
    //###############################
    //視点設定の保存・復元のパネル
    JPanel plViewPointConfiguration = new JPanel();
    {
      String str1 = "wrap 1";
      String str2 = "[center,fill,grow]";
      String str3 = "[fill,grow][][]"; //視点設定の欄が可変サイズになるようにする
      plViewPointConfiguration.setLayout(new MigLayout(str1, str2, str3));
    }
    //plViewPointConfiguration.setBorder(new TitledBorder(new EtchedBorder(), _rm.getString(LABEL_PRE
    //    + "viewPointConfiguration.title")));
    plViewPointConfiguration.add(spViewPointConfiguration);
    plViewPointConfiguration.add(_btGetViewPointConfiguration);
    plViewPointConfiguration.add(_btSetViewPointConfiguration);
    plViewPointConfiguration.add(_btLoadViewPointConfiguration);
    plViewPointConfiguration.add(_btSaveViewPointConfiguration);
    
    JPanel plButton = new JPanel();
    plButton.setLayout(new GridLayout(1, 3));
    plButton.add(new JLabel(""));
    plButton.add(_btClose);
    plButton.add(new JLabel(""));
    
    JTabbedPane tpMain = new JTabbedPane();
    tpMain.add(plViewEnable, _rm.getString(LABEL_PRE + "viewEnable.title"));
    tpMain.add(plViewPoint, _rm.getString(LABEL_PRE + "viewPoint.title"));
    tpMain.add(plViewPointConfiguration, _rm.getString(LABEL_PRE + "viewPointConfiguration.title"));
    
    Container cp = getContentPane();
    {
      String strLayoutSetting1 = "wrap 1";
      String strLayoutSetting2 = "[center,fill,grow]";
      String strLayoutSetting3 = "[fill,grow][]"; //走行車両IDの欄が可変サイズになるようにする
      cp.setLayout(new MigLayout(strLayoutSetting1, strLayoutSetting2, strLayoutSetting3));
    }
    cp.add(tpMain);
    cp.add(plButton);
    
    setTextFieldEnableState();
    addListeners();
    pack();
  }
  
  public void init(ViewConfiguration viewConfiguration) {
    _viewConfiguration = viewConfiguration;
    if (_viewConfiguration != null) {
      //@formatter:off
      _cbRoad          .setSelected(_viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_ROAD));
      _cbRoadLine      .setSelected(_viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_ROAD_LINE));
      _cbRoadSide      .setSelected(_viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_ROAD_SIDE));
      _cbSideWalk      .setSelected(_viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_SIDE_WALK));
      _cbIntersectionID.setSelected(_viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_INTERSECTION_ID));
      _cbVehicle       .setSelected(_viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_VEHICLE));
      _cbVehicleID     .setSelected(_viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_VEHICLE_ID));
      _cbSignal        .setSelected(_viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_SIGNAL));
      _cbGround        .setSelected(_viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_GROUND));
      _cbAxis          .setSelected(_viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_AXIS));
      //@formatter:on
    }
  }
  
  public void setVehicleIDList(String strList) {
    if (strList == null || strList.equals("")) {
      _taVehicleIDList.setText("No vehicles");
    } else {
      _taVehicleIDList.setText(strList);
    }
  }
  
  private void setTextFieldEnableState() {
    if (_rbViewPointBirdEye.isSelected()) {
      _tfVehicleID.setEnabled(false);
    } else if (_rbViewPointWalkThrough.isSelected()) {
      _tfVehicleID.setEnabled(false);
    } else if (_rbViewPointOnVehicle.isSelected()) {
      _tfVehicleID.setEnabled(true);
    }
  }
  
  private void setModeForViewPointControl(boolean showErrorMessage) {
    if (_rbViewPointBirdEye.isSelected()) {
      int mode = Drawable3DPanel.MODE_VIEW_POINT_BIRD_EYE;
      int vehicleID = Drawable3DPanel.DEFAULT_VEHICLE_ID_FOR_VIEW_POINT;
      _eventListener.setModeForViewPointControl(mode, vehicleID);
      _eventListener.reDraw();
    } else if (_rbViewPointWalkThrough.isSelected()) {
      int mode = Drawable3DPanel.MODE_VIEW_POINT_WALK_THROUGH;
      int vehicleID = Drawable3DPanel.DEFAULT_VEHICLE_ID_FOR_VIEW_POINT;
      _eventListener.setModeForViewPointControl(mode, vehicleID);
      _eventListener.reDraw();
    } else if (_rbViewPointOnVehicle.isSelected()) {
      boolean isSuccessParse = false;
      int vehicleID = 0;
      try {
        vehicleID = Integer.parseInt(_tfVehicleID.getText());
        isSuccessParse = true;
      } catch (Exception e) {
      }
      
      if (isSuccessParse) {
        int mode = Drawable3DPanel.MODE_VIEW_POINT_ON_VEHICLE;
        boolean isSuccess = _eventListener.setModeForViewPointControl(mode, vehicleID);
        if (isSuccess) {
          _eventListener.reDraw();
        } else {
          if (showErrorMessage) {
            String message = _rm.getString(LABEL_PRE + "error.setMode.message");
            String title = _rm.getString(LABEL_PRE + "error.setMode.title");
            _eventListener.showErrorMessageDialog(message, title);
          }
        }
      } else {
        if (showErrorMessage) {
          String message = _rm.getString(LABEL_PRE + "error.parse.message");
          String title = _rm.getString(LABEL_PRE + "error.parse.title");
          _eventListener.showErrorMessageDialog(message, title);
        }
      }
    }
  }
  
  private void addListeners() {
    _cbRoad.addActionListener(this);
    _cbRoadLine.addActionListener(this);
    _cbRoadSide.addActionListener(this);
    _cbSideWalk.addActionListener(this);
    _cbIntersectionID.addActionListener(this);
    _cbVehicle.addActionListener(this);
    _cbVehicleID.addActionListener(this);
    _cbSignal.addActionListener(this);
    _cbGround.addActionListener(this);
    _cbAxis.addActionListener(this);
    _cbSimpleView.addActionListener(this);
    
    _rbViewPointBirdEye.addActionListener(this);
    _rbViewPointWalkThrough.addActionListener(this);
    _rbViewPointOnVehicle.addActionListener(this);
    _tfVehicleID.addActionListener(this);
    
    _btGetViewPointConfiguration.addActionListener(this);
    _btSetViewPointConfiguration.addActionListener(this);
    _btLoadViewPointConfiguration.addActionListener(this);
    _btSaveViewPointConfiguration.addActionListener(this);
    
    _btClose.addActionListener(this);
  }
  
  public void actionPerformed(ActionEvent e) {
    Object source = e.getSource();
    if (source == _btClose) {
      setVisible(false);
    } else if (source == _btGetViewPointConfiguration) {
      getViewPointConfigurationOfActive3DPanel();
    } else if (source == _btSetViewPointConfiguration) {
      setViewPointConfigurationOfActive3DPanel();
    } else if (source == _btLoadViewPointConfiguration) {
      loadViewPointConfigurationOfActive3DPanel();
    } else if (source == _btSaveViewPointConfiguration) {
      saveViewPointConfigurationOfActive3DPanel();
    } else if (source == _rbViewPointBirdEye) {
      setTextFieldEnableState();
      setModeForViewPointControl(false);
    } else if (source == _rbViewPointWalkThrough) {
      setTextFieldEnableState();
      setModeForViewPointControl(false);
    } else if (source == _rbViewPointOnVehicle) {
      setTextFieldEnableState();
      setModeForViewPointControl(false);
    } else if (source == _tfVehicleID) {
      setModeForViewPointControl(true);
    } else if (source == _cbRoad || source == _cbRoadLine || source == _cbRoadSide || source == _cbSideWalk
        || source == _cbIntersectionID || source == _cbVehicle || source == _cbVehicleID || source == _cbSignal
        || source == _cbGround || source == _cbAxis) {
      changeViewConfiguration();
    } else if (source == _cbSimpleView) {
      switchMainComponent();
    }
  }
  
  public void itemStateChanged(ItemEvent e) {
    Object source = e.getSource();
    if (source == _comboboxNumActive) {
      Object tmp = _comboboxNumActive.getSelectedItem();
      int nPanels = Drawable3DPanel.MIN_ACTIVE_PANELS;
      try {
        nPanels = ((Integer) (tmp)).intValue();
      } catch (Exception excp) {
        nPanels = Drawable3DPanel.MIN_ACTIVE_PANELS;
      }
      _eventListener.setNumberOfActive3DPanels(nPanels);
    }
  }
  
  private void changeViewConfiguration() {
    if (_viewConfiguration == null) {
      _viewConfiguration = new ViewConfiguration();
    }
    
    //@formatter:off
    _viewConfiguration.setVisible(ViewConfiguration.RENDER_TYPE_ROAD,            _cbRoad.isSelected());
    _viewConfiguration.setVisible(ViewConfiguration.RENDER_TYPE_ROAD_LINE,       _cbRoadLine.isSelected());
    _viewConfiguration.setVisible(ViewConfiguration.RENDER_TYPE_ROAD_SIDE,       _cbRoadSide.isSelected());
    _viewConfiguration.setVisible(ViewConfiguration.RENDER_TYPE_SIDE_WALK,       _cbSideWalk.isSelected());
    _viewConfiguration.setVisible(ViewConfiguration.RENDER_TYPE_INTERSECTION_ID, _cbIntersectionID.isSelected());
    _viewConfiguration.setVisible(ViewConfiguration.RENDER_TYPE_VEHICLE,         _cbVehicle.isSelected());
    _viewConfiguration.setVisible(ViewConfiguration.RENDER_TYPE_VEHICLE_ID,      _cbVehicleID.isSelected());
    _viewConfiguration.setVisible(ViewConfiguration.RENDER_TYPE_SIGNAL,          _cbSignal.isSelected());
    _viewConfiguration.setVisible(ViewConfiguration.RENDER_TYPE_GROUND,          _cbGround.isSelected());
    _viewConfiguration.setVisible(ViewConfiguration.RENDER_TYPE_AXIS,            _cbAxis.isSelected());
    //@formatter:on
    _eventListener.setViewConfiguration(_viewConfiguration);
    _eventListener.reDraw();
  }
  
  private void switchMainComponent() {
    String name = View2D.PANEL_NAME_3D;
    if (_cbSimpleView.isSelected()) {
      name = View2D.PANEL_NAME_2D;
    }
    _eventListener.showMainComponent(name);
  }
  
  public void setIndexOfActive3DPanel(int index, int[] modeViewPointControl, int[] vehicleIDForViewPoint) {
    if (modeViewPointControl != null && vehicleIDForViewPoint != null && index >= 0
        && index < modeViewPointControl.length && index < vehicleIDForViewPoint.length) {
      _lbIndexOfActive3DPanel.setText(Integer.toString(index));
      int tmpMode = modeViewPointControl[index];
      int tmpVehicleID = vehicleIDForViewPoint[index];
      if (tmpMode == Drawable3DPanel.MODE_VIEW_POINT_BIRD_EYE) {
        _rbViewPointBirdEye.setSelected(true);
      } else if (tmpMode == Drawable3DPanel.MODE_VIEW_POINT_WALK_THROUGH) {
        _rbViewPointWalkThrough.setSelected(true);
      } else if (tmpMode == Drawable3DPanel.MODE_VIEW_POINT_ON_VEHICLE) {
        _rbViewPointOnVehicle.setSelected(true);
        _tfVehicleID.setText(Integer.toString(tmpVehicleID));
      }
      
    }
  }
  
  private void getViewPointConfigurationOfActive3DPanel() {
    String[] configuration = _eventListener.getViewPointConfigurationOfActive3DPanel();
    StringBuffer sb = new StringBuffer();
    if (configuration != null && configuration.length > 0) {
      for (int i = 0; i < configuration.length; i++) {
        if (sb.length() > 0) sb.append(Common.EOL);
        sb.append(configuration[i]);
      }
    }
    
    _taViewPointConfiguration.setText(sb.toString());
  }
  
  private void setViewPointConfigurationOfActive3DPanel() {
    String LINE_SEPARATOR = System.getProperty("line.separator");
    String configureRaw = _taViewPointConfiguration.getText();
    if (configureRaw != null && configureRaw.length() > 0) {
      String[] configureRawSplit = configureRaw.split(LINE_SEPARATOR);
      _eventListener.setViewPointConfigurationOfActive3DPanel(configureRawSplit);
    }
  }
  
  private void loadViewPointConfigurationOfActive3DPanel() {
    File file = _eventListener.showSelectSingleFileDialog(null, null);
    if (file != null) {
      try {
        BufferedReader br = new BufferedReader(new FileReader(file));
        StringBuffer sb = new StringBuffer();
        
        while (true) {
          String line = br.readLine();
          if (line == null) break;
          
          sb.append(line).append(Common.EOL);
        }
        
        br.close();
        
        _taViewPointConfiguration.setText(sb.toString());
        setViewPointConfigurationOfActive3DPanel();
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
  }
  
  private void saveViewPointConfigurationOfActive3DPanel() {
    File file = _eventListener.showSaveFileDialog(null, null);
    if (file != null) {
      //まず、操作中の3D画面の視点を_taViewPointConfigurationに設定する。
      getViewPointConfigurationOfActive3DPanel();
      
      try {
        BufferedWriter bw = new BufferedWriter(new FileWriter(file));
        bw.write(_taViewPointConfiguration.getText());
        
        bw.close();
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
  }
}
