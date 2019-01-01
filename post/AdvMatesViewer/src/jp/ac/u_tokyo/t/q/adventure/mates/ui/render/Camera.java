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
 * 3D��ʂ̎��_���Ǘ�����N���X<br>
 * ���_��Z�����̕����������܂ܓ������Ȃ��B
 * ��]�̓��f���𓮂����B
 * �g��k���E���s�ړ��́A�J�������猩����͈͂�ύX���邱�ƂŎ�������B
 */
public class Camera implements MouseListener, MouseMotionListener, MouseWheelListener, KeyListener {
  //@formatter:off
  /**
   * �}�E�X����̏�Ԃ������B���삳��ĂȂ���ԁB
   */
  private static final int MODE_NORMAL                              = 10;
  /**
   * �}�E�X����̏�Ԃ������B��]���쒆
   */
  private static final int MODE_ROTATE                              = 20;
  /**
   * �}�E�X����̏�Ԃ������B��]���쒆�B���������݂̂̉�]���s���B
   */
  private static final int MODE_ROTATE_HORIZONTAL                   = 21;
  /**
   * �}�E�X����̏�Ԃ������B��]���쒆�B���������݂̂̉�]���s���B
   */
  private static final int MODE_ROTATE_VERTICAL                     = 22;
  /**
   * �}�E�X����̏�Ԃ������B�ړ����쒆
   */
  private static final int MODE_TRANSLATE                           = 30;
  /**
   * �}�E�X����̏�Ԃ������B�{���ύX��
   */
  private static final int MODE_SCALE                               = 40;
  /**
   * �}�E�X����̊��x�B��]����p
   */
  private static final float MOUSE_SENSITIVITY_ROTATE               = 3.0f;
  /**
   * �}�E�X����̊��x�B�ړ�����p�B1.0f���ƁA�}�E�X�����3D�\�����قڒǐ�����
   */
  private static final float MOUSE_SENSITIVITY_TRANSLATE            = 1.0f;
  /**
   * �}�E�X����̊��x�B�{���ύX�p
   */
  private static final float MOUSE_SENSITIVITY_SCALE                = 0.01f;
  /**
   * �}�E�X����̊��x�B�}�E�X�z�C�[���ł̔{���ύX�p
   */
  private static final float MOUSE_SENSITIVITY_SCALE_BY_WHEEL       = 0.01f;
  /**
   * �f�t�H���g�ł̕\���{��
   */
  private static final float SCALE_DEFAULT                          = 1.0f;
  /**
   * �\���{���̍ŏ��l�ł���B
   */
  private static final float SCALE_MINIMUM                          = 1.0e-6f;
  /**
   * �\���{���̍ő�l�ł���B
   */
  private static final float SCALE_MAXIMUM                          = 1.0e6f;
  /**
   * X������̉�]��\���ԍ��ł���B
   */
  public static final int    ROTATE_X                               = 1;
  /**
   * Y������̉�]��\���ԍ��ł���B
   */
  public static final int    ROTATE_Y                               = 2;
  /**
   * Z������̉�]��\���ԍ��ł���B
   */
  public static final int    ROTATE_Z                               = 3;
  /**
   * ���W���`��p�̒l�B���W���̃T�C�Y
   */
  public  static final float AXIS_RENDERING_SIZE                          = 1.0f;
  /**
   * ���W���`��p�̒l�B���W���̌�_(���_)�܂ł̋����̔䗦
   */
  private static final float AXIS_RENDERING_DISTANCE                      = 3.0f;
  /**
   * ���W���`��p�̒l�B���_����߂��N���b�s���O�ʂ܂ł̋����̔䗦
   */
  private static final float AXIS_RENDERING_RATIO_NEAR                    = 1.0f;
  /**
   * ���W���`��p�̒l�B���_���牓���N���b�s���O�ʂ܂ł̋����̔䗦
   */
  private static final float AXIS_RENDERING_RATIO_FAR                     = 5.0f;
  /**
   * ���W���`��p�̒l�B���W���̌�_(���_)�̈ʒu�ł̎��̐ς̍����̔䗦
   */
  private static final float AXIS_RENDERING_HEIGHT                        = 10.0f;
  /**
   * ���W���`��p�̒l�B���W���̌�_(���_)�����ʍ��[�܂ł̔䗦
   */
  private static final float AXIS_RENDERING_RATIO_LEFT                    = 1.0f;
  /**
   * ���W���`��p�̒l�B���W���̌�_(���_)�����ʉE�[�܂ł̔䗦
   */
  private static final float AXIS_RENDERING_RATIO_RIGHT                   = 7.0f;
  /**
   * ���W���`��p�̒l�B���W���̌�_(���_)�����ʉ��[�܂ł̔䗦
   */
  private static final float AXIS_RENDERING_RATIO_BOTTOM                  = 1.0f;
  /**
   * ���W���`��p�̒l�B���W���̌�_(���_)�����ʏ�[�܂ł̔䗦
   */
  private static final float AXIS_RENDERING_RATIO_TOP                     = 4.0f;
  /**
   * �������̃N���b�s���O���ʂ̈ʒu�����߂�W���ł���B
   */
  private static final float COEFFICIENT_FOR_FAR_PLANE                    = 1000.0f;
  /**
   * �w�肵���p�x������]������Ƃ��́A�f�t�H���g�̉�]��
   */
  public static final float  DEFAULT_DEGREE_FOR_ROTATION                  = 30.0f;
  /**
   * �߂����̃N���b�s���O���ʂ܂ł́A�f�t�H���g�̋����ł���B
   */
  private static final float DEFAULT_DISTANCE_TO_NEAR_PLANE               = 1.0f;
  /**
   * �E�H�[�N�X���[���_�̏ꍇ�́A�߂����̃N���b�s���O�ʂ܂ł̋����ł���B
   * �f�t�H���g�l�ł͂Ȃ��A�Œ�l�Ƃ��Ďg�p����B
   */
  private static final float DISTANCE_TO_NEAR_PLANE_FOR_MODE_WALK_THROUGH = 1.0f;
  /**
   * �ԗ�����̎��_�̏ꍇ�́A�߂����̃N���b�s���O�ʂ܂ł̋����ł���B
   * �f�t�H���g�l�ł͂Ȃ��A�Œ�l�Ƃ��Ďg�p����B
   */
  private static final float DISTANCE_TO_NEAR_PLANE_FOR_MODE_ON_VEHICLE   = 1.0f;
  /**
   * �E�H�[�N�X���[���_�̏ꍇ�́A�n�ʂƎ��_�Ƃ̋����ł���B
   * �f�t�H���g�l�ł͂Ȃ��A�Œ�l�Ƃ��Ďg�p����B
   */
  private static final float DISTANCE_FROM_GROUND_FOR_MODE_WALK_THROUGH   = 1.0f;
  /**
   * ��]���S�̓_�܂ł́A�f�t�H���g�̋����ł���B
   */
  private static final float DEFAULT_DISTANCE_TO_CENTER_OF_ROTATION       = 10.0f;
  /**
   * �\���̐ݒ��\�����߂̕�����
   */
  public static final String  LABEL_CONFIGURATION_VIEW_POINT_POSITION     = "ViewPointPosition";
  public static final String  LABEL_CONFIGURATION_VIEW_POINT_DIRECTION_X  = "ViewPointDirectionX";
  public static final String  LABEL_CONFIGURATION_VIEW_POINT_DIRECTION_Y  = "ViewPointDirectionY";
  public static final String  LABEL_CONFIGURATION_VIEW_POINT_DIRECTION_Z  = "ViewPointDirectionZ";
  public static final String  LABEL_CONFIGURATION_CENTER_OF_VIEW          = "CenterOfView";
  public static final String  LABEL_CONFIGURATION_SCALE                   = "Scale";
  //@formatter:on
  /**
   * ���O�n���h�����O�N���X
   */
  private Logger _logger;
  /**
   * �n�ʂ̒��S�ʒu�ł���B
   */
  private Point3f _centerOfGround;
  /**
   * �}�b�v�̍L���������l�ł���B
   */
  private float _radiusOfGround;
  /**
   * ���_�̏����ʒu�ł���B
   */
  private Point3f _eyePositionInitial;
  /**
   * �߂����̃N���b�s���O���ʂ܂ł̋����ł���B
   */
  private float _distanceToNearPlane;
  private float _distanceToNearPlaneForModeBirdEye;
  private float _distanceToNearPlaneForModeWalkThrough;
  private float _distanceToNearPlaneForModeOnVehicle;
  /**
   * �������̃N���b�s���O���ʂ܂ł̋����ł���B
   */
  private float _distanceToFarPlane;
  /**
   * ����_�̎���ɉ�]���郂�[�h�ł́A��]���S�܂ł̋����ł���B
   * ��]���S�́A���_�̐��ʕ����ɂ�����̂Ƃ���B
   */
  private float _distanceToCenterOfRotation;
  /**
   * ���_�̈ʒu�ł���B
   */
  private Point3f _eyePosition;
  /**
   * ���E�̍��W�n�ł�X�������̒P�ʃx�N�g���ł���B
   */
  private Point3f _eyeDirX;
  /**
   * ���E�̍��W�n�ł�Y�������̒P�ʃx�N�g���ł���B
   */
  private Point3f _eyeDirY;
  /**
   * ���E�̍��W�n�ł�Z�������̒P�ʃx�N�g���ł���B
   */
  private Point3f _eyeDirZ;
  /**
   * ���W����`�悷��ۂ̕��s�ړ���
   */
  private Point3f _moveForAxisRendering;
  /**
   * �w�肵��ID�̎ԗ��̈ʒu�ł���B
   */
  private Point3f _vehiclePosition;
  /**
   * �w�肵��ID�̎ԗ���XY�����̉�]�ł���B
   */
  private float _vehicleRotateXY;
  /**
   * �w�肵��ID�̎ԗ���Z�����̉�]�ł���B
   */
  private float _vehicleRotateZ;
  /**
   * �\���̔{���B��ʂɕ\������̈�̑傫����\���Ă���̂ŁA���̒l���������قǃ��f���͑傫���\�������B
   */
  private float _scale;
  /**
   * true�̂Ƃ��������e�@�Afalse�̂Ƃ����ˉe
   */
  private boolean _isPerspective;
  /**
   * ���f���`��̂��߂̕ϊ��s��ɑ�������z��ł���B
   */
  private float[] _currentModelTransformation;
  /**
   * ���f���`��̂��߂̕ϊ��s��̂����A��]�����݂̂����o�����s��ł���B
   * ���W���̕ϊ���̌�����\���B
   * �`��ɗp����s��Ƃ͏����قȂ�B
   */
  private float[] _currentModelTransformationForAxis;
  /**
   * �s��v�Z�̂��߂ɗp����z��ł���B
   * new����񐔂��Ȃ����߃����o�Ƃ��Ă����B
   */
  private float[] _bufferMatrix1;
  private float[] _bufferMatrix2;
  private float[] _bufferMatrix3;
  /**
   * �}�E�X��X���W
   */
  private int _mouseX;
  /**
   * �}�E�X��Y���W
   */
  private int _mouseY;
  /**
   * �\�����Ă���͈͂̒��S�́A�߂��N���b�s���O�ʂł�X���W
   */
  private float _centerOfViewX;
  /**
   * �\�����Ă���͈͂̒��S�́A�߂��N���b�s���O�ʂł�Y���W
   */
  private float _centerOfViewY;
  /**
   * �}�E�X����ł̃��[�h�B
   */
  private int _mode;
  /**
   * true�̂Ƃ��A���C�g��ON�ɂ���O�Ƀ��C�g������������
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
   * ���f���̒��S�ƃT�C�Y��^���Ď��_������������B
   * @param positionX ���f���̒��S��X���W
   * @param positionY ���f���̒��S��Y���W
   * @param positionZ ���f���̒��S��Z���W
   * @param radius ���f���̑傫����\���l
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
   * ���_������������
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
      //�S�̂̐^�ォ��̎��E�ŁAX�����E�����ɂȂ�悤�ɉ�]����
      //���E�̍��W�n�͌Œ�ŁA���͂̕�����]������B
      //��]�O�́A�S�̍��W�n�Ǝ��E���W�n�̌����͈�v���Ă���B
      //�܂��A���E���W�n�ł̉������ł���Y������ɉ�]���A
      //�S�̍��W�n��Z�����A�������O�Ɍ������悤�ɂ���B
      float degree = 90;
      float radian = Common.toRadianFromDegree(degree);
      rotate(ROTATE_Y, radian);
      //���ɁA���E���W�n�ł̉�ʂɐ�����X������ɉ�]���A
      //�S�̍��W�n��X�����E�������悤�ɂ���B
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
      //�ԗ��̉�]�ʂ𔽉f������
      rotate(ROTATE_Y, -_vehicleRotateZ);
      rotate(ROTATE_Z, _vehicleRotateXY);
    }
    
    //�\���{���Ƀf�t�H���g�l���Z�b�g����
    _scale = SCALE_DEFAULT;
    
    _toInitializeLight = true;
    
    //�\���̈�̒��S�_�́A�߂��ق��̃N���b�s���O�ʂł�XY���W
    _centerOfViewX = 0;
    _centerOfViewY = 0;
  }
  
  private void resetTransformation() {
    //�s��ɑ�������z���P�ʍs��ɂ�菉��������
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
    //position0�͌����̈ʒu�������Bposition0[3]��0�Ƃ���ƁA���s�����ƂȂ�B
    //position1, position2�ɂĂ����l�ł���B
    //position0�́A���_�̔w�ォ��A���Ă�������ւ̃��C�g�ł���B
    float[] position0      = { 0, 0, 10000, 0 };
    float[] spotDirection0 = { 0, 0, -1 };
    //position1�́A���_�̐^���������A�^�������ւ̃��C�g�ł���B
    float[] position1      = { 0, 10000, 0, 0 };
    float[] spotDirection1 = { 0, -1, 0 };
    //position2�́A���_�̐^����������A�^����ւ̃��C�g�ł���B
    float[] position2      = { 0, -10000, 0, 0 };
    float[] spotDirection2 = { 0, 1, 0 };
    
    //���C�g�̐F�ł���B
    float[] color0 = { 1.0f, 1.0f, 1.0f, 1.0f };
    float[] color1 = { 0.7f, 0.7f, 0.7f, 1.0f };
    float[] color2 = { 0.7f, 0.7f, 0.7f, 1.0f };
    //@formatter:on
    
    //�@���𐳋K������
    gl.glEnable(GL.GL_NORMALIZE);
    
    //glColor*�Őݒ肵���F��ambient��diffuse�Ƃ��ėp����
    gl.glEnable(GL.GL_COLOR_MATERIAL);
    gl.glColorMaterial(GL.GL_FRONT, GL.GL_AMBIENT_AND_DIFFUSE);
    
    //���C�g�̐ݒ�
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
   * ���݂̎��_�̃R�s�[��Ԃ�
   * @return �R�s�[����Camera�N���X�̃C���X�^���X
   */
  public Camera getCopy() {
    Camera tmpCopy = new Camera(_drawable3DPanel);
    copy(this, tmpCopy);
    return tmpCopy;
  }
  
  /**
   * ���_�̐ݒ���R�s�[����B���_�̕����ɗp����B
   * @param source �R�s�[���̃C���X�^���X
   * @param target �R�s�[��̃C���X�^���X
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
    //�����ˉe�Ɛ��ˉe�ŕ��s�ړ��ʂ̒P�ʂ��قȂ�̂ŁA
    //���̍������Ȃ����ĕ\�����傫���ω����Ȃ��悤�ɂ���
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
  * ���f���`��p�ɁA�ϊ��s����Z�b�g����Ȃǎ��E�̐ݒ���s���B
  * @param glAutoDrawable GL�̃I�u�W�F�N�g
  */
  public void setTransformationForModel(GLAutoDrawable glAutoDrawable) {
    final GL gl = glAutoDrawable.getGL();
    
    //########################################
    //���E��ݒ肷��
    //########################################
    gl.glMatrixMode(GL.GL_PROJECTION);
    //���E�́A���_(0,0,0)�ɂ���Z���������������܂ܓ������Ȃ�
    gl.glLoadIdentity();
    
    float aspect = _drawable3DPanel.getAspectRatio(_panelIndex);
    //���E�̒��S�����[�E���[�܂ł̋���
    float tmpHeightHalf = heightOnNearClippingPlane() / 2;
    
    //���E�̒��S����E�[�E���[�܂ł̋���
    float tmpWidthHalf = tmpHeightHalf * aspect;
    //�߂��ق��̃N���b�s���O�ʂ܂ł̋���
    //float tmpNear = _radius * RATIO_CLIPPING_PLANE_NEAR;
    float tmpNear = _distanceToNearPlane;
    //�����ق��̃N���b�s���O�ʂ܂ł̋���
    //float tmpFar = _radius * RATIO_CLIPPING_PLANE_FAR;
    float tmpFar = _distanceToFarPlane;
    //���E�̍��[�́A�߂��ق��̃N���b�s���O�ʂł�X���W
    float tmpLeft = _centerOfViewX - tmpWidthHalf;
    //���E�̉E�[�́A�߂��ق��̃N���b�s���O�ʂł�X���W
    float tmpRight = _centerOfViewX + tmpWidthHalf;
    //���E�̉��[�́A�߂��ق��̃N���b�s���O�ʂł�Y���W
    float tmpBottom = _centerOfViewY - tmpHeightHalf;
    //���E�̏�[�́A�߂��ق��̃N���b�s���O�ʂł�Y���W
    float tmpTop = _centerOfViewY + tmpHeightHalf;
    
    //���̐ς�ݒ肷��
    if (_isPerspective) {
      //�����ˉe�p
      gl.glFrustum(tmpLeft, tmpRight, tmpBottom, tmpTop, tmpNear, tmpFar);
    } else {
      //���ˉe�p
      //gl.glOrtho(tmpLeft, tmpRight, tmpBottom, tmpTop, tmpNear, tmpFar);
      gl.glOrtho(tmpLeft, tmpRight, tmpBottom, tmpTop, -tmpFar, tmpFar);
    }
    
    //########################################
    //���f���ւ̕ϊ��s���ݒ肷��
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
    
    //�ϊ��s����Z�b�g����
    gl.glLoadMatrixf(getModelViewMatrix(), 0);
    
  }
  
  /**
   * ���W���`��p�ɁA�ϊ��s����Z�b�g����Ȃǎ��E�̐ݒ���s���B
   * @param glAutoDrawable GL�̃I�u�W�F�N�g
   */
  public void setTransformationForModelForAxisRendering(GLAutoDrawable glAutoDrawable) {
    final GL gl = glAutoDrawable.getGL();
    
    //########################################
    //���E��ݒ肷��
    //########################################
    gl.glMatrixMode(GL.GL_PROJECTION);
    //���E�́A���_(0,0,0)�ɂ���Z���������������܂ܓ������Ȃ�
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
    
    //���̐ς�ݒ肷��
    if (_isPerspective) {
      //�����ˉe�p
      gl.glFrustum(tmpLeft, tmpRight, tmpBottom, tmpTop, tmpNear, tmpFar);
    } else {
      //���ˉe�p
      gl.glOrtho(tmpLeft, tmpRight, tmpBottom, tmpTop, tmpNear, tmpFar);
    }
    
    //########################################
    //��]�s���ݒ肷��
    //########################################
    gl.glMatrixMode(GL.GL_MODELVIEW);
    
    gl.glLoadMatrixf(getModelViewMatrixForAxisRendering(), 0);
  }
  
  /**
   * 2D�`��p�̎��E�̐ݒ���s���B��ʍ�����(0,0)�A��ʉE�オ(width,height)�B
   * ���W���̌����́AX��(��������)���E�����AY��(��������)��������ł���B
   * Y���͒ʏ��2D�`��Ƃ͋t�����Ƃ��Ă���B
   * @param glAutoDrawable GL�̃I�u�W�F�N�g
   */
  public void setTransformationFor2DRendering(GLAutoDrawable glAutoDrawable) {
    final GL gl = glAutoDrawable.getGL();
    
    int width = _drawable3DPanel.getWidth(_panelIndex);
    int height = _drawable3DPanel.getHeight(_panelIndex);
    
    float distanceNear = 1;
    float distanceFar = 2;//���̒l�͓��ɈӖ��͂Ȃ��BdistanceNear���傫����Ηǂ�
    gl.glMatrixMode(GL.GL_PROJECTION);
    gl.glLoadIdentity();
    gl.glFrustum(0, width, 0, height, distanceNear, distanceFar);
    
    gl.glMatrixMode(GL.GL_MODELVIEW);
    gl.glLoadIdentity();
    gl.glTranslatef(0, 0, -distanceNear);
    
  }
  
  /**
   * ���W���̃��x����`�悷��ʒu�𒲐����邽�߂̒l�ł���B
   * @return ���x���̕`��ʒu�Ɏg�p����l
   */
  public static float getCoefficientForAxisLabel() {
    float coeff = 1.0f / AXIS_RENDERING_HEIGHT;
    return coeff;
  }
  
  /**
   * ���f���`��p�̕ϊ��s���Ԃ��B
   * @return �ϊ��s��
   */
  private float[] getModelViewMatrix() {
    //�Î~���W�n����A���E�̍��W�n�ւ̕ϊ���\���s����v�Z����B
    //�Î~���W�n�̌��_��O�A���E�̍��W�n�̌��_(���_�̈ʒu)��Oe�Ƃ���B
    //���E�̍��W�n�ł�XYZ�̊e�����̒P�ʃx�N�g����
    //Xe, Ye, Ze�Ƃ���B
    //�C�ӂ̓_P�ɂ��āA
    //OP = OOe + r*Xe + s*Ye + t*Ze   (OP, OOe��)
    //�ƕ\�����Ƃ���r, s, t�����E�̍��W�n�ł̍��W�l�ƂȂ�B
    //P�̍��W�l���A�Î~���W�n�ł�(x,y,z)�Ƃ��A���E�̍��W�n�ł�(r,s,t)e�Ƃ���B
    //(x,y,z)����(r,s,t)e�ւ̕ϊ��s����v�Z����B
    //OeP = r*Xe + s*Ye + t*Ze �ł���̂ŁA
    //r = OeP.Xe, s=OeP.Ye, t=OeP.Ze �ƂȂ�B
    //(.�̓x�N�g���̓��ς�\���BXe.Xe=Ye.Ye=Ze.Ze=1���g�����B)
    //�Î~���W�n�Ŋe�_�A�x�N�g���̐����\�����Ƃ�
    //Oe=(x0,y0,z0), Xe=(Xex,Xey,Xez), Ye=(Yex,Yey,Yez), Ze=(Zex,Zey,Zez) �ł���Ƃ���ƁA
    //r,s,t ��4x4�s���p���Ĉȉ��̂悤�ɕ\����B
    // |r|   |OeP.Xe|   |Xex Xey Xez 0| |x-x0|   |Xex Xey Xez 0| |1 0 0 -x0| |x|
    // |s| = |OeP.Ye| = |Yex Yey Yez 0|*|y-y0| = |Yex Yey Yez 0|*|0 1 0 -y0|*|y|
    // |t|   |OeP.Ze|   |Zex Zey Zez 0| |z-z0|   |Zex Zey Zez 0| |0 0 1 -z0| |z|
    // |1|   |   1  |   | 0   0   0  1| |  1 |   | 0   0   0  1| |0 0 0  1 | |1|
    //                                              ��]�s��      ���s�ړ�
    
    MatrixUtils.setIdentityMatrix(_bufferMatrix1);
    MatrixUtils.translateMinus(_bufferMatrix1, _eyePosition);
    
    setRotateToMatrix(_bufferMatrix2);
    
    MatrixUtils.multiply(_bufferMatrix1, _bufferMatrix2, _currentModelTransformation);
    
    MatrixUtils.copyMatrix(_currentModelTransformation, _bufferMatrix1);
    
    return _bufferMatrix1;
  }
  
  /**
   * ���W���`��̂��߂̕ϊ��s���Ԃ��B
   * ���f���`��p�̕ϊ��s��̂����A��]�����݂̂̍s������o���A
   * �ʒu���߂̂��߂̕��s�ړ����s�����s���Ԃ��B
   * @return �ϊ��s��
   */
  private float[] getModelViewMatrixForAxisRendering() {
    setRotateToMatrix(_currentModelTransformationForAxis);
    
    MatrixUtils.copyMatrix(_currentModelTransformationForAxis, _bufferMatrix1);
    MatrixUtils.translateMinus(_bufferMatrix1, _moveForAxisRendering);
    
    return _bufferMatrix1;
  }
  
  private void setRotateToMatrix(float[] target) {
    //���E�̍��W�n�ł�XYZ�����̒P�ʃx�N�g����
    //Xe=(Xex,Xey,Xez), Ye=(Yex,Yey,Yez), Ze=(Zex,Zey,Zez) �Ƃ���B
    //���E�ɂ����ẮAXe�͉�ʉ����AYe�͍��AZe�͏�����Ƃ���
    //OpenGL�ł́AX�����E�AY������AZ������ʎ�O���Ȃ̂ŁA�ϊ����K�v�ł���B
    //(Xe,Ye,Ze) -> (-Ye, Ze, -Xe)�ƕϊ����s���B
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
    //�}�E�X�̉������ړ��ʁB��ʉE����
    int moveX = tmpX - _mouseX;
    //�}�E�X�̏c�����ړ��ʁB��ʏオ��
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
    //TODO �z�C�[����]�̌����Ɣ{���̕ύX�ʂ̒���
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
   * ��]����
   * @param x ��ʉ������̈ړ���
   * @param y ��ʏc�����̈ړ���
   */
  private void rotateConsideringScale(int x, int y, int mode) {
    int width = _drawable3DPanel.getWidth(_panelIndex);
    int height = _drawable3DPanel.getHeight(_panelIndex);
    float tmpCoefficient = (float) (_scale / height * Math.PI * 2);
    if (mode == MODE_ROTATE || mode == MODE_ROTATE_VERTICAL) {
      //�����Ȓ��������Ƃ����A�c�����̉�]
      rotate(ROTATE_Y, y * tmpCoefficient * MOUSE_SENSITIVITY_ROTATE);
    }
    if (mode == MODE_ROTATE || mode == MODE_ROTATE_HORIZONTAL) {
      //�����Ȓ��������Ƃ����A�������̉�]
      rotate(ROTATE_Z, x * tmpCoefficient * MOUSE_SENSITIVITY_ROTATE);
    }
  }
  
  /**
   * ����_�𒆐S�ɂ��ĉ�]����
   * @param x �������̉�]��
   * @param y �c�����̉�]��
   * @param mode ���[�h��\���l
   */
  private void rotateAroundPoint(int x, int y, int mode) {
    int width = _drawable3DPanel.getWidth(_panelIndex);
    int height = _drawable3DPanel.getHeight(_panelIndex);
    float tmpCoefficient = (float) (_scale / height * Math.PI * 2);
    if (mode == MODE_ROTATE || mode == MODE_ROTATE_VERTICAL) {
      //�����Ȓ��������Ƃ����A�c�����̉�]
      float angle = y * tmpCoefficient * MOUSE_SENSITIVITY_ROTATE;
      float sin = (float) (Math.sin(angle) * _distanceToCenterOfRotation);
      float cos = (float) (Math.cos(angle) * _distanceToCenterOfRotation);
      //��]�O�̕����x�N�g�����R�s�[���Ă���
      _bufferPoint3f_1.copy(_eyeDirX);
      _bufferPoint3f_2.copy(_eyeDirZ);
      //��]
      rotate(ROTATE_Y, angle);
      //��]���S�������_�ɂȂ�悤�ɁA���_�̈ʒu���ړ�����
      _eyePosition.add(_bufferPoint3f_1, _distanceToCenterOfRotation - cos);
      _eyePosition.add(_bufferPoint3f_2, sin);
    }
    if (mode == MODE_ROTATE || mode == MODE_ROTATE_HORIZONTAL) {
      //�����Ȓ��������Ƃ����A�������̉�]
      float angle = x * tmpCoefficient * MOUSE_SENSITIVITY_ROTATE;
      float sin = (float) (Math.sin(angle) * _distanceToCenterOfRotation);
      float cos = (float) (Math.cos(angle) * _distanceToCenterOfRotation);
      //��]�O�̕����x�N�g�����R�s�[���Ă���
      _bufferPoint3f_1.copy(_eyeDirX);
      _bufferPoint3f_2.copy(_eyeDirY);
      //��]
      rotate(ROTATE_Z, angle);
      //��]���S�������_�ɂȂ�悤�ɁA���_�̈ʒu���ړ�����
      _eyePosition.add(_bufferPoint3f_1, _distanceToCenterOfRotation - cos);
      _eyePosition.add(_bufferPoint3f_2, -sin);
    }
  }
  
  /**
   * �ϊ��s��ɁAX��(��ʉE����)�AY��(��ʏオ��)�AZ��(��ʎ�O����)��
   * �����ꂩ�����Ƃ�����]��\���s����������Z����B
   * @param axis ��]�����w�肷��B
   *             �w�肷��l�́AMatrixUtils.ROTATE_X, MatrixUtils.ROTATE_Y, MatrixUtils.ROTATE_Z�̂����ꂩ�B
   * @param radian ��]�ʂ��w�肷��B�P�ʂ̓��W�A���B�w�肵�����̐������������āA���v��肪���̊p�x�B
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
   * �g��{����ύX����
   * @param x ��ʉ������̈ړ���
   * @param y ��ʏc�����̈ړ���
   */
  private void scale(int x, int y) {
    scale((x + y) * MOUSE_SENSITIVITY_SCALE);
  }
  
  /**
   * �g��{����ύX����
   * @param scale �{��
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
   * �\���̈���ړ�����
   * @param x �������̈ړ���
   * @param y �c�����̈ړ���
   */
  private void translate(int x, int y) {
    int width = _drawable3DPanel.getWidth(_panelIndex);
    int height = _drawable3DPanel.getHeight(_panelIndex);
    //��ʂ�1�h�b�g���A�߂��ق��̃N���b�s���O�ʏ�łǂꂾ���̒����ɂȂ邩��\��
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
   * �߂��ق��̃N���b�s���O�ʏ�ŁA�\���̈悪�ǂꂾ���̍����ł��邩��\��
   * @return �\���̈�̍���
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
    
    //������̔z�����������
    String[] configurations = Common.toStringArray(listConfigurations);
    
    return configurations;
    
  }
  
  /**
   * ���_��ݒ肷��B
   * @param commandRaw �ݒ�̎�ނ�\��������
   * @param params �ݒ肷�鐔�l��\��������̔z��
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
        //���_�̈ʒu
        if (paramValuesFloat != null && paramValuesFloat.length >= 3) {
          _eyePosition.init(paramValuesFloat[0], paramValuesFloat[1], paramValuesFloat[2]);
        }
      } else if (command.equalsIgnoreCase(LABEL_CONFIGURATION_VIEW_POINT_DIRECTION_X)) {
        //################################
        //���E�̍��W�n�ɂ�����X����\������
        if (paramValuesFloat != null && paramValuesFloat.length >= 3) {
          _eyeDirX.init(paramValuesFloat[0], paramValuesFloat[1], paramValuesFloat[2]);
        }
      } else if (command.equalsIgnoreCase(LABEL_CONFIGURATION_VIEW_POINT_DIRECTION_Y)) {
        //################################
        //���E�̍��W�n�ɂ�����Y����\������
        if (paramValuesFloat != null && paramValuesFloat.length >= 3) {
          _eyeDirY.init(paramValuesFloat[0], paramValuesFloat[1], paramValuesFloat[2]);
        }
      } else if (command.equalsIgnoreCase(LABEL_CONFIGURATION_VIEW_POINT_DIRECTION_Z)) {
        //################################
        //���E�̍��W�n�ɂ�����Z����\������
        if (paramValuesFloat != null && paramValuesFloat.length >= 3) {
          _eyeDirZ.init(paramValuesFloat[0], paramValuesFloat[1], paramValuesFloat[2]);
        }
      } else if (command.equalsIgnoreCase(LABEL_CONFIGURATION_CENTER_OF_VIEW)) {
        //################################
        //�\�����Ă���͈͂̒��S�́A�߂��N���b�s���O�ʂɂ�������W
        if (paramValuesFloat != null && paramValuesFloat.length >= 2) {
          _centerOfViewX = paramValuesFloat[0];
          _centerOfViewY = paramValuesFloat[1];
        }
      } else if (command.equalsIgnoreCase(LABEL_CONFIGURATION_SCALE)) {
        //################################
        //�\���{��
        if (paramValuesFloat != null && paramValuesFloat.length >= 1) {
          _scale = paramValuesFloat[0];
        }
      }
    }
  }
}
