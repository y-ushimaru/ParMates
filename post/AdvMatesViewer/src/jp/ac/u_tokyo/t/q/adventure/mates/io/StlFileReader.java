package jp.ac.u_tokyo.t.q.adventure.mates.io;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * STLファイル用のリーダークラス
 */
public class StlFileReader {
  //@formatter:off
  private static final String VERTEX                          = "VERTEX";
  private static final String FACET                           = "FACET";
  
  private static final String REGULAR_EXPRESSION_NOT_ASCII    = "[^\\p{ASCII}]";
  
  private static final int FILE_TYPE_UNKNOWN                  = 0;
  private static final int FILE_TYPE_ASCII                    = 10;
  private static final int FILE_TYPE_BINARY                   = 20;
  
  /**
   * falseのときは点のマージを行わない(読み込み時間が短縮される)
   */
  private boolean TO_MERGE_NODES                              = true;
  
  private static final float PROGRESS_RATIO_READ_COUNT        = 0.3f;
  private static final float PROGRESS_RATIO_READING           = 0.7f;
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
  
  private boolean _isSuccess;
  
  private float _progress;
  
  private boolean _isCanceled;
  
  public StlFileReader(File file) {
    _logger = Logger.getLogger(StlFileReader.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    _file = file;
    _coords = null;
    _connectivity = null;
    _isCanceled = false;
    _progress = 0;
  }
  
  public void read() {
    _isSuccess = true;
    _logger.info("### Start reading file  : " + _file.getAbsolutePath());
    
    int fileType = determineFileType();
    
    if (fileType == FILE_TYPE_ASCII) {
      readFileAscii();
    } else if (fileType == FILE_TYPE_BINARY) {
      readFileBinary();
    } else {
      if (_isSuccess) {
        _logger.info("Try read file as ASCII file.");
        readFileAscii();
      }
      if (_isSuccess) {
        _logger.info("Try read file as Binary file.");
        readFileBinary();
      }
    }
    
    _logger.info("### Finish reading file : " + _file.getAbsolutePath());
  }
  
  private int determineFileType() {
    int type = FILE_TYPE_UNKNOWN;
    int readLength = 200;
    if (readLength > _file.length()) {
      readLength = (int) (_file.length());
    }
    
    InputStream inputStream = null;
    
    try {
      inputStream = new FileInputStream(_file);
      byte[] buffer = new byte[readLength];
      int offset = 0;
      inputStream.read(buffer, offset, readLength);
      //ファイルのはじめ部分にASCII以外の文字の有無でASCII or Binaryの判定を行う。
      String str = new String(buffer, offset, readLength);
      Pattern pattern = Pattern.compile(REGULAR_EXPRESSION_NOT_ASCII);
      Matcher matcher = pattern.matcher(str);
      
      if (matcher.find() == false) {
        type = FILE_TYPE_ASCII;
        _logger.info("STL file type : ASCII");
      } else {
        type = FILE_TYPE_BINARY;
        _logger.info("STL file type : Binary");
      }
    } catch (Exception e) {
      type = FILE_TYPE_UNKNOWN;
    }
    
    if (inputStream != null) {
      try {
        inputStream.close();
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
    
    return type;
  }
  
  private void readFileAscii() {
    _isSuccess = true;
    _logger.info("### Start reading file  : " + _file.getAbsolutePath());
    BufferedReader br = null;
    String line;
    String[] lineSplit;
    String lineTrim = null;
    String VERTEX_UPPER_CASE = VERTEX.toUpperCase();
    String FACET_UPPER_CASE = FACET.toUpperCase();
    _progress = 0;
    
    //############################################################
    //ファイルを読み込む
    //############################################################
    long lengthFile = _file.length();
    long lengthRead = 0;
    int countNode = 0;
    int countElem = 0;
    double minX = Double.POSITIVE_INFINITY;
    double minY = Double.POSITIVE_INFINITY;
    double minZ = Double.POSITIVE_INFINITY;
    double maxX = Double.NEGATIVE_INFINITY;
    double maxY = Double.NEGATIVE_INFINITY;
    double maxZ = Double.NEGATIVE_INFINITY;
    int linecount = 0;
    boolean isNIOMode = false;
    try {
      br = new BufferedReader(new FileReader(_file));
    } catch (Exception e) {
      _isSuccess = false;
      _logger.info("Failed to open file : " + _file.getAbsolutePath());
    }
    if (_isSuccess) {
      try {
        lengthRead = 0;
        while (true) {
          if (_isCanceled) {
            _isSuccess = false;
            break;
          }
          line = br.readLine();
          linecount++;
          if (line == null) break;
          lengthRead += line.length();
          _progress = PROGRESS_RATIO_READ_COUNT * lengthRead / lengthFile;
          lineTrim = line.trim().toUpperCase();
          if (lineTrim.startsWith(VERTEX_UPPER_CASE)) {
            countNode++;
          } else if (lineTrim.startsWith(FACET_UPPER_CASE)) {
            countElem++;
          }
        }
      } catch (Exception e) {
        _isSuccess = false;
        _logger.info("Error occured in reading STL file at " + linecount + "th line.");
      }
      if (br != null) {
        try {
          br.close();
        } catch (Exception e) {
          e.printStackTrace();
        }
      }
      
      if (countNode == 0) {
        _isSuccess = false;
        _logger.info("### No nodes found!!");
      }
      if (countElem == 0) {
        _isSuccess = false;
        _logger.info("### No elements found!!");
      }
    }
    
    if (_isSuccess) {
      _logger.info("Number on node=" + countNode + " : Number of elements=" + countElem);
      _coords = new float[countNode * 3];
      _connectivity = new int[countElem * 3];
      for (int i = 0; i < _connectivity.length; i++) {
        _connectivity[i] = i;
      }
      try {
        br = new BufferedReader(new FileReader(_file));
      } catch (Exception e) {
        _isSuccess = false;
        _logger.info("Failed to open file : " + _file.getAbsolutePath());
      }
      try {
        lengthRead = 0;
        int countNodex3 = 0;
        while (true) {
          if (_isCanceled) {
            _isSuccess = false;
            break;
          }
          line = br.readLine();
          linecount++;
          if (line == null) break;
          lengthRead += line.length();
          _progress = PROGRESS_RATIO_READ_COUNT;
          _progress += PROGRESS_RATIO_READING * lengthRead / lengthFile;
          lineTrim = line.trim().toUpperCase();
          if (lineTrim.startsWith(VERTEX_UPPER_CASE)) {
            lineSplit = lineTrim.split(Common.REGULAR_EXPRESSION_SEQUENCE_OF_SPACE);
            float tmpX = Float.parseFloat(lineSplit[1]);
            float tmpY = Float.parseFloat(lineSplit[2]);
            float tmpZ = Float.parseFloat(lineSplit[3]);
            _coords[countNodex3] = tmpX;
            _coords[countNodex3 + 1] = tmpY;
            _coords[countNodex3 + 2] = tmpZ;
            if (minX > tmpX) minX = tmpX;
            if (maxX < tmpX) maxX = tmpX;
            if (minY > tmpY) minY = tmpY;
            if (maxY < tmpY) maxY = tmpY;
            if (minZ > tmpZ) minZ = tmpZ;
            if (maxZ < tmpZ) maxZ = tmpZ;
            countNodex3 += 3;
          }
        }
        
      } catch (Exception e) {
        _isSuccess = false;
        _logger.info("Error occured in reading STL file at " + linecount + "th line.");
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
  }
  
  private void readFileBinary() {
    int lengthHeader = 80;
    int lengthInt = 4;
    int lengthFloat = 4;
    int lengthUnused = 2;
    int lengthBuffer = 4;
    
    int nVertices = 3;
    int nAxis = 3;
    
    DataInputStream dis = null;
    //エンディアンの違いを吸収するために用いるバッファーである。
    ByteBuffer byteBuffer = ByteBuffer.allocate(lengthBuffer);
    ByteOrder defaultOrder = byteBuffer.order();
    int nTriangles = 0;
    int offset = 0;
    _progress = 0;
    try {
      dis = new DataInputStream(new FileInputStream(_file));
      byte[] header = new byte[lengthHeader];
      
      //80文字のヘッダーを読み込む
      dis.read(header);
      
      readInt(byteBuffer, defaultOrder, offset, dis);
      nTriangles = byteBuffer.getInt(offset);
      
      if (nTriangles <= 0) {
        _isSuccess = false;
        _logger.info("### Number of triangles must be positive integer. nTriangles = " + nTriangles);
      }
    } catch (Exception e) {
      _isSuccess = false;
      _logger.info("Error occured in reading header of Binary STL file");
    }
    
    if (_isSuccess) {
      _logger.info("Number on triangles=" + nTriangles);
      _coords = new float[nTriangles * nVertices * nAxis];
      _connectivity = new int[nTriangles * nVertices];
      for (int i = 0; i < _connectivity.length; i++) {
        _connectivity[i] = i;
      }
      try {
        int offsetCoords = 0;
        for (int i = 0; i < nTriangles; i++) {
          _progress = 1.0f * i / nTriangles;
          //法線ベクトル。読むだけで、使わない。
          readFloat(byteBuffer, defaultOrder, offset, dis);
          readFloat(byteBuffer, defaultOrder, offset, dis);
          readFloat(byteBuffer, defaultOrder, offset, dis);
          for (int j = 0; j < nVertices; j++) {
            readFloat(byteBuffer, defaultOrder, offset, dis);
            float x = byteBuffer.getFloat(offset);
            readFloat(byteBuffer, defaultOrder, offset, dis);
            float y = byteBuffer.getFloat(offset);
            readFloat(byteBuffer, defaultOrder, offset, dis);
            float z = byteBuffer.getFloat(offset);
            _coords[offsetCoords + 0] = x;
            _coords[offsetCoords + 1] = y;
            _coords[offsetCoords + 2] = z;
            offsetCoords += 3;
          }
          
          //未使用データの2Byteを読み込む。
          byteBuffer.put(offset, dis.readByte());
          byteBuffer.put(offset, dis.readByte());
        }
      } catch (Exception e) {
        _isSuccess = false;
        _logger.info("Error occured in reading body of Binary STL file");
      }
    }
    
    if (dis != null) {
      try {
        dis.close();
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
  }
  
  private static void readInt(ByteBuffer byteBuffer, ByteOrder defaultOrder, int offset, DataInputStream dis)
      throws IOException {
    int value = dis.readInt();
    byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
    byteBuffer.putInt(offset, value);
    byteBuffer.order(defaultOrder);
  }
  
  private static void readFloat(ByteBuffer byteBuffer, ByteOrder defaultOrder, int offset, DataInputStream dis)
      throws IOException {
    float value = dis.readFloat();
    byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
    byteBuffer.putFloat(offset, value);
    byteBuffer.order(defaultOrder);
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
