package jp.ac.u_tokyo.t.q.adventure.mates.io;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.resource.ResourceManager;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * 交差点の位置データを読み込むクラスである。
 */
public class MapPositionFileReader {
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
  
  private int _nIntersections;
  private int[] _intersectionIDs;
  private float[] _intersectionPositionX;
  private float[] _intersectionPositionY;
  private float[] _intersectionPositionZ;
  
  private boolean _isSuccess;
  
  private float _progress;
  
  private boolean _isCanceled;
  
  public MapPositionFileReader(File file) {
    _logger = Logger.getLogger(MapPositionFileReader.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    _rm = ResourceManager.getInstance();
    _sbInfoMessages = new StringBuffer();
    _sbErrorMessages = new StringBuffer();
    _file = file;
    
    _isCanceled = false;
    _progress = 0;
  }
  
  public void read() {
    boolean isSuccess = true;
    
    _logger.info("Start reading MapPosition file : " + _file.getPath());
    
    BufferedReader br = null;
    String line = null;
    String[] lineSplit = null;
    
    //##############################################
    //行数をカウントする
    if (isSuccess) {
      try {
        br = new BufferedReader(new FileReader(_file));
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
    
    //##############################################
    //配列を確保する
    if (isSuccess) {
      _nIntersections = countLines;
      _intersectionIDs = new int[_nIntersections];
      _intersectionPositionX = new float[_nIntersections];
      _intersectionPositionY = new float[_nIntersections];
      _intersectionPositionZ = new float[_nIntersections];
    }
    
    //##############################################
    //行ごとに解析を行う
    if (isSuccess) {
      try {
        br = new BufferedReader(new FileReader(_file));
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
            String strPositionX = lineSplit[1];
            String strPositionY = lineSplit[2];
            String strPositionZ = lineSplit[3];
            
            _intersectionIDs[countLines] = Integer.parseInt(strID);
            _intersectionPositionX[countLines] = Float.parseFloat(strPositionX);
            _intersectionPositionY[countLines] = Float.parseFloat(strPositionY);
            _intersectionPositionZ[countLines] = Float.parseFloat(strPositionZ);
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
    
    if (!isSuccess) {
      _nIntersections = 0;
      _intersectionIDs = null;
      _intersectionPositionX = null;
      _intersectionPositionY = null;
      _intersectionPositionZ = null;
    }
    
    _logger.info("Finish reading MapPosition file : " + _file.getPath());
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
  
  public int getNumIntersections() {
    return _nIntersections;
  }
  
  public float[] getIntersectionPositionX() {
    return _intersectionPositionX;
  }
  
  public float[] getIntersectionPositionY() {
    return _intersectionPositionY;
  }
  
  public float[] getIntersectionPositionZ() {
    return _intersectionPositionZ;
  }
}
