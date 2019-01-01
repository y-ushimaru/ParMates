package jp.ac.u_tokyo.t.q.adventure.mates.ui.render;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.util.ArrayList;
import java.util.logging.Logger;

import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import javax.swing.SwingUtilities;

import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;
import jp.ac.u_tokyo.t.q.adventure.mates.util.MatrixUtils;

/**
 * 3D画面の視点を管理するクラス<br>
 * 視点はZ軸負の方向を見たまま動かさない。
 * 回転はモデルを動かす。
 * 拡大縮小・平行移動は、カメラから見える範囲を変更することで実現する。
 */
public class Camera implements MouseListener, MouseMotionListener, MouseWheelListener, KeyListener {
  //@formatter:off
  /**
   * マウス操作の状態を示す。操作されてない状態。
   */
  private static final int MODE_NORMAL                              = 10;
  /**
   * マウス操作の状態を示す。回転操作中
   */
  private static final int MODE_ROTATE                              = 20;
  /**
   * マウス操作の状態を示す。回転操作中。水平方向のみの回転を行う。
   */
  private static final int MODE_ROTATE_HORIZONTAL                   = 21;
  /**
   * マウス操作の状態を示す。回転操作中。鉛直方向のみの回転を行う。
   */
  private static final int MODE_ROTATE_VERTICAL                     = 22;
  /**
   * マウス操作の状態を示す。移動操作中
   */
  private static final int MODE_TRANSLATE                           = 30;
  /**
   * マウス操作の状態を示す。倍率変更中
   */
  private static final int MODE_SCALE                               = 40;
  /**
   * マウス操作の感度。回転操作用
   */
  private static final float MOUSE_SENSITIVITY_ROTATE               = 3.0f;
  /**
   * マウス操作の感度。移動操作用。1.0fだと、マウス操作に3D表示がほぼ追随する
   */
  private static final float MOUSE_SENSITIVITY_TRANSLATE            = 1.0f;
  /**
   * マウス操作の感度。倍率変更用
   */
  private static final float MOUSE_SENSITIVITY_SCALE                = 0.01f;
  /**
   * マウス操作の感度。マウスホイールでの倍率変更用
   */
  private static final float MOUSE_SENSITIVITY_SCALE_BY_WHEEL       = 0.01f;
  /**
   * デフォルトでの表示倍率
   */
  private static final float SCALE_DEFAULT                          = 1.0f;
  /**
   * 表示倍率の最小値である。
   */
  private static final float SCALE_MINIMUM                          = 1.0e-6f;
  /**
   * 表示倍率の最大値である。
   */
  private static final float SCALE_MAXIMUM                          = 1.0e6f;
  /**
   * X軸周りの回転を表す番号である。
   */
  public static final int    ROTATE_X                               = 1;
  /**
   * Y軸周りの回転を表す番号である。
   */
  public static final int    ROTATE_Y                               = 2;
  /**
   * Z軸周りの回転を表す番号である。
   */
  public static final int    ROTATE_Z                               = 3;
  /**
   * 座標軸描画用の値。座標軸のサイズ
   */
  public  static final float AXIS_RENDERING_SIZE                          = 1.0f;
  /**
   * 座標軸描画用の値。座標軸の交点(原点)までの距離の比率
   */
  private static final float AXIS_RENDERING_DISTANCE                      = 3.0f;
  /**
   * 座標軸描画用の値。視点から近いクリッピング面までの距離の比率
   */
  private static final float AXIS_RENDERING_RATIO_NEAR                    = 1.0f;
  /**
   * 座標軸描画用の値。視点から遠いクリッピング面までの距離の比率
   */
  private static final float AXIS_RENDERING_RATIO_FAR                     = 5.0f;
  /**
   * 座標軸描画用の値。座標軸の交点(原点)の位置での視体積の高さの比率
   */
  private static final float AXIS_RENDERING_HEIGHT                        = 10.0f;
  /**
   * 座標軸描画用の値。座標軸の交点(原点)から画面左端までの比率
   */
  private static final float AXIS_RENDERING_RATIO_LEFT                    = 1.0f;
  /**
   * 座標軸描画用の値。座標軸の交点(原点)から画面右端までの比率
   */
  private static final float AXIS_RENDERING_RATIO_RIGHT                   = 7.0f;
  /**
   * 座標軸描画用の値。座標軸の交点(原点)から画面下端までの比率
   */
  private static final float AXIS_RENDERING_RATIO_BOTTOM                  = 1.0f;
  /**
   * 座標軸描画用の値。座標軸の交点(原点)から画面上端までの比率
   */
  private static final float AXIS_RENDERING_RATIO_TOP                     = 4.0f;
  /**
   * 遠い方のクリッピング平面の位置を決める係数である。
   */
  private static final float COEFFICIENT_FOR_FAR_PLANE                    = 1000.0f;
  /**
   * 指定した角度だけ回転させるときの、デフォルトの回転量
   */
  public static final float  DEFAULT_DEGREE_FOR_ROTATION                  = 30.0f;
  /**
   * 近い方のクリッピング平面までの、デフォルトの距離である。
   */
  private static final float DEFAULT_DISTANCE_TO_NEAR_PLANE               = 1.0f;
  /**
   * ウォークスルー視点の場合の、近い方のクリッピング面までの距離である。
   * デフォルト値ではなく、固定値として使用する。
   */
  private static final float DISTANCE_TO_NEAR_PLANE_FOR_MODE_WALK_THROUGH = 1.0f;
  /**
   * 車両からの視点の場合の、近い方のクリッピング面までの距離である。
   * デフォルト値ではなく、固定値として使用する。
   */
  private static final float DISTANCE_TO_NEAR_PLANE_FOR_MODE_ON_VEHICLE   = 1.0f;
  /**
   * ウォークスルー視点の場合の、地面と視点との距離である。
   * デフォルト値ではなく、固定値として使用する。
   */
  private static final float DISTANCE_FROM_GROUND_FOR_MODE_WALK_THROUGH   = 1.0f;
  /**
   * 回転中心の点までの、デフォルトの距離である。
   */
  private static final float DEFAULT_DISTANCE_TO_CENTER_OF_ROTATION       = 10.0f;
  /**
   * 表示の設定を表すための文字列
   */
  public static final String  LABEL_CONFIGURATION_VIEW_POINT_POSITION     = "ViewPointPosition";
  public static final String  LABEL_CONFIGURATION_VIEW_POINT_DIRECTION_X  = "ViewPointDirectionX";
  public static final String  LABEL_CONFIGURATION_VIEW_POINT_DIRECTION_Y  = "ViewPointDirectionY";
  public static final String  LABEL_CONFIGURATION_VIEW_POINT_DIRECTION_Z  = "ViewPointDirectionZ";
  public static final String  LABEL_CONFIGURATION_CENTER_OF_VIEW          = "CenterOfView";
  public static final String  LABEL_CONFIGURATION_SCALE                   = "Scale";
  //@formatter:on
  /**
   * ログハンドリングクラス
   */
  private Logger _logger;
  /**
   * 地面の中心位置である。
   */
  private Point3f _centerOfGround;
  /**
   * マップの広さを示す値である。
   */
  private float _radiusOfGround;
  /**
   * 視点の初期位置である。
   */
  private Point3f _eyePositionInitial;
  /**
   * 近い方のクリッピング平面までの距離である。
   */
  private float _distanceToNearPlane;
  private float _distanceToNearPlaneForModeBirdEye;
  private float _distanceToNearPlaneForModeWalkThrough;
  private float _distanceToNearPlaneForModeOnVehicle;
  /**
   * 遠い方のクリッピング平面までの距離である。
   */
  private float _distanceToFarPlane;
  /**
   * ある点の周りに回転するモードでの、回転中心までの距離である。
   * 回転中心は、視点の正面方向にあるものとする。
   */
  private float _distanceToCenterOfRotation;
  /**
   * 視点の位置である。
   */
  private Point3f _eyePosition;
  /**
   * 視界の座標系でのX軸方向の単位ベクトルである。
   */
  private Point3f _eyeDirX;
  /**
   * 視界の座標系でのY軸方向の単位ベクトルである。
   */
  private Point3f _eyeDirY;
  /**
   * 視界の座標系でのZ軸方向の単位ベクトルである。
   */
  private Point3f _eyeDirZ;
  /**
   * 座標軸を描画する際の平行移動量
   */
  private Point3f _moveForAxisRendering;
  /**
   * 指定したIDの車両の位置である。
   */
  private Point3f _vehiclePosition;
  /**
   * 指定したIDの車両のXY方向の回転である。
   */
  private float _vehicleRotateXY;
  /**
   * 指定したIDの車両のZ方向の回転である。
   */
  private float _vehicleRotateZ;
  /**
   * 表示の倍率。画面に表示する領域の大きさを表しているので、この値が小さいほどモデルは大きく表示される。
   */
  private float _scale;
  /**
   * trueのとき透視投影法、falseのとき正射影
   */
  private boolean _isPerspective;
  /**
   * モデル描画のための変換行列に相当する配列である。
   */
  private float[] _currentModelTransformation;
  /**
   * モデル描画のための変換行列のうち、回転部分のみを取り出した行列である。
   * 座標軸の変換後の向きを表す。
   * 描画に用いる行列とは少し異なる。
   */
  private float[] _currentModelTransformationForAxis;
  /**
   * 行列計算のために用いる配列である。
   * newする回数を省くためメンバとしておく。
   */
  private float[] _bufferMatrix1;
  private float[] _bufferMatrix2;
  private float[] _bufferMatrix3;
  /**
   * マウスのX座標
   */
  private int _mouseX;
  /**
   * マウスのY座標
   */
  private int _mouseY;
  /**
   * 表示している範囲の中心の、近いクリッピング面でのX座標
   */
  private float _centerOfViewX;
  /**
   * 表示している範囲の中心の、近いクリッピング面でのY座標
   */
  private float _centerOfViewY;
  /**
   * マウス操作でのモード。
   */
  private int _mode;
  /**
   * trueのとき、ライトをONにする前にライトを初期化する
   */
  private boolean _toInitializeLight;
  
  private Drawable3DPanel _drawable3DPanel;
  
  private int _panelIndex;
  
  private int _modeViewPoint;
  
  private int _pressedKey;
  
  private boolean _isLightEnabled;
  
  private Point3f _bufferPoint3f_1;
  private Point3f _bufferPoint3f_2;
  
  private Camera(Drawable3DPanel drawable3DPanel) {
    init(drawable3DPanel);
  }
  
  public Camera(Drawable3DPanel drawable3DPanel, int panelIndex, float positionX, float positionY, float positionZ, float radius) {
    _panelIndex = panelIndex;
    init(drawable3DPanel);
    init(positionX, positionY, positionZ, radius);
    setIsLightEnabled(true);
  }
  
  private void init(Drawable3DPanel drawable3DPanel) {
    _logger = Logger.getLogger(Camera.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    
    _drawable3DPanel = drawable3DPanel;
    _centerOfGround = new Point3f();
    _eyePositionInitial = new Point3f();
    _modeViewPoint = Drawable3DPanel.MODE_VIEW_POINT_BIRD_EYE;
    _distanceToNearPlaneForModeBirdEye = DEFAULT_DISTANCE_TO_NEAR_PLANE;
    _distanceToNearPlaneForModeWalkThrough = DISTANCE_TO_NEAR_PLANE_FOR_MODE_WALK_THROUGH;
    _distanceToNearPlaneForModeOnVehicle = DISTANCE_TO_NEAR_PLANE_FOR_MODE_ON_VEHICLE;
    setDistanceToNearPlane();
    _distanceToCenterOfRotation = DEFAULT_DISTANCE_TO_CENTER_OF_ROTATION;
    _eyePosition = new Point3f();
    _eyeDirX = new Point3f();
    _eyeDirY = new Point3f();
    _eyeDirZ = new Point3f();
    _vehiclePosition = new Point3f();
    _bufferPoint3f_1 = new Point3f();
    _bufferPoint3f_2 = new Point3f();
    
    _moveForAxisRendering = new Point3f(0, 0, AXIS_RENDERING_SIZE * AXIS_RENDERING_DISTANCE);
    
    _isPerspective = true;
    
    _currentModelTransformation = new float[MatrixUtils.LENGTH_OF_MATRIX_AS_ARRAY];
    _currentModelTransformationForAxis = new float[MatrixUtils.LENGTH_OF_MATRIX_AS_ARRAY];
    _bufferMatrix1 = new float[MatrixUtils.LENGTH_OF_MATRIX_AS_ARRAY];
    _bufferMatrix2 = new float[MatrixUtils.LENGTH_OF_MATRIX_AS_ARRAY];
    _bufferMatrix3 = new float[MatrixUtils.LENGTH_OF_MATRIX_AS_ARRAY];
  }
  
  /**
   * モデルの中心とサイズを与えて視点を初期化する。
   * @param positionX モデルの中心のX座標
   * @param positionY モデルの中心のY座標
   * @param positionZ モデルの中心のZ座標
   * @param radius モデルの大きさを表す値
   */
  public void init(float positionX, float positionY, float positionZ, float radius) {
    _centerOfGround.x = positionX;
    _centerOfGround.y = positionY;
    _centerOfGround.z = positionZ;
    _radiusOfGround = radius;
    
    float distanceToEyePosition = _radiusOfGround * 2;
    _eyePositionInitial.x = _centerOfGround.x;
    _eyePositionInitial.y = _centerOfGround.y;
    _eyePositionInitial.z = _centerOfGround.z + distanceToEyePosition;
    _distanceToNearPlaneForModeBirdEye = _radiusOfGround;
    setDistanceToNearPlane();
    _distanceToCenterOfRotation = distanceToEyePosition;
    
    reset();
  }
  
  public void setModeOfViewPoint(int mode) {
    _modeViewPoint = mode;
    setDistanceToNearPlane();
  }
  
  public void setDistanceToNearPlane() {
    _distanceToNearPlane = _distanceToNearPlaneForModeBirdEye;
    if (_modeViewPoint == Drawable3DPanel.MODE_VIEW_POINT_WALK_THROUGH) {
      _distanceToNearPlane = _distanceToNearPlaneForModeWalkThrough;
    } else if (_modeViewPoint == Drawable3DPanel.MODE_VIEW_POINT_ON_VEHICLE) {
      _distanceToNearPlane = _distanceToNearPlaneForModeOnVehicle;
    }
    _distanceToFarPlane = _distanceToNearPlane * COEFFICIENT_FOR_FAR_PLANE;
  }
  
  public void setToInitializeLight(boolean toInit) {
    _toInitializeLight = toInit;
  }
  
  /**
   * 視点を初期化する
   */
  public void reset() {
    resetTransformation();
    
    if (_modeViewPoint == Drawable3DPanel.MODE_VIEW_POINT_BIRD_EYE) {
      _eyePosition.x = _eyePositionInitial.x;
      _eyePosition.y = _eyePositionInitial.y;
      _eyePosition.z = _eyePositionInitial.z;
      _eyeDirX.init(1, 0, 0);
      _eyeDirY.init(0, 1, 0);
      _eyeDirZ.init(0, 0, 1);
      //########################################################
      //全体の真上からの視界で、X軸が右向きになるように回転する
      //視界の座標系は固定で、周囲の方を回転させる。
      //回転前は、全体座標系と視界座標系の向きは一致している。
      //まず、視界座標系での横方向であるY軸周りに回転し、
      //全体座標系のZ軸を、奥から手前に向かうようにする。
      float degree = 90;
      float radian = Common.toRadianFromDegree(degree);
      rotate(ROTATE_Y, radian);
      //次に、視界座標系での画面に垂直なX軸周りに回転し、
      //全体座標系のX軸が右を向くようにする。
      rotate(ROTATE_X, -radian);
    } else if (_modeViewPoint == Drawable3DPanel.MODE_VIEW_POINT_WALK_THROUGH) {
      _eyePosition.x = _centerOfGround.x;
      _eyePosition.y = _centerOfGround.y;
      _eyePosition.z = _centerOfGround.z + DISTANCE_FROM_GROUND_FOR_MODE_WALK_THROUGH;
      _eyeDirX.init(1, 0, 0);
      _eyeDirY.init(0, 1, 0);
      _eyeDirZ.init(0, 0, 1);
    } else if (_modeViewPoint == Drawable3DPanel.MODE_VIEW_POINT_ON_VEHICLE) {
      _eyePosition.x = _vehiclePosition.x;
      _eyePosition.y = _vehiclePosition.y;
      _eyePosition.z = _vehiclePosition.z;
      _eyeDirX.init(1, 0, 0);
      _eyeDirY.init(0, 1, 0);
      _eyeDirZ.init(0, 0, 1);
      //車両の回転量を反映させる
      rotate(ROTATE_Y, -_vehicleRotateZ);
      rotate(ROTATE_Z, _vehicleRotateXY);
    }
    
    //表示倍率にデフォルト値をセットする
    _scale = SCALE_DEFAULT;
    
    _toInitializeLight = true;
    
    //表示領域の中心点の、近いほうのクリッピング面でのXY座標
    _centerOfViewX = 0;
    _centerOfViewY = 0;
  }
  
  private void resetTransformation() {
    //行列に相当する配列を単位行列により初期化する
    MatrixUtils.setIdentityMatrix(_currentModelTransformation);
    MatrixUtils.setIdentityMatrix(_currentModelTransformationForAxis);
    MatrixUtils.setIdentityMatrix(_bufferMatrix1);
    MatrixUtils.setIdentityMatrix(_bufferMatrix2);
    MatrixUtils.setIdentityMatrix(_bufferMatrix3);
  }
  
  /**
   * 
   * @param glAutoDrawable
   */
  private void initializeLight(GLAutoDrawable glAutoDrawable) {
    final GL gl = glAutoDrawable.getGL();
    //@formatter:off
    //position0は光源の位置を示す。position0[3]を0とすると、平行光源となる。
    //position1, position2につても同様である。
    //position0は、視点の背後から、見ている方向へのライトである。
    float[] position0      = { 0, 0, 10000, 0 };
    float[] spotDirection0 = { 0, 0, -1 };
    //position1は、視点の真上方向から、真下方向へのライトである。
    float[] position1      = { 0, 10000, 0, 0 };
    float[] spotDirection1 = { 0, -1, 0 };
    //position2は、視点の真下方向から、真上向へのライトである。
    float[] position2      = { 0, -10000, 0, 0 };
    float[] spotDirection2 = { 0, 1, 0 };
    
    //ライトの色である。
    float[] color0 = { 1.0f, 1.0f, 1.0f, 1.0f };
    float[] color1 = { 0.7f, 0.7f, 0.7f, 1.0f };
    float[] color2 = { 0.7f, 0.7f, 0.7f, 1.0f };
    //@formatter:on
    
    //法線を正規化する
    gl.glEnable(GL.GL_NORMALIZE);
    
    //glColor*で設定した色をambientとdiffuseとして用いる
    gl.glEnable(GL.GL_COLOR_MATERIAL);
    gl.glColorMaterial(GL.GL_FRONT, GL.GL_AMBIENT_AND_DIFFUSE);
    
    //ライトの設定
    gl.glLightfv(GL.GL_LIGHT0, GL.GL_POSITION, position0, 0);
    gl.glLightfv(GL.GL_LIGHT0, GL.GL_SPOT_DIRECTION, spotDirection0, 0);
    gl.glLightfv(GL.GL_LIGHT0, GL.GL_DIFFUSE, color0, 0);
    gl.glLightfv(GL.GL_LIGHT1, GL.GL_POSITION, position1, 0);
    gl.glLightfv(GL.GL_LIGHT1, GL.GL_SPOT_DIRECTION, spotDirection1, 0);
    gl.glLightfv(GL.GL_LIGHT1, GL.GL_DIFFUSE, color1, 0);
    gl.glLightfv(GL.GL_LIGHT2, GL.GL_POSITION, position2, 0);
    gl.glLightfv(GL.GL_LIGHT2, GL.GL_SPOT_DIRECTION, spotDirection2, 0);
    gl.glLightfv(GL.GL_LIGHT2, GL.GL_DIFFUSE, color2, 0);
    
    gl.glEnable(GL.GL_LIGHT0);
    gl.glEnable(GL.GL_LIGHT1);
    gl.glEnable(GL.GL_LIGHT2);
  }
  
  /**
   * 現在の視点のコピーを返す
   * @return コピーしたCameraクラスのインスタンス
   */
  public Camera getCopy() {
    Camera tmpCopy = new Camera(_drawable3DPanel);
    copy(this, tmpCopy);
    return tmpCopy;
  }
  
  /**
   * 視点の設定をコピーする。視点の復元に用いる。
   * @param source コピー元のインスタンス
   * @param target コピー先のインスタンス
   */
  public static void copy(Camera source, Camera target) {
    if (source != null && target != null) {
      target._panelIndex = source._panelIndex;
      target._eyePosition.x = source._eyePosition.x;
      target._eyePosition.y = source._eyePosition.y;
      target._eyePosition.z = source._eyePosition.z;
      target._eyeDirX.x = source._eyeDirX.x;
      target._eyeDirX.y = source._eyeDirX.y;
      target._eyeDirX.z = source._eyeDirX.z;
      target._eyeDirY.x = source._eyeDirY.x;
      target._eyeDirY.y = source._eyeDirY.y;
      target._eyeDirY.z = source._eyeDirY.z;
      target._eyeDirZ.x = source._eyeDirZ.x;
      target._eyeDirZ.y = source._eyeDirZ.y;
      target._eyeDirZ.z = source._eyeDirZ.z;
      target._moveForAxisRendering.x = source._moveForAxisRendering.x;
      target._moveForAxisRendering.y = source._moveForAxisRendering.y;
      target._moveForAxisRendering.z = source._moveForAxisRendering.z;
      target._scale = source._scale;
      target._isPerspective = source._isPerspective;
      MatrixUtils.copyMatrix(source._currentModelTransformation, target._currentModelTransformation);
      MatrixUtils.copyMatrix(source._currentModelTransformationForAxis, target._currentModelTransformationForAxis);
      MatrixUtils.copyMatrix(source._bufferMatrix1, target._bufferMatrix1);
      MatrixUtils.copyMatrix(source._bufferMatrix2, target._bufferMatrix2);
      MatrixUtils.copyMatrix(source._bufferMatrix3, target._bufferMatrix3);
      target._mouseX = source._mouseX;
      target._mouseY = source._mouseY;
      target._centerOfViewX = source._centerOfViewX;
      target._centerOfViewY = source._centerOfViewY;
      target._mode = source._mode;
    }
  }
  
  public void setIsPerspective(boolean isPerspective) {
    //透視射影と正射影で平行移動量の単位が異なるので、
    //その差を少なくして表示が大きく変化しないようにする
    //if (_isPerspective != isPerspective) {
    //  if (_isPerspective) {
    //    _centerOfViewX *= 1.0f * RATIO_CLIPPING_PLANE_FAR / RATIO_CLIPPING_PLANE_NEAR;
    //    _centerOfViewY *= 1.0f * RATIO_CLIPPING_PLANE_FAR / RATIO_CLIPPING_PLANE_NEAR;
    //  } else {
    //    _centerOfViewX /= 1.0f * RATIO_CLIPPING_PLANE_FAR / RATIO_CLIPPING_PLANE_NEAR;
    //    _centerOfViewY /= 1.0f * RATIO_CLIPPING_PLANE_FAR / RATIO_CLIPPING_PLANE_NEAR;
    //  }
    //}
    
    _isPerspective = isPerspective;
  }
  
  public void setIsLightEnabled(boolean isEnabled) {
    _isLightEnabled = isEnabled;
  }
  
  /**
  * モデル描画用に、変換行列をセットするなど視界の設定を行う。
  * @param glAutoDrawable GLのオブジェクト
  */
  public void setTransformationForModel(GLAutoDrawable glAutoDrawable) {
    final GL gl = glAutoDrawable.getGL();
    
    //########################################
    //視界を設定する
    //########################################
    gl.glMatrixMode(GL.GL_PROJECTION);
    //視界は、原点(0,0,0)にありZ軸負方向を見たまま動かさない
    gl.glLoadIdentity();
    
    float aspect = _drawable3DPanel.getAspectRatio(_panelIndex);
    //視界の中心から上端・下端までの距離
    float tmpHeightHalf = heightOnNearClippingPlane() / 2;
    
    //視界の中心から右端・左端までの距離
    float tmpWidthHalf = tmpHeightHalf * aspect;
    //近いほうのクリッピング面までの距離
    //float tmpNear = _radius * RATIO_CLIPPING_PLANE_NEAR;
    float tmpNear = _distanceToNearPlane;
    //遠いほうのクリッピング面までの距離
    //float tmpFar = _radius * RATIO_CLIPPING_PLANE_FAR;
    float tmpFar = _distanceToFarPlane;
    //視界の左端の、近いほうのクリッピング面でのX座標
    float tmpLeft = _centerOfViewX - tmpWidthHalf;
    //視界の右端の、近いほうのクリッピング面でのX座標
    float tmpRight = _centerOfViewX + tmpWidthHalf;
    //視界の下端の、近いほうのクリッピング面でのY座標
    float tmpBottom = _centerOfViewY - tmpHeightHalf;
    //視界の上端の、近いほうのクリッピング面でのY座標
    float tmpTop = _centerOfViewY + tmpHeightHalf;
    
    //視体積を設定する
    if (_isPerspective) {
      //透視射影用
      gl.glFrustum(tmpLeft, tmpRight, tmpBottom, tmpTop, tmpNear, tmpFar);
    } else {
      //正射影用
      //gl.glOrtho(tmpLeft, tmpRight, tmpBottom, tmpTop, tmpNear, tmpFar);
      gl.glOrtho(tmpLeft, tmpRight, tmpBottom, tmpTop, -tmpFar, tmpFar);
    }
    
    //########################################
    //モデルへの変換行列を設定する
    //########################################
    gl.glMatrixMode(GL.GL_MODELVIEW);
    
    gl.glLoadIdentity();
    if (_toInitializeLight) {
      initializeLight(glAutoDrawable);
      _toInitializeLight = false;
    }
    if (_isLightEnabled) {
      gl.glEnable(GL.GL_LIGHTING);
    } else {
      gl.glDisable(GL.GL_LIGHTING);
    }
    
    //変換行列をセットする
    gl.glLoadMatrixf(getModelViewMatrix(), 0);
    
  }
  
  /**
   * 座標軸描画用に、変換行列をセットするなど視界の設定を行う。
   * @param glAutoDrawable GLのオブジェクト
   */
  public void setTransformationForModelForAxisRendering(GLAutoDrawable glAutoDrawable) {
    final GL gl = glAutoDrawable.getGL();
    
    //########################################
    //視界を設定する
    //########################################
    gl.glMatrixMode(GL.GL_PROJECTION);
    //視界は、原点(0,0,0)にありZ軸負方向を見たまま動かさない
    gl.glLoadIdentity();
    
    float aspect = _drawable3DPanel.getAspectRatio(_panelIndex);
    float tmpHeight = 1.0f;
    if (_isPerspective) {
      tmpHeight = AXIS_RENDERING_SIZE * AXIS_RENDERING_HEIGHT / AXIS_RENDERING_DISTANCE * AXIS_RENDERING_RATIO_NEAR;
    } else {
      tmpHeight = AXIS_RENDERING_SIZE * AXIS_RENDERING_HEIGHT;
    }
    float tmpWidth = tmpHeight * aspect;
    
    float ratioHorizontal = AXIS_RENDERING_RATIO_LEFT + AXIS_RENDERING_RATIO_RIGHT;
    float ratioVertical = AXIS_RENDERING_RATIO_BOTTOM + AXIS_RENDERING_RATIO_TOP;
    float tmpLeft = -(tmpWidth * AXIS_RENDERING_RATIO_LEFT / ratioHorizontal);
    float tmpRight = tmpWidth * AXIS_RENDERING_RATIO_RIGHT / ratioHorizontal;
    float tmpBottom = -(tmpHeight * AXIS_RENDERING_RATIO_BOTTOM / ratioVertical);
    float tmpTop = tmpHeight * AXIS_RENDERING_RATIO_TOP / ratioVertical;
    float tmpNear = AXIS_RENDERING_SIZE * AXIS_RENDERING_RATIO_NEAR;
    float tmpFar = AXIS_RENDERING_SIZE * AXIS_RENDERING_RATIO_FAR;
    
    //視体積を設定する
    if (_isPerspective) {
      //透視射影用
      gl.glFrustum(tmpLeft, tmpRight, tmpBottom, tmpTop, tmpNear, tmpFar);
    } else {
      //正射影用
      gl.glOrtho(tmpLeft, tmpRight, tmpBottom, tmpTop, tmpNear, tmpFar);
    }
    
    //########################################
    //回転行列を設定する
    //########################################
    gl.glMatrixMode(GL.GL_MODELVIEW);
    
    gl.glLoadMatrixf(getModelViewMatrixForAxisRendering(), 0);
  }
  
  /**
   * 2D描画用の視界の設定を行う。画面左下が(0,0)、画面右上が(width,height)。
   * 座標軸の向きは、X軸(水平方向)が右向き、Y軸(垂直方向)が上向きである。
   * Y軸は通常の2D描画とは逆向きとしている。
   * @param glAutoDrawable GLのオブジェクト
   */
  public void setTransformationFor2DRendering(GLAutoDrawable glAutoDrawable) {
    final GL gl = glAutoDrawable.getGL();
    
    int width = _drawable3DPanel.getWidth(_panelIndex);
    int height = _drawable3DPanel.getHeight(_panelIndex);
    
    float distanceNear = 1;
    float distanceFar = 2;//この値は特に意味はない。distanceNearより大きければ良い
    gl.glMatrixMode(GL.GL_PROJECTION);
    gl.glLoadIdentity();
    gl.glFrustum(0, width, 0, height, distanceNear, distanceFar);
    
    gl.glMatrixMode(GL.GL_MODELVIEW);
    gl.glLoadIdentity();
    gl.glTranslatef(0, 0, -distanceNear);
    
  }
  
  /**
   * 座標軸のラベルを描画する位置を調整するための値である。
   * @return ラベルの描画位置に使用する値
   */
  public static float getCoefficientForAxisLabel() {
    float coeff = 1.0f / AXIS_RENDERING_HEIGHT;
    return coeff;
  }
  
  /**
   * モデル描画用の変換行列を返す。
   * @return 変換行列
   */
  private float[] getModelViewMatrix() {
    //静止座標系から、視界の座標系への変換を表す行列を計算する。
    //静止座標系の原点をO、視界の座標系の原点(視点の位置)をOeとする。
    //視界の座標系でのXYZの各方向の単位ベクトルを
    //Xe, Ye, Zeとする。
    //任意の点Pについて、
    //OP = OOe + r*Xe + s*Ye + t*Ze   (OP, OOeは)
    //と表したときのr, s, tが視界の座標系での座標値となる。
    //Pの座標値を、静止座標系では(x,y,z)とし、視界の座標系では(r,s,t)eとする。
    //(x,y,z)から(r,s,t)eへの変換行列を計算する。
    //OeP = r*Xe + s*Ye + t*Ze であるので、
    //r = OeP.Xe, s=OeP.Ye, t=OeP.Ze となる。
    //(.はベクトルの内積を表す。Xe.Xe=Ye.Ye=Ze.Ze=1を使った。)
    //静止座標系で各点、ベクトルの成分表したとき
    //Oe=(x0,y0,z0), Xe=(Xex,Xey,Xez), Ye=(Yex,Yey,Yez), Ze=(Zex,Zey,Zez) であるとすると、
    //r,s,t は4x4行列を用いて以下のように表せる。
    // |r|   |OeP.Xe|   |Xex Xey Xez 0| |x-x0|   |Xex Xey Xez 0| |1 0 0 -x0| |x|
    // |s| = |OeP.Ye| = |Yex Yey Yez 0|*|y-y0| = |Yex Yey Yez 0|*|0 1 0 -y0|*|y|
    // |t|   |OeP.Ze|   |Zex Zey Zez 0| |z-z0|   |Zex Zey Zez 0| |0 0 1 -z0| |z|
    // |1|   |   1  |   | 0   0   0  1| |  1 |   | 0   0   0  1| |0 0 0  1 | |1|
    //                                              回転行列      平行移動
    
    MatrixUtils.setIdentityMatrix(_bufferMatrix1);
    MatrixUtils.translateMinus(_bufferMatrix1, _eyePosition);
    
    setRotateToMatrix(_bufferMatrix2);
    
    MatrixUtils.multiply(_bufferMatrix1, _bufferMatrix2, _currentModelTransformation);
    
    MatrixUtils.copyMatrix(_currentModelTransformation, _bufferMatrix1);
    
    return _bufferMatrix1;
  }
  
  /**
   * 座標軸描画のための変換行列を返す。
   * モデル描画用の変換行列のうち、回転部分のみの行列を取り出し、
   * 位置調節のための平行移動を行った行列を返す。
   * @return 変換行列
   */
  private float[] getModelViewMatrixForAxisRendering() {
    setRotateToMatrix(_currentModelTransformationForAxis);
    
    MatrixUtils.copyMatrix(_currentModelTransformationForAxis, _bufferMatrix1);
    MatrixUtils.translateMinus(_bufferMatrix1, _moveForAxisRendering);
    
    return _bufferMatrix1;
  }
  
  private void setRotateToMatrix(float[] target) {
    //視界の座標系でのXYZ方向の単位ベクトルを
    //Xe=(Xex,Xey,Xez), Ye=(Yex,Yey,Yez), Ze=(Zex,Zey,Zez) とする。
    //視界においては、Xeは画面奥側、Yeは左、Zeは上方向とする
    //OpenGLでは、X軸が右、Y軸が上、Z軸が画面手前側なので、変換が必要である。
    //(Xe,Ye,Ze) -> (-Ye, Ze, -Xe)と変換を行う。
    // | 0 -1 0 0| |Xex Xey Xez 0|   |-Yex -Yey -Yez 0|
    // | 0  0 1 0|*|Yex Yey Yez 0| = | Zex  Zey  Zez 0|
    // |-1  0 0 0| |Zex Zey Zez 0|   |-Xex -Xey -Xez 0|
    // | 0  0 0 1| | 0   0   0  1|   |  0    0    0  1|
    //
    MatrixUtils.setIdentityMatrix(target);
    //target[0] = _eyeDirX.x;
    //target[1] = _eyeDirY.x;
    //target[2] = _eyeDirZ.x;
    //target[4] = _eyeDirX.y;
    //target[5] = _eyeDirY.y;
    //target[6] = _eyeDirZ.y;
    //target[8] = _eyeDirX.z;
    //target[9] = _eyeDirY.z;
    //target[10] = _eyeDirZ.z;
    
    //@formatter:off
    //X->-Z, Y->-X, Z->Y
    target[ 0] = -_eyeDirY.x;
    target[ 1] =  _eyeDirZ.x;
    target[ 2] = -_eyeDirX.x;
    target[ 4] = -_eyeDirY.y;
    target[ 5] =  _eyeDirZ.y;
    target[ 6] = -_eyeDirX.y;
    target[ 8] = -_eyeDirY.z;
    target[ 9] =  _eyeDirZ.z;
    target[10] = -_eyeDirX.z;
    //@formatter:on
  }
  
  public void setViewPointOnVehicle(float[] params) {
    float x = params[0];
    float y = params[1];
    float z = params[2];
    float angleXY = params[3];
    float angleZ = params[4];
    _vehiclePosition.init(x, y, z);
    _vehicleRotateXY = angleXY;
    _vehicleRotateZ = angleZ;
    
    reset();
  }
  
  public void mouseClicked(MouseEvent e) {
  }
  
  public void mousePressed(MouseEvent e) {
    _mouseX = e.getX();
    _mouseY = e.getY();
    
    if (SwingUtilities.isLeftMouseButton(e)) {
      if (_pressedKey == KeyEvent.VK_C) {
      } else if (_pressedKey == KeyEvent.VK_S) {
        _mode = MODE_SCALE;
      } else if (_pressedKey == KeyEvent.VK_T) {
        _mode = MODE_TRANSLATE;
      } else if (_pressedKey == KeyEvent.VK_H) {
        _mode = MODE_ROTATE_HORIZONTAL;
      } else if (_pressedKey == KeyEvent.VK_V) {
        _mode = MODE_ROTATE_VERTICAL;
      } else {
        if (e.isShiftDown()) {
        } else {
          _mode = MODE_ROTATE;
        }
      }
    } else if (SwingUtilities.isRightMouseButton(e)) {
      if (e.isShiftDown()) {
        //_mode = MODE_MOVE_CENTER_OF_ROTATION;
      } else {
        _mode = MODE_TRANSLATE;
      }
    } else {
      if (e.isShiftDown()) {
      } else {
        _mode = MODE_SCALE;
      }
    }
    _drawable3DPanel.setIndexOfActive3DPanel(_panelIndex);
  }
  
  public void mouseReleased(MouseEvent e) {
    _mode = MODE_NORMAL;
  }
  
  public void mouseEntered(MouseEvent e) {
  }
  
  public void mouseExited(MouseEvent e) {
  }
  
  public void mouseDragged(MouseEvent e) {
    int tmpX = e.getX();
    int tmpY = e.getY();
    //マウスの横方向移動量。画面右が正
    int moveX = tmpX - _mouseX;
    //マウスの縦方向移動量。画面上が正
    int moveY = _mouseY - tmpY;
    
    if (_mode == MODE_ROTATE || _mode == MODE_ROTATE_HORIZONTAL || _mode == MODE_ROTATE_VERTICAL) {
      if (_modeViewPoint == Drawable3DPanel.MODE_VIEW_POINT_BIRD_EYE) {
        rotateAroundPoint(-moveX, -moveY, _mode);
      }
      if (_modeViewPoint == Drawable3DPanel.MODE_VIEW_POINT_WALK_THROUGH) {
        rotateConsideringScale(moveX, moveY, _mode);
      }
      if (_modeViewPoint == Drawable3DPanel.MODE_VIEW_POINT_ON_VEHICLE) {
        rotateConsideringScale(moveX, moveY, _mode);
      }
      _drawable3DPanel.reDraw();
    } else if (_mode == MODE_SCALE) {
      scale(moveX, moveY);
      _drawable3DPanel.reDraw();
    } else if (_mode == MODE_TRANSLATE) {
      translate(moveX, moveY);
      _drawable3DPanel.reDraw();
    }
    _mouseX = tmpX;
    _mouseY = tmpY;
  }
  
  public void mouseMoved(MouseEvent e) {
  }
  
  public void mouseWheelMoved(MouseWheelEvent e) {
    //TODO ホイール回転の向きと倍率の変更量の調整
    if (_mode == MODE_NORMAL) {
      int tmpScroll = e.getUnitsToScroll();
      scale(-tmpScroll * MOUSE_SENSITIVITY_SCALE_BY_WHEEL);
      _drawable3DPanel.reDraw();
    }
  }
  
  public void keyTyped(KeyEvent e) {
  }
  
  public void keyPressed(KeyEvent e) {
    _pressedKey = e.getKeyCode();
    
    float moveAmount = 10.0f;
    if (e.isShiftDown()) {
      moveAmount = 5.0f;
    } else if (e.isControlDown()) {
      moveAmount = 1.0f;
    }
    
    boolean toReDraw = true;
    if (_pressedKey == KeyEvent.VK_R) {
      //reset();
      _drawable3DPanel.resetCamera(_panelIndex);
    } else if (_pressedKey == KeyEvent.VK_N) {
      _drawable3DPanel.nextTime();
    } else if (_pressedKey == KeyEvent.VK_P) {
      _drawable3DPanel.previousTime();
    } else if (_pressedKey == KeyEvent.VK_X) {
      moveX(moveAmount);
    } else if (_pressedKey == KeyEvent.VK_Z) {
      moveX(-moveAmount);
    } else if (_pressedKey == KeyEvent.VK_S) {
      moveY(moveAmount);
    } else if (_pressedKey == KeyEvent.VK_A) {
      moveY(-moveAmount);
    } else if (_pressedKey == KeyEvent.VK_W) {
      moveZ(-moveAmount);
    } else if (_pressedKey == KeyEvent.VK_Q) {
      moveZ(moveAmount);
    } else if (_pressedKey == KeyEvent.VK_J) {
      rotateAroundPoint(-3, 0, MODE_ROTATE);
    } else if (_pressedKey == KeyEvent.VK_K) {
      rotateAroundPoint(3, 0, MODE_ROTATE);
    } else if (_pressedKey == KeyEvent.VK_U) {
      rotateAroundPoint(0, 3, MODE_ROTATE);
    } else if (_pressedKey == KeyEvent.VK_I) {
      rotateAroundPoint(0, -3, MODE_ROTATE);
    } else if (_pressedKey == KeyEvent.VK_LEFT) {
      rotateConsideringScale(1, 0, MODE_ROTATE);
    } else if (_pressedKey == KeyEvent.VK_RIGHT) {
      rotateConsideringScale(-1, 0, MODE_ROTATE);
    } else if (_pressedKey == KeyEvent.VK_UP) {
      rotateConsideringScale(0, -1, MODE_ROTATE);
    } else if (_pressedKey == KeyEvent.VK_DOWN) {
      rotateConsideringScale(0, 1, MODE_ROTATE);
    } else {
      toReDraw = false;
    }
    
    if (toReDraw) {
      _drawable3DPanel.reDraw();
    }
  }
  
  public void keyReleased(KeyEvent e) {
    _pressedKey = -1;
  }
  
  private void moveX(float move) {
    _eyePosition.add(_eyeDirX, move);
  }
  
  private void moveY(float move) {
    _eyePosition.add(_eyeDirY, move);
  }
  
  private void moveZ(float move) {
    _eyePosition.add(_eyeDirZ, move);
  }
  
  /**
   * 回転操作
   * @param x 画面横方向の移動量
   * @param y 画面縦方向の移動量
   */
  private void rotateConsideringScale(int x, int y, int mode) {
    int width = _drawable3DPanel.getWidth(_panelIndex);
    int height = _drawable3DPanel.getHeight(_panelIndex);
    float tmpCoefficient = (float) (_scale / height * Math.PI * 2);
    if (mode == MODE_ROTATE || mode == MODE_ROTATE_VERTICAL) {
      //水平な直線を軸とした、縦方向の回転
      rotate(ROTATE_Y, y * tmpCoefficient * MOUSE_SENSITIVITY_ROTATE);
    }
    if (mode == MODE_ROTATE || mode == MODE_ROTATE_HORIZONTAL) {
      //鉛直な直線を軸とした、横方向の回転
      rotate(ROTATE_Z, x * tmpCoefficient * MOUSE_SENSITIVITY_ROTATE);
    }
  }
  
  /**
   * ある点を中心にして回転する
   * @param x 横方向の回転量
   * @param y 縦方向の回転量
   * @param mode モードを表す値
   */
  private void rotateAroundPoint(int x, int y, int mode) {
    int width = _drawable3DPanel.getWidth(_panelIndex);
    int height = _drawable3DPanel.getHeight(_panelIndex);
    float tmpCoefficient = (float) (_scale / height * Math.PI * 2);
    if (mode == MODE_ROTATE || mode == MODE_ROTATE_VERTICAL) {
      //水平な直線を軸とした、縦方向の回転
      float angle = y * tmpCoefficient * MOUSE_SENSITIVITY_ROTATE;
      float sin = (float) (Math.sin(angle) * _distanceToCenterOfRotation);
      float cos = (float) (Math.cos(angle) * _distanceToCenterOfRotation);
      //回転前の方向ベクトルをコピーしておく
      _bufferPoint3f_1.copy(_eyeDirX);
      _bufferPoint3f_2.copy(_eyeDirZ);
      //回転
      rotate(ROTATE_Y, angle);
      //回転中心が同じ点になるように、視点の位置を移動する
      _eyePosition.add(_bufferPoint3f_1, _distanceToCenterOfRotation - cos);
      _eyePosition.add(_bufferPoint3f_2, sin);
    }
    if (mode == MODE_ROTATE || mode == MODE_ROTATE_HORIZONTAL) {
      //鉛直な直線を軸とした、横方向の回転
      float angle = x * tmpCoefficient * MOUSE_SENSITIVITY_ROTATE;
      float sin = (float) (Math.sin(angle) * _distanceToCenterOfRotation);
      float cos = (float) (Math.cos(angle) * _distanceToCenterOfRotation);
      //回転前の方向ベクトルをコピーしておく
      _bufferPoint3f_1.copy(_eyeDirX);
      _bufferPoint3f_2.copy(_eyeDirY);
      //回転
      rotate(ROTATE_Z, angle);
      //回転中心が同じ点になるように、視点の位置を移動する
      _eyePosition.add(_bufferPoint3f_1, _distanceToCenterOfRotation - cos);
      _eyePosition.add(_bufferPoint3f_2, -sin);
    }
  }
  
  /**
   * 変換行列に、X軸(画面右が正)、Y軸(画面上が正)、Z軸(画面手前が正)の
   * いずれかを軸とした回転を表す行列を左から乗算する。
   * @param axis 回転軸を指定する。
   *             指定する値は、MatrixUtils.ROTATE_X, MatrixUtils.ROTATE_Y, MatrixUtils.ROTATE_Zのいずれか。
   * @param radian 回転量を指定する。単位はラジアン。指定した軸の正方向を向いて、時計回りが正の角度。
   */
  public void rotate(int axis, float radian) {
    Point3f vector1 = null;
    Point3f vector2 = null;
    if (axis == ROTATE_X) {
      vector1 = _eyeDirY;
      vector2 = _eyeDirZ;
    } else if (axis == ROTATE_Y) {
      vector1 = _eyeDirZ;
      vector2 = _eyeDirX;
    } else if (axis == ROTATE_Z) {
      vector1 = _eyeDirX;
      vector2 = _eyeDirY;
    }
    if (vector1 != null && vector2 != null) {
      float cos = (float) (Math.cos(radian));
      float sin = (float) (Math.sin(radian));
      float after1X = vector1.x * cos + vector2.x * sin;
      float after1Y = vector1.y * cos + vector2.y * sin;
      float after1Z = vector1.z * cos + vector2.z * sin;
      float after2X = vector1.x * (-sin) + vector2.x * cos;
      float after2Y = vector1.y * (-sin) + vector2.y * cos;
      float after2Z = vector1.z * (-sin) + vector2.z * cos;
      
      vector1.x = after1X;
      vector1.y = after1Y;
      vector1.z = after1Z;
      vector2.x = after2X;
      vector2.y = after2Y;
      vector2.z = after2Z;
    }
  }
  
  /**
   * 拡大倍率を変更する
   * @param x 画面横方向の移動量
   * @param y 画面縦方向の移動量
   */
  private void scale(int x, int y) {
    scale((x + y) * MOUSE_SENSITIVITY_SCALE);
  }
  
  /**
   * 拡大倍率を変更する
   * @param scale 倍率
   */
  private void scale(float scale) {
    double tmpLog = Math.log(_scale);
    tmpLog += scale;
    setScale((float) (Math.exp(tmpLog)));
  }
  
  private void setScale(float scale) {
    _scale = scale;
    if (_scale < SCALE_MINIMUM) _scale = SCALE_MINIMUM;
    if (_scale > SCALE_MAXIMUM) _scale = SCALE_MAXIMUM;
  }
  
  /**
   * 表示領域を移動する
   * @param x 横方向の移動量
   * @param y 縦方向の移動量
   */
  private void translate(int x, int y) {
    int width = _drawable3DPanel.getWidth(_panelIndex);
    int height = _drawable3DPanel.getHeight(_panelIndex);
    //画面の1ドットが、近いほうのクリッピング面上でどれだけの長さになるかを表す
    float tmpUnit = heightOnNearClippingPlane() / height;
    translateX(-x * MOUSE_SENSITIVITY_TRANSLATE * tmpUnit);
    translateY(-y * MOUSE_SENSITIVITY_TRANSLATE * tmpUnit);
  }
  
  private void translateX(float moveX) {
    _centerOfViewX += moveX;
  }
  
  private void translateY(float moveY) {
    _centerOfViewY += moveY;
  }
  
  /**
   * 近いほうのクリッピング面上で、表示領域がどれだけの高さであるかを表す
   * @return 表示領域の高さ
   */
  private float heightOnNearClippingPlane() {
    float tmpUnit = 1.0f;
    tmpUnit = _scale * _distanceToNearPlane;
    
    return tmpUnit;
  }
  
  public String[] getViewPointConfiguration() {
    ArrayList<String> listConfigurations = new ArrayList<String>();
    String label = null;
    String configuration = null;
    String SPACE = Common.SPACE;
    String EQUAL = Common.EQUAL;
    String COMMA = Common.COMMA;
    
    label = LABEL_CONFIGURATION_VIEW_POINT_POSITION;
    configuration = label + EQUAL + _eyePosition.x + COMMA + _eyePosition.y + COMMA + _eyePosition.z;
    listConfigurations.add(configuration);
    
    label = LABEL_CONFIGURATION_VIEW_POINT_DIRECTION_X;
    configuration = label + EQUAL + _eyeDirX.x + COMMA + _eyeDirX.y + COMMA + _eyeDirX.z;
    listConfigurations.add(configuration);
    
    label = LABEL_CONFIGURATION_VIEW_POINT_DIRECTION_Y;
    configuration = label + EQUAL + _eyeDirY.x + COMMA + _eyeDirY.y + COMMA + _eyeDirY.z;
    listConfigurations.add(configuration);
    
    label = LABEL_CONFIGURATION_VIEW_POINT_DIRECTION_Z;
    configuration = label + EQUAL + _eyeDirZ.x + COMMA + _eyeDirZ.y + COMMA + _eyeDirZ.z;
    listConfigurations.add(configuration);
    
    label = LABEL_CONFIGURATION_CENTER_OF_VIEW;
    configuration = label + EQUAL + _centerOfViewX + COMMA + _centerOfViewY;
    listConfigurations.add(configuration);
    
    label = LABEL_CONFIGURATION_SCALE;
    configuration = label + EQUAL + _scale;
    listConfigurations.add(configuration);
    
    //文字列の配列を結合する
    String[] configurations = Common.toStringArray(listConfigurations);
    
    return configurations;
    
  }
  
  /**
   * 視点を設定する。
   * @param commandRaw 設定の種類を表す文字列
   * @param params 設定する数値を表す文字列の配列
   */
  public void setViewPointConfiguration(String commandRaw, String[] params) {
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
      if (command.equalsIgnoreCase(LABEL_CONFIGURATION_VIEW_POINT_POSITION)) {
        //################################
        //視点の位置
        if (paramValuesFloat != null && paramValuesFloat.length >= 3) {
          _eyePosition.init(paramValuesFloat[0], paramValuesFloat[1], paramValuesFloat[2]);
        }
      } else if (command.equalsIgnoreCase(LABEL_CONFIGURATION_VIEW_POINT_DIRECTION_X)) {
        //################################
        //視界の座標系におけるX軸を表す方向
        if (paramValuesFloat != null && paramValuesFloat.length >= 3) {
          _eyeDirX.init(paramValuesFloat[0], paramValuesFloat[1], paramValuesFloat[2]);
        }
      } else if (command.equalsIgnoreCase(LABEL_CONFIGURATION_VIEW_POINT_DIRECTION_Y)) {
        //################################
        //視界の座標系におけるY軸を表す方向
        if (paramValuesFloat != null && paramValuesFloat.length >= 3) {
          _eyeDirY.init(paramValuesFloat[0], paramValuesFloat[1], paramValuesFloat[2]);
        }
      } else if (command.equalsIgnoreCase(LABEL_CONFIGURATION_VIEW_POINT_DIRECTION_Z)) {
        //################################
        //視界の座標系におけるZ軸を表す方向
        if (paramValuesFloat != null && paramValuesFloat.length >= 3) {
          _eyeDirZ.init(paramValuesFloat[0], paramValuesFloat[1], paramValuesFloat[2]);
        }
      } else if (command.equalsIgnoreCase(LABEL_CONFIGURATION_CENTER_OF_VIEW)) {
        //################################
        //表示している範囲の中心の、近いクリッピング面における座標
        if (paramValuesFloat != null && paramValuesFloat.length >= 2) {
          _centerOfViewX = paramValuesFloat[0];
          _centerOfViewY = paramValuesFloat[1];
        }
      } else if (command.equalsIgnoreCase(LABEL_CONFIGURATION_SCALE)) {
        //################################
        //表示倍率
        if (paramValuesFloat != null && paramValuesFloat.length >= 1) {
          _scale = paramValuesFloat[0];
        }
      }
    }
  }
}
