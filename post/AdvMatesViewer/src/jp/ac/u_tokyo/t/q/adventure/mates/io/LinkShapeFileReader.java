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
public class LinkShapeFileReader {
  //@formatter:off
  //private static final int INVALID         = -1;
  private static final int FLAG_EXIST      = 1;
  private static final int FLAG_NOT_EXIST  = 0;
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
  
  private int _nRoads;
  private int[] _nLanesRight;
  private int[] _nLanesLeft;
  private int[] _nPointsCenter;
  private int[] _nPointsRight;
  private int[] _nPointsLeft;
  private int[] _intersectionIDStart;
  private int[] _intersectionIDEnd;
  private boolean[] _existSignalStart;
  private boolean[] _existSignalEnd;
  private float[][] _positionCenterX;
  private float[][] _positionCenterY;
  private float[][] _positionCenterZ;
  private float[][] _positionRightX;
  private float[][] _positionRightY;
  private float[][] _positionRightZ;
  private float[][] _positionLeftX;
  private float[][] _positionLeftY;
  private float[][] _positionLeftZ;
  
  private boolean _isSuccess;
  
  private float _progress;
  
  private boolean _isCanceled;
  
  public LinkShapeFileReader(File file) {
    _logger = Logger.getLogger(LinkShapeFileReader.class.getName());
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
    
    _logger.info("Start reading LinkShape file : " + _file.getPath());
    
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
        _nRoads = Integer.parseInt(line.trim());
        lineNumber++;
        
        _nLanesRight = new int[_nRoads];
        _nLanesLeft = new int[_nRoads];
        _nPointsCenter = new int[_nRoads];
        _nPointsRight = new int[_nRoads];
        _nPointsLeft = new int[_nRoads];
        _intersectionIDStart = new int[_nRoads];
        _intersectionIDEnd = new int[_nRoads];
        _existSignalStart = new boolean[_nRoads];
        _existSignalEnd = new boolean[_nRoads];
        _positionCenterX = new float[_nRoads][];
        _positionCenterY = new float[_nRoads][];
        _positionCenterZ = new float[_nRoads][];
        _positionRightX = new float[_nRoads][];
        _positionRightY = new float[_nRoads][];
        _positionRightZ = new float[_nRoads][];
        _positionLeftX = new float[_nRoads][];
        _positionLeftY = new float[_nRoads][];
        _positionLeftZ = new float[_nRoads][];
        
        for (int i = 0; i < _nRoads; i++) {
          //単路ID
          line = br.readLine();
          //long roadID = Long.parseLong(line.trim());
          lineNumber++;
          
          //レーン数
          line = br.readLine();
          lineSplit = line.split(Common.COMMA);
          //TODO 順序には要注意
          _nLanesRight[i] = Integer.parseInt(lineSplit[1]);
          _nLanesLeft[i] = Integer.parseInt(lineSplit[0]);
          lineNumber++;
          
          //端点の交差点ID
          line = br.readLine();
          lineSplit = line.split(Common.COMMA);
          int intersectionIDStart = Integer.parseInt(lineSplit[0]);
          int intersectionIDEnd = Integer.parseInt(lineSplit[1]);
          lineNumber++;
          _intersectionIDStart[i] = intersectionIDStart;
          _intersectionIDEnd[i] = intersectionIDEnd;
          
          //端点の信号の有無のフラグ
          line = br.readLine();
          lineSplit = line.split(Common.COMMA);
          int flgSignaleStart = Integer.parseInt(lineSplit[0]);
          int flgSignaleEnd = Integer.parseInt(lineSplit[1]);
          lineNumber++;
          _existSignalStart[i] = (flgSignaleStart == FLAG_EXIST);
          _existSignalEnd[i] = (flgSignaleEnd == FLAG_EXIST);
          
          //中心のラインの点の個数
          line = br.readLine();
          _nPointsCenter[i] = Integer.parseInt(line.trim());
          lineNumber++;
          _positionCenterX[i] = new float[_nPointsCenter[i]];
          _positionCenterY[i] = new float[_nPointsCenter[i]];
          _positionCenterZ[i] = new float[_nPointsCenter[i]];
          for (int j = 0; j < _nPointsCenter[i]; j++) {
            //中心のラインの点の座標値
            line = br.readLine();
            lineSplit = line.split(Common.COMMA);
            _positionCenterX[i][j] = Float.parseFloat(lineSplit[0]);
            _positionCenterY[i][j] = Float.parseFloat(lineSplit[1]);
            _positionCenterZ[i][j] = Float.parseFloat(lineSplit[2]);
            lineNumber++;
          }
          
          //右端のラインの点の個数
          line = br.readLine();
          _nPointsRight[i] = Integer.parseInt(line.trim());
          lineNumber++;
          _positionRightX[i] = new float[_nPointsRight[i]];
          _positionRightY[i] = new float[_nPointsRight[i]];
          _positionRightZ[i] = new float[_nPointsRight[i]];
          for (int j = 0; j < _nPointsCenter[i]; j++) {
            //右端のラインの点の座標値
            line = br.readLine();
            lineSplit = line.split(Common.COMMA);
            _positionRightX[i][j] = Float.parseFloat(lineSplit[0]);
            _positionRightY[i][j] = Float.parseFloat(lineSplit[1]);
            _positionRightZ[i][j] = Float.parseFloat(lineSplit[2]);
            lineNumber++;
          }
          
          //左端のラインの点の個数
          line = br.readLine();
          _nPointsLeft[i] = Integer.parseInt(line.trim());
          lineNumber++;
          _positionLeftX[i] = new float[_nPointsLeft[i]];
          _positionLeftY[i] = new float[_nPointsLeft[i]];
          _positionLeftZ[i] = new float[_nPointsLeft[i]];
          for (int j = 0; j < _nPointsCenter[i]; j++) {
            //左端のラインの点の座標値
            line = br.readLine();
            lineSplit = line.split(Common.COMMA);
            _positionLeftX[i][j] = Float.parseFloat(lineSplit[0]);
            _positionLeftY[i][j] = Float.parseFloat(lineSplit[1]);
            _positionLeftZ[i][j] = Float.parseFloat(lineSplit[2]);
            lineNumber++;
          }
        }
      } catch (Exception e) {
        isSuccess = false;
        String message = "Error occured at " + lineNumber + "th line : " + _file.getPath();
        message += Common.EOL;
        message += "line=\"" + line + "\"";
        _logger.info(message);
        e.printStackTrace();
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
      _nRoads = 0;
      _nLanesRight = null;
      _nLanesLeft = null;
      _nPointsCenter = null;
      _nPointsRight = null;
      _nPointsLeft = null;
      _intersectionIDStart = null;
      _intersectionIDEnd = null;
      _existSignalStart = null;
      _existSignalEnd = null;
      _positionCenterX = null;
      _positionCenterY = null;
      _positionCenterZ = null;
      _positionRightX = null;
      _positionRightY = null;
      _positionRightZ = null;
      _positionLeftX = null;
      _positionLeftY = null;
      _positionLeftZ = null;
    }
    
    _logger.info("Finish reading LinkShape file : " + _file.getPath());
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
  
  public int getNumRoads() {
    return _nRoads;
  }
  
  public int[] getNumLanesRight() {
    return _nLanesRight;
  }
  
  public int[] getNumLanesLeft() {
    return _nLanesLeft;
  }
  
  public int[] getNumPointsCenter() {
    return _nPointsCenter;
  }
  
  public int[] getNumPointsRight() {
    return _nPointsRight;
  }
  
  public int[] getNumPointsLeft() {
    return _nPointsLeft;
  }
  
  public int[] getIntersectionIDStart() {
    return _intersectionIDStart;
  }
  
  public int[] getIntersectionIDEnd() {
    return _intersectionIDEnd;
  }
  
  public boolean[] getExistSignalStart() {
    return _existSignalStart;
  }
  
  public boolean[] getExistSignalEnd() {
    return _existSignalEnd;
  }
  
  public float[][] getPositionCenterX() {
    return _positionCenterX;
  }
  
  public float[][] getPositionCenterY() {
    return _positionCenterY;
  }
  
  public float[][] getPositionCenterZ() {
    return _positionCenterZ;
  }
  
  public float[][] getPositionRightX() {
    return _positionRightX;
  }
  
  public float[][] getPositionRightY() {
    return _positionRightY;
  }
  
  public float[][] getPositionRightZ() {
    return _positionRightZ;
  }
  
  public float[][] getPositionLeftX() {
    return _positionLeftX;
  }
  
  public float[][] getPositionLeftY() {
    return _positionLeftY;
  }
  
  public float[][] getPositionLeftZ() {
    return _positionLeftZ;
  }
}
