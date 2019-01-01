package jp.ac.u_tokyo.t.q.adventure.mates.model;

import java.util.HashMap;
import java.util.Properties;
import java.util.Set;
import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

public class TrafficParameters {
  //@formatter:off
  //######################################################################
  //GVInitializer.cpp����󂯌p��������
  
  // �ڍ׏����o�͂��邩
  //private static final boolean FLAG_VERBOSE                      = true;
  // �n�}������͂��邩
  //private static final boolean FLAG_INPUT_MAP                    = true;
  // �M��������͂��邩
  //private static final boolean FLAG_INPUT_SIGNAL                 = true;
  // �ԗ�������͂��邩
  //private static final boolean FLAG_INPUT_VEHICLE                = true;
  // �ԗ�������񂪒�`����Ă��Ȃ������_����ԗ��𔭐������邩
  //private static final boolean FLAG_GEN_RAND_VEHICLE             = true;
  // ���n��f�[�^���o�͂��邩
  //private static final boolean FLAG_OUTPUT_TIMELINE              = true;
  // �v���@��̏ڍ׃f�[�^���o�͂��邩
  //private static final boolean FLAG_OUTPUT_MONITOR_D             = true;
  // �v���@��̓��v�f�[�^���o�͂��邩
  //private static final boolean FLAG_OUTPUT_MONITOR_S             = true;
  // �G�[�W�F���g�����f�[�^���o�͂��邩
  //private static final boolean FLAG_OUTPUT_GEN_COUNTER           = true;
  // �G�[�W�F���g�̑��s�����C���s���Ԃ��o�͂��邩
  //private static final boolean FLAG_OUTPUT_TRIP_INFO             = true;
  // ���x������ۑ����邩
  //private static final boolean VEHICLE_VELOCITY_HISTORY_RECORD   = true;
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // �O���[�o���ϐ��ݒ�t�@�C��
  public static final String  FILE_NAME_GV_INIT                         = "init.txt";
  // �n�}�Ɋ֘A����t�@�C��
  private static final String DEFAULT_FILE_MAP_POSITION                 = "mapPosition.txt";
  private static final String DEFAULT_FILE_NETWORK                      = "network.txt";
  private static final String DEFAULT_FILE_SPEED_LIMIT                  = "speedLimit.txt";
  // �ԗ������Ɋւ���t�@�C��
  private static final String DEFAULT_FILE_GENERATE_TABLE               = "generateTable.txt";
  private static final String DEFAULT_FILE_DEFAULT_GENERATE_TABLE       = "defaultGenerateTable.txt";
  // �Ԏ�Ɋւ���t�@�C��
  private static final String DEFAULT_FILE_VEHICLE_FAMILY               = "vehicleFamily.txt";
  // �����ԃG�[�W�F���g�̌o�H�Ɋւ���t�@�C��
  private static final String DEFAULT_FILE_VEHICLE_ROUTE_PARAM          = "vehicleRoutingParam.txt";
  // �H����Ɋւ���t�@�C��
  private static final String DEFAULT_FILE_GENCOUNTER                   = "genCounter.txt";
  private static final String DEFAULT_FILE_DETECTOR                     = "detector.txt";
  // �M���Ɋւ���f�B���N�g��or�t�@�C��or�g���q
  private static final String DEFAULT_DIRECTORY_SIGNAL_CONTROL          = "signals";
  private static final String DEFAULT_FILE_DEFAULT_SIGNAL_CONTROL       = "default";
  private static final String DEFAULT_SIGNAL_ASPECT_FILE_PREFIX         = "defaultInter";
  private static final String DEFAULT_CONTROL_FILE_EXTENSION            = ".msf";
  private static final String DEFAULT_ASPECT_FILE_EXTENSION             = ".msa";
  // �����_�����w��t�@�C��
  private static final String DEFAULT_DIRECTORY_INTERSECTION_ATTRIBUTE  = "intersection";
  // ���H�`��Ɋւ���t�@�C��
  private static final String DEFAULT_FILE_INTERSECTION_STRUCT          = "intersectionStruct.txt";
  private static final String DEFAULT_FILE_SECTION_STRUCT               = "sectionStruct.txt";
  // �o�͐�
  private static final String DEFAULT_DIRECTORY_RESULT                  = "result";
  private static final String DEFAULT_DIRECTORY_RESULT_OUTPUT           = DEFAULT_DIRECTORY_RESULT;
  private static final String DEFAULT_DIRECTORY_RESULT_TIMELINE         = DEFAULT_DIRECTORY_RESULT + Common.FILE_SEARATOR + "timeline";
  private static final String DEFAULT_DIRECTORY_RESULT_IMG              = DEFAULT_DIRECTORY_RESULT + Common.FILE_SEARATOR + "img";
  private static final String DEFAULT_DIRECTORY_RESULT_INSTRUMENT       = DEFAULT_DIRECTORY_RESULT + Common.FILE_SEARATOR + "inst";
  private static final String DEFAULT_RESULT_DETECTOR_D_PREFIX          = "detD";
  private static final String DEFAULT_RESULT_DETECTOR_S_PREFIX          = "detS";
  private static final String DEFAULT_RESULT_GENCOUNTER_PREFIX          = "gen";
  private static final String DEFAULT_FILE_RESULT_RUN_INFO              = DEFAULT_DIRECTORY_RESULT + Common.FILE_SEARATOR + "runInfo.txt";
  private static final String DEFAULT_FILE_RESULT_NODE_SHAPE            = DEFAULT_DIRECTORY_RESULT + Common.FILE_SEARATOR + "nodeShape.txt";
  private static final String DEFAULT_FILE_RESULT_LINK_SHAPE            = DEFAULT_DIRECTORY_RESULT + Common.FILE_SEARATOR + "linkShape.txt";
  private static final String DEFAULT_FILE_RESULT_SIGNAL_COUNT          = DEFAULT_DIRECTORY_RESULT + Common.FILE_SEARATOR + "signalCount.txt";
  private static final String DEFAULT_FILE_RESULT_VEHICLE_ATTRIBUTE     = DEFAULT_DIRECTORY_RESULT + Common.FILE_SEARATOR + "vehicleAttribute.txt";
  private static final String DEFAULT_FILE_RESULT_VEHICLE_TRIP          = DEFAULT_DIRECTORY_RESULT + Common.FILE_SEARATOR + "vehicleTrip.txt";
  private static final String DEFAULT_FILE_RESULT_VEHICLE_COUNT         = DEFAULT_DIRECTORY_RESULT + Common.FILE_SEARATOR + "vehicleCount.txt";
  
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // �萔�̒�`
  // ���GVManager::setVariablesFromFile()�ɂ���ď㏑�������
  // �V�~�����[�V�����̎��s�Ɋւ������
  // �o�̓t�@�C���Ɋe��R�����g��t���ďo�͂���
  //private static final float OUTPUT_COMMENT_IN_FILE              = 0;
  // �v���̗L���A0 �ȊO�Ȃ炠��
  //private static final float CHECK_TIME                          = 0;
  // �G�[�W�F���g�Ɋւ������
  // �����Ԃ̔����x�ꎞ��[sec]
  //private static final float REACTION_TIME_VEHICLE               = 0.74f;
  // �����Ԃ������_�ŉE�܂���ꍇ�Ȃǂ�
  // �Ό��ԂƂ̃M���b�v�A�N�Z�v�^���X[sec]
  //private static final float GAP_ACCEPTANCE_VEHICLE_CROSS        = 3.0f;
  // ���ʎ�(VehicleFamily::passenger())�̍ő�����x�C�����x[m/(sec^2)]
  //private static final float MAX_ACCELERATION_PASSENGER          = 3.0f;
  //private static final float MAX_DECELERATION_PASSENGER          = -5.0f;
  // �o�X(VehicleFamily::bus())�̍ő�����x�C�����x[m/(sec^2)]
  //private static final float MAX_ACCELERATION_BUS                = 3.0f;
  //private static final float MAX_DECELERATION_BUS                = -5.0f;
  // ��^��(VehicleFamily::truck())�̍ő�����x�C�����x[m/(sec^2)]
  //private static final float MAX_ACCELERATION_TRUCK              = 3.0f;
  //private static final float MAX_DECELERATION_TRUCK              = -5.0f;
  // �Ԑ��ύX���ɗ^���鉡�����̑��x[km/h]
  //private static final float ERROR_VELOCITY                      = 7.5f;
  // �����[�_����L[m]�ȓ��̎ԗ��͏o�͂��Ȃ�
  //private static final float NO_OUTPUT_LENGTH_FROM_ORIGIN_NODE   = 0f;
  // �ԗ����O�̒ǉ������o�͂���
  //private static final float OUTPUT_VEHICLE_EXTENSION            = 0f;
  
  //VehicleFamily�Ɉړ�
  // ���ʎ�(VehicleFamily::passenger())�̃T�C�Y[m]
  //private static final float DEFAULT_VEHICLE_LENGTH_PASSENGER            = 4.400f;
  //private static final float DEFAULT_VEHICLE_WIDTH_PASSENGER             = 1.830f;
  //private static final float DEFAULT_VEHICLE_HEIGHT_PASSENGER            = 1.315f;
  // �o�X(VehicleFamily::bus())�̃T�C�Y[m]
  //private static final float DEFAULT_VEHICLE_LENGTH_BUS                  = 8.465f;
  //private static final float DEFAULT_VEHICLE_WIDTH_BUS                   = 2.230f;
  //private static final float DEFAULT_VEHICLE_HEIGHT_BUS                  = 3.420f;
  // ��^��(VehicleFamily::truck())�̃T�C�Y[m]
  //private static final float DEFAULT_VEHICLE_LENGTH_TRUCK                = 8.465f;
  //private static final float DEFAULT_VEHICLE_WIDTH_TRUCK                 = 2.230f;
  //private static final float DEFAULT_VEHICLE_HEIGHT_TRUCK                = 3.420f;
    
  // �����������ɕ]��
  //private static final float STRICT_COLLISION_CHECK              = 1f;
  // ���x������ۑ�����X�e�b�v��
  //private static final float VEHICLE_VELOCITY_HISTORY_SIZE       = 180f;
  // ���x������ۑ�����X�e�b�v�Ԋu
  //private static final float VEHICLE_VELOCITY_HISTORY_INTERVAL   = 10f;
  // ���H�Ɋւ������
  // �E�ܐ�p���[���̕W������[m]
  private static final float DEFAULT_RIGHT_TURN_LANE_LENGTH              = 30f;
  // �W���������x[km/h]
  // ������SPEED_LIMIT_INTERSECTION���p�����邱�Ƃ͂قƂ�ǖ����C
  // �E���܎��͉���VELOCITY_AT���g���C
  // ���i���͎��̒P�H�ł�SPEED_LIMIT���Q�Ƃ����D
  //private static final float SPEED_LIMIT_SECTION                 = 60f;
  //private static final float SPEED_LIMIT_INTERSECTION            = 60f;
  // ���s���x[km/h]
  //private static final float VELOCITY_CRAWL                      = 10f;
  // �E���܎��̑��x[km/h]
  //private static final float VELOCITY_AT_TURNING_RIGHT           = 40f;
  //private static final float VELOCITY_AT_TURNING_LEFT            = 40f;
  // �ԗ��������̐������x[km/h]�A���Ȃ�Ȃ�
  //private static final float GENERATE_VELOCITY_LIMIT             = -1f;
  // �E���܎��̍ŏ��w�b�h�E�F�C[�b]
  //private static final float MIN_HEADWAY_AT_TURNING              = 1.7f;
  // �ԗ������ʊ���l[��/h]�A�������[�� 3 �ȏ� / 2 / 1�A��{��ʗe�ʂ�10%
  //private static final float DEFAULT_TRAFFIC_VOLUME_WIDE         = 660f;
  //private static final float DEFAULT_TRAFFIC_VOLUME_NORMAL       = 440f;
  //private static final float DEFAULT_TRAFFIC_VOLUME_NARROW       = 125f;
  
  // �W���̃��[�����A�������A���f�������A�H����
  private static final String DEFAULT_LANE_WIDTH_KEY              = "DEFAULT_LANE_WIDTH";
  private static final float  DEFAULT_LANE_WIDTH                  = 3.5f;
  private static final String DEFAULT_SIDEWALK_WIDTH_KEY          = "DEFAULT_SIDEWALK_WIDTH";
  private static final float  DEFAULT_SIDEWALK_WIDTH              = 5.0f;
  private static final String DEFAULT_CROSSWALK_WIDTH_KEY         = "DEFAULT_CROSSWALK_WIDTH";
  private static final float  DEFAULT_CROSSWALK_WIDTH             = 5.0f;
  private static final String DEFAULT_ROADSIDE_WIDTH_KEY          = "DEFAULT_ROADSIDE_WIDTH";
  private static final float  DEFAULT_ROADSIDE_WIDTH              = 1.0f;
  // �P�H�̕����������ݒ肷��ۂ̃��[�����A-1 �Ȃ玩���ݒ�Ȃ�
  // �����ݒ肠��̏ꍇ�A�P�H�̑S���[����������ȏ�Ȃ������ݒ肷��
  // ����ȉ��Ȃ�ԓ��ʍs�\�ɂ���
  //private static final float AUTO_SIDEWALK_SECTION_LANE          = -1f;
  // ���H�G���e�B�e�B�̌����ȓ�������A1 �Ȃ炠��
  // ���^�̓��H�G���e�B�e�B�ł�����\�ɂȂ�A�������������x�͒x��
  //private static final float ROAD_ENTITY_STRICT_JUDGE_INSIDE     = 1f;
  // �����_�T�C�Y�����A���S����̋���[m]
  //private static final float INTERSECTION_SIZE_LIMIT             = 20f;
  
  
  //######################################################################
  //GVInitializer.cpp�ɂ͖���������萔
  private static final String DEFAULT_DIRECTORY_RESULT_VEHICLE       = DEFAULT_DIRECTORY_RESULT_TIMELINE + Common.FILE_SEARATOR + "vehicle";
  private static final String DEFAULT_DIRECTORY_RESULT_SIGNAL        = DEFAULT_DIRECTORY_RESULT_TIMELINE + Common.FILE_SEARATOR + "signal";
  
  //                                                                   {  red,green, blue,alpha }
  private static final String  DEFAULT_COLOR_GLASS_KEY               = "Color.GLASS";
  private static final float[] DEFAULT_COLOR_GLASS                   = { 0.9f, 0.9f, 0.9f, 1.0f};
  private static final String  DEFAULT_COLOR_TIRE_KEY                = "Color.TIRE";
  private static final float[] DEFAULT_COLOR_TIRE                    = { 0.0f, 0.0f, 0.0f, 1.0f};
  private static final String  DEFAULT_COLOR_WHEEL_KEY               = "Color.WHEEL";
  private static final float[] DEFAULT_COLOR_WHEEL                   = { 0.7f, 0.7f, 0.7f, 1.0f};
  
  private static final String  DEFAULT_COLOR_GROUND_KEY              = "Color.GROUND";
  private static final float[] DEFAULT_COLOR_GROUND                  = { 0.3f, 0.2f, 0.0f, 1.0f };
  private static final String  DEFAULT_COLOR_GROUND_BACK_KEY         = "Color.GROUND_BACK";
  private static final float[] DEFAULT_COLOR_GROUND_BACK             = { 1.0f, 0.0f, 0.0f, 1.0f };
  private static final String  DEFAULT_COLOR_ROAD_KEY                = "Color.ROAD";
  private static final float[] DEFAULT_COLOR_ROAD                    = { 0.7f, 0.7f, 0.7f, 1.0f };
  //private static final String  DEFAULT_COLOR_ROAD_RIGHT_KEY          = "Color.ROAD_RIGHT";
  //private static final float[] DEFAULT_COLOR_ROAD_RIGHT              = { 0.0f, 0.0f, 1.0f, 1.0f };
  //private static final String  DEFAULT_COLOR_ROAD_LEFT_KEY           = "Color.ROAD_LEFT";
  //private static final float[] DEFAULT_COLOR_ROAD_LEFT               = { 1.0f, 0.0f, 0.0f, 1.0f };
  private static final String  DEFAULT_COLOR_ROAD_CENTER_LINE_1_KEY  = "Color.ROAD_CENTER_LINE_1";
  private static final float[] DEFAULT_COLOR_ROAD_CENTER_LINE_1      = { 1.0f, 0.5f, 0.0f, 1.0f };
  private static final String  DEFAULT_COLOR_ROAD_CENTER_LINE_2_KEY  = "Color.ROAD_CENTER_LINE_2";
  private static final float[] DEFAULT_COLOR_ROAD_CENTER_LINE_2      = { 0.9f, 0.9f, 0.9f, 1.0f };
  private static final String  DEFAULT_COLOR_ROADSIDE_KEY            = "Color.ROADSIDE";
  private static final float[] DEFAULT_COLOR_ROADSIDE                = { 0.5f, 0.5f, 0.5f, 1.0f };
  private static final String  DEFAULT_COLOR_SIDEWALK_KEY            = "Color.SIDEWALK";
  private static final float[] DEFAULT_COLOR_SIDEWALK                = { 0.0f, 1.0f, 1.0f, 1.0f };
  //private static final String  DEFAULT_COLOR_ROAD_LINE_KEY           = "Color.ROAD_LINE";
  //private static final float[] DEFAULT_COLOR_ROAD_LINE               = { 0.0f, 0.0f, 0.0f, 1.0f };
  
  private static final String  DEFAULT_COLOR_SIGNAL_GREEN_KEY        = "Color.SIGNAL_GREEN";
  private static final float[] DEFAULT_COLOR_SIGNAL_GREEN            = { 0.0f, 1.0f, 0.5f, 1.0f };
  private static final String  DEFAULT_COLOR_SIGNAL_YELLOW_KEY       = "Color.SIGNAL_YELLOW";
  private static final float[] DEFAULT_COLOR_SIGNAL_YELLOW           = { 1.0f, 1.0f, 0.0f, 1.0f };
  private static final String  DEFAULT_COLOR_SIGNAL_RED_KEY          = "Color.SIGNAL_RED";
  private static final float[] DEFAULT_COLOR_SIGNAL_RED              = { 1.0f, 0.0f, 0.0f, 1.0f };
  private static final String  DEFAULT_COLOR_SIGNAL_DIRECTION_KEY    = "Color.SIGNAL_DIRECTION";
  private static final float[] DEFAULT_COLOR_SIGNAL_DIRECTION        = { 0.0f, 1.0f, 1.0f, 1.0f };
  
  private static final String  KEY_COLOR_PRE                         = "Color.";
  //@formatter:on
  
  /**
   * ���O�n���h�����O�N���X
   */
  private Logger _logger;
  
  private String _fileNameMapPosition = DEFAULT_FILE_MAP_POSITION;
  private String _fileNameNetwork = DEFAULT_FILE_NETWORK;
  private String _fileNameNodeShape = DEFAULT_FILE_RESULT_NODE_SHAPE;
  private String _fileNameLinkShape = DEFAULT_FILE_RESULT_LINK_SHAPE;
  private String _fileNameVehicleFamily = DEFAULT_FILE_VEHICLE_FAMILY;
  private String _dirNameResultVehicle = DEFAULT_DIRECTORY_RESULT_VEHICLE;
  private String _dirNameResultSignal = DEFAULT_DIRECTORY_RESULT_SIGNAL;
  
  private float _widthLane = DEFAULT_LANE_WIDTH;
  private float _widthSideWalk = DEFAULT_SIDEWALK_WIDTH;
  private float _widthRoadSide = DEFAULT_ROADSIDE_WIDTH;
  
  //private float[] _colorGlass = null;
  //private float[] _colorTire = null;
  //private float[] _colorWheel = null;
  
  private float[] _colorGround = null;
  private float[] _colorGroundBack = null;
  private float[] _colorRoad = null;
  private float[] _colorRoadCenterLine1 = null;
  private float[] _colorRoadCenterLine2 = null;
  private float[] _colorRoadSide = null;
  private float[] _colorSideWalk = null;
  private float[] _colorSignalGreen = null;
  private float[] _colorSignalYellow = null;
  private float[] _colorSignalRed = null;
  private float[] _colorSignalDirection = null;
  
  private HashMap<String, float[]> _colorMap;
  
  public TrafficParameters() {
    _logger = Logger.getLogger(TrafficParameters.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    
    _colorMap = new HashMap<String, float[]>();
    setDefaultColor();
  }
  
  private void setDefaultColor() {
    _colorGround = Common.copyArrayFloat(DEFAULT_COLOR_GROUND);
    _colorGroundBack = Common.copyArrayFloat(DEFAULT_COLOR_GROUND_BACK);
    _colorRoad = Common.copyArrayFloat(DEFAULT_COLOR_ROAD);
    _colorRoadCenterLine1 = Common.copyArrayFloat(DEFAULT_COLOR_ROAD_CENTER_LINE_1);
    _colorRoadCenterLine2 = Common.copyArrayFloat(DEFAULT_COLOR_ROAD_CENTER_LINE_2);
    _colorRoadSide = Common.copyArrayFloat(DEFAULT_COLOR_ROADSIDE);
    _colorSideWalk = Common.copyArrayFloat(DEFAULT_COLOR_SIDEWALK);
    _colorSignalGreen = Common.copyArrayFloat(DEFAULT_COLOR_SIGNAL_GREEN);
    _colorSignalYellow = Common.copyArrayFloat(DEFAULT_COLOR_SIGNAL_YELLOW);
    _colorSignalRed = Common.copyArrayFloat(DEFAULT_COLOR_SIGNAL_RED);
    _colorSignalDirection = Common.copyArrayFloat(DEFAULT_COLOR_SIGNAL_DIRECTION);
    
    _colorMap.put(DEFAULT_COLOR_GLASS_KEY, Common.copyArrayFloat(DEFAULT_COLOR_GLASS));
    _colorMap.put(DEFAULT_COLOR_TIRE_KEY, Common.copyArrayFloat(DEFAULT_COLOR_TIRE));
    _colorMap.put(DEFAULT_COLOR_WHEEL_KEY, Common.copyArrayFloat(DEFAULT_COLOR_WHEEL));
    
    _colorMap.put(DEFAULT_COLOR_GROUND_KEY, _colorGround);
    _colorMap.put(DEFAULT_COLOR_GROUND_BACK_KEY, _colorGroundBack);
    _colorMap.put(DEFAULT_COLOR_ROAD_KEY, _colorRoad);
    _colorMap.put(DEFAULT_COLOR_ROAD_CENTER_LINE_1_KEY, _colorRoadCenterLine1);
    _colorMap.put(DEFAULT_COLOR_ROAD_CENTER_LINE_2_KEY, _colorRoadCenterLine2);
    _colorMap.put(DEFAULT_COLOR_ROADSIDE_KEY, _colorRoadSide);
    _colorMap.put(DEFAULT_COLOR_SIDEWALK_KEY, _colorSideWalk);
    _colorMap.put(DEFAULT_COLOR_SIGNAL_GREEN_KEY, _colorSignalGreen);
    _colorMap.put(DEFAULT_COLOR_SIGNAL_YELLOW_KEY, _colorSignalYellow);
    _colorMap.put(DEFAULT_COLOR_SIGNAL_RED_KEY, _colorSignalRed);
    _colorMap.put(DEFAULT_COLOR_SIGNAL_DIRECTION_KEY, _colorSignalDirection);
  }
  
  public void setParameters(Properties properties) {
    if (properties != null) {
      Set<String> setPropertyNames = properties.stringPropertyNames();
      StringBuffer sbError = new StringBuffer();
      
      for (String key : setPropertyNames) {
        if (key != null) {
          String value = properties.getProperty(key);
          
          int[] valuesInt = null;
          float[] valuesFloat = null;
          boolean[] valuesBoolean = null;
          boolean isOKInt = false;
          boolean isOKFloat = false;
          boolean isOKBoolean = false;
          
          if (value != null) {
            String[] valueSplit = value.split(Common.COMMA);
            
            //value�ɑ΂�int��,float��,boolean�������݂�
            try {
              if (valueSplit.length >= 0) {
                valuesInt = new int[valueSplit.length];
                for (int i = 0; i < valueSplit.length; i++) {
                  String tmpValue = valueSplit[i].trim();
                  valuesInt[i] = Integer.parseInt(tmpValue);
                }
                isOKInt = true;
              }
            } catch (Exception e) {
            }
            try {
              if (valueSplit.length >= 0) {
                valuesFloat = new float[valueSplit.length];
                for (int i = 0; i < valueSplit.length; i++) {
                  String tmpValue = valueSplit[i].trim();
                  valuesFloat[i] = Float.parseFloat(tmpValue);
                }
                isOKFloat = true;
              }
            } catch (Exception e) {
            }
            try {
              if (valueSplit.length >= 0) {
                valuesBoolean = new boolean[valueSplit.length];
                boolean tmpIsOK = true;
                for (int i = 0; i < valueSplit.length; i++) {
                  String tmpValue = valueSplit[i].trim();
                  if (tmpValue.equalsIgnoreCase(Common.LABEL_TRUE) || tmpValue.equalsIgnoreCase(Common.LABEL_ON)) {
                    valuesBoolean[i] = true;
                  } else if (tmpValue.equalsIgnoreCase(Common.LABEL_FALSE)
                      || tmpValue.equalsIgnoreCase(Common.LABEL_OFF)) {
                    valuesBoolean[i] = false;
                  } else {
                    tmpIsOK = false;
                  }
                }
                if (tmpIsOK) {
                  isOKBoolean = true;
                }
              }
            } catch (Exception e) {
            }
            
            if (key.equals(DEFAULT_LANE_WIDTH_KEY)) {
              if (isOKFloat && valuesFloat.length >= 1) {
                _widthLane = valuesFloat[0];
              } else {
                if (sbError.length() > 0) sbError.append(Common.EOL);
                sbError.append("key=" + key + " : value=" + value);
              }
            } else if (key.equals(DEFAULT_SIDEWALK_WIDTH_KEY)) {
              if (isOKFloat && valuesFloat.length >= 1) {
                _widthSideWalk = valuesFloat[0];
              } else {
                if (sbError.length() > 0) sbError.append(Common.EOL);
                sbError.append("key=" + key + " : value=" + value);
              }
            } else if (key.equals(DEFAULT_ROADSIDE_WIDTH_KEY)) {
              if (isOKFloat && valuesFloat.length >= 1) {
                _widthRoadSide = valuesFloat[0];
              } else {
                if (sbError.length() > 0) sbError.append(Common.EOL);
                sbError.append("key=" + key + " : value=" + value);
              }
            } else if (key.startsWith(KEY_COLOR_PRE)) {
              boolean isColorOK = false;
              if (isOKFloat && valuesFloat.length >= 3) {
                isColorOK = true;
                if (valuesFloat[0] < 0.0) isColorOK = false;
                if (valuesFloat[0] > 1.0) isColorOK = false;
                if (valuesFloat[1] < 0.0) isColorOK = false;
                if (valuesFloat[1] > 1.0) isColorOK = false;
                if (valuesFloat[2] < 0.0) isColorOK = false;
                if (valuesFloat[2] > 1.0) isColorOK = false;
                if (valuesFloat.length >= 4) {
                  //�����x���L�q����Ă���ꍇ�́A���̒l�����ׂ�
                  if (valuesFloat[3] < 0.0) isColorOK = false;
                  if (valuesFloat[3] > 1.0) isColorOK = false;
                }
              }
              
              if (isColorOK) {
                float[] colorForMap = new float[4];
                colorForMap[0] = valuesFloat[0];
                colorForMap[1] = valuesFloat[1];
                colorForMap[2] = valuesFloat[2];
                colorForMap[3] = 1.0f;
                if (valuesFloat.length >= 4) {
                  colorForMap[3] = valuesFloat[3];
                }
                _colorMap.put(key, colorForMap);
                
                if (key.equals(DEFAULT_COLOR_GROUND_KEY)) {
                  copyColor(valuesFloat, _colorGround);
                } else if (key.equals(DEFAULT_COLOR_GROUND_BACK_KEY)) {
                  copyColor(valuesFloat, _colorGroundBack);
                } else if (key.equals(DEFAULT_COLOR_ROAD_KEY)) {
                  copyColor(valuesFloat, _colorRoad);
                } else if (key.equals(DEFAULT_COLOR_ROAD_CENTER_LINE_1_KEY)) {
                  copyColor(valuesFloat, _colorRoadCenterLine1);
                } else if (key.equals(DEFAULT_COLOR_ROAD_CENTER_LINE_2_KEY)) {
                  copyColor(valuesFloat, _colorRoadCenterLine2);
                } else if (key.equals(DEFAULT_COLOR_ROADSIDE_KEY)) {
                  copyColor(valuesFloat, _colorRoadSide);
                } else if (key.equals(DEFAULT_COLOR_SIDEWALK_KEY)) {
                  copyColor(valuesFloat, _colorSideWalk);
                } else if (key.equals(DEFAULT_COLOR_SIGNAL_GREEN_KEY)) {
                  copyColor(valuesFloat, _colorSignalGreen);
                } else if (key.equals(DEFAULT_COLOR_SIGNAL_YELLOW_KEY)) {
                  copyColor(valuesFloat, _colorSignalYellow);
                } else if (key.equals(DEFAULT_COLOR_SIGNAL_RED_KEY)) {
                  copyColor(valuesFloat, _colorSignalRed);
                } else if (key.equals(DEFAULT_COLOR_SIGNAL_DIRECTION_KEY)) {
                  copyColor(valuesFloat, _colorSignalDirection);
                }
              } else {
                if (sbError.length() > 0) sbError.append(Common.EOL);
                sbError.append("key=" + key + " : value=" + value);
              }
            }
          }
        }
      }
      
      if (sbError.length() > 0) {
        sbError.insert(0, Common.EOL);
        sbError.insert(0, "");
        _logger.info(sbError.toString());
      }
    }
  }
  
  //private static void setParameterForColor(String key, String value, boolean isOKFloat, float[] valuesFloat,
  //    float[] colorTarget, StringBuffer sbError) {
  //  if (isOKFloat && valuesFloat.length >= 3) {
  //    for (int i = 0; i < 3; i++) {
  //      colorTarget[i] = valuesFloat[i];
  //    }
  //  } else {
  //    if (sbError.length() > 0) sbError.append(Common.EOL);
  //    sbError.append("key=" + key + " : value=" + value);
  //  }
  //}
  
  private static void copyColor(float[] colorSource, float[] colorTarget) {
    for (int i = 0; i < 3; i++) {
      colorTarget[i] = colorSource[i];
    }
    if (colorSource.length >= 4 && colorTarget.length >= 4) {
      colorTarget[3] = colorSource[3];
    }
  }
  
  public String getFileNameOfMapPosition() {
    return _fileNameMapPosition;
  }
  
  public String getFileNameOfNetwork() {
    return _fileNameNetwork;
  }
  
  public String getFileNameOfNodeShape() {
    return _fileNameNodeShape;
  }
  
  public String getFileNameOfLinkShape() {
    return _fileNameLinkShape;
  }
  
  public String getFileNameOfVehicleFamily() {
    return _fileNameVehicleFamily;
  }
  
  public String getDirectoryNameOfResultVehicle() {
    return _dirNameResultVehicle;
  }
  
  public String getDirectoryNameOfResultSignal() {
    return _dirNameResultSignal;
  }
  
  public float getWidthOfLane() {
    return _widthLane;
  }
  
  public float getWidthOfSideWalk() {
    return _widthSideWalk;
  }
  
  public float getWidthOfRoadSide() {
    return _widthRoadSide;
  }
  
  public float[] getColor(String key) {
    return _colorMap.get(key);
  }
  
  //public float[] getColorForGlass() {
  //  return _colorGlass;
  //}
  
  //public float[] getColorForTire() {
  //  return _colorTire;
  //}
  
  //public float[] getColorForWheel() {
  //  return _colorWheel;
  //}
  
  public float[] getColorForGround() {
    return _colorGround;
  }
  
  public float[] getColorForGroundBack() {
    return _colorGroundBack;
  }
  
  public float[] getColorForRoad() {
    return _colorRoad;
  }
  
  public float[] getColorForRoadCenterLine1() {
    return _colorRoadCenterLine1;
  }
  
  public float[] getColorForRoadCenterLine2() {
    return _colorRoadCenterLine2;
  }
  
  public float[] getColorForRoadSide() {
    return _colorRoadSide;
  }
  
  public float[] getColorForSideWalk() {
    return _colorSideWalk;
  }
  
  public float[] getColorForSignalGreen() {
    return _colorSignalGreen;
  }
  
  public float[] getColorForSignalYellow() {
    return _colorSignalYellow;
  }
  
  public float[] getColorForSignalRed() {
    return _colorSignalRed;
  }
  
  public float[] getColorForSignalDirection() {
    return _colorSignalDirection;
  }
}
