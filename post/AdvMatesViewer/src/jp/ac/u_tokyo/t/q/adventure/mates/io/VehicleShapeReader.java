package jp.ac.u_tokyo.t.q.adventure.mates.io;

import java.io.File;
import java.io.FileInputStream;
import java.util.Properties;
import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.model.TrafficParameters;
import jp.ac.u_tokyo.t.q.adventure.mates.model.VehicleFamily;
import jp.ac.u_tokyo.t.q.adventure.mates.resource.ResourceManager;
import jp.ac.u_tokyo.t.q.adventure.mates.ui.render.Drawable3DPanel;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * 車両の形状データファイルを読み込むクラスである。
 */
public class VehicleShapeReader {
  //@formatter:off
  private static final String FILE_EXTENSION_PCH  = ".pch";
  private static final String FILE_EXTENSION_STL  = ".stl";
  
  private static final String KEY_NUM_SHAPES      = "nShapes";
  private static final String KEY_PRE             = "Shape.";
  private static final String KEY_POST_FILE       = ".file";
  private static final String KEY_POST_COLOR      = ".color";
  
  private static final String KEY_COLOR_BODY      = "Color.BODY";
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
   * 車両の形状と色の設定を記述したファイル。
   */
  private File _file;
  
  /**
   * 解析の各種設定を保持するクラスである。
   */
  private TrafficParameters _trafficParameters;
  
  /**
   * 車種の設定を保持するクラスである。
   */
  private VehicleFamily _vehicleFamily;
  
  private float[][] _coords;
  private int[][] _connectivity;
  //private float[][] _normals;
  private float[][] _colors;
  
  private boolean _isSuccess;
  
  private float _progress;
  
  private boolean _isCanceled;
  
  public VehicleShapeReader(File file, TrafficParameters trafficParameters, VehicleFamily vehicleFamily) {
    _logger = Logger.getLogger(VehicleShapeReader.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    _rm = ResourceManager.getInstance();
    _sbInfoMessages = new StringBuffer();
    _sbErrorMessages = new StringBuffer();
    
    _file = file;
    _trafficParameters = trafficParameters;
    _vehicleFamily = vehicleFamily;
    
    _isSuccess = false;
    _isCanceled = false;
    _progress = 0;
  }
  
  public void read() {
    boolean isSuccess = true;
    
    int nShapes = 0;
    String key = null;
    String value = null;
    
    _sbErrorMessages.setLength(0);
    
    //車両の設定ファイルがあるディレクトリ
    File dir = _file.getParentFile();
    
    _logger.info("Start reading shape files");
    
    Properties properties = new Properties();
    try {
      isSuccess = false;
      FileInputStream inputStrem = new FileInputStream(_file);
      properties.load(inputStrem);
      inputStrem.close();
      
      isSuccess = true;
    } catch (Exception e) {
    }
    
    if (isSuccess) {
      key = KEY_NUM_SHAPES;
      try {
        value = properties.getProperty(key);
        nShapes = Integer.parseInt(value);
      } catch (Exception e) {
        if (_sbErrorMessages.length() > 0) _sbErrorMessages.append(Common.EOL);
        _sbErrorMessages.append("Error : Not found : Number of shapes. key=" + key);
        isSuccess = false;
      }
    }
    
    if (isSuccess) {
      if (nShapes <= 0) {
        if (_sbErrorMessages.length() > 0) _sbErrorMessages.append(Common.EOL);
        _sbErrorMessages.append("Error : Number of shapes must be positive integer. nShapes=" + nShapes);
        isSuccess = false;
      }
    }
    
    if (isSuccess) {
      _coords = new float[nShapes][];
      _connectivity = new int[nShapes][];
      _colors = new float[nShapes][];
      for (int i = 0; i < nShapes; i++) {
        String keyFile = KEY_PRE + i + KEY_POST_FILE;
        String strFileName = properties.getProperty(keyFile);
        String keyColor = KEY_PRE + i + KEY_POST_COLOR;
        String strColor = properties.getProperty(keyColor);
        
        File file = new File(dir, strFileName);
        boolean isSuccessFile = false;
        if (file.isFile()) {
          String strFileNameLowerCase = strFileName.toLowerCase();
          if (strFileNameLowerCase.endsWith(FILE_EXTENSION_PCH)) {
            PchFileReader reader = new PchFileReader(file);
            reader.read();
            if (reader.isSuccess()) {
              _coords[i] = reader.getCoords();
              _connectivity[i] = reader.getConnectivity();
              isSuccessFile = true;
            }
          } else if (strFileNameLowerCase.endsWith(FILE_EXTENSION_STL)) {
            StlFileReader reader = new StlFileReader(file);
            reader.read();
            if (reader.isSuccess()) {
              _coords[i] = reader.getCoords();
              _connectivity[i] = reader.getConnectivity();
              isSuccessFile = true;
            }
          }
        }
        if (isSuccessFile == false) {
          if (_sbErrorMessages.length() > 0) _sbErrorMessages.append(Common.EOL);
          _sbErrorMessages.append("Error : key=" + keyFile);
          _sbErrorMessages.append(Common.EOL);
          _sbErrorMessages.append("Failed to read file : " + file.getAbsolutePath());
          isSuccess = false;
        }
        
        float[] color = toColor(strColor, _sbErrorMessages);
        if (color != null) {
          _colors[i] = color;
        } else {
          if (_sbErrorMessages.length() > 0) _sbErrorMessages.append(Common.EOL);
          _sbErrorMessages.append("Error : key=" + keyColor);
          isSuccess = false;
        }
      }
    }
    
    if (isSuccess) {
      _isSuccess = true;
    } else {
      _coords = null;
      _connectivity = null;
      _colors = null;
      _logger.info("Failed to read shape files : " + _file.getAbsolutePath());
    }
    
    if (_sbErrorMessages.length() > 0) {
      _sbErrorMessages.insert(0, Common.EOL);
      _sbErrorMessages.insert(0, "Error occured : " + _file.getAbsolutePath());
      
      _logger.info(_sbErrorMessages.toString());
    }
  }
  
  private float[] toColor(String strColor, StringBuffer sbError) {
    float[] color = null;
    
    if (strColor != null) {
      boolean isSuccess = true;
      try {
        String[] split = strColor.split(Common.COMMA);
        float colorR = Float.parseFloat(split[0].trim());
        float colorG = Float.parseFloat(split[1].trim());
        float colorB = Float.parseFloat(split[2].trim());
        color = new float[4];
        color[0] = colorR;
        color[1] = colorG;
        color[2] = colorB;
        color[3] = Drawable3DPanel.ALPHA_FOR_OPAQUE;
      } catch (Exception e) {
        isSuccess = false;
      }
      
      if (isSuccess == false) {
        if (strColor.equalsIgnoreCase(KEY_COLOR_BODY) && _vehicleFamily != null) {
          color = _vehicleFamily.getColorForBody();
          isSuccess = true;
        } else if (_trafficParameters != null) {
          color = _trafficParameters.getColor(strColor);
          if (color != null) {
            isSuccess = true;
          } else {
            if (sbError != null) {
              if (sbError.length() > 0) sbError.append(Common.EOL);
              sbError.append("Invalid color name : " + strColor);
            }
          }
        }
      }
      
      if (isSuccess == false) {
        color = null;
      }
    }
    
    return color;
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
  
  public float[][] getCoords() {
    return _coords;
  }
  
  public int[][] getConnectivity() {
    return _connectivity;
  }
  
  //public float[][] getNormals() {
  //  return _normals;
  //}
  
  public float[][] getColors() {
    return _colors;
  }
}
