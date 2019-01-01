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
 * 信号の状態のデータファイルを読み込むクラスである。
 */
public class SignalResultReader {
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
  
  private int _nSignals;
  private int[] _intersectionIDSource;
  private int[] _intersectionIDDestination;
  private int[] _signalState;
  
  private boolean _isSuccess;
  
  private float _progress;
  
  private boolean _isCanceled;
  
  public SignalResultReader(File file) {
    _logger = Logger.getLogger(SignalResultReader.class.getName());
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
      _nSignals = countLines;
      _intersectionIDSource = new int[_nSignals];
      _intersectionIDDestination = new int[_nSignals];
      _signalState = new int[_nSignals];
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
            
            String strSignalID = lineSplit[0];
            String strSignalState = lineSplit[1];
            
            String strIntersectionIDDestination = strSignalID.substring(0, 6);
            String strIntersectionlIDSource = strSignalID.substring(6, 12);
            
            int intersectionIDDestination = Integer.parseInt(strIntersectionIDDestination);
            int intersectionIDSource = Integer.parseInt(strIntersectionlIDSource);
            int signalState = Integer.parseInt(strSignalState);
            
            _intersectionIDDestination[countLines] = intersectionIDDestination;
            _intersectionIDSource[countLines] = intersectionIDSource;
            _signalState[countLines] = signalState;
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
      _nSignals = 0;
      _intersectionIDSource = null;
      _intersectionIDDestination = null;
      _signalState = null;
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
  
  public int getNumSignals() {
    return _nSignals;
  }
  
  public int[] getIntersectionIDDestination() {
    return _intersectionIDDestination;
  }
  
  public int[] getIntersectionIDSource() {
    return _intersectionIDSource;
  }
  
  public int[] getSignalState() {
    return _signalState;
  }
}
