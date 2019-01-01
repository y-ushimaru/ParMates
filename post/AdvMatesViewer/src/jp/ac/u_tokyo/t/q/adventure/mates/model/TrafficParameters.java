package jp.ac.u_tokyo.t.q.adventure.mates.model;

import java.util.HashMap;
import java.util.Properties;
import java.util.Set;
import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

public class TrafficParameters {
  //@formatter:off
  //######################################################################
  //GVInitializer.cppから受け継いだ項目
  
  // 詳細情報を出力するか
  //private static final boolean FLAG_VERBOSE                      = true;
  // 地図情報を入力するか
  //private static final boolean FLAG_INPUT_MAP                    = true;
  // 信号情報を入力するか
  //private static final boolean FLAG_INPUT_SIGNAL                 = true;
  // 車両情報を入力するか
  //private static final boolean FLAG_INPUT_VEHICLE                = true;
  // 車両発生情報が定義されていない交差点から車両を発生させるか
  //private static final boolean FLAG_GEN_RAND_VEHICLE             = true;
  // 時系列データを出力するか
  //private static final boolean FLAG_OUTPUT_TIMELINE              = true;
  // 計測機器の詳細データを出力するか
  //private static final boolean FLAG_OUTPUT_MONITOR_D             = true;
  // 計測機器の統計データを出力するか
  //private static final boolean FLAG_OUTPUT_MONITOR_S             = true;
  // エージェント発生データを出力するか
  //private static final boolean FLAG_OUTPUT_GEN_COUNTER           = true;
  // エージェントの走行距離，旅行時間を出力するか
  //private static final boolean FLAG_OUTPUT_TRIP_INFO             = true;
  // 速度履歴を保存するか
  //private static final boolean VEHICLE_VELOCITY_HISTORY_RECORD   = true;
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // グローバル変数設定ファイル
  public static final String  FILE_NAME_GV_INIT                         = "init.txt";
  // 地図に関連するファイル
  private static final String DEFAULT_FILE_MAP_POSITION                 = "mapPosition.txt";
  private static final String DEFAULT_FILE_NETWORK                      = "network.txt";
  private static final String DEFAULT_FILE_SPEED_LIMIT                  = "speedLimit.txt";
  // 車両発生に関するファイル
  private static final String DEFAULT_FILE_GENERATE_TABLE               = "generateTable.txt";
  private static final String DEFAULT_FILE_DEFAULT_GENERATE_TABLE       = "defaultGenerateTable.txt";
  // 車種に関するファイル
  private static final String DEFAULT_FILE_VEHICLE_FAMILY               = "vehicleFamily.txt";
  // 自動車エージェントの経路に関するファイル
  private static final String DEFAULT_FILE_VEHICLE_ROUTE_PARAM          = "vehicleRoutingParam.txt";
  // 路側器に関するファイル
  private static final String DEFAULT_FILE_GENCOUNTER                   = "genCounter.txt";
  private static final String DEFAULT_FILE_DETECTOR                     = "detector.txt";
  // 信号に関するディレクトリorファイルor拡張子
  private static final String DEFAULT_DIRECTORY_SIGNAL_CONTROL          = "signals";
  private static final String DEFAULT_FILE_DEFAULT_SIGNAL_CONTROL       = "default";
  private static final String DEFAULT_SIGNAL_ASPECT_FILE_PREFIX         = "defaultInter";
  private static final String DEFAULT_CONTROL_FILE_EXTENSION            = ".msf";
  private static final String DEFAULT_ASPECT_FILE_EXTENSION             = ".msa";
  // 交差点属性指定ファイル
  private static final String DEFAULT_DIRECTORY_INTERSECTION_ATTRIBUTE  = "intersection";
  // 道路形状に関するファイル
  private static final String DEFAULT_FILE_INTERSECTION_STRUCT          = "intersectionStruct.txt";
  private static final String DEFAULT_FILE_SECTION_STRUCT               = "sectionStruct.txt";
  // 出力先
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
  // 定数の定義
  // 後でGVManager::setVariablesFromFile()によって上書きされる
  // シミュレーションの実行に関するもの
  // 出力ファイルに各種コメントを付して出力する
  //private static final float OUTPUT_COMMENT_IN_FILE              = 0;
  // 計時の有無、0 以外ならあり
  //private static final float CHECK_TIME                          = 0;
  // エージェントに関するもの
  // 自動車の反応遅れ時間[sec]
  //private static final float REACTION_TIME_VEHICLE               = 0.74f;
  // 自動車が交差点で右折する場合などの
  // 対向車とのギャップアクセプタンス[sec]
  //private static final float GAP_ACCEPTANCE_VEHICLE_CROSS        = 3.0f;
  // 普通車(VehicleFamily::passenger())の最大加速度，減速度[m/(sec^2)]
  //private static final float MAX_ACCELERATION_PASSENGER          = 3.0f;
  //private static final float MAX_DECELERATION_PASSENGER          = -5.0f;
  // バス(VehicleFamily::bus())の最大加速度，減速度[m/(sec^2)]
  //private static final float MAX_ACCELERATION_BUS                = 3.0f;
  //private static final float MAX_DECELERATION_BUS                = -5.0f;
  // 大型車(VehicleFamily::truck())の最大加速度，減速度[m/(sec^2)]
  //private static final float MAX_ACCELERATION_TRUCK              = 3.0f;
  //private static final float MAX_DECELERATION_TRUCK              = -5.0f;
  // 車線変更時に与える横向きの速度[km/h]
  //private static final float ERROR_VELOCITY                      = 7.5f;
  // 発生端点からL[m]以内の車両は出力しない
  //private static final float NO_OUTPUT_LENGTH_FROM_ORIGIN_NODE   = 0f;
  // 車両ログの追加情報を出力する
  //private static final float OUTPUT_VEHICLE_EXTENSION            = 0f;
  
  //VehicleFamilyに移動
  // 普通車(VehicleFamily::passenger())のサイズ[m]
  //private static final float DEFAULT_VEHICLE_LENGTH_PASSENGER            = 4.400f;
  //private static final float DEFAULT_VEHICLE_WIDTH_PASSENGER             = 1.830f;
  //private static final float DEFAULT_VEHICLE_HEIGHT_PASSENGER            = 1.315f;
  // バス(VehicleFamily::bus())のサイズ[m]
  //private static final float DEFAULT_VEHICLE_LENGTH_BUS                  = 8.465f;
  //private static final float DEFAULT_VEHICLE_WIDTH_BUS                   = 2.230f;
  //private static final float DEFAULT_VEHICLE_HEIGHT_BUS                  = 3.420f;
  // 大型車(VehicleFamily::truck())のサイズ[m]
  //private static final float DEFAULT_VEHICLE_LENGTH_TRUCK                = 8.465f;
  //private static final float DEFAULT_VEHICLE_WIDTH_TRUCK                 = 2.230f;
  //private static final float DEFAULT_VEHICLE_HEIGHT_TRUCK                = 3.420f;
    
  // 交錯を厳密に評価
  //private static final float STRICT_COLLISION_CHECK              = 1f;
  // 速度履歴を保存するステップ数
  //private static final float VEHICLE_VELOCITY_HISTORY_SIZE       = 180f;
  // 速度履歴を保存するステップ間隔
  //private static final float VEHICLE_VELOCITY_HISTORY_INTERVAL   = 10f;
  // 道路に関するもの
  // 右折専用レーンの標準長さ[m]
  private static final float DEFAULT_RIGHT_TURN_LANE_LENGTH              = 30f;
  // 標準制限速度[km/h]
  // ただしSPEED_LIMIT_INTERSECTIONが用いられることはほとんど無く，
  // 右左折時は下のVELOCITY_ATが使われ，
  // 直進時は次の単路でのSPEED_LIMITが参照される．
  //private static final float SPEED_LIMIT_SECTION                 = 60f;
  //private static final float SPEED_LIMIT_INTERSECTION            = 60f;
  // 徐行速度[km/h]
  //private static final float VELOCITY_CRAWL                      = 10f;
  // 右左折時の速度[km/h]
  //private static final float VELOCITY_AT_TURNING_RIGHT           = 40f;
  //private static final float VELOCITY_AT_TURNING_LEFT            = 40f;
  // 車両発生時の制限速度[km/h]、負ならなし
  //private static final float GENERATE_VELOCITY_LIMIT             = -1f;
  // 右左折時の最小ヘッドウェイ[秒]
  //private static final float MIN_HEADWAY_AT_TURNING              = 1.7f;
  // 車両発生量既定値[台/h]、入口レーン 3 以上 / 2 / 1、基本交通容量の10%
  //private static final float DEFAULT_TRAFFIC_VOLUME_WIDE         = 660f;
  //private static final float DEFAULT_TRAFFIC_VOLUME_NORMAL       = 440f;
  //private static final float DEFAULT_TRAFFIC_VOLUME_NARROW       = 125f;
  
  // 標準のレーン幅、歩道幅、横断歩道幅、路側幅
  private static final String DEFAULT_LANE_WIDTH_KEY              = "DEFAULT_LANE_WIDTH";
  private static final float  DEFAULT_LANE_WIDTH                  = 3.5f;
  private static final String DEFAULT_SIDEWALK_WIDTH_KEY          = "DEFAULT_SIDEWALK_WIDTH";
  private static final float  DEFAULT_SIDEWALK_WIDTH              = 5.0f;
  private static final String DEFAULT_CROSSWALK_WIDTH_KEY         = "DEFAULT_CROSSWALK_WIDTH";
  private static final float  DEFAULT_CROSSWALK_WIDTH             = 5.0f;
  private static final String DEFAULT_ROADSIDE_WIDTH_KEY          = "DEFAULT_ROADSIDE_WIDTH";
  private static final float  DEFAULT_ROADSIDE_WIDTH              = 1.0f;
  // 単路の歩道を自動設定する際のレーン数、-1 なら自動設定なし
  // 自動設定ありの場合、単路の全レーン数がこれ以上なら歩道を設定する
  // これ以下なら車道通行可能にする
  //private static final float AUTO_SIDEWALK_SECTION_LANE          = -1f;
  // 道路エンティティの厳密な内部判定、1 ならあり
  // 凹型の道路エンティティでも判定可能になる、ただし処理速度は遅い
  //private static final float ROAD_ENTITY_STRICT_JUDGE_INSIDE     = 1f;
  // 交差点サイズ制限、中心からの距離[m]
  //private static final float INTERSECTION_SIZE_LIMIT             = 20f;
  
  
  //######################################################################
  //GVInitializer.cppには無い文字列定数
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
   * ログハンドリングクラス
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
            
            //valueに対しint化,float化,boolean化を試みる
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
                  //透明度が記述されている場合は、その値も調べる
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
