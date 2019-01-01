package jp.ac.u_tokyo.t.q.adventure.mates.io;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * PCHファイル用のリーダークラス
 */
public class PchFileReader {
  //@formatter:off
  private static final float PROGRESS_RATIO_READ_COORDINATE   = 0.4f;
  private static final float PROGRESS_RATIO_READ_CONNECTIVITY = 0.3f;
  //@formatter:on
  
  /**
   * ログハンドリングクラス
   */
  private Logger _logger;
  /**
   * 読み込む対象のファイル
   */
  private File _file;
  
  private float[] _coords;
  
  private int[] _connectivity;
  
  private int _nNodes;
  
  private boolean _isSuccess;
  
  private float _progress;
  
  private boolean _isCanceled;
  
  public PchFileReader(File file) {
    _logger = Logger.getLogger(PchFileReader.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    _file = file;
    _coords = null;
    _connectivity = null;
    _nNodes = 0;
    _isCanceled = false;
    _progress = 0;
  }
  
  public void read() {
    _isSuccess = true;
    //_logger.info("### Start reading file : " + _file.getPath());
    BufferedReader br = null;
    String line;
    String[] lineSplit;
    _progress = 0;
    
    int linecount = 0;
    try {
      br = new BufferedReader(new FileReader(_file));
    } catch (Exception e) {
      _isSuccess = false;
      _logger.info("Failed to open file : " + _file.getPath());
    }
    try {
      line = br.readLine();
      _nNodes = Integer.parseInt(line.trim());
      linecount++;
      _coords = new float[_nNodes * 3];
      
      int ix3 = 0;
      for (int i = 0; i < _nNodes; i++) {
        if (_isCanceled) {
          _isSuccess = false;
          break;
        }
        _progress = PROGRESS_RATIO_READ_COORDINATE * i / _nNodes;
        line = br.readLine();
        lineSplit = line.trim().split(Common.REGULAR_EXPRESSION_SEQUENCE_OF_SPACE);
        _coords[ix3] = Float.parseFloat(lineSplit[0]);
        _coords[ix3 + 1] = Float.parseFloat(lineSplit[1]);
        _coords[ix3 + 2] = Float.parseFloat(lineSplit[2]);
        linecount++;
        ix3 += 3;
      }
      
      if (_isSuccess) {
        line = br.readLine();
        int nElements = Integer.parseInt(line.trim());
        linecount++;
        _connectivity = new int[nElements * 3];
        ix3 = 0;
        for (int i = 0; i < nElements; i++) {
          if (_isCanceled) {
            _isSuccess = false;
            break;
          }
          _progress = PROGRESS_RATIO_READ_COORDINATE;
          _progress += PROGRESS_RATIO_READ_CONNECTIVITY * i / _nNodes;
          line = br.readLine();
          lineSplit = line.trim().split(Common.REGULAR_EXPRESSION_SEQUENCE_OF_SPACE);
          int p0 = Integer.parseInt(lineSplit[0]);
          int p2 = Integer.parseInt(lineSplit[1]);
          int p1 = Integer.parseInt(lineSplit[2]);
          linecount++;
          _connectivity[ix3] = p0;
          _connectivity[ix3 + 1] = p1;
          _connectivity[ix3 + 2] = p2;
          ix3 += 3;
        }
      }
    } catch (Exception e) {
      _isSuccess = false;
      _logger.info("Error occured in reading PCH file at " + linecount + "th line.");
      e.printStackTrace();
    }
    if (br != null) {
      try {
        br.close();
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
    
    if (_isSuccess == false) {
      _coords = null;
      _connectivity = null;
    }
    
    //_logger.info("### Finish reading file : " + _file.getPath());
  }
  
  public void saveCacheFile() {
    
  }
  
  public float[] getCoords() {
    return _coords;
  }
  
  public int[] getConnectivity() {
    return _connectivity;
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
