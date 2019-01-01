package jp.ac.u_tokyo.t.q.adventure.mates.io;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.model.VehicleFamily;
import jp.ac.u_tokyo.t.q.adventure.mates.resource.ResourceManager;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * 交差点の接続関係データを読み込むクラスである。
 */
public class VehicleFamilyFileReader {
  //@formatter:off
  private static final String START_OF_COMMENT = "#";
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
   * 読み込むファイル
   */
  private File _file;
  
  private boolean _isSuccess;
  
  private float _progress;
  
  private boolean _isCanceled;
  
  public VehicleFamilyFileReader(File file) {
    _logger = Logger.getLogger(VehicleFamilyFileReader.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    _rm = ResourceManager.getInstance();
    _file = file;
    
    _isCanceled = false;
    _progress = 0;
  }
  
  public void read() {
    boolean isSuccess = true;
    
    _logger.info("Start reading vehicle family file : " + _file.getPath());
    
    BufferedReader br = null;
    String line = null;
    String[] lineSplit = null;
    
    //##############################################
    //1行単位で、車種の設定を解析する
    if (isSuccess) {
      try {
        br = new BufferedReader(new FileReader(_file));
      } catch (Exception e) {
        isSuccess = false;
        _logger.info("Failed to open file : " + _file.getPath());
      }
    }
    int lineNumber = 0;
    if (isSuccess) {
      try {
        while (true) {
          line = br.readLine();
          if (line == null) break;
          lineNumber++;
          
          if (line.trim().startsWith(START_OF_COMMENT)) {
            continue;
          }
          
          lineSplit = line.split(Common.COMMA);
          
          int vehicleFamilyID = Integer.parseInt(lineSplit[0].trim());
          float vehicleFamilyLength = Float.parseFloat(lineSplit[1].trim());
          float vehicleFamilyWidth = Float.parseFloat(lineSplit[2].trim());
          float vehicleFamilyHeight = Float.parseFloat(lineSplit[3].trim());
          float vehicleFamilyDummy = Float.parseFloat(lineSplit[4].trim());
          float vehicleFamilyMaxAcceleration = Float.parseFloat(lineSplit[5].trim());
          float vehicleFamilyMaxDeceleration = Float.parseFloat(lineSplit[6].trim());
          float vehicleFamilyColorR = Float.parseFloat(lineSplit[7].trim());
          float vehicleFamilyColorG = Float.parseFloat(lineSplit[8].trim());
          float vehicleFamilyColorB = Float.parseFloat(lineSplit[9].trim());
          
          VehicleFamily.addVehicleFamily(vehicleFamilyID, vehicleFamilyLength, vehicleFamilyWidth, vehicleFamilyHeight,
              vehicleFamilyDummy, vehicleFamilyMaxAcceleration, vehicleFamilyMaxDeceleration, vehicleFamilyColorR,
              vehicleFamilyColorG, vehicleFamilyColorB);
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
    
    _logger.info("Finish reading vehicle family file : " + _file.getPath());
    _isSuccess = isSuccess;
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
}
