package jp.ac.u_tokyo.t.q.adventure.mates.io;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.resource.ResourceManager;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * 交差点の接続関係データを読み込むクラスである。
 */
public class NodeShapeFileReader {
  //@formatter:off
  private static final int INVALID = -1;
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
  
  private int _nIntersections;
  private int[] _nShapePoints;
  private int[] _intersectionID;
  private String[] _intersectionIDString;
  private float[] _centerPositionX;
  private float[] _centerPositionY;
  private float[] _centerPositionZ;
  private float[][] _shapePositionX;
  private float[][] _shapePositionY;
  private float[][] _shapePositionZ;
  
  private boolean _isSuccess;
  
  private float _progress;
  
  private boolean _isCanceled;
  
  public NodeShapeFileReader(File file) {
    _logger = Logger.getLogger(NodeShapeFileReader.class.getName());
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
    
    _logger.info("Start reading NodeShape file : " + _file.getPath());
    
    BufferedReader br = null;
    String line = null;
    String[] lineSplit = null;
    
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
        line = br.readLine();
        _nIntersections = Integer.parseInt(line.trim());
        lineNumber++;
        
        _nShapePoints = new int[_nIntersections];
        _intersectionID = new int[_nIntersections];
        _intersectionIDString = new String[_nIntersections];
        _centerPositionX = new float[_nIntersections];
        _centerPositionY = new float[_nIntersections];
        _centerPositionZ = new float[_nIntersections];
        _shapePositionX = new float[_nIntersections][];
        _shapePositionY = new float[_nIntersections][];
        _shapePositionZ = new float[_nIntersections][];
        
        for (int i = 0; i < _nIntersections; i++) {
          line = br.readLine();
          String strIntersectionID = line.trim();
          int intersectionID = Integer.parseInt(strIntersectionID);
          lineNumber++;
          _intersectionID[i] = intersectionID;
          _intersectionIDString[i] = strIntersectionID;
          
          line = br.readLine();
          lineSplit = line.split(Common.COMMA);
          _centerPositionX[i] = Float.parseFloat(lineSplit[0]);
          _centerPositionY[i] = Float.parseFloat(lineSplit[1]);
          _centerPositionZ[i] = Float.parseFloat(lineSplit[2]);
          lineNumber++;
          
          line = br.readLine();
          _nShapePoints[i] = Integer.parseInt(line.trim());
          lineNumber++;
          _shapePositionX[i] = new float[_nShapePoints[i]];
          _shapePositionY[i] = new float[_nShapePoints[i]];
          _shapePositionZ[i] = new float[_nShapePoints[i]];
          
          for (int j = 0; j < _nShapePoints[i]; j++) {
            line = br.readLine();
            lineSplit = line.split(Common.COMMA);
            _shapePositionX[i][j] = Float.parseFloat(lineSplit[0]);
            _shapePositionY[i][j] = Float.parseFloat(lineSplit[1]);
            _shapePositionZ[i][j] = Float.parseFloat(lineSplit[2]);
            lineNumber++;
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
      _nShapePoints = null;
      _intersectionID = null;
      _intersectionIDString = null;
      _centerPositionX = null;
      _centerPositionY = null;
      _centerPositionZ = null;
      _shapePositionX = null;
      _shapePositionY = null;
      _shapePositionZ = null;
    }
    
    _logger.info("Finish reading NodeShape file : " + _file.getPath());
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
  
  public int[] getNumShapePoints() {
    return _nShapePoints;
  }
  
  public int[] getIntersectionID() {
    return _intersectionID;
  }
  
  public String[] getIntersectionIDString() {
    return _intersectionIDString;
  }
  
  public float[] getCenterPositionX() {
    return _centerPositionX;
  }
  
  public float[] getCenterPositionY() {
    return _centerPositionY;
  }
  
  public float[] getCenterPositionZ() {
    return _centerPositionZ;
  }
  
  public float[][] getShapePositionX() {
    return _shapePositionX;
  }
  
  public float[][] getShapePositionY() {
    return _shapePositionY;
  }
  
  public float[][] getShapePositionZ() {
    return _shapePositionZ;
  }
}
