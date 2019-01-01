package jp.ac.u_tokyo.t.q.adventure.mates.ui.render;

import java.awt.Color;
import java.awt.Font;
import java.awt.GridLayout;
import java.nio.IntBuffer;
import java.text.DecimalFormat;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.logging.Logger;

import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLCapabilities;
import javax.media.opengl.GLJPanel;
import javax.swing.JPanel;
import javax.swing.border.LineBorder;

import jp.ac.u_tokyo.t.q.adventure.mates.control.StatusLabelListener;
import jp.ac.u_tokyo.t.q.adventure.mates.model.TrafficModel;
import jp.ac.u_tokyo.t.q.adventure.mates.resource.ResourceManager;
import jp.ac.u_tokyo.t.q.adventure.mates.ui.View2D;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * 3D画面に描画するクラス
 */
public class Drawable3DPanel {
  // @formatter:off
  //                                                                  {  red,green, blue,alpha }
  public static final float[] COLOR_WHITE                           = { 1.0f, 1.0f, 1.0f, 1.0f };
  public static final float[] COLOR_RED                             = { 1.0f, 0.0f, 0.0f, 1.0f };
  public static final float[] COLOR_GREEN                           = { 0.0f, 1.0f, 0.0f, 1.0f };
  public static final float[] COLOR_BLUE                            = { 0.0f, 0.0f, 1.0f, 1.0f };
  public static final float[] COLOR_LIGHT_BLUE                      = { 0.0f, 1.0f, 1.0f, 1.0f };
  public static final float[] COLOR_YELLOW                          = { 1.0f, 1.0f, 0.0f, 1.0f };
  public static final float[] COLOR_GRAY                            = { 0.3f, 0.3f, 0.3f, 1.0f };
  public static final float[] COLOR_BLACK                           = { 0.0f, 0.0f, 0.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_BACK_GROUND             = { 1.0f, 1.0f, 1.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_LABELS                  = { 0.0f, 0.0f, 0.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_AXIS_X                  = { 1.0f, 0.0f, 0.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_AXIS_Y                  = { 0.0f, 1.0f, 0.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_AXIS_Z                  = { 0.0f, 1.0f, 1.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_PLATE_XY_1              = { 0.0f, 0.0f, 1.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_PLATE_XZ_1              = { 0.0f, 0.0f, 1.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_PLATE_YZ_1              = { 0.0f, 0.0f, 1.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_PLATE_XY_2              = { 1.0f, 0.0f, 0.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_PLATE_XZ_2              = { 1.0f, 0.0f, 0.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_PLATE_YZ_2              = { 1.0f, 0.0f, 0.0f, 1.0f };
  
  public static final float[] DEFAULT_COLOR_PARTICLE                = { 1.0f, 0.0f, 0.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_EDGE                    = { 0.0f, 1.0f, 1.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_FACE                    = { 0.0f, 0.0f, 1.0f, 1.0f };
  public static final float   DEFAULT_TEXTURE_COORD_FACE_COLOR      = 0.0f;
  public static final float   DEFAULT_TEXTURE_COORD_FACE_TRANSPARENCY = 1.0f;
  public static final float[] DEFAULT_COLOR_VECTOR_ARROW            = { 0.0f, 0.0f, 0.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_SELECTED_PARTICLE       = { 0.0f, 1.0f, 0.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_SELECTED_EDGE           = { 1.0f, 0.0f, 0.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_SELECTED_FACE           = { 1.0f, 0.0f, 0.0f, 1.0f };
  
  public static final float[] DEFAULT_COLOR_SIMPLE_FIGURE_POINT     = { 0.2f, 1.0f, 0.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_SIMPLE_FIGURE_EDGE      = { 1.0f, 0.0f, 0.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_SIMPLE_FIGURE_FACE      = { 0.0f, 0.0f, 1.0f, 1.0f };
  
  public static final float[] DEFAULT_COLOR_SLICE_PLANE             = { 1.0f, 0.0f, 0.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_SLICE_PLANE_SUB         = { 0.0f, 1.0f, 1.0f, 0.3f };
  
  public static final float[] DEFAULT_COLOR_MIN_VALUE_PARTICLE      = { 0.6f, 0.0f, 1.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_MAX_VALUE_PARTICLE      = { 1.0f, 0.3f, 0.7f, 1.0f };
  
  public static final float[] DEFAULT_COLOR_MIN_VALUE_ELEMENT       = { 0.6f, 0.0f, 1.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_MAX_VALUE_ELEMENT       = { 1.0f, 0.3f, 0.7f, 1.0f };
  
  public static final String  COLOR_NAME_BACK_GROUND                = "color.backGround";
  
  public static final int     SELECTION_NOTHING                     = -1;
  
  public static final int     DEFAULT_SIZE_POINT                    = 4;
  public static final int     DEFAULT_SIZE_SPECIAL_POINT            = 10;
  public static final int     DEFAULT_SIZE_SIMPLE_FIGURE_POINT      = 8;
  public static final float   DEFAULT_THICKNESS_LINE                = 2.0f;
  public static final float   DEFAULT_THICKNESS_LINE_OF_AXIS        = 1.0f;
  public static final float   DEFAULT_THICKNESS_LINE_OF_SCALE       = 1.0f;
  public static final float   DEFAULT_THICKNESS_LINE_OF_RECTANGLE   = 1.0f;
  public static final float   DEFAULT_THICKNESS_LINE_OF_VECTOR      = 1.0f;
  
  public static final float   RATIO_OF_ARROW_HEAD                   = 0.2f;
  public static final float   RATIO_OF_ARROW_HEAD_SHARPNESS         = 0.1f;
  
  private static final int    NAME_DUMMY                            = -1;
  
  private static final String LABEL_AXIS_X                          = "X";
  private static final String LABEL_AXIS_Y                          = "Y";
  private static final String LABEL_AXIS_Z                          = "Z";
  
  public static final int     DEFAULT_SELECT_BUFFER_SIZE            = 1000000;
  
  public static final float   ALPHA_FOR_TRANCPARENT                 = 0.0f;
  public static final float   ALPHA_FOR_OPAQUE                      = 1.0f;
  
  private static final int    NUMBER_OF_MODEL_INFO                  = 5;
  
  private static final int    NUMBER_OF_MAX_SIMPLE_FIGURES          = 30;
  private static final int    LENGTH_OF_BUFFER_FOR_SIMPLE_FIGURES   = 9;
  /**
   * 全体を俯瞰する視点のモードである。
   */
  public static final int     MODE_VIEW_POINT_BIRD_EYE              = 10;
  /**
   * マップ内を歩き回るような視点のモードである。
   */
  public static final int     MODE_VIEW_POINT_WALK_THROUGH          = 20;
  /**
   * 指定した車両からの視点のモードである。
   */
  public static final int     MODE_VIEW_POINT_ON_VEHICLE            = 30;
  /**
   * デフォルトの車両を指定するIDである。
   */
  public static final int     DEFAULT_VEHICLE_ID_FOR_VIEW_POINT     = -1;
  
  public static final String  LABEL_MODE_VIEW_POINT_BIRD_EYE        = "ViewPointBirdEye";
  public static final String  LABEL_MODE_VIEW_POINT_WALK_THROUGH    = "ViewPointWalkThrough";
  public static final String  LABEL_MODE_VIEW_POINT_ON_VEHICLE      = "ViewPointOnVehicle";
  
  /**
   * 簡単な形状(点・辺・面)を描くためのIDを返すときに、
   * すでに最大個数に達している場合はそれ以上は受け入れないので、この値を返す。
   */
  public static final int     SIMPLE_FIGURE_NOT_ALLOWED             = -1;
  
  /**
   * 視点(Camera)が異なるパネルの最大個数。
   */
  public static final int     MAX_NUM_PANELS                        = 3;
  
  /**
   * 可視状態にするパネルの最小個数。
   */
  public static final int     MIN_ACTIVE_PANELS                     = 1;
  
  /**
   * デフォルトの、アクティブなパネルのインデックス。
   * アクティブなパネルとは、描画要素の表示のON/OFFを設定する対象である。
   */
  public static final int     DEFAULT_INDEX_OF_ACTIVE_PANEL         = 0;
  
  /**
   * 表示の設定を表すための文字列
   */
  public static final String  LABEL_CONFIGURATION_RESET                     = "Reset";
  public static final String  LABEL_CONFIGURATION_MODE_VIEW_POINT           = "ModeViewPoint";
  public static final String  LABEL_CONFIGURATION_VEHICLE_ID_FOR_VIEW_POINT = "VehicleIDForViewPoint";
  /**
   * 表示の設定を表すための文字列
   */
  public static final String  LABEL_CONFIGURATION_COLOR_BACK_GROUND         = "ColorBackGround";
  
  //private static final float BOX_SIZE = 2.0f;
  //private static final float[] BOX_COORDS = {
  //  -BOX_SIZE, -BOX_SIZE, -BOX_SIZE,
  //   BOX_SIZE, -BOX_SIZE, -BOX_SIZE,
  //   BOX_SIZE,  BOX_SIZE, -BOX_SIZE,
  //  -BOX_SIZE,  BOX_SIZE, -BOX_SIZE,
  //  -BOX_SIZE, -BOX_SIZE,  BOX_SIZE,
  //   BOX_SIZE, -BOX_SIZE,  BOX_SIZE,
  //   BOX_SIZE,  BOX_SIZE,  BOX_SIZE,
  //  -BOX_SIZE,  BOX_SIZE,  BOX_SIZE
  //};
  
  //private static final int[] BOX_CONNECTIVITY = {
  //  0, 2, 1,
  //  0, 3, 2,
  //  0, 1, 5,
  //  0, 5, 4,
  //  1, 2, 6,
  //  1, 6, 5,
  //  2, 3, 6,
  //  3, 7, 6,
  //  0, 4, 7,
  //  0, 7, 3,
  //  4, 5, 6,
  //  4, 6, 7
  //};
  
  //private static final float[] BOX_NORMAL = {
  //   0.0f,  0.0f, -1.0f,
  //   0.0f,  0.0f, -1.0f,
  //   0.0f, -1.0f,  0.0f,
  //   0.0f, -1.0f,  0.0f,
  //   1.0f,  0.0f,  0.0f,
  //   1.0f,  0.0f,  0.0f,
  //   0.0f,  1.0f,  0.0f,
  //   0.0f,  1.0f,  0.0f,
  //  -1.0f,  0.0f,  0.0f,
  //  -1.0f,  0.0f,  0.0f,
  //   0.0f,  0.0f,  1.0f,
  //   0.0f,  0.0f,  1.0f
  //};
  
  //private float[][] BOX_COLOR = {
  //    { 0.0f, 0.0f, 1.0f, 1.0f },
  //    { 0.0f, 0.0f, 1.0f, 1.0f },
  //    { 1.0f, 0.0f, 0.0f, 1.0f },
  //    { 1.0f, 0.0f, 0.0f, 1.0f },
  //    { 1.0f, 1.0f, 0.0f, 1.0f },
  //    { 1.0f, 1.0f, 0.0f, 1.0f },
  //    { 0.0f, 1.0f, 0.0f, 1.0f },
  //    { 0.0f, 1.0f, 0.0f, 1.0f },
  //    { 0.0f, 1.0f, 1.0f, 1.0f },
  //    { 0.0f, 1.0f, 1.0f, 1.0f },
  //    { 1.0f, 0.0f, 1.0f, 1.0f },
  //    { 1.0f, 0.0f, 1.0f, 1.0f }
  //};
  // @formatter:on
  /**
   * ログハンドリングクラス
   */
  private Logger _logger;
  /**
   * リソースマネージャ
   */
  private ResourceManager _rm;
  /**
   * 各種情報を表示するためのリスナー
   */
  private StatusLabelListener _statusLabelListener;
  /**
   * 3D描画用のパネル
   */
  private GLJPanel[] _glJPanel;
  /**
   * GLJPanelを配置するパネル
   */
  private JPanel _panel3D;
  
  private Drawable2DPanel _drawable2DPanel;
  
  /**
   * 視点を管理するカメラクラス
   */
  private Camera[] _camera;
  /**
   * 複数あるGLJPanelごとに、Cameraとの関連付けを保持するためのクラス
   */
  private SetOf3DPanelAndCamera[] _setOf3DPanelAndCamera;
  
  private ViewConfiguration _viewConfiguration;
  
  private float[] _colorBackGround;
  
  private IntBuffer _selectBuffer;
  
  private int _selectBufferSize;
  
  private boolean _toInitializeVBO = false;
  
  private boolean _isVBORendering = false;
  
  private long _time;
  
  private int[] _modeViewPointControl;
  private int[] _vehicleIDForViewPoint;
  
  private DecimalFormat _formatFPS;
  private DecimalFormat _formatTime;
  
  private TextDrawer _axisLabelDrawerX;
  private TextDrawer _axisLabelDrawerY;
  private TextDrawer _axisLabelDrawerZ;
  private TextDrawer _timeStepLabelDrawer;
  
  private TrafficDrawer _trafficDrawer;
  private int _trafficTime;
  
  /**
   * ある操作の補助として点、辺、面を描画するときに用いる。
   */
  private int[] _simpleFigureTypes;
  private float[][] _simpleFigureCoords;
  
  private Font _font;
  
  private boolean _is3DEnabled;
  private boolean _is2DEnabled;
  
  private ArrayDeque<String> _queueImageFilePath;
  private ArrayDeque<String> _queueImageType;
  
  private int _nActivePanels;
  private int _indexActivePanel;
  
  /**
   * コンストラクタ
   * @param statusLabelListener ステータス表示用のリスナー
   * @param panel3D 3D表示用のパネル
   * @param drawable2DPanel 2D表示用のパネル
   */
  public Drawable3DPanel(StatusLabelListener statusLabelListener, JPanel panel3D, Drawable2DPanel drawable2DPanel) {
    _logger = Logger.getLogger(Drawable3DPanel.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    
    _rm = ResourceManager.getInstance();
    
    _panel3D = panel3D;
    
    _queueImageFilePath = new ArrayDeque<String>();
    _queueImageType = new ArrayDeque<String>();
    
    _viewConfiguration = new ViewConfiguration();
    
    _statusLabelListener = statusLabelListener;
    
    _font = null;
    
    _selectBufferSize = DEFAULT_SELECT_BUFFER_SIZE;
    
    {
      //座標軸のラベル用のTextDrawer
      _axisLabelDrawerX = new TextDrawer();
      _axisLabelDrawerY = new TextDrawer();
      _axisLabelDrawerZ = new TextDrawer();
      _axisLabelDrawerX.setText(LABEL_AXIS_X);
      _axisLabelDrawerY.setText(LABEL_AXIS_Y);
      _axisLabelDrawerZ.setText(LABEL_AXIS_Z);
      _axisLabelDrawerX.setColor(DEFAULT_COLOR_AXIS_X);
      _axisLabelDrawerY.setColor(DEFAULT_COLOR_AXIS_Y);
      _axisLabelDrawerZ.setColor(DEFAULT_COLOR_AXIS_Z);
      
      _timeStepLabelDrawer = new TextDrawer();
      _timeStepLabelDrawer.setText("");
      _timeStepLabelDrawer.setColor(COLOR_BLACK);
    }
    
    _colorBackGround = new float[4];
    _colorBackGround[0] = DEFAULT_COLOR_BACK_GROUND[0];
    _colorBackGround[1] = DEFAULT_COLOR_BACK_GROUND[1];
    _colorBackGround[2] = DEFAULT_COLOR_BACK_GROUND[2];
    _colorBackGround[3] = DEFAULT_COLOR_BACK_GROUND[3];
    
    _simpleFigureTypes = new int[NUMBER_OF_MAX_SIMPLE_FIGURES];
    //面を描画するときのために、(XYZ座標)*(3点)で、配列の長さは9を確保しておく。
    //点や辺なら余分なところは使わない。
    _simpleFigureCoords = new float[NUMBER_OF_MAX_SIMPLE_FIGURES][LENGTH_OF_BUFFER_FOR_SIMPLE_FIGURES];
    
    _trafficDrawer = null;
    _trafficTime = 0;
    _formatTime = new DecimalFormat("0.0");
    
    _nActivePanels = MIN_ACTIVE_PANELS;
    _indexActivePanel = DEFAULT_INDEX_OF_ACTIVE_PANEL;
    _glJPanel = new GLJPanel[MAX_NUM_PANELS];
    _camera = new Camera[MAX_NUM_PANELS];
    _modeViewPointControl = new int[MAX_NUM_PANELS];
    _vehicleIDForViewPoint = new int[MAX_NUM_PANELS];
    _setOf3DPanelAndCamera = new SetOf3DPanelAndCamera[MAX_NUM_PANELS];
    for (int i = 0; i < MAX_NUM_PANELS; i++) {
      _setOf3DPanelAndCamera[i] = new SetOf3DPanelAndCamera(this, i);
      GLCapabilities capabilities = new GLCapabilities();
      _glJPanel[i] = new GLJPanel(capabilities);
      _glJPanel[i].setBorder(new LineBorder(Color.GRAY));
      _glJPanel[i].addGLEventListener(_setOf3DPanelAndCamera[i]);
      _camera[i] = new Camera(this, i, 50, 50, 100, 100);
      _modeViewPointControl[i] = MODE_VIEW_POINT_BIRD_EYE;
      _vehicleIDForViewPoint[i] = DEFAULT_VEHICLE_ID_FOR_VIEW_POINT;
    }
    resetCamera();
    
    _drawable2DPanel = drawable2DPanel;
    _drawable2DPanel.setClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    _time = System.nanoTime();
    _formatFPS = new DecimalFormat("0.00");
    
    setNumberOfActive3DPanels(1);
    enable3D();
    
    addListeners();
    initView();
    //initModelInfo();
  }
  
  /**
   * リスナをGLJPanelに追加する
   */
  private void addListeners() {
    for (int i = 0; i < MAX_NUM_PANELS; i++) {
      _glJPanel[i].addMouseListener(_camera[i]);
      _glJPanel[i].addMouseMotionListener(_camera[i]);
      _glJPanel[i].addMouseWheelListener(_camera[i]);
      //キーイベントに反応できるように、View2D側でフォーカスの面倒を見る。
      _glJPanel[i].addKeyListener(_camera[i]);
    }
  }
  
  private static void setColor(GLAutoDrawable glAutoDrawable, float r, float g, float b, float a) {
    final GL gl = glAutoDrawable.getGL();
    gl.glColor4f(r, g, b, a);
  }
  
  private static void setColor(GLAutoDrawable glAutoDrawable, float[] color) {
    setColor(glAutoDrawable, color[0], color[1], color[2], color[3]);
  }
  
  public void setTrafficModel(TrafficModel model) {
    _trafficDrawer = null;
    _trafficTime = 0;
    if (model != null) {
      _trafficDrawer = new TrafficDrawer(model, _viewConfiguration);
      _trafficDrawer.setFont(_font);
      _trafficDrawer.setViewConfiguration(_viewConfiguration);
      float fieldXMin = model.getFieldXMin();
      float fieldYMin = model.getFieldYMin();
      float fieldZMin = model.getFieldZMin();
      float fieldXMax = model.getFieldXMax();
      float fieldYMax = model.getFieldYMax();
      float fieldZMax = model.getFieldZMax();
      float centerX = (fieldXMin + fieldXMax) / 2;
      float centerY = (fieldYMin + fieldYMax) / 2;
      float centerZ = (fieldZMin + fieldZMax) / 2;
      float rangeX = fieldXMax - fieldXMin;
      float rangeY = fieldXMax - fieldXMin;
      float rangeZ = fieldXMax - fieldXMin;
      float range = rangeX;
      if (range < rangeY) range = rangeY;
      if (range < rangeZ) range = rangeZ;
      for (int i = 0; i < MAX_NUM_PANELS; i++) {
        _camera[i].setModeOfViewPoint(MODE_VIEW_POINT_BIRD_EYE);
        _camera[i].init(centerX, centerY, centerZ, range);
      }
      resetCamera();
      
      _drawable2DPanel.setCenter(centerX, centerY, 0);
      _drawable2DPanel.setScale(1f);
      _drawable2DPanel.setRadius(range);
    }
    
    setTrafficTime();
  }
  
  public ViewConfiguration getViewConfiguration() {
    return _viewConfiguration;
  }
  
  public void setViewConfiguration(ViewConfiguration configuration) {
    _viewConfiguration = configuration;
    if (_trafficDrawer != null) {
      _trafficDrawer.setViewConfiguration(_viewConfiguration);
    }
  }
  
  public void nextTime() {
    _trafficTime++;
    setTrafficTime();
  }
  
  public void previousTime() {
    _trafficTime--;
    setTrafficTime();
  }
  
  public boolean setTrafficTime(int trafficTime) {
    boolean isOK = true;
    
    if (trafficTime < 0) {
      isOK = false;
    }
    if (_trafficDrawer != null && _trafficDrawer.getTrafficModel().getNumTimeSteps() <= trafficTime) {
      isOK = false;
    }
    
    if (isOK) {
      _trafficTime = trafficTime;
      setTrafficTime();
    }
    return isOK;
  }
  
  private void setTrafficTime() {
    if (_trafficTime < 0) {
      _trafficTime = 0;
    }
    if (_trafficDrawer != null) {
      int nTimeSteps = _trafficDrawer.getTrafficModel().getNumTimeSteps();
      if (_trafficTime >= nTimeSteps) {
        _trafficTime = nTimeSteps - 1;
      }
      //現在のステップ数と総ステップ数を3D画面に表示する
      float timeInSecond = _trafficTime * 0.1f;
      String strTimeInSecond = _formatTime.format(timeInSecond);
      _timeStepLabelDrawer.setText("Time=" + strTimeInSecond + ", step=" + _trafficTime + "/" + nTimeSteps);
      
      for (int i = 0; i < _nActivePanels; i++) {
        if (_modeViewPointControl[i] == MODE_VIEW_POINT_ON_VEHICLE) {
          //指定した車両からの視点を設定する
          boolean tmpIsSuccess = setViewPointOnVehicle(i, _modeViewPointControl[i], _vehicleIDForViewPoint[i]);
        }
      }
      
      //現在のステップに存在する車両番号の一覧を取得する
      String strListOfCurrentVehicleID = _trafficDrawer.getListOfCurrentVehicleID(_trafficTime);
      _statusLabelListener.setStatusText(StatusLabelListener.LABEL_INDEX_PICK_INFO, strListOfCurrentVehicleID);
    } else {
      _timeStepLabelDrawer.setText("");
    }
  }
  
  /**
   * (x,y)はViewport長方形の左下角の(X,Y)座標(ピクセル)。初期値は(0,0)
   * (width, height)はViewportの幅と高さ表す。
   * @param glAutoDrawable
   * @param x
   * @param y
   * @param width
   * @param height
   * @return
   */
  private static boolean setupViewport(GLAutoDrawable glAutoDrawable, int x, int y, int width, int height) {
    final GL gl = glAutoDrawable.getGL();
    gl.glViewport(x, y, width, height);
    return true;
  }
  
  /**
   * 透明とデプステストについての設定を通常の描画用に戻す。
   * デプステストを有効にし、
   * 透明度が1以下、つまりすべての色を描画する。
   * @param glAutoDrawable
   */
  private static void setAlphaFunctionForRegularRendering(GLAutoDrawable glAutoDrawable) {
    final GL gl = glAutoDrawable.getGL();
    //DEPTH_TESTを有効にする
    gl.glEnable(GL.GL_DEPTH_TEST);
    //デプスバッファへの書き込みを許可する
    gl.glDepthMask(true);
    //アルファ値が1の不透明な要素のみ描画する
    gl.glAlphaFunc(GL.GL_LEQUAL, 1.0f);
  }
  
  /**
   * 透明・不透明の混在をうまく描画するために用いるメソッド。
   * 不透明な要素のみを描画する場合の設定を行う。
   * @param glAutoDrawable
   */
  private static void setAlphaFunctionForOpaqueRendering(GLAutoDrawable glAutoDrawable) {
    final GL gl = glAutoDrawable.getGL();
    //DEPTH_TESTを有効にする
    gl.glEnable(GL.GL_DEPTH_TEST);
    //デプスバッファへの書き込みを許可する
    gl.glDepthMask(true);
    //アルファ値が1の不透明な要素のみ描画する
    gl.glAlphaFunc(GL.GL_EQUAL, 1.0f);
  }
  
  /**
   * 透明・不透明の混在をうまく描画するために用いるメソッド。
   * 透明な要素を描画する場合の設定を行う。
   * 不透明な要素の描画の前に行う透明な要素の描画、または透明な要素のみの描画の際に用いる。
   * @param glAutoDrawable
   */
  private static void setAlphaFunctionForTransparentRenderingPre(GLAutoDrawable glAutoDrawable) {
    final GL gl = glAutoDrawable.getGL();
    //DEPTH_TESTを無効にする
    gl.glDisable(GL.GL_DEPTH_TEST);
    //デプスバッファを読み込み専用にする
    gl.glDepthMask(false);
    //アルファ値が1より小さい半透明な要素のみ描画する
    gl.glAlphaFunc(GL.GL_LESS, 1.0f);
  }
  
  /**
   * 透明・不透明の混在をうまく描画するために用いるメソッド。
   * 透明な要素を描画する場合の設定を行う。
   * 不透明な要素の描画の後に行う透明な要素の描画の際に用いる。
   * @param glAutoDrawable
   */
  private static void setAlphaFunctionForTransparentRenderingPost(GLAutoDrawable glAutoDrawable) {
    final GL gl = glAutoDrawable.getGL();
    //DEPTH_TESTを有効にする
    gl.glEnable(GL.GL_DEPTH_TEST);
    //デプスバッファを読み込み専用にする
    gl.glDepthMask(false);
    //アルファ値が1より小さい半透明な要素のみ描画する
    gl.glAlphaFunc(GL.GL_LESS, 1.0f);
  }
  
  public int getNumberOfActive3DPanels() {
    return _nActivePanels;
  }
  
  public void setNumberOfActive3DPanels(int nPanels) {
    if (nPanels > 0 && nPanels <= MAX_NUM_PANELS) {
      _nActivePanels = nPanels;
    } else {
      _nActivePanels = MIN_ACTIVE_PANELS;
    }
    _panel3D.removeAll();
    _panel3D.setLayout(new GridLayout(1, _nActivePanels));
    for (int i = 0; i < _nActivePanels; i++) {
      _panel3D.add(_glJPanel[i]);
    }
    
    setIndexOfActive3DPanel(DEFAULT_INDEX_OF_ACTIVE_PANEL);
    //レイアウト変更を反映させる
    _panel3D.validate();
  }
  
  public void setIndexOfActive3DPanel(int index) {
    int THICKNESS = 2;
    _indexActivePanel = index;
    if (_nActivePanels == 1) {
      _glJPanel[0].setBorder(new LineBorder(Color.GRAY, THICKNESS));
      _glJPanel[0].requestFocusInWindow();
    } else {
      for (int i = 0; i < _nActivePanels; i++) {
        if (i == _indexActivePanel) {
          _glJPanel[i].setBorder(new LineBorder(Color.RED, THICKNESS));
          _glJPanel[i].requestFocusInWindow();
        } else {
          _glJPanel[i].setBorder(new LineBorder(Color.GRAY, THICKNESS));
        }
      }
    }
    _statusLabelListener.setIndexOfActive3DPanel(_indexActivePanel);
  }
  
  public String[] getViewPointConfigurationOfActive3DPanel() {
    ArrayList<String> listConfigurations = new ArrayList<String>();
    String label = null;
    String configuration = null;
    String[] configurations = null;
    String SPACE = Common.SPACE;
    String EQUAL = Common.EQUAL;
    String COMMA = Common.COMMA;
    
    if (_indexActivePanel >= 0 && _indexActivePanel < _nActivePanels) {
      label = LABEL_CONFIGURATION_MODE_VIEW_POINT;
      configuration = label + EQUAL + _modeViewPointControl[_indexActivePanel];
      listConfigurations.add(configuration);
      
      label = LABEL_CONFIGURATION_VEHICLE_ID_FOR_VIEW_POINT;
      configuration = label + EQUAL + _vehicleIDForViewPoint[_indexActivePanel];
      listConfigurations.add(configuration);
      
      String[] conf3DPanel = Common.toStringArray(listConfigurations);
      String[] confCamera = _camera[_indexActivePanel].getViewPointConfiguration();
      
      //文字列の配列を結合する
      configurations = Common.joinStringArray(conf3DPanel, confCamera);
    }
    return configurations;
  }
  
  public void setViewPointConfigurationOfActive3DPanel(String[] configuration) {
    if (configuration != null && configuration.length > 0) {
      for (int i = 0; i < configuration.length; i++) {
        String line = configuration[i];
        String[] lineSplit = line.split(Common.EQUAL);
        if (lineSplit.length >= 1) {
          String command = lineSplit[0].trim();
          String[] params = null;
          if (lineSplit.length >= 2) {
            params = lineSplit[1].split(Common.COMMA);
          }
          setViewPointConfigurationOfActive3DPanelImpl(command, params);
          _camera[_indexActivePanel].setViewPointConfiguration(command, params);
        }
      }
      reDraw();
    }
  }
  
  private void setViewPointConfigurationOfActive3DPanelImpl(String commandRaw, String[] params) {
    boolean isOK = false;
    String command = null;
    if (commandRaw != null) {
      command = commandRaw.trim();
    }
    if (command != null) {
      int paramsLength = 0;
      if (params != null) {
        paramsLength = params.length;
      }
      int[] paramValuesInt = Common.toIntArrayFromStringArray(params);
      float[] paramValuesFloat = Common.toFloatArrayFromStringArray(params);
      if (command.equalsIgnoreCase(LABEL_CONFIGURATION_RESET)) {
        //################################
        //視点のリセット
        resetCamera();
      } else if (command.equalsIgnoreCase(LABEL_CONFIGURATION_MODE_VIEW_POINT)) {
        //################################
        //視点のモード
        int mode = MODE_VIEW_POINT_BIRD_EYE;
        if (paramValuesInt != null && paramValuesInt.length >= 1) {
          mode = paramValuesInt[0];
          if (isAcceptableModeOfViewPoint(mode)) {
            isOK = true;
          }
        }
        if (isOK) {
          setModeForViewPointControl(mode, _vehicleIDForViewPoint[_indexActivePanel]);
        }
      } else if (command.equalsIgnoreCase(LABEL_CONFIGURATION_VEHICLE_ID_FOR_VIEW_POINT)) {
        //################################
        //車両からの視点の場合に使う車両ID
        int vehicleID = DEFAULT_VEHICLE_ID_FOR_VIEW_POINT;
        if (paramValuesInt != null && paramValuesInt.length >= 1) {
          vehicleID = paramValuesInt[0];
          isOK = true;
        }
        if (isOK) {
          setModeForViewPointControl(_modeViewPointControl[_indexActivePanel], vehicleID);
        }
      }
    }
  }
  
  private static boolean isAcceptableModeOfViewPoint(int mode) {
    boolean isOK = false;
    if (mode == MODE_VIEW_POINT_BIRD_EYE) isOK = true;
    if (mode == MODE_VIEW_POINT_WALK_THROUGH) isOK = true;
    if (mode == MODE_VIEW_POINT_ON_VEHICLE) isOK = true;
    return isOK;
  }
  
  public GLJPanel getGLJPanel(int index) {
    GLJPanel panel = null;
    if (index >= 0 && index < _nActivePanels) {
      panel = _glJPanel[index];
    }
    return panel;
  }
  
  //public Drawable2DPanel getDrawable2DPanel() {
  //  return _drawable2DPanel;
  //}
  
  public TrafficDrawer getTrafficDrawer() {
    return _trafficDrawer;
  }
  
  public int getCurrentTrafficTime() {
    return _trafficTime;
  }
  
  public int getWidth(int index) {
    int width = 0;
    if (index >= 0 && index < _nActivePanels) {
      width = _glJPanel[index].getWidth();
    }
    return width;
  }
  
  public int getHeight(int index) {
    int height = 0;
    if (index >= 0 && index < _nActivePanels) {
      height = _glJPanel[index].getHeight();
    }
    return height;
  }
  
  public float getAspectRatio(int index) {
    float ratio = 1.0f;
    if (index >= 0 && index < _nActivePanels) {
      ratio = 1.0f * _glJPanel[index].getWidth() / _glJPanel[index].getHeight();
    }
    return ratio;
  }
  
  public void getNewSimpleFigureID(int nIDs, int[] buffer) {
    if (nIDs > 0 && buffer != null && buffer.length > 0 && nIDs <= buffer.length) {
      for (int i = 0; i < nIDs; i++) {
        buffer[i] = SIMPLE_FIGURE_NOT_ALLOWED;
      }
      
      int count = 0;
      for (int i = 0; i < NUMBER_OF_MAX_SIMPLE_FIGURES; i++) {
        if (_simpleFigureTypes[i] == ViewConfiguration.RENDER_TYPE_UNKNOWN) {
          buffer[count] = i;
          count++;
          if (count >= nIDs) break;
        }
      }
    }
  }
  
  public void setSimpleFigureData(int id, int type, float[] data) {
    if (id != SIMPLE_FIGURE_NOT_ALLOWED && id >= 0 && id < NUMBER_OF_MAX_SIMPLE_FIGURES) {
      if (data != null) {
        _simpleFigureTypes[id] = type;
        int length = data.length;
        if (length > LENGTH_OF_BUFFER_FOR_SIMPLE_FIGURES) {
          length = LENGTH_OF_BUFFER_FOR_SIMPLE_FIGURES;
        }
        for (int i = 0; i < length; i++) {
          _simpleFigureCoords[id][i] = data[i];
        }
      }
    } else {
      _logger.info("Invalid ID for simple figure : id=" + id);
    }
  }
  
  public void disableSimpleFigure(int id) {
    if (id != SIMPLE_FIGURE_NOT_ALLOWED && id >= 0 && id < NUMBER_OF_MAX_SIMPLE_FIGURES) {
      _simpleFigureTypes[id] = ViewConfiguration.RENDER_TYPE_UNKNOWN;
    } else {
      _logger.info("Invalid ID for simple figure : id=" + id);
    }
  }
  
  public void setColor(String target, Color color) {
    //if (target != null && color != null) {
    //  _logger.info("Set color : name=" + target + " : color=" + color.toString());
    //  if (target.equalsIgnoreCase(COLOR_NAME_BACK_GROUND)) {
    //    Common.copyColorToFloatArray(color, _colorBackGround);
    //    Color complementaryColor = Common.getComplementaryColor(color);
    //    _colorBarDrawer.setLabelColor(complementaryColor);
    //    for (int i = 0; i < NUMBER_OF_MODEL_INFO; i++) {
    //      _modelInfoDrawers[i].setColor(complementaryColor);
    //    }
    //  }
    //} else {
    //  _logger.info("Failed to set color.");
    //}
  }
  
  public void setFont(Font font) {
    _font = font;
    _axisLabelDrawerX.setFont(font);
    _axisLabelDrawerY.setFont(font);
    _axisLabelDrawerZ.setFont(font);
    _timeStepLabelDrawer.setFont(font);
    if (_trafficDrawer != null) {
      _trafficDrawer.setFont(font);
    }
  }
  
  //public void setIsPerspective(boolean isPerspective,int index) {
  //  _camera[index].setIsPerspective(isPerspective);
  //}
  
  public void setRenderingAcceleration(boolean flg) {
    _isVBORendering = flg;
  }
  
  public void setSelectBufferSize(int size) {
    _selectBufferSize = size;
  }
  
  public int[] getModeForViewPointControl() {
    return _modeViewPointControl;
  }
  
  public int[] getVehicleIDForViewPoint() {
    return _vehicleIDForViewPoint;
  }
  
  public boolean setModeForViewPointControl(int mode, int vehicleID) {
    boolean isSuccess = false;
    if (mode == MODE_VIEW_POINT_BIRD_EYE) {
      //俯瞰モードにする場合
      isSuccess = true;
    } else if (mode == MODE_VIEW_POINT_WALK_THROUGH) {
      //ウォークスルーモードにする場合
      isSuccess = true;
    } else if (mode == MODE_VIEW_POINT_ON_VEHICLE) {
      //車両からの視点にする場合は、
      //現在の時刻ステップで、指定するIDの車両があるかをチェックする
      isSuccess = setViewPointOnVehicle(_indexActivePanel, mode, vehicleID);
    }
    
    if (isSuccess) {
      _modeViewPointControl[_indexActivePanel] = mode;
      _vehicleIDForViewPoint[_indexActivePanel] = vehicleID;
      _camera[_indexActivePanel].setModeOfViewPoint(_modeViewPointControl[_indexActivePanel]);
      resetCamera(_indexActivePanel);
    }
    
    return isSuccess;
  }
  
  private boolean setViewPointOnVehicle(int index, int mode, int vehicleID) {
    boolean isSuccess = false;
    if (_trafficDrawer != null) {
      float[] buffer = new float[5];
      isSuccess = _trafficDrawer.getVehicleState(_trafficTime, vehicleID, buffer);
      if (isSuccess) {
        _camera[index].setViewPointOnVehicle(buffer);
      }
    }
    
    return isSuccess;
    
  }
  
  /**
   * 描画内容を画像ファイルとして保存する。
   * @param filePath 保存先のファイルパス
   * @param imageType 画像の種類
   */
  public void saveScreenImage(String filePath, String imageType) {
    boolean isSuccess = false;
    if (_is3DEnabled) {
      //isSuccess = View2D.saveComponentToImageFile(filePath, _glJPanel, imageType);
      isSuccess = View2D.saveComponentToImageFile(filePath, _panel3D, imageType);
    } else if (_is2DEnabled) {
      isSuccess = View2D.saveComponentToImageFile(filePath, _drawable2DPanel, imageType);
    }
    
    if (isSuccess) {
      _logger.info("Save screen image as " + imageType + " file to :" + filePath);
    } else {
      _logger.info("Failed to save screen image as " + imageType + " file to :" + filePath);
    }
  }
  
  private void saveScreenImageAfterDrawing() {
    while (_queueImageFilePath.size() > 0 && _queueImageType.size() > 0) {
      String tmpFilePath = _queueImageFilePath.pollFirst();
      String tmpImageType = _queueImageType.pollFirst();
      saveScreenImage(tmpFilePath, tmpImageType);
    }
  }
  
  /**
   * 描画内容の保存先のファイルパスを設定する。
   * 実際にファイルへの保存は、display()の最後で実行する。
   * @param filePath 保存先のファイルパス
   * @param imageType 画像の種類
   */
  public void reserveToSaveScreenImage(String filePath, String imageType) {
    _queueImageFilePath.addLast(filePath);
    _queueImageType.addLast(imageType);
  }
  
  /**
  * 再描画を行う。
  */
  public void reDraw() {
    if (_is3DEnabled) {
      for (int i = 0; i < _nActivePanels; i++) {
        _glJPanel[i].display();
        if (_indexActivePanel == i) {
          _glJPanel[i].requestFocusInWindow();
        }
      }
    }
    
    if (_is2DEnabled) {
      if (_trafficDrawer != null) {
        _drawable2DPanel.clear();
        _trafficDrawer.draw(_drawable2DPanel, _trafficTime);
        _timeStepLabelDrawer.setPosition(10, 20);
        _timeStepLabelDrawer.draw2D(_drawable2DPanel);
        _drawable2DPanel.switchImage();
        _drawable2DPanel.requestFocusInWindow();
        
        computeFPS();
        
        saveScreenImageAfterDrawing();
      }
    }
  }
  
  public void enable3D() {
    _is3DEnabled = true;
    _is2DEnabled = false;
  }
  
  public void enable2D() {
    _is3DEnabled = false;
    _is2DEnabled = true;
  }
  
  /**
   * 視点をリセットする
   */
  public void resetCamera() {
    for (int i = 0; i < _nActivePanels; i++) {
      resetCamera(i);
    }
  }
  
  /**
   * 視点をリセットする
   * @param index パネルの番号
   */
  public void resetCamera(int index) {
    if (index >= 0 && index < _nActivePanels) {
      _camera[index].reset();
    }
  }
  
  //public void rotate(int axis, float radian) {
  //  _camera.rotate(axis, radian);
  //}
  
  //public void rotateByDegree(int axis, float degree) {
  //  float radian = Common.toRadianFromDegree(degree);
  //  _camera.rotate(axis, radian);
  //}
  
  /**
   * 表示に関する設定を初期化する。
   */
  public void initView() {
    Arrays.fill(_simpleFigureTypes, ViewConfiguration.RENDER_TYPE_UNKNOWN);
    
    resetCamera();
  }
  
  /**
   * 各項目の表示/非表示を切り替える
   * @param type 項目を表す値
   * @param isVisible オンオフのフラグ
   */
  public void setVisible(int type, boolean isVisible) {
  }
  
  /**
   * OpenGLの初期化メソッド
   * @param index パネルの番号
   * @param glAutoDrawable GLのオブジェクト
   */
  public void init(int index, GLAutoDrawable glAutoDrawable) {
    _logger.info("init view3D");
    final GL gl = glAutoDrawable.getGL();
    
    float[] color = _colorBackGround;
    gl.glClearColor(color[0], color[1], color[2], color[3]);
    
    gl.glEnable(GL.GL_CULL_FACE);
    
    gl.glEnable(GL.GL_BLEND);
    gl.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA);
    
    gl.glEnable(GL.GL_ALPHA_TEST);
    setAlphaFunctionForOpaqueRendering(glAutoDrawable);
  }
  
  /**
   * 描画領域のサイズ変更時に呼ばれるメソッド
   * @param index パネルの番号
   * @param glAutoDrawable GLのオブジェクト
   * @param x 横方向位置
   * @param y 縦方向位置
   * @param width 幅
   * @param height 高さ
   */
  public void reshape(int index, GLAutoDrawable glAutoDrawable, int x, int y, int width, int height) {
    if (width <= 0 || height <= 0) {
      return;
    }
    _logger.info("reshape view3D");
    //VBOによる描画の場合、画面サイズが変わるとGPU側に転送した座標データ等が影響を受けることがある
    //そのためreshapeのたびに、データを転送し直す。
    //if (_modelDrawers != null && _isVBORendering) {
    //  for (int i = 0; i < _modelDrawers.length; i++) {
    //    _modelDrawers[i].initVBO(glAutoDrawable);
    //  }
    //}
    _toInitializeVBO = true;
    setupViewport(glAutoDrawable, 0, 0, width, height);
    
    //_drawable2DPanel.setSize(width, height);
    
    //これを実行しないと、画面を最大化したときにライトが機能しなくなってしまう
    _camera[index].setToInitializeLight(true);
  }
  
  /**
   * JOGLでは実装されてないので、空で良い。
   * @param index パネルの番号
   * @param glAutoDrawable GLのオブジェクト
   * @param modeChanged モードのフラグ
   * @param deviceChanged デバイスのフラグ
   */
  public void displayChanged(int index, GLAutoDrawable glAutoDrawable, boolean modeChanged, boolean deviceChanged) {
  }
  
  /**
   * OpengGLの描画メソッド
   * @param index パネルの番号
   * @param glAutoDrawable GLのオブジェクト
   */
  public void display(int index, GLAutoDrawable glAutoDrawable) {
    // _logger.info("display view3D");
    
    final GL gl = glAutoDrawable.getGL();
    
    float[] color = _colorBackGround;
    gl.glClearColor(color[0], color[1], color[2], color[3]);
    //スクリーンとデプスバッファをクリアする
    gl.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT);
    
    gl.glEnable(GL.GL_DEPTH_TEST);
    
    //##############################
    //モデルを描画する
    //##############################
    renderScene(index, glAutoDrawable);
    
    //##############################
    //モデルより手前に描画するため、DepthBufferをクリアし、
    //setAlphaFunctionForRegularRendering()を呼んで透明・不透明混在でも描画できるようにする。
    //##############################
    gl.glClear(GL.GL_DEPTH_BUFFER_BIT);
    setAlphaFunctionForRegularRendering(glAutoDrawable);
    
    //##############################
    //座標軸を描画する
    //##############################
    if (_viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_AXIS)) {
      renderAxis(index, glAutoDrawable);
    }
    
    // ##############################
    // 2D描画
    // ##############################
    renderScene2D(index, glAutoDrawable);
    
    gl.glFlush();
    
    computeFPS();
    
    saveScreenImageAfterDrawing();
  }
  
  private void computeFPS() {
    //前回のdisplay()終了時からの経過時間を計る。
    //(display()のはじめから終わりまでの経過時間ではあまり正確ではない)
    long endTimeNano = System.nanoTime();
    long elapseTime = endTimeNano - _time;
    _time = endTimeNano;
    double fps = 1000000000.0 / elapseTime;
    String strFPS = _formatFPS.format(fps);
    _statusLabelListener.setStatusText(StatusLabelListener.LABEL_INDEX_FPS, strFPS + " fps");
  }
  
  /**
   * シーン全体を描画する
   * @param index 描画先を表す番号
   * @param glAutoDrawable GLのオブジェクト
   */
  private void renderScene(int index, GLAutoDrawable glAutoDrawable) {
    final GL gl = glAutoDrawable.getGL();
    //視界の設定を行う
    _camera[index].setTransformationForModel(glAutoDrawable);
    
    gl.glEnable(GL.GL_LIGHTING);
    
    if (_trafficDrawer == null) {
      //renderTestObjects(index, glAutoDrawable);
    } else {
      _trafficDrawer.draw(glAutoDrawable, _trafficTime);
    }
    
    {
      setAlphaFunctionForRegularRendering(glAutoDrawable);
      renderSimpleFigures(index, glAutoDrawable);
    }
  }
  
  //private void renderTestObjects(int index, GLAutoDrawable glAutoDrawable) {
  //  final GL gl = glAutoDrawable.getGL();
  //  //視界の設定を行う
  //  
  //  float base0x = 0.0f;
  //  float base0y = 0.0f;
  //  float base1x = 100.0f;
  //  float base1y = 0.0f;
  //  float base2x = 100.0f;
  //  float base2y = 100.0f;
  //  float base3x = 0.0f;
  //  float base3y = 100.0f;
  //  gl.glBegin(GL.GL_TRIANGLES);
  //  
  //  gl.glColor4fv(COLOR_BLUE, 0);
  //  
  //  gl.glNormal3f(0, 0, 1);
  //  gl.glVertex3f(base0x, base0y, 0);
  //  gl.glVertex3f(base1x, base1y, 0);
  //  gl.glVertex3f(base2x, base2y, 0);
  //  gl.glNormal3f(0, 0, 1);
  //  gl.glVertex3f(base0x, base0y, 0);
  //  gl.glVertex3f(base2x, base2y, 0);
  //  gl.glVertex3f(base3x, base3y, 0);
  //  
  //  gl.glColor4fv(COLOR_RED, 0);
  //  gl.glNormal3f(0, 0, -1);
  //  gl.glVertex3f(base0x, base0y, 0);
  //  gl.glVertex3f(base2x, base2y, 0);
  //  gl.glVertex3f(base1x, base1y, 0);
  //  gl.glNormal3f(0, 0, -1);
  //  gl.glVertex3f(base0x, base0y, 0);
  //  gl.glVertex3f(base3x, base3y, 0);
  //  gl.glVertex3f(base2x, base2y, 0);
  //  
  //  for (int loop = 0; loop < 4; loop++) {
  //    float moveX = base0x;
  //    float moveY = base0y;
  //    float moveZ = 0.0f;
  //    if (loop == 1) {
  //      moveX = base1x;
  //      moveY = base1y;
  //    } else if (loop == 2) {
  //      moveX = base2x;
  //      moveY = base2y;
  //    } else if (loop == 3) {
  //      moveX = base3x;
  //      moveY = base3y;
  //    }
  //    for (int i = 0, ix3 = 0; ix3 < BOX_CONNECTIVITY.length; i++, ix3 += 3) {
  //      int p0x3 = BOX_CONNECTIVITY[ix3 + 0] * 3;
  //      int p1x3 = BOX_CONNECTIVITY[ix3 + 1] * 3;
  //      int p2x3 = BOX_CONNECTIVITY[ix3 + 2] * 3;
  //      float normalx = BOX_NORMAL[ix3 + 0];
  //      float normaly = BOX_NORMAL[ix3 + 1];
  //      float normalz = BOX_NORMAL[ix3 + 2];
  //      float p0x = BOX_COORDS[p0x3 + 0] + moveX;
  //      float p0y = BOX_COORDS[p0x3 + 1] + moveY;
  //      float p0z = BOX_COORDS[p0x3 + 2] + moveZ;
  //      float p1x = BOX_COORDS[p1x3 + 0] + moveX;
  //      float p1y = BOX_COORDS[p1x3 + 1] + moveY;
  //      float p1z = BOX_COORDS[p1x3 + 2] + moveZ;
  //      float p2x = BOX_COORDS[p2x3 + 0] + moveX;
  //      float p2y = BOX_COORDS[p2x3 + 1] + moveY;
  //      float p2z = BOX_COORDS[p2x3 + 2] + moveZ;
  //      float[] color = BOX_COLOR[i];
  //      gl.glColor4fv(color, 0);
  //      gl.glNormal3f(normalx, normaly, normalz);
  //      gl.glVertex3f(p0x, p0y, p0z);
  //      gl.glVertex3f(p1x, p1y, p1z);
  //      gl.glVertex3f(p2x, p2y, p2z);
  //      gl.glNormal3f(-normalx, -normaly, -normalz);
  //      gl.glVertex3f(p0x, p0y, p0z);
  //      gl.glVertex3f(p2x, p2y, p2z);
  //      gl.glVertex3f(p1x, p1y, p1z);
  //    }
  //  }
  //  gl.glEnd();
  //}
  
  /**
   * 座標軸を描画する
   * @param index 描画先を表す番号
   * @param glAutoDrawable GLのオブジェクト
   */
  private void renderAxis(int index, GLAutoDrawable glAutoDrawable) {
    final GL gl = glAutoDrawable.getGL();
    
    //座標軸描画用の視界の設定を行う
    _camera[index].setTransformationForModelForAxisRendering(glAutoDrawable);
    
    float widthLabelX = _axisLabelDrawerX.getWidth();
    float heightLabelX = _axisLabelDrawerX.getHeight();
    float widthLabelY = _axisLabelDrawerY.getWidth();
    float heightLabelY = _axisLabelDrawerY.getHeight();
    float widthLabelZ = _axisLabelDrawerZ.getWidth();
    float heightLabelZ = _axisLabelDrawerZ.getHeight();
    float sizeOfLabel = widthLabelX;
    if (sizeOfLabel > heightLabelX) sizeOfLabel = heightLabelX;
    if (sizeOfLabel > widthLabelY) sizeOfLabel = widthLabelY;
    if (sizeOfLabel > heightLabelY) sizeOfLabel = heightLabelY;
    if (sizeOfLabel > widthLabelZ) sizeOfLabel = widthLabelZ;
    if (sizeOfLabel > heightLabelZ) sizeOfLabel = heightLabelZ;
    
    //座標軸描画のための単位長さ
    float tmpUnit = Camera.AXIS_RENDERING_SIZE;
    float lengthAxis = tmpUnit;
    float lengthTriangle = lengthAxis * 0.8f;
    float positionCaption = lengthAxis + tmpUnit * 0.15f;
    //TODO ラベルを描画する位置とフォントサイズの調整
    if (sizeOfLabel > 0) {
      float coeff = Camera.getCoefficientForAxisLabel();
      
      positionCaption = lengthAxis + tmpUnit * sizeOfLabel / (getHeight(index) * coeff);
    }
    gl.glLineWidth(DEFAULT_THICKNESS_LINE_OF_AXIS);
    
    //##############################
    //座標軸
    //##############################
    //軸は陰影なしで描画する
    gl.glDisable(GL.GL_LIGHTING);
    // X
    gl.glBegin(GL.GL_LINES);
    setColor(glAutoDrawable, DEFAULT_COLOR_AXIS_X);
    gl.glVertex3f(0, 0, 0);
    gl.glVertex3f(lengthAxis, 0, 0);
    gl.glEnd();
    // Y
    gl.glBegin(GL.GL_LINES);
    setColor(glAutoDrawable, DEFAULT_COLOR_AXIS_Y);
    gl.glVertex3f(0, 0, 0);
    gl.glVertex3f(0, lengthAxis, 0);
    gl.glEnd();
    // Z
    gl.glBegin(GL.GL_LINES);
    setColor(glAutoDrawable, DEFAULT_COLOR_AXIS_Z);
    gl.glVertex3f(0, 0, 0);
    gl.glVertex3f(0, 0, lengthAxis);
    gl.glEnd();
    
    //##############################
    //座標軸の間の三角形
    //##############################
    //陰影ありで描画する
    gl.glEnable(GL.GL_LIGHTING);
    //三角形同士が接する部分の軸が隠れないよう描画する位置をほんの少しずらす
    float tmpEPS = 1.0e-2f * tmpUnit;
    gl.glBegin(GL.GL_TRIANGLES);
    // XY
    setColor(glAutoDrawable, DEFAULT_COLOR_PLATE_XY_1);
    gl.glNormal3f(0, 0, 1);
    gl.glVertex3f(0, 0, -tmpEPS);
    gl.glVertex3f(lengthTriangle, 0, -tmpEPS);
    gl.glVertex3f(0, lengthTriangle, -tmpEPS);
    setColor(glAutoDrawable, DEFAULT_COLOR_PLATE_XY_2);
    gl.glNormal3f(0, 0, -1);
    gl.glVertex3f(0, 0, tmpEPS);
    gl.glVertex3f(0, lengthTriangle, tmpEPS);
    gl.glVertex3f(lengthTriangle, 0, tmpEPS);
    // XZ
    setColor(glAutoDrawable, DEFAULT_COLOR_PLATE_XZ_1);
    gl.glNormal3f(0, 1, 0);
    gl.glVertex3f(0, -tmpEPS, 0);
    gl.glVertex3f(0, -tmpEPS, lengthTriangle);
    gl.glVertex3f(lengthTriangle, -tmpEPS, 0);
    setColor(glAutoDrawable, DEFAULT_COLOR_PLATE_XZ_2);
    gl.glNormal3f(0, -1, 0);
    gl.glVertex3f(0, tmpEPS, 0);
    gl.glVertex3f(lengthTriangle, tmpEPS, 0);
    gl.glVertex3f(0, tmpEPS, lengthTriangle);
    // YZ
    setColor(glAutoDrawable, DEFAULT_COLOR_PLATE_YZ_1);
    gl.glNormal3f(1, 0, 0);
    gl.glVertex3f(-tmpEPS, 0, 0);
    gl.glVertex3f(-tmpEPS, lengthTriangle, 0);
    gl.glVertex3f(-tmpEPS, 0, lengthTriangle);
    setColor(glAutoDrawable, DEFAULT_COLOR_PLATE_YZ_2);
    gl.glNormal3f(-1, 0, 0);
    gl.glVertex3f(tmpEPS, 0, 0);
    gl.glVertex3f(tmpEPS, 0, lengthTriangle);
    gl.glVertex3f(tmpEPS, lengthTriangle, 0);
    gl.glEnd();
    
    //##############################
    //各軸のラベル
    //##############################
    // "X"
    _axisLabelDrawerX.setPosition3D(positionCaption, 0, 0);
    _axisLabelDrawerX.draw(glAutoDrawable);
    // "Y"
    _axisLabelDrawerY.setPosition3D(0, positionCaption, 0);
    _axisLabelDrawerY.draw(glAutoDrawable);
    // "Z"
    _axisLabelDrawerZ.setPosition3D(0, 0, positionCaption);
    _axisLabelDrawerZ.draw(glAutoDrawable);
    
    gl.glPopAttrib();
  }
  
  /**
   * 2D描画
   * @param index 描画先を表す番号
   * @param glAutoDrawable GLのオブジェクト
   */
  private void renderScene2D(int index, GLAutoDrawable glAutoDrawable) {
    final GL gl = glAutoDrawable.getGL();
    //2D描画用の視界の設定を行う
    _camera[index].setTransformationFor2DRendering(glAutoDrawable);
    
    _timeStepLabelDrawer.setPosition(10, 10);
    _timeStepLabelDrawer.draw(glAutoDrawable);
    
    gl.glDisable(GL.GL_LIGHTING);
  }
  
  private void renderSimpleFigures(int index, GLAutoDrawable glAutoDrawable) {
    final GL gl = glAutoDrawable.getGL();
    
    gl.glDisable(GL.GL_LIGHTING);
    
    for (int i = 0; i < NUMBER_OF_MAX_SIMPLE_FIGURES; i++) {
      int type = _simpleFigureTypes[i];
      if (type == ViewConfiguration.RENDER_TYPE_PARTICLE) {
        float p0x = _simpleFigureCoords[i][0];
        float p0y = _simpleFigureCoords[i][1];
        float p0z = _simpleFigureCoords[i][2];
        
        gl.glPointSize(DEFAULT_SIZE_SIMPLE_FIGURE_POINT);
        setColor(glAutoDrawable, DEFAULT_COLOR_SIMPLE_FIGURE_POINT);
        gl.glBegin(GL.GL_POINTS);
        gl.glVertex3f(p0x, p0y, p0z);
        gl.glEnd();
      }
      if (type == ViewConfiguration.RENDER_TYPE_EDGE) {
        float p0x = _simpleFigureCoords[i][0];
        float p0y = _simpleFigureCoords[i][1];
        float p0z = _simpleFigureCoords[i][2];
        float p1x = _simpleFigureCoords[i][3];
        float p1y = _simpleFigureCoords[i][4];
        float p1z = _simpleFigureCoords[i][5];
        
        gl.glLineWidth(DEFAULT_THICKNESS_LINE_OF_AXIS);
        setColor(glAutoDrawable, DEFAULT_COLOR_SIMPLE_FIGURE_EDGE);
        gl.glBegin(GL.GL_LINES);
        gl.glVertex3f(p0x, p0y, p0z);
        gl.glVertex3f(p1x, p1y, p1z);
        gl.glEnd();
      }
      if (type == ViewConfiguration.RENDER_TYPE_FACE) {
        float p0x = _simpleFigureCoords[i][0];
        float p0y = _simpleFigureCoords[i][1];
        float p0z = _simpleFigureCoords[i][2];
        float p1x = _simpleFigureCoords[i][3];
        float p1y = _simpleFigureCoords[i][4];
        float p1z = _simpleFigureCoords[i][5];
        float p2x = _simpleFigureCoords[i][6];
        float p2y = _simpleFigureCoords[i][7];
        float p2z = _simpleFigureCoords[i][8];
        
        setColor(glAutoDrawable, DEFAULT_COLOR_SIMPLE_FIGURE_FACE);
        gl.glBegin(GL.GL_TRIANGLES);
        gl.glVertex3f(p0x, p0y, p0z);
        gl.glVertex3f(p1x, p1y, p1z);
        gl.glVertex3f(p2x, p2y, p2z);
        gl.glEnd();
      }
    }
  }
}
