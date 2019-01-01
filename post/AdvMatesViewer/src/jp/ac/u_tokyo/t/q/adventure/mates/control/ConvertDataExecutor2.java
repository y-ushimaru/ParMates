package jp.ac.u_tokyo.t.q.adventure.mates.control;

import java.io.File;

public class ConvertDataExecutor2 implements Runnable {
  private IOControl _ioControl;
  
  private File _dir;
  
  private boolean _isSuccess;
  
  private Thread _thread;
  
  public ConvertDataExecutor2(IOControl ioControl, File dir) {
    _ioControl = ioControl;
    _dir = dir;
    
    _isSuccess = false;
    
    _thread = new Thread(this);
    _thread.start();
  }
  
  public void run() {
    _isSuccess = _ioControl.convert(_dir);
  }
  
  public boolean isSuccess() {
    return _isSuccess;
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
