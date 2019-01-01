package jp.ac.u_tokyo.t.q.adventure.mates.io;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.model.TrafficParameters;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

public class TimeLineConverter {
  //@formatter:off
  public static final String FILENAME_RESULT_OF_TIME_LINE  = "resultTimeLine.rtl";
  
  public static final String LABEL_NUM_VEHICLE_FILES       = "nVehicleFiles";
  public static final String LABEL_NUM_VEHICLES            = "nVehicles";
  public static final String LABEL_NUM_SIGNAL_FILES        = "nSignalFiles";
  public static final String LABEL_NUM_SIGNALS             = "nSignals";
  public static final String LABEL_STEP_NUMBER             = "step";
  public static final String LABEL_FILE_NAME               = "file";
  
  /**
   * ファイル保存用のStringBufferの長さがこの値を超えたらファイルに書き込む
   * 
   */
  private static final int   THRESHOLD_TO_SAVE             = 1000000;
  //@formatter:on
  
  /**
   * ログハンドリングクラス
   */
  private Logger _logger;
  
  private boolean _isCanceled;
  
  private File _dir;
  
  private StringBuffer _buffer;
  
  public TimeLineConverter(File dir) {
    _logger = Logger.getLogger(TimeLineConverter.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    
    _isCanceled = false;
    
    _dir = dir;
    
    _buffer = new StringBuffer();
  }
  
  public void cancel() {
    _isCanceled = true;
  }
  
  public boolean convert() {
    boolean isSucces = false;
    
    _isCanceled = false;
    
    _buffer.setLength(0);
    
    File fileOutput = new File(_dir, FILENAME_RESULT_OF_TIME_LINE);
    BufferedWriter bw = null;
    
    try {
      bw = new BufferedWriter(new FileWriter(fileOutput));
      TrafficParameters trafficParameters = new TrafficParameters();
      boolean isSuccesVehilce = loadVehicleResult(_dir, bw, trafficParameters);
      boolean isSuccesSignal = loadSignalResult(_dir, bw, trafficParameters);
      
      if (isSuccesVehilce && isSuccesSignal) {
        isSucces = true;
      }
      if (_isCanceled) {
        isSucces = false;
      }
    } catch (Exception e) {
    }
    
    if (bw != null) {
      try {
        bw.close();
      } catch (Exception e) {
      }
    }
    
    if (_isCanceled) {
      fileOutput.delete();
    }
    
    return isSucces;
  }
  
  private boolean loadVehicleResult(File dir, BufferedWriter bw, TrafficParameters trafficParameters)
      throws IOException {
    boolean isSuccess = true;
    
    _logger.info("Start converting vehicle result.");
    
    String dirNameResultVehicle = trafficParameters.getDirectoryNameOfResultVehicle();
    File dirVehicle = new File(dir, dirNameResultVehicle);
    //File dirResult = new File(dir, "result");
    //File dirTimeLine = new File(dirResult, "timeline");
    //File dirVehicle = new File(dirTimeLine, "vehicle");
    
    File[] files1 = dirVehicle.listFiles();
    
    if (files1 == null) {
      isSuccess = false;
    } else {
      int countFiles = 0;
      for (int i = 0; i < files1.length; i++) {
        File[] files2 = files1[i].listFiles();
        if (files2 != null) {
          for (int j = 0; j < files2.length; j++) {
            File[] files3 = files2[j].listFiles();
            if (files3 != null) {
              for (int k = 0; k < files3.length; k++) {
                countFiles++;
              }
            }
          }
        }
      }
      
      int nFiles = countFiles;
      append(_buffer, bw, LABEL_NUM_VEHICLE_FILES + Common.EQUAL + nFiles + Common.EOL);
      
      countFiles = 0;
      for (int i = 0; i < files1.length; i++) {
        String name1 = files1[i].getName();
        File[] files2 = files1[i].listFiles();
        if (files2 == null) {
          isSuccess = false;
          break;
        } else {
          for (int j = 0; j < files2.length; j++) {
            String name2 = files2[j].getName();
            File[] files3 = files2[j].listFiles();
            if (files3 == null) {
              isSuccess = false;
              break;
            } else {
              for (int k = 0; k < files3.length; k++) {
                if (_isCanceled) {
                  isSuccess = false;
                  break;
                }
                String name3 = files3[k].getName();
                VehicleResultReader reader = new VehicleResultReader(files3[k]);
                reader.read();
                boolean tmpisSuccess = reader.isSuccess();
                if (tmpisSuccess) {
                  int nVehicles = reader.getNumVehicles();
                  int[] vehicleID = reader.getVehicleID();
                  int[] vehicleFamilyID = reader.getVehicleFamilyID();
                  float[] vehiclePositionX = reader.getVehiclePositionX();
                  float[] vehiclePositionY = reader.getVehiclePositionY();
                  float[] vehiclePositionZ = reader.getVehiclePositionZ();
                  float[] vehicleAngleXY = reader.getVehicleAngleXY();
                  float[] vehicleAngleZ = reader.getVehicleAngleZ();
                  float[] vehicleVelocity = reader.getVehicleVelocity();
                  float[] vehicleAcceleration = reader.getVehicleAcceleration();
                  int[] vehicleIntersectionID = reader.getVehicleIntersectionID();
                  int[] vehicleRoadID = reader.getVehicleRoadID();
                  
                  append(_buffer, bw, Common.EOL);
                  append(_buffer, bw, LABEL_STEP_NUMBER + Common.EQUAL + countFiles + Common.EOL);
                  append(_buffer, bw, LABEL_FILE_NAME + Common.EQUAL + name1 + "_" + name2 + "_" + name3 + Common.EOL);
                  append(_buffer, bw, LABEL_NUM_VEHICLES + Common.EQUAL + nVehicles + Common.EOL);
                  
                  for (int m = 0; m < nVehicles; m++) {
                    append(_buffer, bw, vehicleID[m] + Common.COMMA);
                    append(_buffer, bw, vehicleFamilyID[m] + Common.COMMA);
                    append(_buffer, bw, vehiclePositionX[m] + Common.COMMA);
                    append(_buffer, bw, vehiclePositionY[m] + Common.COMMA);
                    append(_buffer, bw, vehiclePositionZ[m] + Common.COMMA);
                    append(_buffer, bw, vehicleAngleXY[m] + Common.COMMA);
                    append(_buffer, bw, vehicleAngleZ[m] + Common.COMMA);
                    append(_buffer, bw, vehicleVelocity[m] + Common.COMMA);
                    append(_buffer, bw, vehicleAcceleration[m] + Common.COMMA);
                    append(_buffer, bw, vehicleIntersectionID[m] + Common.COMMA);
                    append(_buffer, bw, vehicleRoadID[m] + Common.EOL);
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
      append(_buffer, bw, Common.EOL);
      
      if (isSuccess) {
        if (_buffer.length() > 0) {
          bw.write(_buffer.toString());
        }
      }
    }
    
    _logger.info("Finish converting vehicle result.");
    
    return isSuccess;
  }
  
  private boolean loadSignalResult(File dir, BufferedWriter bw, TrafficParameters trafficParameters) throws IOException {
    boolean isSuccess = true;
    
    _logger.info("Start converting signal result.");
    
    String dirNameResultSignal = trafficParameters.getDirectoryNameOfResultSignal();
    File dirSignal = new File(dir, dirNameResultSignal);
    //File dirResult = new File(dir, "result");
    //File dirTimeLine = new File(dirResult, "timeline");
    //File dirVehicle = new File(dirTimeLine, "vehicle");
    
    File[] files1 = dirSignal.listFiles();
    
    if (files1 == null) {
      isSuccess = false;
    } else {
      int countFiles = 0;
      for (int i = 0; i < files1.length; i++) {
        File[] files2 = files1[i].listFiles();
        if (files2 != null) {
          for (int j = 0; j < files2.length; j++) {
            File[] files3 = files2[j].listFiles();
            if (files3 != null) {
              for (int k = 0; k < files3.length; k++) {
                countFiles++;
              }
            }
          }
        }
      }
      
      int nFiles = countFiles;
      append(_buffer, bw, LABEL_NUM_SIGNAL_FILES + Common.EQUAL + nFiles + Common.EOL);
      
      countFiles = 0;
      for (int i = 0; i < files1.length; i++) {
        String name1 = files1[i].getName();
        File[] files2 = files1[i].listFiles();
        if (files2 == null) {
          isSuccess = false;
          break;
        } else {
          for (int j = 0; j < files2.length; j++) {
            String name2 = files2[j].getName();
            File[] files3 = files2[j].listFiles();
            if (files3 == null) {
              isSuccess = false;
              break;
            } else {
              for (int k = 0; k < files3.length; k++) {
                if (_isCanceled) {
                  isSuccess = false;
                  break;
                }
                String name3 = files3[k].getName();
                SignalResultReader reader = new SignalResultReader(files3[k]);
                reader.read();
                boolean tmpisSuccess = reader.isSuccess();
                if (tmpisSuccess) {
                  int nSignales = reader.getNumSignals();
                  int[] intersectionIDStart = reader.getIntersectionIDSource();
                  int[] intersectionIDEnd = reader.getIntersectionIDDestination();
                  int[] signalState = reader.getSignalState();
                  
                  append(_buffer, bw, Common.EOL);
                  append(_buffer, bw, LABEL_STEP_NUMBER + Common.EQUAL + countFiles + Common.EOL);
                  append(_buffer, bw, LABEL_FILE_NAME + Common.EQUAL + name1 + "_" + name2 + "_" + name3 + Common.EOL);
                  append(_buffer, bw, LABEL_NUM_SIGNALS + Common.EQUAL + nSignales + Common.EOL);
                  
                  for (int m = 0; m < nSignales; m++) {
                    append(_buffer, bw, intersectionIDStart[m] + Common.COMMA);
                    append(_buffer, bw, intersectionIDEnd[m] + Common.COMMA);
                    append(_buffer, bw, signalState[m] + Common.EOL);
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
      append(_buffer, bw, Common.EOL);
      
      if (isSuccess) {
        if (_buffer.length() > 0) {
          bw.write(_buffer.toString());
        }
      }
    }
    
    _logger.info("Finish converting signal result.");
    
    return isSuccess;
  }
  
  private static void append(StringBuffer buffer, BufferedWriter writer, String str) throws IOException {
    buffer.append(str);
    if (buffer.length() >= THRESHOLD_TO_SAVE) {
      writer.write(buffer.toString());
      buffer.setLength(0);
    }
  }
}
