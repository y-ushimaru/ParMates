package jp.ac.u_tokyo.t.q.adventure.mates.control;

import java.io.File;

import jp.ac.u_tokyo.t.q.adventure.mates.model.TrafficModel;

public class LoadDataExecutor2 implements Runnable {
  private IOControl _ioControl;
  
  private File _dir;
  
  private TrafficModel _model;
  
  private Thread _thread;
  
  public LoadDataExecutor2(IOControl ioControl, File dir) {
    _ioControl = ioControl;
    _dir = dir;
    
    _model = null;
    _thread = new Thread(this);
    _thread.start();
  }
  
  public void run() {
    _model = _ioControl.load(_dir);
  }
  
  public TrafficModel getModel() {
    return _model;
  }
  
  //public float getProgress() {
  //  _ioControl.getProgress();
  //}
  
  public String getMessage1() {
    return _ioControl.getMessage1();
  }
  
  public String getMessage2() {
    return _ioControl.getMessage2();
  }
  
  public void cancel() {
    _ioControl.cancel();
  }
  
  public boolean isAlive() {
    if (_thread != null) {
      return _thread.isAlive();
    }
    return false;
  }
}
