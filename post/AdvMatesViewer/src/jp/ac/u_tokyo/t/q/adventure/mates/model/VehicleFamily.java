package jp.ac.u_tokyo.t.q.adventure.mates.model;

import java.util.ArrayList;

import jp.ac.u_tokyo.t.q.adventure.mates.ui.render.Drawable3DPanel;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

public class VehicleFamily {
  //@formatter:off
  // 普通車(VehicleFamily::passenger())のサイズ[m]など
  private static final int    DEFAULT_VEHICLE_FAMILY_ID_PASSENGER         = 20;
  private static final float  DEFAULT_VEHICLE_LENGTH_PASSENGER            = 4.400f;
  private static final float  DEFAULT_VEHICLE_WIDTH_PASSENGER             = 1.830f;
  private static final float  DEFAULT_VEHICLE_HEIGHT_PASSENGER            = 1.315f;
  // バス(VehicleFamily::bus())のサイズ[m]など
  private static final int    DEFAULT_VEHICLE_FAMILY_ID_BUS               = 30;
  private static final float  DEFAULT_VEHICLE_LENGTH_BUS                  = 8.465f;
  private static final float  DEFAULT_VEHICLE_WIDTH_BUS                   = 2.230f;
  private static final float  DEFAULT_VEHICLE_HEIGHT_BUS                  = 3.420f;
  // 大型車(VehicleFamily::truck())のサイズ[m]など
  private static final int    DEFAULT_VEHICLE_FAMILY_ID_TRUCK             = 50;
  private static final float  DEFAULT_VEHICLE_LENGTH_TRUCK                = 8.465f;
  private static final float  DEFAULT_VEHICLE_WIDTH_TRUCK                 = 2.230f;
  private static final float  DEFAULT_VEHICLE_HEIGHT_TRUCK                = 3.420f;
  
  private static final int    DEFAULT_FAMILY_ID                           = -1;
  private static final float  DEFAULT_VEHICLE_LENGTH                      = DEFAULT_VEHICLE_LENGTH_PASSENGER;
  private static final float  DEFAULT_VEHICLE_WIDTH                       = DEFAULT_VEHICLE_WIDTH_PASSENGER;
  private static final float  DEFAULT_VEHICLE_HEIGHT                      = DEFAULT_VEHICLE_HEIGHT_PASSENGER;
  
  //                                                                        {  red,green, blue,alpha }
  public static final float[] DEFAULT_COLOR                               = { 1.0f, 0.0f, 0.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_PASSENGER                     = { 1.0f, 0.0f, 0.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_BUS                           = { 0.0f, 1.0f, 0.0f, 1.0f };
  public static final float[] DEFAULT_COLOR_TRUCK                         = { 0.3f, 0.7f, 1.0f, 1.0f };
  //@formatter:on
  
  private static ArrayList<VehicleFamily> _stackVechicleFamilies = null;
  
  private static VehicleFamily _defaultVehicleFamily = null;
  
  private int _familyID;
  private float _length;
  private float _width;
  private float _height;
  //private float _dummy;
  //private float _maxAcceleration;
  //private float _maxDeceleration;
  private float[] _color = new float[4];
  
  private float[][] _shapeCoords = null;
  private int[][] _shapeConnectivity = null;
  private float[][] _shapeNormals = null;
  private float[][] _shapeColors = null;
  
  private VehicleFamily() {
    init(DEFAULT_FAMILY_ID, DEFAULT_VEHICLE_LENGTH, DEFAULT_VEHICLE_WIDTH_PASSENGER, DEFAULT_VEHICLE_HEIGHT_PASSENGER,
        DEFAULT_COLOR[0], DEFAULT_COLOR[1], DEFAULT_COLOR[2], DEFAULT_COLOR[3]);
  }
  
  public VehicleFamily(int familyID, float length, float width, float height, float dummy, float maxAcceleration,
      float maxDeceleration, float colorR, float colorG, float colorB) {
    init(familyID, length, width, height, colorR, colorG, colorB, Drawable3DPanel.ALPHA_FOR_OPAQUE);
  }
  
  private void init(int familyID, float length, float width, float height, float colorR, float colorG, float colorB,
      float colorA) {
    _familyID = familyID;
    _length = length;
    _width = width;
    _height = height;
    _color[0] = colorR;
    _color[1] = colorG;
    _color[2] = colorB;
    _color[3] = colorA;
  }
  
  public static void loadDefault() {
    if (_stackVechicleFamilies == null) {
      _stackVechicleFamilies = new ArrayList<VehicleFamily>();
    }
    if (_defaultVehicleFamily == null) {
      _defaultVehicleFamily = new VehicleFamily();
    }
    
    _stackVechicleFamilies.clear();
    
    int familyID = DEFAULT_VEHICLE_FAMILY_ID_PASSENGER;
    float length = DEFAULT_VEHICLE_LENGTH_PASSENGER;
    float width = DEFAULT_VEHICLE_WIDTH_PASSENGER;
    float height = DEFAULT_VEHICLE_HEIGHT_PASSENGER;
    float dummy = 0;
    float maxAcceleration = 0;
    float maxDeceleration = 0;
    float[] color = DEFAULT_COLOR_PASSENGER;
    addVehicleFamily(familyID, length, width, height, dummy, maxAcceleration, maxDeceleration, color);
    
    familyID = DEFAULT_VEHICLE_FAMILY_ID_TRUCK;
    length = DEFAULT_VEHICLE_LENGTH_TRUCK;
    width = DEFAULT_VEHICLE_WIDTH_TRUCK;
    height = DEFAULT_VEHICLE_HEIGHT_TRUCK;
    dummy = 0;
    maxAcceleration = 0;
    maxDeceleration = 0;
    color = DEFAULT_COLOR_TRUCK;
    addVehicleFamily(familyID, length, width, height, dummy, maxAcceleration, maxDeceleration, color);
  }
  
  public static void addVehicleFamily(int familyID, float length, float width, float height, float dummy,
      float maxAcceleration, float maxDeceleration, float[] color) {
    addVehicleFamily(familyID, length, width, height, dummy, maxAcceleration, maxDeceleration, color[0], color[1],
        color[2]);
  }
  
  public static void addVehicleFamily(int familyID, float length, float width, float height, float dummy,
      float maxAcceleration, float maxDeceleration, float colorR, float colorG, float colorB) {
    boolean flgHit = false;
    for (int i = 0; i < _stackVechicleFamilies.size(); i++) {
      VehicleFamily vehicleFamily = _stackVechicleFamilies.get(i);
      if (vehicleFamily._familyID == familyID) {
        vehicleFamily.init(familyID, length, width, height, colorR, colorG, colorB, Drawable3DPanel.ALPHA_FOR_OPAQUE);
        flgHit = true;
        break;
      }
    }
    
    if (flgHit == false) {
      VehicleFamily vehicleFamily = new VehicleFamily(familyID, length, width, height, dummy, maxAcceleration,
          maxDeceleration, colorR, colorG, colorB);
      _stackVechicleFamilies.add(vehicleFamily);
    }
  }
  
  public void setData(float[][] coords, int[][] connectivity, float[][] colors) {
    _shapeCoords = coords;
    _shapeConnectivity = connectivity;
    _shapeColors = colors;
    
    fitToVehicleSize();
    
    _shapeNormals = new float[_shapeConnectivity.length][];
    for (int i = 0; i < _shapeConnectivity.length; i++) {
      _shapeNormals[i] = new float[_shapeConnectivity[i].length];
      Common.computeNormalVector(_shapeCoords[i], _shapeConnectivity[i], _shapeNormals[i]);
    }
  }
  
  private void fitToVehicleSize() {
    float minx = 0;
    float miny = 0;
    float minz = 0;
    float maxx = 0;
    float maxy = 0;
    float maxz = 0;
    boolean isFirst = true;
    
    for (int i = 0; i < _shapeCoords.length; i++) {
      for (int jx3 = 0; jx3 < _shapeCoords[i].length; jx3 += 3) {
        float x = _shapeCoords[i][jx3 + 0];
        float y = _shapeCoords[i][jx3 + 1];
        float z = _shapeCoords[i][jx3 + 2];
        if (isFirst) {
          minx = x;
          miny = y;
          minz = z;
          maxx = x;
          maxy = y;
          maxz = z;
          isFirst = false;
        } else {
          if (minx > x) minx = x;
          if (miny > y) miny = y;
          if (minz > z) minz = z;
          if (maxx < x) maxx = x;
          if (maxy < y) maxy = y;
          if (maxz < z) maxz = z;
        }
      }
    }
    
    float rangex = maxx - minx;
    float rangey = maxy - miny;
    float rangez = maxz - minz;
    float centerx = (maxx + minx) / 2;
    float centery = (maxy + miny) / 2;
    float centerz = (maxz + minz) / 2;
    float ratiox = 1;
    float ratioy = 1;
    float ratioz = 1;
    if (rangex > 0) ratiox = _length / rangex;
    if (rangey > 0) ratioy = _width / rangey;
    if (rangez > 0) ratioz = _height / rangez;
    for (int i = 0; i < _shapeCoords.length; i++) {
      for (int jx3 = 0; jx3 < _shapeCoords[i].length; jx3 += 3) {
        _shapeCoords[i][jx3 + 0] -= centerx;
        _shapeCoords[i][jx3 + 1] -= centery;
        _shapeCoords[i][jx3 + 2] -= minz;
        
        _shapeCoords[i][jx3 + 0] *= ratiox;
        _shapeCoords[i][jx3 + 1] *= ratioy;
        _shapeCoords[i][jx3 + 2] *= ratioz;
      }
    }
  }
  
  public static VehicleFamily getVehicleFamily(int familyID) {
    VehicleFamily vehicleFamily = null;
    for (int i = 0; i < _stackVechicleFamilies.size(); i++) {
      VehicleFamily tmpVehicleFamily = _stackVechicleFamilies.get(i);
      if (tmpVehicleFamily._familyID == familyID) {
        vehicleFamily = tmpVehicleFamily;
        break;
      }
    }
    if (vehicleFamily == null) {
      vehicleFamily = _defaultVehicleFamily;
    }
    return vehicleFamily;
  }
  
  public static VehicleFamily getVehicleFamilyByIndex(int index) {
    return _stackVechicleFamilies.get(index);
  }
  
  public static int getNumVehicleFamilies() {
    return _stackVechicleFamilies.size();
  }
  
  public static VehicleFamily getDefaultVehicleFamily() {
    return _defaultVehicleFamily;
  }
  
  public int getVehicleFamilyID() {
    return _familyID;
  }
  
  public float getLength() {
    return _length;
  }
  
  public float getWidth() {
    return _width;
  }
  
  public float getHeight() {
    return _height;
  }
  
  public float[] getColorForBody() {
    return _color;
  }
  
  public float[][] getShapeCoords() {
    return _shapeCoords;
  }
  
  public int[][] getShapeConnectivity() {
    return _shapeConnectivity;
  }
  
  public float[][] getShapeNormals() {
    return _shapeNormals;
  }
  
  public float[][] getShapeColors() {
    return _shapeColors;
  }
}
