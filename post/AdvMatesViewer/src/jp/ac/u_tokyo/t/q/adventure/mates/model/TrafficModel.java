package jp.ac.u_tokyo.t.q.adventure.mates.model;

import java.util.Arrays;
import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * ��ʗ��V�~�����[�V�����̊e��f�[�^��ێ�����N���X�ł���B
 */
public class TrafficModel {
  //@formatter:off
  public static final int SIGNAL_STATE_DEFAULT  = 0;
  
  public static final int INDEX_SIGNAL_GREEN    = 0;
  public static final int INDEX_SIGNAL_YELLOW   = 1;
  public static final int INDEX_SIGNAL_RED      = 2;
  public static final int INDEX_SIGNAL_LEFT     = 3;
  public static final int INDEX_SIGNAL_STRAIGHT = 4;
  public static final int INDEX_SIGNAL_RIGHT    = 5;
  
  public static final int MASK_SIGNAL_GREEN     = 32;
  public static final int MASK_SIGNAL_YELLOW    = 16;
  public static final int MASK_SIGNAL_RED       = 8;
  public static final int MASK_SIGNAL_LEFT      = 4;
  public static final int MASK_SIGNAL_STRAIGHT  = 2;
  public static final int MASK_SIGNAL_RIGHT     = 1;
  //@formatter:on
  
  /**
   * ���O�n���h�����O�N���X
   */
  private Logger _logger;
  
  private TrafficParameters _trafficParameters;
  
  private int _nTimeSteps;
  private int[] _nVehicles;
  private int[][] _vehicleID;
  private int[][] _vehicleFamilyID;
  private VehicleFamily[][] _vehicleFamily;
  private float[][] _vehiclePositionX;
  private float[][] _vehiclePositionY;
  private float[][] _vehiclePositionZ;
  private float[][] _vehicleAngleXY;
  private float[][] _vehicleAngleZ;
  private float[][] _vehicleVelocity;
  private float[][] _vehicleAcceleration;
  private int[][] _vehicleIntersectionID;
  private int[][] _vehicleRoadID;
  
  /**
   * �M���@�̏�Ԃ��i�[����z��ł���B
   * [roadID][time] : roadID�̓��H�̎n�_�ɂ��ẮAtime�ɂ�����M���̏�Ԃł���B
   */
  private int[][] _signalStateOfStartOfRoad;
  /**
   * �M���@�̏�Ԃ��i�[����z��ł���B
   * [roadID][time] : roadID�̓��H�̏I�_�ɂ��ẮAtime�ɂ�����M���̏�Ԃł���B
   */
  private int[][] _signalStateOfEndOfRoad;
  
  private float _fieldXMin;
  private float _fieldYMin;
  private float _fieldZMin;
  private float _fieldXMax;
  private float _fieldYMax;
  private float _fieldZMax;
  
  //private float[][] _shapeCoords;
  //private int[][] _shapeConnectivity;
  //private float[][] _shapeNormals;
  //private float[][] _shapeColors;
  
  private int _nIntersections;
  //private float[] _intersectionPositionX;
  //private float[] _intersectionPositionY;
  //private float[] _intersectionPositionZ;
  private int[] _intersectionNumShapePoints;
  private int[] _intersectionID;
  private String[] _intersectionIDString;
  private float[] _intersectionCenterPositionX;
  private float[] _intersectionCenterPositionY;
  private float[] _intersectionCenterPositionZ;
  private float[][] _intersectionShapePositionX;
  private float[][] _intersectionShapePositionY;
  private float[][] _intersectionShapePositionZ;
  
  private int _nRoads;
  //private int[] _roadPointIDIn;
  //private int[] _roadPointIDOut;
  //private int[] _roadNumLanesIn;
  //private int[] _roadNumLanesOut;
  //�E���[����
  private int[] _roadNumLanesRight;
  //�����[����
  private int[] _roadNumLanesLeft;
  //�����̓_�̌�
  private int[] _roadNumPointsCenter;
  //�E�[�̓_�̌�
  private int[] _roadNumPointsRight;
  //���[�̓_�̌�
  private int[] _roadNumPointsLeft;
  //���H�̎n�_�̌����_�ԍ�
  private int[] _roadIntersectionIDStart;
  //���H�̏I�_�̌����_�ԍ�
  private int[] _roadIntersectionIDEnd;
  //true���H�̎n�_���ɐM�������݂���
  private boolean[] _roadExistSignalStart;
  //true���H�̏I�_���ɐM�������݂���
  private boolean[] _roadExistSignalEnd;
  //�����̓_��X���W
  private float[][] _roadPositionCenterX;
  //�����̓_��Y���W
  private float[][] _roadPositionCenterY;
  //�����̓_��Z���W
  private float[][] _roadPositionCenterZ;
  //�E�[�̓_��X���W
  private float[][] _roadPositionRightX;
  //�E�[�̓_��Y���W
  private float[][] _roadPositionRightY;
  //�E�[�̓_��Z���W
  private float[][] _roadPositionRightZ;
  //���[�̓_��X���W
  private float[][] _roadPositionLeftX;
  //�E�[�̓_��Y���W
  private float[][] _roadPositionLeftY;
  //�E�[�̓_��Z���W
  private float[][] _roadPositionLeftZ;
  //���H�̐i�s�����x�N�g����X����
  private float[][] _roadVectorDirectionX;
  //���H�̐i�s�����x�N�g����Y����
  private float[][] _roadVectorDirectionY;
  //���H�̐i�s�����x�N�g����Z����
  private float[][] _roadVectorDirectionZ;
  //���H�̐��������x�N�g����X����
  private float[][] _roadVectorPerpendicularX;
  //���H�̐��������x�N�g����Y����
  private float[][] _roadVectorPerpendicularY;
  //���H�̐��������x�N�g����Z����
  private float[][] _roadVectorPerpendicularZ;
  //�E�̃��[���Ԃ̋��E�̓_��X���W
  private float[][][] _roadPositionLanesRightX;
  //�E�̃��[���Ԃ̋��E�̓_��Y���W
  private float[][][] _roadPositionLanesRightY;
  //�E�̃��[���Ԃ̋��E�̓_��Z���W
  private float[][][] _roadPositionLanesRightZ;
  //���̃��[���Ԃ̋��E�̓_��X���W
  private float[][][] _roadPositionLanesLeftX;
  //���̃��[���Ԃ̋��E�̓_��Y���W
  private float[][][] _roadPositionLanesLeftY;
  //���̃��[���Ԃ̋��E�̓_��Z���W
  private float[][][] _roadPositionLanesLeftZ;
  //�E�̘H���̗��[�̓_��X���W
  private float[][][] _roadPositionRoadSideRightX;
  //�E�̘H���̗��[�̓_��Y���W
  private float[][][] _roadPositionRoadSideRightY;
  //�E�̘H���̗��[�̓_��Z���W
  private float[][][] _roadPositionRoadSideRightZ;
  //���̘H���̗��[�̓_��X���W
  private float[][][] _roadPositionRoadSideLeftX;
  //���̘H���̗��[�̓_��Y���W
  private float[][][] _roadPositionRoadSideLeftY;
  //���̘H���̗��[�̓_��Z���W
  private float[][][] _roadPositionRoadSideLeftZ;
  //�E�̕����̗��[�̓_��X���W
  private float[][][] _roadPositionSideWalkRightX;
  //�E�̕����̗��[�̓_��Y���W
  private float[][][] _roadPositionSideWalkRightY;
  //�E�̕����̗��[�̓_��Z���W
  private float[][][] _roadPositionSideWalkRightZ;
  //���̕����̗��[�̓_��X���W
  private float[][][] _roadPositionSideWalkLeftX;
  //���̕����̗��[�̓_��Y���W
  private float[][][] _roadPositionSideWalkLeftY;
  //���̕����̗��[�̓_��Z���W
  private float[][][] _roadPositionSideWalkLeftZ;
  
  //�������Ƃɕ����Ċi�[�������H�̐ݒ�ł���B
  private int _nSingleRoad;
  private int[] _singleRoadPointIDIn;
  private int[] _singleRoadPointIDOut;
  private int[] _singleRoadPNumLanesIn;
  private int[] _singleRoadPNumLanesOut;
  
  //�M���@�̌`��ƐF�f�[�^
  private float[][] _signalShapeCoords = null;
  private int[][] _signalShapeConnectivity = null;
  private float[][] _signalShapeNormals = null;
  private float[][] _signalShapeColors = null;
  //�ȈՕ`��p�̐M���̌`��f�[�^
  private float[][] _signalSimpleShapeCoords = null;
  private int[][] _signalSimpleShapeConnectivity = null;
  private float[][] _signalSimpleShapeNormals = null;
  private float[][] _signalSimpleShapeColors = null;
  
  public TrafficModel(TrafficParameters trafficParameters) {
    _logger = Logger.getLogger(TrafficModel.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    
    _trafficParameters = trafficParameters;
  }
  
  public TrafficParameters getTrafficParameters() {
    return _trafficParameters;
  }
  
  public void setTrafficData(int nTimeSteps, int[] nVehicles, int[][] vehicleID, int[][] vehicleFamilyID,
      VehicleFamily[][] vehicleFamily, float[][] vehiclePositionX, float[][] vehiclePositionY,
      float[][] vehiclePositionZ, float[][] vehicleAngleXY, float[][] vehicleAngleZ, float[][] vehicleVelocity,
      float[][] vehicleAcceleration, int[][] vehicleIntersectionID, int[][] vehicleRoadID) {
    _nTimeSteps = nTimeSteps;
    _nVehicles = nVehicles;
    _vehicleID = vehicleID;
    _vehicleFamilyID = vehicleFamilyID;
    _vehicleFamily = vehicleFamily;
    _vehiclePositionX = vehiclePositionX;
    _vehiclePositionY = vehiclePositionY;
    _vehiclePositionZ = vehiclePositionZ;
    _vehicleAngleXY = vehicleAngleXY;
    _vehicleAngleZ = vehicleAngleZ;
    _vehicleVelocity = vehicleVelocity;
    _vehicleAcceleration = vehicleAcceleration;
    _vehicleIntersectionID = vehicleIntersectionID;
    _vehicleRoadID = vehicleRoadID;
  }
  
  public void setIntersectionData(int nIntersections, int[] nShapePoints, int[] intersectionID,
      String[] intersectionIDString, float[] centerX, float[] centerY, float[] centerZ, float[][] shapeX,
      float[][] shapeY, float[][] shapeZ) {
    _nIntersections = nIntersections;
    _intersectionNumShapePoints = nShapePoints;
    _intersectionID = intersectionID;
    _intersectionIDString = intersectionIDString;
    _intersectionCenterPositionX = centerX;
    _intersectionCenterPositionY = centerY;
    _intersectionCenterPositionZ = centerZ;
    _intersectionShapePositionX = shapeX;
    _intersectionShapePositionY = shapeY;
    _intersectionShapePositionZ = shapeZ;
    
    computeBoundingBox();
  }
  
  private void computeBoundingBox() {
    boolean isFirst = true;
    _fieldXMin = 0;
    _fieldYMin = 0;
    _fieldZMin = 0;
    _fieldXMax = 0;
    _fieldYMax = 0;
    _fieldZMax = 0;
    for (int i = 0; i < _nIntersections; i++) {
      float cx = _intersectionCenterPositionX[i];
      float cy = _intersectionCenterPositionY[i];
      float cz = _intersectionCenterPositionZ[i];
      if (isFirst) {
        _fieldXMin = cx;
        _fieldYMin = cy;
        _fieldZMin = cz;
        _fieldXMax = cx;
        _fieldYMax = cy;
        _fieldZMax = cz;
        isFirst = false;
      } else {
        if (_fieldXMin > cx) _fieldXMin = cx;
        if (_fieldXMax < cx) _fieldXMax = cx;
        if (_fieldYMin > cy) _fieldYMin = cy;
        if (_fieldYMax < cy) _fieldYMax = cy;
        if (_fieldZMin > cz) _fieldZMin = cz;
        if (_fieldZMax < cz) _fieldZMax = cz;
      }
      
      for (int j = 0; j < _intersectionNumShapePoints[i]; j++) {
        float sx = _intersectionShapePositionX[i][j];
        float sy = _intersectionShapePositionY[i][j];
        float sz = _intersectionShapePositionZ[i][j];
        if (isFirst) {
          _fieldXMin = sx;
          _fieldYMin = sy;
          _fieldZMin = sz;
          _fieldXMax = sx;
          _fieldYMax = sy;
          _fieldZMax = sz;
          isFirst = false;
        } else {
          if (_fieldXMin > sx) _fieldXMin = sx;
          if (_fieldXMax < sx) _fieldXMax = sx;
          if (_fieldYMin > sy) _fieldYMin = sy;
          if (_fieldYMax < sy) _fieldYMax = sy;
          if (_fieldZMin > sz) _fieldZMin = sz;
          if (_fieldZMax < sz) _fieldZMax = sz;
        }
      }
    }
  }
  
  //public void setNetworkData(int nRoads, int[] pointIDIn, int[] pointIDOut, int[] nLanesIn, int[] nLanesOut) {
  //  _nRoads = nRoads;
  //  _roadPointIDIn = pointIDIn;
  //  _roadPointIDOut = pointIDOut;
  //  _roadNumLanesIn = nLanesIn;
  //  _roadNumLanesOut = nLanesOut;
  //}
  
  public void setRoadData(int nRoads, int[] nLanesRight, int[] nLanesLeft, int[] nPointsCenter, int[] nPointsRight,
      int[] nPointsLeft, int[] intersectionIDStart, int[] intersectionIDEnd, boolean[] existSignalStart,
      boolean[] existSignalEnd, float[][] positionCenterX, float[][] positionCenterY, float[][] positionCenterZ,
      float[][] positionRightX, float[][] positionRightY, float[][] positionRightZ, float[][] positionLeftX,
      float[][] positionLeftY, float[][] positionLeftZ, int nSingleRoad, int[] singleRoadPointIDIn,
      int[] singleRoadPointIDOut, int[] singleRoadPNumLanesIn, int[] singleRoadPNumLanesOut) {
    _nRoads = nRoads;
    _roadNumLanesRight = nLanesRight;
    _roadNumLanesLeft = nLanesLeft;
    _roadNumPointsCenter = nPointsCenter;
    _roadNumPointsRight = nPointsRight;
    _roadNumPointsLeft = nPointsLeft;
    _roadIntersectionIDStart = intersectionIDStart;
    _roadIntersectionIDEnd = intersectionIDEnd;
    _roadExistSignalStart = existSignalStart;
    _roadExistSignalEnd = existSignalEnd;
    _roadPositionCenterX = positionCenterX;
    _roadPositionCenterY = positionCenterY;
    _roadPositionCenterZ = positionCenterZ;
    _roadPositionRightX = positionRightX;
    _roadPositionRightY = positionRightY;
    _roadPositionRightZ = positionRightZ;
    _roadPositionLeftX = positionLeftX;
    _roadPositionLeftY = positionLeftY;
    _roadPositionLeftZ = positionLeftZ;
    
    _nSingleRoad = nSingleRoad;
    _singleRoadPointIDIn = singleRoadPointIDIn;
    _singleRoadPointIDOut = singleRoadPointIDOut;
    _singleRoadPNumLanesIn = singleRoadPNumLanesIn;
    _singleRoadPNumLanesOut = singleRoadPNumLanesOut;
    
    computeRoadData();
  }
  
  /**
   * ���H�́A���E�̃��[�����ƁA�����E�E�E���̓_�̈ʒu����A
   * ���[���Ԃ̋��E�ƁA���E�̕����̂��߂̍��W�l���v�Z����B
   */
  private void computeRoadData() {
    _roadVectorDirectionX = new float[_nRoads][];
    _roadVectorDirectionY = new float[_nRoads][];
    _roadVectorDirectionZ = new float[_nRoads][];
    _roadVectorPerpendicularX = new float[_nRoads][];
    _roadVectorPerpendicularY = new float[_nRoads][];
    _roadVectorPerpendicularZ = new float[_nRoads][];
    
    _roadPositionLanesRightX = new float[_nRoads][][];
    _roadPositionLanesRightY = new float[_nRoads][][];
    _roadPositionLanesRightZ = new float[_nRoads][][];
    _roadPositionLanesLeftX = new float[_nRoads][][];
    _roadPositionLanesLeftY = new float[_nRoads][][];
    _roadPositionLanesLeftZ = new float[_nRoads][][];
    _roadPositionRoadSideRightX = new float[_nRoads][][];
    _roadPositionRoadSideRightY = new float[_nRoads][][];
    _roadPositionRoadSideRightZ = new float[_nRoads][][];
    _roadPositionRoadSideLeftX = new float[_nRoads][][];
    _roadPositionRoadSideLeftY = new float[_nRoads][][];
    _roadPositionRoadSideLeftZ = new float[_nRoads][][];
    _roadPositionSideWalkRightX = new float[_nRoads][][];
    _roadPositionSideWalkRightY = new float[_nRoads][][];
    _roadPositionSideWalkRightZ = new float[_nRoads][][];
    _roadPositionSideWalkLeftX = new float[_nRoads][][];
    _roadPositionSideWalkLeftY = new float[_nRoads][][];
    _roadPositionSideWalkLeftZ = new float[_nRoads][][];
    
    float widthSideWalk = _trafficParameters.getWidthOfSideWalk();
    float widthRoadSide = _trafficParameters.getWidthOfRoadSide();
    
    for (int i = 0; i < _nRoads; i++) {
      //TODO �����E�E�E���̓_�̌��͓����Ɖ��肷��
      int nPoints = _roadNumPointsCenter[i];
      int nLanesRight = _roadNumLanesRight[i];
      int nLanesLeft = _roadNumLanesLeft[i];
      
      int intersectionIDStart = _roadIntersectionIDStart[i];
      int intersectionIDEnd = _roadIntersectionIDEnd[i];
      int nLanesRightStart = nLanesRight;
      int nLanesRightEnd = nLanesRight;
      int nLanesLeftStart = nLanesLeft;
      int nLanesLeftEnd = nLanesLeft;
      
      //TODO �E�ܐ�p���[���ɂ��ẮA�s�\���ȓ_������
      //���݂̓��H�ɂ��āA�E���̎Ԑ��A�����̎Ԑ��A���ꂼ��ɉE�ܐ�p���[�������邩�ǂ����̃t���O
      boolean flgTurnRightLane_Right = false;
      boolean flgTurnRightLane_Left = false;
      {
        for (int ii = 0; ii < _nSingleRoad; ii++) {
          int tmpPointIDIn = _singleRoadPointIDIn[ii];
          int tmpPointIDOut = _singleRoadPointIDOut[ii];
          int tmpNumLanesIn = _singleRoadPNumLanesIn[ii];
          int tmpNumLanesOut = _singleRoadPNumLanesOut[ii];
          if (tmpPointIDIn == intersectionIDStart && tmpPointIDOut == intersectionIDEnd) {
            nLanesLeftStart = tmpNumLanesIn;
            nLanesLeftEnd = tmpNumLanesOut;
          }
          if (tmpPointIDIn == intersectionIDEnd && tmpPointIDOut == intersectionIDStart) {
            nLanesRightStart = tmpNumLanesIn;
            nLanesRightEnd = tmpNumLanesOut;
          }
        }
        
        //TODO �E�ܐ�p���[���ɂ��ẮA�s�\���ȓ_������
        if (nLanesRight == nLanesRightStart && nLanesRight == nLanesRightEnd - 1) {
          nLanesRight++;
          _roadNumLanesRight[i]++;
          flgTurnRightLane_Right = true;
        }
        if (nLanesLeft == nLanesLeftStart && nLanesLeft == nLanesLeftEnd - 1) {
          nLanesLeft++;
          _roadNumLanesLeft[i]++;
          flgTurnRightLane_Left = true;
        }
        //��O�I�ȏꍇ������邽�߁A�����I�ɒl��ݒ肷��
        nLanesRightStart = nLanesRight;
        nLanesRightEnd = nLanesRight;
        nLanesLeftStart = nLanesLeft;
        nLanesLeftEnd = nLanesLeft;
        if (flgTurnRightLane_Right) {
          nLanesRightStart = nLanesRight - 1;
        }
        if (flgTurnRightLane_Left) {
          nLanesLeftStart = nLanesLeft - 1;
        }
      }
      
      //�E���̃��[���́A���[���Ԃ���ї��[�̓_�̍��W�l
      float[][] positionLanesRightX = new float[nLanesRight + 1][nPoints];
      float[][] positionLanesRightY = new float[nLanesRight + 1][nPoints];
      float[][] positionLanesRightZ = new float[nLanesRight + 1][nPoints];
      //�����̃��[���́A���[���Ԃ���ї��[�̓_�̍��W�l
      float[][] positionLanesLeftX = new float[nLanesLeft + 1][nPoints];
      float[][] positionLanesLeftY = new float[nLanesLeft + 1][nPoints];
      float[][] positionLanesLeftZ = new float[nLanesLeft + 1][nPoints];
      
      float[] bufferRatios = new float[nLanesRight + nLanesLeft + 1];
      
      _roadVectorDirectionX[i] = new float[nPoints];
      _roadVectorDirectionY[i] = new float[nPoints];
      _roadVectorDirectionZ[i] = new float[nPoints];
      _roadVectorPerpendicularX[i] = new float[nPoints];
      _roadVectorPerpendicularY[i] = new float[nPoints];
      _roadVectorPerpendicularZ[i] = new float[nPoints];
      
      _roadPositionRoadSideRightX[i] = new float[2][nPoints];
      _roadPositionRoadSideRightY[i] = new float[2][nPoints];
      _roadPositionRoadSideRightZ[i] = new float[2][nPoints];
      _roadPositionRoadSideLeftX[i] = new float[2][nPoints];
      _roadPositionRoadSideLeftY[i] = new float[2][nPoints];
      _roadPositionRoadSideLeftZ[i] = new float[2][nPoints];
      _roadPositionSideWalkRightX[i] = new float[2][nPoints];
      _roadPositionSideWalkRightY[i] = new float[2][nPoints];
      _roadPositionSideWalkRightZ[i] = new float[2][nPoints];
      _roadPositionSideWalkLeftX[i] = new float[2][nPoints];
      _roadPositionSideWalkLeftY[i] = new float[2][nPoints];
      _roadPositionSideWalkLeftZ[i] = new float[2][nPoints];
      
      for (int j = 0; j < nPoints; j++) {
        //�����̓_�́A���H�̒��������ɐ����ȃx�N�g�����v�Z���邽�߂ɂ����p����B
        //���H���̊e���[���̂��߂̍��W�l�́A���E�̒[�̓_����v�Z����
        
        //���H�̒��������̃x�N�g���𓾂邽�߂ɁA������2�_�̍��W�l���g��
        float center1X = _roadPositionCenterX[i][j];
        float center1Y = _roadPositionCenterY[i][j];
        float center1Z = _roadPositionCenterZ[i][j];
        float center2X = 0;
        float center2Y = 0;
        float center2Z = 0;
        if (j < nPoints - 1) {
          //���̒����̓_������Ƃ�
          center2X = _roadPositionCenterX[i][j + 1];
          center2Y = _roadPositionCenterY[i][j + 1];
          center2Z = _roadPositionCenterZ[i][j + 1];
        } else {
          //�Ō�̒����̓_�̏ꍇ
          center2X = center1X;
          center2Y = center1Y;
          center2Z = center1Z;
          center1X = _roadPositionCenterX[i][j - 1];
          center1Y = _roadPositionCenterY[i][j - 1];
          center1Z = _roadPositionCenterZ[i][j - 1];
        }
        
        //���H�̒��������̃x�N�g��
        float vDir1X = center2X - center1X;
        float vDir1Y = center2Y - center1Y;
        float vDir1Z = center2Z - center1Z;
        //���H�̒��������ɐ����ȃx�N�g��
        float vDir2X = -vDir1Y;
        float vDir2Y = vDir1X;
        float vDir2Z = 0;
        {
          float lengthDir2 = (float) (Math.sqrt(vDir2X * vDir2X + vDir2Y * vDir2Y + vDir2Z * vDir2Z));
          if (lengthDir2 > 0) {
            vDir2X /= lengthDir2;
            vDir2Y /= lengthDir2;
            vDir2Z /= lengthDir2;
          }
        }
        _roadVectorDirectionX[i][j] = vDir1X;
        _roadVectorDirectionY[i][j] = vDir1Y;
        _roadVectorDirectionZ[i][j] = vDir1Z;
        _roadVectorPerpendicularX[i][j] = vDir2X;
        _roadVectorPerpendicularY[i][j] = vDir2Y;
        _roadVectorPerpendicularZ[i][j] = vDir2Z;
        
        float rightX = _roadPositionRightX[i][j];
        float rightY = _roadPositionRightY[i][j];
        float rightZ = _roadPositionRightZ[i][j];
        float leftX = _roadPositionLeftX[i][j];
        float leftY = _roadPositionLeftY[i][j];
        float leftZ = _roadPositionLeftZ[i][j];
        //���H�̕������̃x�N�g�� (���������ɐ����ȃx�N�g���Ɠ����Ƃ͌���Ȃ�)
        float vWidthX = leftX - rightX;
        float vWidthY = leftY - rightY;
        float vWidthZ = leftZ - rightZ;
        //���H�̕�
        float widthRoad = vDir2X * vWidthX + vDir2Y * vWidthY + vDir2Z * vWidthZ;
        {
          if (widthRoad > 0) {
            vWidthX /= widthRoad;
            vWidthY /= widthRoad;
            vWidthZ /= widthRoad;
          }
        }
        
        if (flgTurnRightLane_Right == false && flgTurnRightLane_Left == false) {
          for (int k = 0; k < nLanesRight + nLanesLeft + 1; k++) {
            float tmpRatio = 1.0f * k / (nLanesRight + nLanesLeft);
            bufferRatios[k] = (widthRoad - widthRoadSide * 2) * tmpRatio + widthRoadSide;
          }
        } else {
          //TODO �E�ܐ�p���[���ɂ��ẮA�s�\���ȓ_������
          //�E�ܐ�p���[��������ꍇ
          int tmpNumLanesRight = nLanesRight;
          int tmpNumLanesLeft = nLanesLeft;
          //�E�ܐ�p���[���͓��H�̒[�ɂ݂̂���̂ŁA����ȊO�̉ӏ��ł̓��[������1���炷
          if (flgTurnRightLane_Right && j != 0) {
            tmpNumLanesRight--;
          }
          if (flgTurnRightLane_Left && j != nPoints - 1) {
            tmpNumLanesLeft--;
          }
          
          float tmpWidth = widthRoad - widthRoadSide * 2;
          float tmpWidthRight = tmpWidth * tmpNumLanesRight / (tmpNumLanesRight + tmpNumLanesLeft);
          float tmpWidthLeft = tmpWidth * tmpNumLanesLeft / (tmpNumLanesRight + tmpNumLanesLeft);
          
          for (int k = 0; k < tmpNumLanesRight + 1; k++) {
            float tmpRatio = 1.0f * k / tmpNumLanesRight;
            bufferRatios[k] = tmpWidthRight * tmpRatio + widthRoadSide;
          }
          if (flgTurnRightLane_Right && j != 0) {
            //�E�ܐ�p���[��
            bufferRatios[nLanesRight] = bufferRatios[tmpNumLanesRight];
          }
          
          int offset1 = nLanesRight + 1;
          int offset2 = 1;
          if (flgTurnRightLane_Left && j != nPoints - 1) {
            //bufferRatios[nLanesRight + 1] = bufferRatios[nLanesRight];
            //offset1++;
            offset2 = 0;
          }
          
          for (int k = 0; k < nLanesLeft; k++) {
            float tmpRatio = 1.0f * (k + offset2) / tmpNumLanesLeft;
            bufferRatios[k + offset1] = tmpWidthLeft * tmpRatio + tmpWidthRight + widthRoadSide;
          }
        }
        
        for (int k = 0; k < nLanesRight + 1; k++) {
          positionLanesRightX[k][j] = rightX + vWidthX * bufferRatios[k];
          positionLanesRightY[k][j] = rightY + vWidthY * bufferRatios[k];
          positionLanesRightZ[k][j] = rightZ + vWidthZ * bufferRatios[k];
        }
        for (int k = 0; k < nLanesLeft + 1; k++) {
          positionLanesLeftX[k][j] = rightX + vWidthX * bufferRatios[k + nLanesRight];
          positionLanesLeftY[k][j] = rightY + vWidthY * bufferRatios[k + nLanesRight];
          positionLanesLeftZ[k][j] = rightZ + vWidthZ * bufferRatios[k + nLanesRight];
        }
        
        _roadPositionRoadSideRightX[i][0][j] = rightX;
        _roadPositionRoadSideRightY[i][0][j] = rightY;
        _roadPositionRoadSideRightZ[i][0][j] = rightZ;
        _roadPositionRoadSideRightX[i][1][j] = positionLanesRightX[0][j];
        _roadPositionRoadSideRightY[i][1][j] = positionLanesRightY[0][j];
        _roadPositionRoadSideRightZ[i][1][j] = positionLanesRightZ[0][j];
        
        _roadPositionRoadSideLeftX[i][0][j] = positionLanesLeftX[nLanesLeft][j];
        _roadPositionRoadSideLeftY[i][0][j] = positionLanesLeftY[nLanesLeft][j];
        _roadPositionRoadSideLeftZ[i][0][j] = positionLanesLeftZ[nLanesLeft][j];
        _roadPositionRoadSideLeftX[i][1][j] = leftX;
        _roadPositionRoadSideLeftY[i][1][j] = leftY;
        _roadPositionRoadSideLeftZ[i][1][j] = leftZ;
        
        _roadPositionSideWalkRightX[i][0][j] = rightX - vWidthX * widthSideWalk;
        _roadPositionSideWalkRightY[i][0][j] = rightY - vWidthY * widthSideWalk;
        _roadPositionSideWalkRightZ[i][0][j] = rightZ - vWidthZ * widthSideWalk;
        _roadPositionSideWalkRightX[i][1][j] = rightX;
        _roadPositionSideWalkRightY[i][1][j] = rightY;
        _roadPositionSideWalkRightZ[i][1][j] = rightZ;
        
        _roadPositionSideWalkLeftX[i][0][j] = leftX;
        _roadPositionSideWalkLeftY[i][0][j] = leftY;
        _roadPositionSideWalkLeftZ[i][0][j] = leftZ;
        _roadPositionSideWalkLeftX[i][1][j] = leftX + vWidthX * widthSideWalk;
        _roadPositionSideWalkLeftY[i][1][j] = leftY + vWidthY * widthSideWalk;
        _roadPositionSideWalkLeftZ[i][1][j] = leftZ + vWidthZ * widthSideWalk;
        
        _roadPositionLanesRightX[i] = positionLanesRightX;
        _roadPositionLanesRightY[i] = positionLanesRightY;
        _roadPositionLanesRightZ[i] = positionLanesRightZ;
        _roadPositionLanesLeftX[i] = positionLanesLeftX;
        _roadPositionLanesLeftY[i] = positionLanesLeftY;
        _roadPositionLanesLeftZ[i] = positionLanesLeftZ;
      }
    }
  }
  
  public void setSignalData(int nTimeSteps, int[] nSignals, int[][] intersectionIDStart, int[][] intersectionIDEnd,
      int[][] signalState) {
    //############################################
    //���setRoadData()�����s����Ă���K�v������
    //############################################
    
    _logger.info("set signal data to roads");
    
    //�M����ID���A���H�̎n�_�ƏI�_�ŕt�����Ă��邽�߁A
    //�g���₷���z��ɕϊ�����
    _signalStateOfStartOfRoad = new int[_nRoads][];
    _signalStateOfEndOfRoad = new int[_nRoads][];
    //�e���H�̎n�_�A�I�_�ɂ��āA�M���@������ꍇ�́A
    //�e���ԃX�e�b�v�ł̐M���̏�Ԃ��i�[����z����m�ۂ���
    for (int i = 0; i < _nRoads; i++) {
      _signalStateOfStartOfRoad[i] = null;
      _signalStateOfStartOfRoad[i] = null;
      if (_roadExistSignalStart[i]) {
        _signalStateOfStartOfRoad[i] = new int[nTimeSteps];
        Arrays.fill(_signalStateOfStartOfRoad[i], SIGNAL_STATE_DEFAULT);
      }
      if (_roadExistSignalEnd[i]) {
        _signalStateOfEndOfRoad[i] = new int[nTimeSteps];
        Arrays.fill(_signalStateOfEndOfRoad[i], SIGNAL_STATE_DEFAULT);
      }
    }
    
    for (int i = 0; i < nTimeSteps; i++) {
      int tmpNumSignals = nSignals[i];
      for (int j = 0; j < tmpNumSignals; j++) {
        int idStart = intersectionIDStart[i][j];
        int idEnd = intersectionIDEnd[i][j];
        int state = signalState[i][j];
        
        boolean flgHit = false;
        for (int roadID = 0; roadID < _nRoads; roadID++) {
          int tmpIDStart = _roadIntersectionIDStart[roadID];
          int tmpIDEnd = _roadIntersectionIDEnd[roadID];
          
          if ((idStart == tmpIDStart) && (idEnd == tmpIDEnd)) {
            flgHit = true;
            //���H�̏I�_�̐M���ɂ���
            if (_roadExistSignalEnd[roadID]) {
              _signalStateOfEndOfRoad[roadID][i] = state;
            } else {
              _logger.info("Error : step=" + i + " : intersection source=" + idStart + " dest=" + idEnd);
            }
          }
          if ((idStart == tmpIDEnd) && (idEnd == tmpIDStart)) {
            flgHit = true;
            //���H�̎n�_�̐M���ɂ���
            if (_roadExistSignalStart[roadID]) {
              _signalStateOfStartOfRoad[roadID][i] = state;
            } else {
              _logger.info("Error : step=" + i + " : intersection source=" + idStart + " dest=" + idEnd);
            }
          }
        }
        if (flgHit == false) {
          _logger.info("Error : no hit : step=" + i + " : intersection source=" + idStart + " dest=" + idEnd);
        }
      }
    }
  }
  
  public void setSignalShapeData(float[][] shapeCoords, int[][] shapeConnectivity, float[][] shapeNormals,
      float[][] shapeColors) {
    _signalShapeCoords = shapeCoords;
    _signalShapeConnectivity = shapeConnectivity;
    _signalShapeNormals = shapeNormals;
    _signalShapeColors = shapeColors;
  }
  
  public void setSignalSimpleShapeData(float[][] shapeCoords, int[][] shapeConnectivity, float[][] shapeNormals,
      float[][] shapeColors) {
    _signalSimpleShapeCoords = shapeCoords;
    _signalSimpleShapeConnectivity = shapeConnectivity;
    _signalSimpleShapeNormals = shapeNormals;
    _signalSimpleShapeColors = shapeColors;
  }
  
  public int getNumTimeSteps() {
    return _nTimeSteps;
  }
  
  public int[] getNumVehicles() {
    return _nVehicles;
  }
  
  public int[][] getVehicleID() {
    return _vehicleID;
  }
  
  public int[][] getVehicleFamilyID() {
    return _vehicleFamilyID;
  }
  
  public VehicleFamily[][] getVehicleFamily() {
    return _vehicleFamily;
  }
  
  public float[][] getVehiclePositionX() {
    return _vehiclePositionX;
  }
  
  public float[][] getVehiclePositionY() {
    return _vehiclePositionY;
  }
  
  public float[][] getVehiclePositionZ() {
    return _vehiclePositionZ;
  }
  
  public float[][] getVehicleAngleXY() {
    return _vehicleAngleXY;
  }
  
  public float[][] getVehicleAngleZ() {
    return _vehicleAngleZ;
  }
  
  public float[][] getVehicleVelocity() {
    return _vehicleVelocity;
  }
  
  public float[][] getVehicleAcceleration() {
    return _vehicleAcceleration;
  }
  
  public int[][] getVehicleIntersectionID() {
    return _vehicleIntersectionID;
  }
  
  public int[][] getVehicleRoadID() {
    return _vehicleRoadID;
  }
  
  public int[][] getSignalStateOfStartOfRoad() {
    return _signalStateOfStartOfRoad;
  }
  
  public int[][] getSignalStateOfEndOfRoad() {
    return _signalStateOfEndOfRoad;
  }
  
  public float getFieldXMin() {
    return _fieldXMin;
  }
  
  public float getFieldYMin() {
    return _fieldYMin;
  }
  
  public float getFieldZMin() {
    return _fieldZMin;
  }
  
  public float getFieldXMax() {
    return _fieldXMax;
  }
  
  public float getFieldYMax() {
    return _fieldYMax;
  }
  
  public float getFieldZMax() {
    return _fieldZMax;
  }
  
  public int getNumIntersections() {
    return _nIntersections;
  }
  
  //public float[] getIntersectionPositionX() {
  //  return _intersectionPositionX;
  //}
  //public float[] getIntersectionPositionY() {
  //  return _intersectionPositionY;
  //}
  //public float[] getIntersectionPositionZ() {
  //  return _intersectionPositionZ;
  //}
  
  public int[] getIntersectionNumShapePoints() {
    return _intersectionNumShapePoints;
  }
  
  public int[] getIntersectionID() {
    return _intersectionID;
  }
  
  public String[] getIntersectionIDString() {
    return _intersectionIDString;
  }
  
  public float[] getIntersectionCenterPositionX() {
    return _intersectionCenterPositionX;
  }
  
  public float[] getIntersectionCenterPositionY() {
    return _intersectionCenterPositionY;
  }
  
  public float[] getIntersectionCenterPositionZ() {
    return _intersectionCenterPositionZ;
  }
  
  public float[][] getIntersectionShapePositionX() {
    return _intersectionShapePositionX;
  }
  
  public float[][] getIntersectionShapePositionY() {
    return _intersectionShapePositionY;
  }
  
  public float[][] getIntersectionShapePositionZ() {
    return _intersectionShapePositionZ;
  }
  
  public int getNumRoads() {
    return _nRoads;
  }
  
  //public int[] getRoadPointIDIn() {
  //  return _roadPointIDIn;
  //}
  //public int[] getRoadPointIDOut() {
  //  return _roadPointIDOut;
  //}
  //public int[] getRoadNumLanesIn() {
  //  return _roadNumLanesIn;
  //}
  //public int[] getRoadNumLanesOut() {
  //  return _roadNumLanesOut;
  //}
  
  public int[] getRoadNumLanesRight() {
    return _roadNumLanesRight;
  }
  
  public int[] getRoadNumLanesLeft() {
    return _roadNumLanesLeft;
  }
  
  public int[] getRoadNumPointsCenter() {
    return _roadNumPointsCenter;
  }
  
  public int[] getRoadNumPointsRight() {
    return _roadNumPointsRight;
  }
  
  public int[] getRoadNumPointsLeft() {
    return _roadNumPointsLeft;
  }
  
  public boolean[] getRoadExistSignalStart() {
    return _roadExistSignalStart;
  }
  
  public boolean[] getRoadExistSignalEnd() {
    return _roadExistSignalEnd;
  }
  
  public float[][] getRoadPositionCenterX() {
    return _roadPositionCenterX;
  }
  
  public float[][] getRoadPositionCenterY() {
    return _roadPositionCenterY;
  }
  
  public float[][] getRoadPositionCenterZ() {
    return _roadPositionCenterZ;
  }
  
  public float[][] getRoadPositionRightX() {
    return _roadPositionRightX;
  }
  
  public float[][] getRoadPositionRightY() {
    return _roadPositionRightY;
  }
  
  public float[][] getRoadPositionRightZ() {
    return _roadPositionRightZ;
  }
  
  public float[][] getRoadPositionLeftX() {
    return _roadPositionLeftX;
  }
  
  public float[][] getRoadPositionLeftY() {
    return _roadPositionLeftY;
  }
  
  public float[][] getRoadPositionLeftZ() {
    return _roadPositionLeftZ;
  }
  
  public float[][] getRoadVectorDirectionX() {
    return _roadVectorDirectionX;
  }
  
  public float[][] getRoadVectorDirectionY() {
    return _roadVectorDirectionY;
  }
  
  public float[][] getRoadVectorDirectionZ() {
    return _roadVectorDirectionZ;
  }
  
  public float[][] getRoadVectorPerpendicularX() {
    return _roadVectorPerpendicularX;
  }
  
  public float[][] getRoadVectorPerpendicularY() {
    return _roadVectorPerpendicularY;
  }
  
  public float[][] getRoadVectorPerpendicularZ() {
    return _roadVectorPerpendicularZ;
  }
  
  public float[][][] getRoadPositionLanesRightX() {
    return _roadPositionLanesRightX;
  }
  
  public float[][][] getRoadPositionLanesRightY() {
    return _roadPositionLanesRightY;
  }
  
  public float[][][] getRoadPositionLanesRightZ() {
    return _roadPositionLanesRightZ;
  }
  
  public float[][][] getRoadPositionLanesLeftX() {
    return _roadPositionLanesLeftX;
  }
  
  public float[][][] getRoadPositionLanesLeftY() {
    return _roadPositionLanesLeftY;
  }
  
  public float[][][] getRoadPositionLanesLeftZ() {
    return _roadPositionLanesLeftZ;
  }
  
  public float[][][] getRoadPositionRoadSideRightX() {
    return _roadPositionRoadSideRightX;
  }
  
  public float[][][] getRoadPositionRoadSideRightY() {
    return _roadPositionRoadSideRightY;
  }
  
  public float[][][] getRoadPositionRoadSideRightZ() {
    return _roadPositionRoadSideRightZ;
  }
  
  public float[][][] getRoadPositionRoadSideLeftX() {
    return _roadPositionRoadSideLeftX;
  }
  
  public float[][][] getRoadPositionRoadSideLeftY() {
    return _roadPositionRoadSideLeftY;
  }
  
  public float[][][] getRoadPositionRoadSideLeftZ() {
    return _roadPositionRoadSideLeftZ;
  }
  
  public float[][][] getRoadPositionSideWalkRightX() {
    return _roadPositionSideWalkRightX;
  }
  
  public float[][][] getRoadPositionSideWalkRightY() {
    return _roadPositionSideWalkRightY;
  }
  
  public float[][][] getRoadPositionSideWalkRightZ() {
    return _roadPositionSideWalkRightZ;
  }
  
  public float[][][] getRoadPositionSideWalkLeftX() {
    return _roadPositionSideWalkLeftX;
  }
  
  public float[][][] getRoadPositionSideWalkLeftY() {
    return _roadPositionSideWalkLeftY;
  }
  
  public float[][][] getRoadPositionSideWalkLeftZ() {
    return _roadPositionSideWalkLeftZ;
  }
  
  public float[][] getSignalShapeCoords() {
    return _signalShapeCoords;
  }
  
  public int[][] getSignalShapeConnectivity() {
    return _signalShapeConnectivity;
  }
  
  public float[][] getSignalShapeNormals() {
    return _signalShapeNormals;
  }
  
  public float[][] getSignalShapeColors() {
    return _signalShapeColors;
  }
  
  public float[][] getSignalSimpleShapeCoords() {
    return _signalSimpleShapeCoords;
  }
  
  public int[][] getSignalSimpleShapeConnectivity() {
    return _signalSimpleShapeConnectivity;
  }
  
  public float[][] getSignalSimpleShapeNormals() {
    return _signalSimpleShapeNormals;
  }
  
  public float[][] getSignalSimpleShapeColors() {
    return _signalSimpleShapeColors;
  }
}
