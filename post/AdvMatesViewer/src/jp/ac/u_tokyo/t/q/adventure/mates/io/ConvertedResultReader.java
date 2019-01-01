package jp.ac.u_tokyo.t.q.adventure.mates.io;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.model.TrafficModel;
import jp.ac.u_tokyo.t.q.adventure.mates.model.VehicleFamily;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

public class ConvertedResultReader {
  //@formatter:off
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
  
  private TrafficModel _model;
  
  public ConvertedResultReader(File dir, TrafficModel model) {
    _logger = Logger.getLogger(TimeLineConverter.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    
    _isCanceled = false;
    
    _dir = dir;
    _model = model;
  }
  
  public void cancel() {
    _isCanceled = true;
  }
  
  public boolean read() {
    boolean isSuccess = true;
    
    _isCanceled = false;
    
    _logger.info("Start reading converted result.");
    
    File fileResultTimeLine = new File(_dir, TimeLineConverter.FILENAME_RESULT_OF_TIME_LINE);
    BufferedReader br = null;
    String line = null;
    String[] lineSplit = null;
    int lineCount = 0;
    try {
      br = new BufferedReader(new FileReader(fileResultTimeLine));
      lineCount = 0;
      
      while (true) {
        line = br.readLine();
        if (line == null) break;
        lineCount++;
        
        //###################################################################
        if (line.startsWith(TimeLineConverter.LABEL_NUM_VEHICLE_FILES)) {
          lineSplit = line.split(Common.EQUAL);
          int nVehicleFiles = Integer.parseInt(lineSplit[1].trim());
          int[] nVehicles = new int[nVehicleFiles];
          int[][] vehicleID = new int[nVehicleFiles][];
          int[][] vehicleFamilyID = new int[nVehicleFiles][];
          float[][] vehiclePositionX = new float[nVehicleFiles][];
          float[][] vehiclePositionY = new float[nVehicleFiles][];
          float[][] vehiclePositionZ = new float[nVehicleFiles][];
          float[][] vehicleAngleXY = new float[nVehicleFiles][];
          float[][] vehicleAngleZ = new float[nVehicleFiles][];
          float[][] vehicleVelocity = new float[nVehicleFiles][];
          float[][] vehicleAcceleration = new float[nVehicleFiles][];
          int[][] vehicleIntersectionID = new int[nVehicleFiles][];
          int[][] vehicleRoadID = new int[nVehicleFiles][];
          
          boolean isFirst = true;
          float minx = 0;
          float miny = 0;
          float minz = 0;
          float maxx = 0;
          float maxy = 0;
          float maxz = 0;
          
          for (int i = 0; i < nVehicleFiles; i++) {
            if (_isCanceled) {
              isSuccess = false;
              break;
            }
            while (true) {
              line = br.readLine();
              if (line == null) break;
              lineCount++;
              if (line.startsWith(TimeLineConverter.LABEL_NUM_VEHICLES)) break;
            }
            lineSplit = line.split(Common.EQUAL);
            
            nVehicles[i] = Integer.parseInt(lineSplit[1].trim());
            vehicleID[i] = new int[nVehicles[i]];
            vehicleFamilyID[i] = new int[nVehicles[i]];
            vehiclePositionX[i] = new float[nVehicles[i]];
            vehiclePositionY[i] = new float[nVehicles[i]];
            vehiclePositionZ[i] = new float[nVehicles[i]];
            vehicleAngleXY[i] = new float[nVehicles[i]];
            vehicleAngleZ[i] = new float[nVehicles[i]];
            vehicleVelocity[i] = new float[nVehicles[i]];
            vehicleAcceleration[i] = new float[nVehicles[i]];
            vehicleIntersectionID[i] = new int[nVehicles[i]];
            vehicleRoadID[i] = new int[nVehicles[i]];
            
            for (int j = 0; j < nVehicles[i]; j++) {
              if (_isCanceled) {
                isSuccess = false;
                break;
              }
              line = br.readLine();
              lineCount++;
              lineSplit = line.split(Common.COMMA);
              vehicleID[i][j] = Integer.parseInt(lineSplit[0].trim());
              vehicleFamilyID[i][j] = Integer.parseInt(lineSplit[1].trim());
              vehiclePositionX[i][j] = Float.parseFloat(lineSplit[2].trim());
              vehiclePositionY[i][j] = Float.parseFloat(lineSplit[3].trim());
              vehiclePositionZ[i][j] = Float.parseFloat(lineSplit[4].trim());
              vehicleAngleXY[i][j] = Float.parseFloat(lineSplit[5].trim());
              vehicleAngleZ[i][j] = Float.parseFloat(lineSplit[6].trim());
              vehicleVelocity[i][j] = Float.parseFloat(lineSplit[7].trim());
              vehicleAcceleration[i][j] = Float.parseFloat(lineSplit[8].trim());
              vehicleIntersectionID[i][j] = Integer.parseInt(lineSplit[9].trim());
              vehicleRoadID[i][j] = Integer.parseInt(lineSplit[10].trim());
              
              float x = vehiclePositionX[i][j];
              float y = vehiclePositionY[i][j];
              float z = vehiclePositionZ[i][j];
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
            _model.setTrafficData(nVehicleFiles, nVehicles, vehicleID, vehicleFamilyID, vehicleFamily,
                vehiclePositionX, vehiclePositionY, vehiclePositionZ, vehicleAngleXY, vehicleAngleZ, vehicleVelocity,
                vehicleAcceleration, vehicleIntersectionID, vehicleRoadID);
          }
        }
        if (isSuccess == false) {
          break;
        }
        
        //###################################################################
        if (line.startsWith(TimeLineConverter.LABEL_NUM_SIGNAL_FILES)) {
          lineSplit = line.split(Common.EQUAL);
          int nSignalFiles = Integer.parseInt(lineSplit[1].trim());
          int[] nSignals = new int[nSignalFiles];
          int[][] intersectionIDStart = new int[nSignalFiles][];
          int[][] intersectionIDEnd = new int[nSignalFiles][];
          int[][] signalState = new int[nSignalFiles][];
          
          for (int i = 0; i < nSignalFiles; i++) {
            if (_isCanceled) {
              isSuccess = false;
              break;
            }
            while (true) {
              line = br.readLine();
              if (line == null) break;
              lineCount++;
              if (line.startsWith(TimeLineConverter.LABEL_NUM_SIGNALS)) break;
            }
            lineSplit = line.split(Common.EQUAL);
            
            nSignals[i] = Integer.parseInt(lineSplit[1].trim());
            intersectionIDStart[i] = new int[nSignals[i]];
            intersectionIDEnd[i] = new int[nSignals[i]];
            signalState[i] = new int[nSignals[i]];
            
            for (int j = 0; j < nSignals[i]; j++) {
              if (_isCanceled) {
                isSuccess = false;
                break;
              }
              line = br.readLine();
              lineCount++;
              lineSplit = line.split(Common.COMMA);
              intersectionIDStart[i][j] = Integer.parseInt(lineSplit[0].trim());
              intersectionIDEnd[i][j] = Integer.parseInt(lineSplit[1].trim());
              signalState[i][j] = Integer.parseInt(lineSplit[2].trim());
            }
          }
          if (isSuccess) {
            _model.setSignalData(nSignalFiles, nSignals, intersectionIDStart, intersectionIDEnd, signalState);
          }
        }
        
        if (isSuccess == false) {
          break;
        }
      }
    } catch (Exception e) {
      _logger.info("Error occured at " + lineCount + "th line of " + fileResultTimeLine.getAbsolutePath());
      e.printStackTrace();
      isSuccess = false;
    }
    if (br != null) {
      try {
        br.close();
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
    
    _logger.info("Finish reading converted result.");
    
    return isSuccess;
  }
}
