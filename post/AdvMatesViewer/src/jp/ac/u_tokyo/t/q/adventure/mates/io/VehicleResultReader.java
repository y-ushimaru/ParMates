package jp.ac.u_tokyo.t.q.adventure.mates.io;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.logging.Logger;
import java.util.zip.GZIPInputStream;

import jp.ac.u_tokyo.t.q.adventure.mates.resource.ResourceManager;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * 車両の位置等のデータファイルを読み込むクラスである。
 */
public class VehicleResultReader {
  //@formatter:off
  private static final String LABEL_NULL   = "NULL";
  private static final int    INVALID_ID   = -1;
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
   * ログ補助のためのStringBuffer
   */
  private StringBuffer _sbInfoMessages;
  /**
   * エラーメッセージを蓄積するStringBuffer
   */
  private StringBuffer _sbErrorMessages;
  /**
  * 読み込むファイル
  */
  private File _file;
  
  private int _nVehicles;
  private int[] _vehicleID;
  private int[] _vehicleFamilyID;
  private float[] _vehiclePositionX;
  private float[] _vehiclePositionY;
  private float[] _vehiclePositionZ;
  private float[] _vehicleAngleXY;
  private float[] _vehicleAngleZ;
  private float[] _vehicleVelocity;
  private float[] _vehicleAcceleration;
  private int[] _vehicleIntersectionID;
  private int[] _vehicleRoadID;
  
  private boolean _isSuccess;
  
  private float _progress;
  
  private boolean _isCanceled;
  
  public VehicleResultReader(File file) {
    _logger = Logger.getLogger(VehicleResultReader.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    _rm = ResourceManager.getInstance();
    _sbInfoMessages = new StringBuffer();
    _sbErrorMessages = new StringBuffer();
    _file = file;
    
    _isSuccess = false;
    _isCanceled = false;
    _progress = 0;
  }
  
  public void read() {
    boolean isSuccess = true;
    
    //_logger.info("Start readRegular");
    
    BufferedReader br = null;
    FileInputStream fis = null;
    String line = null;
    String[] lineSplit = null;
    
    //##############################################
    //行数をカウントする
    if (isSuccess) {
      try {
        fis = new FileInputStream(_file);
        br = new BufferedReader(new InputStreamReader(new GZIPInputStream(fis)));
      } catch (Exception e) {
        isSuccess = false;
        _logger.info("Failed to open file : " + _file.getPath());
      }
    }
    int lineNumber = 0;
    int countLines = 0;
    if (isSuccess) {
      try {
        while (true) {
          line = br.readLine();
          if (line == null) break;
          lineNumber++;
          if (line.trim().length() > 0) countLines++;
        }
      } catch (Exception e) {
        isSuccess = false;
        String message = "Error occured at " + lineNumber + "th line : " + _file.getPath();
        message += Common.EOL;
        message += "line=\"" + line + "\"";
        _logger.info(message);
      }
    }
    if (br != null) {
      try {
        br.close();
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
    if (fis != null) {
      try {
        fis.close();
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
    
    //##############################################
    //配列を確保する
    if (isSuccess) {
      _nVehicles = countLines;
      _vehicleID = new int[_nVehicles];
      _vehicleFamilyID = new int[_nVehicles];
      _vehiclePositionX = new float[_nVehicles];
      _vehiclePositionY = new float[_nVehicles];
      _vehiclePositionZ = new float[_nVehicles];
      _vehicleAngleXY = new float[_nVehicles];
      _vehicleAngleZ = new float[_nVehicles];
      _vehicleVelocity = new float[_nVehicles];
      _vehicleAcceleration = new float[_nVehicles];
      _vehicleIntersectionID = new int[_nVehicles];
      _vehicleRoadID = new int[_nVehicles];
    }
    
    //##############################################
    //行ごとに解析を行う
    if (isSuccess) {
      try {
        fis = new FileInputStream(_file);
        br = new BufferedReader(new InputStreamReader(new GZIPInputStream(fis)));
      } catch (Exception e) {
        isSuccess = false;
        _logger.info("Failed to open file : " + _file.getPath());
      }
    }
    lineNumber = 0;
    countLines = 0;
    if (isSuccess) {
      try {
        while (true) {
          line = br.readLine();
          if (line == null) break;
          lineNumber++;
          if (line.trim().length() > 0) {
            lineSplit = line.split(Common.COMMA);
            
            String strID = lineSplit[0];
            String strFamilyID = lineSplit[1];
            String strPositionX = lineSplit[2];
            String strPositionY = lineSplit[3];
            String strPositionZ = lineSplit[4];
            String strAngleXY = lineSplit[5];
            String strAngleZ = lineSplit[6];
            String strVelocity = lineSplit[7];
            String strAcceleration = lineSplit[8];
            String strIntersectionID = lineSplit[9];
            String strRoadID = lineSplit[10];
            
            _vehicleID[countLines] = Integer.parseInt(strID);
            _vehicleFamilyID[countLines] = Integer.parseInt(strFamilyID);
            _vehiclePositionX[countLines] = Float.parseFloat(strPositionX);
            _vehiclePositionY[countLines] = Float.parseFloat(strPositionY);
            _vehiclePositionZ[countLines] = Float.parseFloat(strPositionZ);
            _vehicleAngleXY[countLines] = Float.parseFloat(strAngleXY);
            _vehicleAngleZ[countLines] = Float.parseFloat(strAngleZ);
            _vehicleVelocity[countLines] = Float.parseFloat(strVelocity);
            _vehicleAcceleration[countLines] = Float.parseFloat(strAcceleration);
            if (strIntersectionID.equals(LABEL_NULL)) {
              _vehicleIntersectionID[countLines] = INVALID_ID;
            } else {
              _vehicleIntersectionID[countLines] = Integer.parseInt(strIntersectionID);
            }
            _vehicleRoadID[countLines] = 0;
            //if (strRoadID.equals(LABEL_NULL)) {
            //  _vehicleRoadID[countLines] = INVALID_ID;
            //} else {
            //  _vehicleRoadID[countLines] = Integer.parseInt(strRoadID);
            //}
            countLines++;
          }
        }
      } catch (Exception e) {
        isSuccess = false;
        String message = "Error occured at " + lineNumber + "th line : " + _file.getPath();
        message += Common.EOL;
        message += "line=\"" + line + "\"";
        _logger.info(message);
      }
    }
    if (br != null) {
      try {
        br.close();
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
    if (fis != null) {
      try {
        fis.close();
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
    
    if (!isSuccess) {
      _nVehicles = 0;
      _vehicleID = null;
      _vehicleFamilyID = null;
      _vehiclePositionX = null;
      _vehiclePositionY = null;
      _vehiclePositionZ = null;
      _vehicleAngleXY = null;
      _vehicleAngleZ = null;
      _vehicleVelocity = null;
      _vehicleAcceleration = null;
      _vehicleIntersectionID = null;
      _vehicleRoadID = null;
    }
    
    _isSuccess = isSuccess;
  }
  
  public void clearErrorMessages() {
    _sbErrorMessages.setLength(0);
  }
  
  public String getErrorMessages() {
    if (_sbErrorMessages.length() == 0) {
      return null;
    }
    return _sbErrorMessages.toString();
  }
  
  public boolean isSuccess() {
    return _isSuccess;
  }
  
  public void cancel() {
    _isCanceled = true;
  }
  
  public float getProgress() {
    return _progress;
  }
  
  public int getNumVehicles() {
    return _nVehicles;
  }
  
  public int[] getVehicleID() {
    return _vehicleID;
  }
  
  public int[] getVehicleFamilyID() {
    return _vehicleFamilyID;
  }
  
  public float[] getVehiclePositionX() {
    return _vehiclePositionX;
  }
  
  public float[] getVehiclePositionY() {
    return _vehiclePositionY;
  }
  
  public float[] getVehiclePositionZ() {
    return _vehiclePositionZ;
  }
  
  public float[] getVehicleAngleXY() {
    return _vehicleAngleXY;
  }
  
  public float[] getVehicleAngleZ() {
    return _vehicleAngleZ;
  }
  
  public float[] getVehicleVelocity() {
    return _vehicleVelocity;
  }
  
  public float[] getVehicleAcceleration() {
    return _vehicleAcceleration;
  }
  
  public int[] getVehicleIntersectionID() {
    return _vehicleIntersectionID;
  }
  
  public int[] getVehicleRoadID() {
    return _vehicleRoadID;
  }
}
