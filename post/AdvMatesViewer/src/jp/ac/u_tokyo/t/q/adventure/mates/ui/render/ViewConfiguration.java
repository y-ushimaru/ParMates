package jp.ac.u_tokyo.t.q.adventure.mates.ui.render;

/**
 * 視点に関する設定を格納する。
 * 視点の記憶・復元や、ムービー作成等での視点操作に用いる。
 */
public class ViewConfiguration {
  //@formatter:off
  public static final int     RENDER_TYPE_UNKNOWN                           = -1;
  public static final int     RENDER_TYPE_ROAD                              = 1;
  public static final int     RENDER_TYPE_ROAD_LINE                         = 2;
  public static final int     RENDER_TYPE_ROAD_SIDE                         = 3;
  public static final int     RENDER_TYPE_SIDE_WALK                         = 10;
  public static final int     RENDER_TYPE_INTERSECTION_ID                   = 20;
  public static final int     RENDER_TYPE_VEHICLE                           = 30;
  public static final int     RENDER_TYPE_VEHICLE_ID                        = 31;
  public static final int     RENDER_TYPE_SIGNAL                            = 40;
  public static final int     RENDER_TYPE_GROUND                            = 50;
  
  public static final int     RENDER_TYPE_AXIS                              = 100;
  
  public static final int     RENDER_TYPE_PARTICLE                          = 1001;
  public static final int     RENDER_TYPE_EDGE                              = 1002;
  public static final int     RENDER_TYPE_FACE                              = 1003;
  
  public static final boolean DEFAULT_STATE_ROAD_VISIBLE                    = true;
  public static final boolean DEFAULT_STATE_ROAD_LINE_VISIBLE               = true;
  public static final boolean DEFAULT_STATE_ROAD_SIDE_VISIBLE               = true;
  public static final boolean DEFAULT_STATE_SIDE_WALK_VISIBLE               = false;
  public static final boolean DEFAULT_STATE_INTERSECTION_ID_VISIBLE         = false;
  public static final boolean DEFAULT_STATE_VEHICLE_VISIBLE                 = true;
  public static final boolean DEFAULT_STATE_VEHICLE_ID_VISIBLE              = false;
  public static final boolean DEFAULT_STATE_SIGNAL_VISIBLE                  = true;
  public static final boolean DEFAULT_STATE_GROUND_VISIBLE                  = true;
  public static final boolean DEFAULT_STATE_AXIS_VISIBLE                    = true;
  
  /**
   * 表示の設定を表すための文字列
   */
  public static final String  LABEL_CONFIGURATION_ROAD_VISIBLE              = "RoadVisible";
  public static final String  LABEL_CONFIGURATION_ROAD_LINE_VISIBLE         = "RoadLineVisible";
  public static final String  LABEL_CONFIGURATION_ROAD_SIDE_VISIBLE         = "RoadSideVisible";
  public static final String  LABEL_CONFIGURATION_SIDE_WALK_VISIBLE         = "SideWalkVisible";
  public static final String  LABEL_CONFIGURATION_INTERSECTION_ID_VISIBLE   = "IntersectionIDVisible";
  public static final String  LABEL_CONFIGURATION_VEHICLE_VISIBLE           = "VehicleVisible";
  public static final String  LABEL_CONFIGURATION_VEHICLE_ID_VISIBLE        = "VehicleIDVisible";
  public static final String  LABEL_CONFIGURATION_SIGNAL_VISIBLE            = "SignalVisible";
  public static final String  LABEL_CONFIGURATION_GROUND_VISIBLE            = "GroundVisible";
  public static final String  LABEL_CONFIGURATION_AXIS_VISIBLE              = "AxisVisible";
  //@formatter:on
  
  private boolean _isRoadVisible;
  private boolean _isRoadLineVisible;
  private boolean _isRoadSideVisible;
  private boolean _isSideWalkVisible;
  private boolean _isIntersectionIDVisible;
  private boolean _isVehicleVisible;
  private boolean _isVehicleIDVisible;
  private boolean _isSignalVisible;
  private boolean _isGroundVisible;
  private boolean _isAxisVisible;
  
  public ViewConfiguration() {
    init();
  }
  
  public ViewConfiguration getCopy() {
    //@formatter:off
    ViewConfiguration viewConfiguration = new ViewConfiguration();
    viewConfiguration._isRoadVisible             = this._isRoadVisible;
    viewConfiguration._isRoadLineVisible         = this._isRoadLineVisible;
    viewConfiguration._isRoadSideVisible         = this._isRoadSideVisible;
    viewConfiguration._isSideWalkVisible         = this._isSideWalkVisible;
    viewConfiguration._isIntersectionIDVisible   = this._isIntersectionIDVisible;
    viewConfiguration._isVehicleVisible          = this._isVehicleVisible;
    viewConfiguration._isVehicleIDVisible        = this._isVehicleIDVisible;
    viewConfiguration._isSignalVisible           = this._isSignalVisible;
    viewConfiguration._isGroundVisible           = this._isGroundVisible;
    viewConfiguration._isAxisVisible             = this._isAxisVisible;
    //@formatter:on
    return viewConfiguration;
  }
  
  public void init() {
    //@formatter:off
    _isRoadVisible            = DEFAULT_STATE_ROAD_VISIBLE;
    _isRoadLineVisible        = DEFAULT_STATE_ROAD_LINE_VISIBLE;
    _isRoadSideVisible        = DEFAULT_STATE_ROAD_SIDE_VISIBLE;
    _isSideWalkVisible        = DEFAULT_STATE_SIDE_WALK_VISIBLE;
    _isIntersectionIDVisible  = DEFAULT_STATE_INTERSECTION_ID_VISIBLE;
    _isVehicleVisible         = DEFAULT_STATE_VEHICLE_VISIBLE;
    _isVehicleIDVisible       = DEFAULT_STATE_VEHICLE_ID_VISIBLE;
    _isSignalVisible          = DEFAULT_STATE_SIGNAL_VISIBLE;
    _isGroundVisible          = DEFAULT_STATE_GROUND_VISIBLE;
    _isAxisVisible            = DEFAULT_STATE_AXIS_VISIBLE;
    //@formatter:on
  }
  
  public boolean isVisible(int type) {
    boolean isVisible = false;
    if (type == RENDER_TYPE_ROAD) {
      isVisible = _isRoadVisible;
    } else if (type == RENDER_TYPE_ROAD_LINE) {
      isVisible = _isRoadLineVisible;
    } else if (type == RENDER_TYPE_ROAD_SIDE) {
      isVisible = _isRoadSideVisible;
    } else if (type == RENDER_TYPE_SIDE_WALK) {
      isVisible = _isSideWalkVisible;
    } else if (type == RENDER_TYPE_INTERSECTION_ID) {
      isVisible = _isIntersectionIDVisible;
    } else if (type == RENDER_TYPE_VEHICLE) {
      isVisible = _isVehicleVisible;
    } else if (type == RENDER_TYPE_VEHICLE_ID) {
      isVisible = _isVehicleIDVisible;
    } else if (type == RENDER_TYPE_SIGNAL) {
      isVisible = _isSignalVisible;
    } else if (type == RENDER_TYPE_GROUND) {
      isVisible = _isGroundVisible;
    } else if (type == RENDER_TYPE_AXIS) {
      isVisible = _isAxisVisible;
    }
    return isVisible;
  }
  
  public void setVisible(int type, boolean isVisible) {
    if (type == RENDER_TYPE_ROAD) {
      _isRoadVisible = isVisible;
    } else if (type == RENDER_TYPE_ROAD_LINE) {
      _isRoadLineVisible = isVisible;
    } else if (type == RENDER_TYPE_ROAD_SIDE) {
      _isRoadSideVisible = isVisible;
    } else if (type == RENDER_TYPE_SIDE_WALK) {
      _isSideWalkVisible = isVisible;
    } else if (type == RENDER_TYPE_INTERSECTION_ID) {
      _isIntersectionIDVisible = isVisible;
    } else if (type == RENDER_TYPE_VEHICLE) {
      _isVehicleVisible = isVisible;
    } else if (type == RENDER_TYPE_VEHICLE_ID) {
      _isVehicleIDVisible = isVisible;
    } else if (type == RENDER_TYPE_SIGNAL) {
      _isSignalVisible = isVisible;
    } else if (type == RENDER_TYPE_GROUND) {
      _isGroundVisible = isVisible;
    } else if (type == RENDER_TYPE_AXIS) {
      _isAxisVisible = isVisible;
    }
  }
}
