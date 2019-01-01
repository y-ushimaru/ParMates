package jp.ac.u_tokyo.t.q.adventure.mates.io;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.resource.ResourceManager;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * 交差点の接続関係データを読み込むクラスである。
 */
public class NetworkFileReader {
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
  
  private int _nRoads;
  private int[] _pointIDIn;
  private int[] _pointIDOut;
  private int[] _nLanesIn;
  private int[] _nLanesOut;
  
  private boolean _isSuccess;
  
  private float _progress;
  
  private boolean _isCanceled;
  
  public NetworkFileReader(File file) {
    _logger = Logger.getLogger(NetworkFileReader.class.getName());
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
    
    _logger.info("Start reading Network file : " + _file.getPath());
    
    BufferedReader br = null;
    String line = null;
    String[] lineSplit = null;
    
    ArrayList<String> listLines = new ArrayList<String>();
    String[] lines = null;
    
    //##############################################
    //空行以外をArrayListに追加していく
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
          if (line.trim().length() > 0) {
            listLines.add(line);
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
    
    //##############################################
    //配列を確保する
    if (listLines.size() > 0) {
      lines = new String[listLines.size()];
      for (int i = 0; i < listLines.size(); i++) {
        lines[i] = listLines.get(i);
      }
    } else {
      isSuccess = false;
      _logger.info("Network definition not found!");
    }
    
    //##############################################
    //行ごとに解析を行う
    if (isSuccess) {
      isSuccess = parseNetworkDefinition(lines);
    }
    
    if (!isSuccess) {
      _nRoads = 0;
      _pointIDIn = null;
      _pointIDOut = null;
      _nLanesIn = null;
      _nLanesOut = null;
    }
    
    _logger.info("Finish reading Network file : " + _file.getPath());
    _isSuccess = isSuccess;
  }
  
  private boolean parseNetworkDefinition(String[] lines) {
    boolean isSuccess = true;
    
    String[][] linesSplit = new String[lines.length][];
    if (isSuccess) {
      _nRoads = 0;
      for (int i = 0; i < lines.length; i++) {
        try {
          linesSplit[i] = lines[i].split(Common.COMMA);
          //はじめの2つの項目以外は、接続先の交差点番号である
          int nConnectedPoints = linesSplit[i].length - 2;
          //2つ目の項目は、接続交差点数*2の桁数の文字列である
          int nConnectedPointsx2 = linesSplit[i][1].length();
          if (nConnectedPoints > 0 && nConnectedPointsx2 > 0 && nConnectedPoints * 2 == nConnectedPointsx2) {
            _nRoads += nConnectedPoints;
          } else {
            isSuccess = false;
          }
        } catch (Exception e) {
          isSuccess = false;
        }
        if (isSuccess == false) {
          break;
        }
      }
    }
    
    if (isSuccess) {
      _pointIDIn = new int[_nRoads];
      _pointIDOut = new int[_nRoads];
      _nLanesIn = new int[_nRoads];
      _nLanesOut = new int[_nRoads];
      for (int i = 0; i < _nRoads; i++) {
        _pointIDIn[i] = INVALID;
        _pointIDOut[i] = INVALID;
        _nLanesIn[i] = INVALID;
        _nLanesOut[i] = INVALID;
      }
      
      for (int i = 0; i < lines.length; i++) {
        //はじめの2つの項目以外は、接続先の交差点番号である
        int nConnectedPoints = linesSplit[i].length - 2;
        //2つ目の項目は、接続交差点数*2の桁数の文字列である
        int nConnectedPointsx2 = linesSplit[i][1].length();
        int[] tmpNumLanes = new int[nConnectedPointsx2];
        int[] tmpPointIDs = new int[nConnectedPoints];
        
        int currentPointID = INVALID;
        try {
          currentPointID = Integer.parseInt(linesSplit[i][0]);
        } catch (Exception e) {
          isSuccess = false;
        }
        
        for (int j = 0; j < nConnectedPointsx2; j++) {
          try {
            tmpNumLanes[j] = Integer.parseInt(linesSplit[i][1].substring(j, j + 1));
          } catch (Exception e) {
            isSuccess = false;
          }
          if (isSuccess == false) {
            break;
          }
        }
        
        int offset = 2;
        for (int j = 0; j < nConnectedPoints; j++) {
          try {
            tmpPointIDs[j] = Integer.parseInt(linesSplit[i][offset + j]);
          } catch (Exception e) {
            isSuccess = false;
          }
          if (isSuccess == false) {
            break;
          }
        }
        
        if (isSuccess) {
          for (int j = 0, jx2 = 0; j < nConnectedPoints; j++, jx2 += 2) {
            int connectedPointID = tmpPointIDs[j];
            int nLane1 = tmpNumLanes[jx2 + 0];
            int nLane2 = tmpNumLanes[jx2 + 1];
            boolean flgHitIn = false;
            boolean flgHitOut = false;
            for (int k = 0; k < _nRoads; k++) {
              if (_pointIDIn[k] != INVALID && _pointIDOut[k] != INVALID) {
                if (_pointIDIn[k] == currentPointID && _pointIDOut[k] == connectedPointID) {
                  if (_nLanesIn[k] == INVALID) {
                    _nLanesIn[k] = nLane2;
                    flgHitIn = true;
                  } else {
                    isSuccess = false;
                  }
                } else if (_pointIDIn[k] == connectedPointID && _pointIDOut[k] == currentPointID) {
                  if (_nLanesOut[k] == INVALID) {
                    _nLanesOut[k] = nLane1;
                    flgHitOut = true;
                  } else {
                    isSuccess = false;
                  }
                }
              }
              
              if (isSuccess == false) {
                break;
              }
            }
            
            if (isSuccess) {
              for (int k = 0; k < _nRoads; k++) {
                if (_pointIDIn[k] == INVALID && _pointIDOut[k] == INVALID) {
                  if (flgHitIn == false) {
                    _pointIDIn[k] = currentPointID;
                    _pointIDOut[k] = connectedPointID;
                    _nLanesIn[k] = nLane2;
                    flgHitIn = true;
                    continue;
                  } else if (flgHitOut == false) {
                    _pointIDIn[k] = connectedPointID;
                    _pointIDOut[k] = currentPointID;
                    _nLanesOut[k] = nLane1;
                    flgHitOut = true;
                    continue;
                  }
                }
              }
              if (flgHitIn == false || flgHitOut == false) {
                isSuccess = false;
              }
            }
            
            if (isSuccess == false) {
              break;
            }
          }
        }
      }
    }
    
    return isSuccess;
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
  
  public int[] getPointIDIn() {
    return _pointIDIn;
  }
  
  public int[] getPointIDOut() {
    return _pointIDOut;
  }
  
  public int[] getNumLanesIn() {
    return _nLanesIn;
  }
  
  public int[] getNumLanesOut() {
    return _nLanesOut;
  }
}
