package jp.ac.u_tokyo.t.q.adventure.mates.control;

import java.io.File;
import java.io.FileInputStream;
import java.util.Properties;
import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.io.ConvertedResultReader;
import jp.ac.u_tokyo.t.q.adventure.mates.io.LinkShapeFileReader;
import jp.ac.u_tokyo.t.q.adventure.mates.io.MapPositionFileReader;
import jp.ac.u_tokyo.t.q.adventure.mates.io.NetworkFileReader;
import jp.ac.u_tokyo.t.q.adventure.mates.io.NodeShapeFileReader;
import jp.ac.u_tokyo.t.q.adventure.mates.io.SignalResultReader;
import jp.ac.u_tokyo.t.q.adventure.mates.io.TimeLineConverter;
import jp.ac.u_tokyo.t.q.adventure.mates.io.VehicleFamilyFileReader;
import jp.ac.u_tokyo.t.q.adventure.mates.io.VehicleResultReader;
import jp.ac.u_tokyo.t.q.adventure.mates.io.VehicleShapeReader;
import jp.ac.u_tokyo.t.q.adventure.mates.model.TrafficModel;
import jp.ac.u_tokyo.t.q.adventure.mates.model.TrafficParameters;
import jp.ac.u_tokyo.t.q.adventure.mates.model.VehicleFamily;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * メッシュ生成を行うクラスである。
 */
public class IOControl {
  /**
   * ログハンドリングクラス
   */
  private Logger _logger;
  
  private boolean _isCanceled;
  
  private String _message1;
  private String _message2;
  
  private ConvertedResultReader _convertedResultReader;
  private TimeLineConverter _timeLineConverter;
  
  public IOControl() {
    _logger = Logger.getLogger(IOControl.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    
    _isCanceled = false;
    _message1 = "";
    _message2 = "";
    
    _convertedResultReader = null;
    _timeLineConverter = null;
  }
  
  public void cancel() {
    _isCanceled = true;
    if (_convertedResultReader != null) {
      _convertedResultReader.cancel();
    }
    if (_timeLineConverter != null) {
      _timeLineConverter.cancel();
    }
  }
  
  public String getMessage1() {
    return _message1;
  }
  
  public String getMessage2() {
    return _message2;
  }
  
  /**
   * ディレクトリ内のファイルを、ソートして返す。
   * @param dir
   * @return
   */
  private static File[] getSortedFileList(File dir) {
    File[] files = null;
    
    if (dir != null) {
      files = dir.listFiles();
      if (files != null) {
        for (int i = 0; i < files.length; i++) {
          for (int j = i + 1; j < files.length; j++) {
            String fileName1 = files[i].getName();
            String fileName2 = files[j].getName();
            if (fileName1.compareTo(fileName2) > 0) {
              File tempFile = files[i];
              files[i] = files[j];
              files[j] = tempFile;
            }
          }
        }
      }
    }
    
    return files;
  }
  
  public TrafficModel load(File dir) {
    boolean isSuccess = true;
    
    _isCanceled = false;
    _message1 = "Loading";
    _message2 = "";
    
    _convertedResultReader = null;
    
    TrafficModel model = null;
    
    //グローバル設定を読み込む
    TrafficParameters trafficParameters = new TrafficParameters();
    boolean isSuccessGlobalSettings = loadGlobalSettings(dir, trafficParameters);
    if (isSuccessGlobalSettings == false) {
      isSuccess = false;
    }
    
    if (isSuccess) {
      //#############################################
      //車種データ
      _message2 = "Load vehicle family data";
      boolean isSuccessVehicleFamily = loadVehicleFamily(dir, trafficParameters);
      isSuccess &= isSuccessVehicleFamily;
      
      //#############################################
      //道路データ
      model = new TrafficModel(trafficParameters);
      _message2 = "Load intersection data";
      boolean isSuccessIntersection = loadIntersection(dir, model);
      _message2 = "Load road data";
      boolean isSuccessRoad = loadRoad(dir, model);
      isSuccess &= isSuccessIntersection;
      isSuccess &= isSuccessRoad;
      
      //#############################################
      //車両形状、信号機形状
      _message2 = "Load vehicle shape data";
      boolean isSuccessVehicleShape = loadVehicleShape(trafficParameters);
      _message2 = "Load signal shape data";
      boolean isSuccessSignalShape = loadSignalShape(trafficParameters, model);
      isSuccess &= isSuccessVehicleShape;
      isSuccess &= isSuccessSignalShape;
      
      //#############################################
      //解析結果
      File fileResultTimeLine = new File(dir, TimeLineConverter.FILENAME_RESULT_OF_TIME_LINE);
      boolean toUseConvertedResult = fileResultTimeLine.exists();
      boolean isSuccessConvertedResult = false;
      boolean isSuccessVehicleResult = false;
      boolean isSuccessSignalResult = false;
      if (toUseConvertedResult) {
        //解析結果を1ファイルにコンバートしたものが存在する場合
        isSuccessConvertedResult = loadConvertedResult(dir, model);
        isSuccess &= isSuccessConvertedResult;
      } else {
        _message2 = "Load vehicle result data";
        isSuccessVehicleResult = loadVehicleResult(dir, model);
        _message2 = "Load signal result data";
        isSuccessSignalResult = loadSignalResult(dir, model);
        //boolean isSuccessMapPosition = loadMapPosition(dir, model);
        //boolean isSuccessNetwork = loadNetwork(dir, model);
        
        isSuccess &= isSuccessVehicleResult;
        isSuccess &= isSuccessSignalResult;
      }
      
      StringBuffer sb = new StringBuffer();
      sb.append("load status").append(Common.EOL);
      sb.append("# vehicle shape    : " + isSuccessVehicleShape).append(Common.EOL);
      sb.append("# signal shape     : " + isSuccessSignalShape).append(Common.EOL);
      sb.append("# vehicle family   : " + isSuccessVehicleFamily).append(Common.EOL);
      if (toUseConvertedResult) {
        sb.append("# converted result : " + isSuccessConvertedResult).append(Common.EOL);
      } else {
        sb.append("# vehicle result   : " + isSuccessVehicleResult).append(Common.EOL);
        sb.append("# signal result    : " + isSuccessSignalResult).append(Common.EOL);
      }
      //sb.append("# map              : " + isSuccessMapPosition).append(Common.EOL);
      //sb.append("# network          : " + isSuccessNetwork).append(Common.EOL);
      sb.append("# intersection     : " + isSuccessIntersection).append(Common.EOL);
      sb.append("# road             : " + isSuccessRoad).append(Common.EOL);
      sb.append("# total            : " + isSuccess);
      _logger.info(sb.toString());
      
      if (_isCanceled) {
        isSuccess = false;
      }
      
      if (isSuccess == false) {
        model = null;
      }
    }
    
    return model;
  }
  
  public boolean convert(File dir) {
    boolean isSucces = false;
    
    _message1 = "Converting";
    _message2 = "";
    
    _timeLineConverter = new TimeLineConverter(dir);
    isSucces = _timeLineConverter.convert();
    
    return isSucces;
  }
  
  private boolean loadGlobalSettings(File dir, TrafficParameters trafficParameters) {
    boolean isSuccess = true;
    
    //解析の設定ファイル
    String fileName = TrafficParameters.FILE_NAME_GV_INIT;
    File fileGlobalSettings = new File(dir, fileName);
    
    //AdvMatesViewerの設定ファイル
    File fileAdvMatesViewerSettings = new File("settings.txt");
    
    File[] files = { fileGlobalSettings, fileAdvMatesViewerSettings };
    
    for (int i = 0; i < files.length; i++) {
      //設定ファイルは、存在する時のみ読み込む
      if (files[i].isFile()) {
        _logger.info("### " + files[i].getAbsolutePath());
        isSuccess = false;
        Properties properties = new Properties();
        try {
          FileInputStream inputStrem = new FileInputStream(files[i]);
          properties.load(inputStrem);
          inputStrem.close();
          
          trafficParameters.setParameters(properties);
          isSuccess = true;
        } catch (Exception e) {
        }
        if (isSuccess == false) {
          _logger.info("Failed to read setting file : " + files[i].getAbsolutePath());
        }
      }
    }
    
    return isSuccess;
  }
  
  private boolean loadVehicleFamily(File dir, TrafficParameters trafficParameters) {
    boolean isSuccess = true;
    
    String fileName = trafficParameters.getFileNameOfVehicleFamily();
    File fileVehicleFamily = new File(dir, fileName);
    
    //デフォルトの車種設定をロードする。
    //既存の設定は消去される。
    VehicleFamily.loadDefault();
    
    //車種の設定ファイルは、存在しなくてもよい
    if (fileVehicleFamily.isFile()) {
      VehicleFamilyFileReader reader = new VehicleFamilyFileReader(fileVehicleFamily);
      reader.read();
      isSuccess = reader.isSuccess();
    }
    
    if (isSuccess == false) {
      _logger.info("Failed to read vehicle family file : " + fileVehicleFamily.getAbsolutePath());
    }
    
    return isSuccess;
  }
  
  private boolean loadVehicleShape(TrafficParameters trafficParameters) {
    boolean isSuccess = true;
    
    File dirShape = new File("shape");
    
    int nVehicleFamilies = VehicleFamily.getNumVehicleFamilies();
    
    for (int i = 0; i < nVehicleFamilies; i++) {
      VehicleFamily vehicleFamily = VehicleFamily.getVehicleFamilyByIndex(i);
      int vehicleFamilyID = vehicleFamily.getVehicleFamilyID();
      
      String fileNameVehicleShape = "vehicleShape_" + vehicleFamilyID + ".txt";
      File file = new File(dirShape, fileNameVehicleShape);
      
      boolean tmpSuccess = loadVehicleShapeSub(file, trafficParameters, vehicleFamily);
      
      if (tmpSuccess == false) {
        isSuccess = false;
      }
    }
    
    {
      VehicleFamily vehicleFamily = VehicleFamily.getDefaultVehicleFamily();
      String fileNameVehicleShape = "vehicleShape_default.txt";
      File file = new File(dirShape, fileNameVehicleShape);
      
      boolean tmpSuccess = loadVehicleShapeSub(file, trafficParameters, vehicleFamily);
      
      if (tmpSuccess == false) {
        isSuccess = false;
      }
    }
    
    return isSuccess;
  }
  
  private boolean loadVehicleShapeSub(File file, TrafficParameters trafficParameters, VehicleFamily vehicleFamily) {
    boolean isSuccess = true;
    
    VehicleShapeReader reader = new VehicleShapeReader(file, trafficParameters, vehicleFamily);
    reader.read();
    
    if (reader.isSuccess()) {
      float[][] shapeCoords = reader.getCoords();
      int[][] shapeConnectivity = reader.getConnectivity();
      float[][] shapeColors = reader.getColors();
      
      vehicleFamily.setData(shapeCoords, shapeConnectivity, shapeColors);
    } else {
      _logger.info("Error occured in reading file : " + file.getAbsolutePath());
      isSuccess = false;
    }
    
    return isSuccess;
  }
  
  private boolean loadSignalShape(TrafficParameters trafficParameters, TrafficModel model) {
    boolean isSuccess = true;
    
    File dirShape = new File("shape");
    
    String fileNameSignalShape = "signal.txt";
    String fileNameSignalSimpleShape = "signalSimple.txt";
    
    File fileSignal = new File(dirShape, fileNameSignalShape);
    File fileSignalSimple = new File(dirShape, fileNameSignalSimpleShape);
    
    //車両用のリーダーを借用する
    VehicleShapeReader readerSignal = new VehicleShapeReader(fileSignal, trafficParameters, null);
    readerSignal.read();
    boolean isSuccessSignal = readerSignal.isSuccess();
    
    VehicleShapeReader readerSignalSimple = new VehicleShapeReader(fileSignalSimple, trafficParameters, null);
    readerSignalSimple.read();
    boolean isSuccessSignalSimple = readerSignalSimple.isSuccess();
    
    if (isSuccessSignal) {
      float[][] shapeCoords = readerSignal.getCoords();
      int[][] shapeConnectivity = readerSignal.getConnectivity();
      float[][] shapeColors = readerSignal.getColors();
      
      float[][] shapeNormals = new float[shapeConnectivity.length][];
      for (int i = 0; i < shapeConnectivity.length; i++) {
        shapeNormals[i] = new float[shapeConnectivity[i].length];
        Common.computeNormalVector(shapeCoords[i], shapeConnectivity[i], shapeNormals[i]);
      }
      
      model.setSignalShapeData(shapeCoords, shapeConnectivity, shapeNormals, shapeColors);
    } else {
      _logger.info("Error occured in reading file : " + fileSignal.getAbsolutePath());
      isSuccess = false;
    }
    
    if (isSuccessSignalSimple) {
      float[][] shapeCoords = readerSignalSimple.getCoords();
      int[][] shapeConnectivity = readerSignalSimple.getConnectivity();
      float[][] shapeColors = readerSignalSimple.getColors();
      
      float[][] shapeNormals = new float[shapeConnectivity.length][];
      for (int i = 0; i < shapeConnectivity.length; i++) {
        shapeNormals[i] = new float[shapeConnectivity[i].length];
        Common.computeNormalVector(shapeCoords[i], shapeConnectivity[i], shapeNormals[i]);
      }
      
      model.setSignalSimpleShapeData(shapeCoords, shapeConnectivity, shapeNormals, shapeColors);
    } else {
      _logger.info("Error occured in reading file : " + fileSignal.getAbsolutePath());
      isSuccess = false;
    }
    
    if (isSuccessSignal && isSuccessSignalSimple) {
      isSuccess = true;
    }
    
    return isSuccess;
  }
  
  private boolean loadVehicleResult(File dir, TrafficModel model) {
    boolean isSuccess = true;
    
    _logger.info("Start reading vehicle result.");
    
    String dirNameResultVehicle = model.getTrafficParameters().getDirectoryNameOfResultVehicle();
    File dirVehicle = new File(dir, dirNameResultVehicle);
    //File dirResult = new File(dir, "result");
    //File dirTimeLine = new File(dirResult, "timeline");
    //File dirVehicle = new File(dirTimeLine, "vehicle");
    
    //File[] files1 = dirVehicle.listFiles();
    File[] files1 = getSortedFileList(dirVehicle);
    
    if (files1 == null) {
      isSuccess = false;
    } else {
      int countFiles = 0;
      for (int i = 0; i < files1.length; i++) {
        //File[] files2 = files1[i].listFiles();
        File[] files2 = getSortedFileList(files1[i]);
        if (files2 != null) {
          for (int j = 0; j < files2.length; j++) {
            //File[] files3 = files2[j].listFiles();
            File[] files3 = getSortedFileList(files2[j]);
            if (files3 != null) {
              for (int k = 0; k < files3.length; k++) {
                countFiles++;
              }
            }
          }
        }
      }
      
      int nFiles = countFiles;
      int[] nVehicles = new int[nFiles];
      int[][] vehicleID = new int[nFiles][];
      int[][] vehicleFamilyID = new int[nFiles][];
      float[][] vehiclePositionX = new float[nFiles][];
      float[][] vehiclePositionY = new float[nFiles][];
      float[][] vehiclePositionZ = new float[nFiles][];
      float[][] vehicleAngleXY = new float[nFiles][];
      float[][] vehicleAngleZ = new float[nFiles][];
      float[][] vehicleVelocity = new float[nFiles][];
      float[][] vehicleAcceleration = new float[nFiles][];
      int[][] vehicleIntersectionID = new int[nFiles][];
      int[][] vehicleRoadID = new int[nFiles][];
      
      boolean isFirst = true;
      float minx = 0;
      float miny = 0;
      float minz = 0;
      float maxx = 0;
      float maxy = 0;
      float maxz = 0;
      
      countFiles = 0;
      for (int i = 0; i < files1.length; i++) {
        //File[] files2 = files1[i].listFiles();
        File[] files2 = getSortedFileList(files1[i]);
        if (files2 == null) {
          isSuccess = false;
          break;
        } else {
          for (int j = 0; j < files2.length; j++) {
            //File[] files3 = files2[j].listFiles();
            File[] files3 = getSortedFileList(files2[j]);
            if (files3 == null) {
              isSuccess = false;
              break;
            } else {
              for (int k = 0; k < files3.length; k++) {
                if (_isCanceled) {
                  isSuccess = false;
                  break;
                }
                VehicleResultReader reader = new VehicleResultReader(files3[k]);
                reader.read();
                boolean tmpisSuccess = reader.isSuccess();
                if (tmpisSuccess) {
                  nVehicles[countFiles] = reader.getNumVehicles();
                  vehicleID[countFiles] = reader.getVehicleID();
                  vehicleFamilyID[countFiles] = reader.getVehicleFamilyID();
                  vehiclePositionX[countFiles] = reader.getVehiclePositionX();
                  vehiclePositionY[countFiles] = reader.getVehiclePositionY();
                  vehiclePositionZ[countFiles] = reader.getVehiclePositionZ();
                  vehicleAngleXY[countFiles] = reader.getVehicleAngleXY();
                  vehicleAngleZ[countFiles] = reader.getVehicleAngleZ();
                  vehicleVelocity[countFiles] = reader.getVehicleVelocity();
                  vehicleAcceleration[countFiles] = reader.getVehicleAcceleration();
                  vehicleIntersectionID[countFiles] = reader.getVehicleIntersectionID();
                  vehicleRoadID[countFiles] = reader.getVehicleRoadID();
                  
                  for (int m = 0; m < nVehicles[countFiles]; m++) {
                    float x = vehiclePositionX[countFiles][m];
                    float y = vehiclePositionY[countFiles][m];
                    float z = vehiclePositionZ[countFiles][m];
                    if (isFirst) {
                      minx = x;
                      miny = y;
                      minz = z;
                      maxx = x;
                      maxy = y;
                      maxz = z;
                      isFirst = false;
                    } else {
                      if (minx > x) minx = x;
                      if (miny > y) miny = y;
                      if (minz > z) minz = z;
                      if (maxx < x) maxx = x;
                      if (maxy < y) maxy = y;
                      if (maxz < z) maxz = z;
                    }
                  }
                  countFiles++;
                } else {
                  isSuccess = false;
                  break;
                }
              }
            }
            if (isSuccess == false) {
              break;
            }
          }
        }
        if (isSuccess == false) {
          break;
        }
      }
      
      if (isSuccess) {
        StringBuffer sb = new StringBuffer();
        sb.append("Range of coordinates of vehicles").append(Common.EOL);
        sb.append("# min=" + minx + ", " + miny + ", " + minz).append(Common.EOL);
        sb.append("# max=" + maxx + ", " + maxy + ", " + maxz);
        _logger.info(sb.toString());
        VehicleFamily[][] vehicleFamily = new VehicleFamily[vehicleFamilyID.length][];
        for (int i = 0; i < vehicleFamilyID.length; i++) {
          vehicleFamily[i] = new VehicleFamily[vehicleFamilyID[i].length];
          for (int j = 0; j < vehicleFamilyID[i].length; j++) {
            vehicleFamily[i][j] = VehicleFamily.getVehicleFamily(vehicleFamilyID[i][j]);
          }
        }
        model.setTrafficData(nFiles, nVehicles, vehicleID, vehicleFamilyID, vehicleFamily, vehiclePositionX, vehiclePositionY, vehiclePositionZ,
            vehicleAngleXY, vehicleAngleZ, vehicleVelocity, vehicleAcceleration, vehicleIntersectionID, vehicleRoadID);
      }
    }
    
    _logger.info("Finish reading vehicle result.");
    
    return isSuccess;
  }
  
  private boolean loadSignalResult(File dir, TrafficModel model) {
    boolean isSuccess = true;
    
    _logger.info("Start reading signal result.");
    
    String dirNameResultSignal = model.getTrafficParameters().getDirectoryNameOfResultSignal();
    File dirSignal = new File(dir, dirNameResultSignal);
    //File dirResult = new File(dir, "result");
    //File dirTimeLine = new File(dirResult, "timeline");
    //File dirVehicle = new File(dirTimeLine, "vehicle");
    
    //File[] files1 = dirSignal.listFiles();
    File[] files1 = getSortedFileList(dirSignal);
    
    if (files1 == null) {
      isSuccess = false;
    } else {
      int countFiles = 0;
      for (int i = 0; i < files1.length; i++) {
        //File[] files2 = files1[i].listFiles();
        File[] files2 = getSortedFileList(files1[i]);
        if (files2 != null) {
          for (int j = 0; j < files2.length; j++) {
            //File[] files3 = files2[j].listFiles();
            File[] files3 = getSortedFileList(files2[j]);
            if (files3 != null) {
              for (int k = 0; k < files3.length; k++) {
                countFiles++;
              }
            }
          }
        }
      }
      
      int nFiles = countFiles;
      int[] nSignals = new int[nFiles];
      int[][] intersectionIDStart = new int[nFiles][];
      int[][] intersectionIDEnd = new int[nFiles][];
      int[][] signalState = new int[nFiles][];
      
      countFiles = 0;
      for (int i = 0; i < files1.length; i++) {
        //File[] files2 = files1[i].listFiles();
        File[] files2 = getSortedFileList(files1[i]);
        if (files2 == null) {
          isSuccess = false;
          break;
        } else {
          for (int j = 0; j < files2.length; j++) {
            //File[] files3 = files2[j].listFiles();
            File[] files3 = getSortedFileList(files2[j]);
            if (files3 == null) {
              isSuccess = false;
              break;
            } else {
              for (int k = 0; k < files3.length; k++) {
                if (_isCanceled) {
                  isSuccess = false;
                  break;
                }
                SignalResultReader reader = new SignalResultReader(files3[k]);
                reader.read();
                boolean tmpisSuccess = reader.isSuccess();
                if (tmpisSuccess) {
                  nSignals[countFiles] = reader.getNumSignals();
                  intersectionIDStart[countFiles] = reader.getIntersectionIDSource();
                  intersectionIDEnd[countFiles] = reader.getIntersectionIDDestination();
                  signalState[countFiles] = reader.getSignalState();
                  
                  countFiles++;
                } else {
                  isSuccess = false;
                  break;
                }
              }
            }
            if (isSuccess == false) {
              break;
            }
          }
        }
        if (isSuccess == false) {
          break;
        }
      }
      
      if (isSuccess) {
        model.setSignalData(nFiles, nSignals, intersectionIDStart, intersectionIDEnd, signalState);
      }
    }
    
    _logger.info("Finish reading signal result.");
    
    return isSuccess;
  }
  
  private boolean loadConvertedResult(File dir, TrafficModel model) {
    boolean isSuccess = true;
    
    _convertedResultReader = new ConvertedResultReader(dir, model);
    isSuccess = _convertedResultReader.read();
    
    return isSuccess;
  }
  
  private boolean loadMapPosition(File dir, TrafficModel model) {
    boolean isSuccess = true;
    
    String fileNameMapPosition = model.getTrafficParameters().getFileNameOfMapPosition();
    File file = new File(dir, fileNameMapPosition);
    //File file = new File(dir, "mapPosition.txt");
    MapPositionFileReader reader = new MapPositionFileReader(file);
    reader.read();
    isSuccess = reader.isSuccess();
    
    if (isSuccess) {
      int nIntersections = reader.getNumIntersections();
      float[] intersectionPositionX = reader.getIntersectionPositionX();
      float[] intersectionPositionY = reader.getIntersectionPositionY();
      float[] intersectionPositionZ = reader.getIntersectionPositionZ();
      {
        String str = "nIntersections=" + nIntersections + Common.EOL;
        for (int i = 0; i < nIntersections; i++) {
          str += i + "th : " + intersectionPositionX[i] + ", " + intersectionPositionY[i] + ", " + intersectionPositionZ[i] + Common.EOL;
        }
        _logger.info(str);
      }
      //model.setMapPositionData(nIntersections, intersectionPositionX, intersectionPositionY, intersectionPositionZ);
    }
    
    return isSuccess;
  }
  
  private boolean loadNetwork(File dir, TrafficModel model) {
    boolean isSuccess = true;
    
    String fileNameNetwork = model.getTrafficParameters().getFileNameOfNetwork();
    File file = new File(dir, fileNameNetwork);
    //File file = new File(dir, "network.txt");
    NetworkFileReader reader = new NetworkFileReader(file);
    reader.read();
    isSuccess = reader.isSuccess();
    
    if (isSuccess) {
      int nRoads = reader.getNumRoads();
      int[] pointIDIn = reader.getPointIDIn();
      int[] pointIDOut = reader.getPointIDOut();
      int[] nLanesIn = reader.getNumLanesIn();
      int[] nLanesOut = reader.getNumLanesOut();
      {
        String str = "nRoads=" + nRoads + Common.EOL;
        for (int i = 0; i < nRoads; i++) {
          str += i + "th : " + pointIDIn[i] + ", " + pointIDOut[i] + " : " + nLanesIn[i] + ", " + nLanesOut[i] + Common.EOL;
        }
        _logger.info(str);
      }
      //model.setNetworkData(nRoads, pointIDIn, pointIDOut, nLanesIn, nLanesOut);
    }
    
    return isSuccess;
  }
  
  private boolean loadIntersection(File dir, TrafficModel model) {
    boolean isSuccess = true;
    
    String fileNameNodeShape = model.getTrafficParameters().getFileNameOfNodeShape();
    File file = new File(dir, fileNameNodeShape);
    //File dirResult = new File(dir, "result");
    //File file = new File(dirResult, "nodeShape.txt");
    NodeShapeFileReader reader = new NodeShapeFileReader(file);
    reader.read();
    isSuccess = reader.isSuccess();
    
    if (isSuccess) {
      int nIntersections = reader.getNumIntersections();
      int[] nShapePoints = reader.getNumShapePoints();
      int[] intersectionID = reader.getIntersectionID();
      String[] intersectionIDString = reader.getIntersectionIDString();
      float[] centerPositionX = reader.getCenterPositionX();
      float[] centerPositionY = reader.getCenterPositionY();
      float[] centerPositionZ = reader.getCenterPositionZ();
      float[][] shapePositionX = reader.getShapePositionX();
      float[][] shapePositionY = reader.getShapePositionY();
      float[][] shapePositionZ = reader.getShapePositionZ();
      model.setIntersectionData(nIntersections, nShapePoints, intersectionID, intersectionIDString, centerPositionX, centerPositionY, centerPositionZ,
          shapePositionX, shapePositionY, shapePositionZ);
    }
    
    return isSuccess;
  }
  
  private boolean loadRoad(File dir, TrafficModel model) {
    boolean isSuccess = true;
    
    String fileNameLinkShape = model.getTrafficParameters().getFileNameOfLinkShape();
    File fileLinkShape = new File(dir, fileNameLinkShape);
    //File dirResult = new File(dir, "result");
    //File file = new File(dirResult, "linkShape.txt");
    LinkShapeFileReader readerLinkShape = new LinkShapeFileReader(fileLinkShape);
    readerLinkShape.read();
    boolean isSuccessLinkShape = readerLinkShape.isSuccess();
    
    String fileNameNetwork = model.getTrafficParameters().getFileNameOfNetwork();
    File fileNetwork = new File(dir, fileNameNetwork);
    NetworkFileReader readerNetwork = new NetworkFileReader(fileNetwork);
    readerNetwork.read();
    boolean isSuccessNetwork = readerNetwork.isSuccess();
    
    if (isSuccessLinkShape && isSuccessNetwork) {
      isSuccess = true;
      int nRoads = readerLinkShape.getNumRoads();
      int[] nLanesRight = readerLinkShape.getNumLanesRight();
      int[] nLanesLeft = readerLinkShape.getNumLanesLeft();
      int[] nPointsCenter = readerLinkShape.getNumPointsCenter();
      int[] nPointsRight = readerLinkShape.getNumPointsRight();
      int[] nPointsLeft = readerLinkShape.getNumPointsLeft();
      int[] intersectionIDStart = readerLinkShape.getIntersectionIDStart();
      int[] intersectionIDEnd = readerLinkShape.getIntersectionIDEnd();
      boolean[] existSignalStart = readerLinkShape.getExistSignalStart();
      boolean[] existSignalEnd = readerLinkShape.getExistSignalEnd();
      float[][] positionCenterX = readerLinkShape.getPositionCenterX();
      float[][] positionCenterY = readerLinkShape.getPositionCenterY();
      float[][] positionCenterZ = readerLinkShape.getPositionCenterZ();
      float[][] positionRightX = readerLinkShape.getPositionRightX();
      float[][] positionRightY = readerLinkShape.getPositionRightY();
      float[][] positionRightZ = readerLinkShape.getPositionRightZ();
      float[][] positionLeftX = readerLinkShape.getPositionLeftX();
      float[][] positionLeftY = readerLinkShape.getPositionLeftY();
      float[][] positionLeftZ = readerLinkShape.getPositionLeftZ();
      
      //network.txtから読み込んだ設定である。
      //同じ2つの交差点を結ぶ道路でも、向きによって分けて格納している。
      int nSingleRoad = readerNetwork.getNumRoads();
      int[] singleRoadPointIDIn = readerNetwork.getPointIDIn();
      int[] singleRoadPointIDOut = readerNetwork.getPointIDOut();
      int[] singleRoadPNumLanesIn = readerNetwork.getNumLanesIn();
      int[] singleRoadPNumLanesOut = readerNetwork.getNumLanesOut();
      
      model.setRoadData(nRoads, nLanesRight, nLanesLeft, nPointsCenter, nPointsRight, nPointsLeft, intersectionIDStart, intersectionIDEnd,
          existSignalStart, existSignalEnd, positionCenterX, positionCenterY, positionCenterZ, positionRightX, positionRightY, positionRightZ,
          positionLeftX, positionLeftY, positionLeftZ, nSingleRoad, singleRoadPointIDIn, singleRoadPointIDOut, singleRoadPNumLanesIn,
          singleRoadPNumLanesOut);
    }
    
    return isSuccess;
  }
}
