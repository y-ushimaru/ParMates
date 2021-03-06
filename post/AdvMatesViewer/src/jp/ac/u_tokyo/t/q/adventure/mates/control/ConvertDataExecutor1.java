package jp.ac.u_tokyo.t.q.adventure.mates.control;

import java.io.File;

public class ConvertDataExecutor1 implements Runnable {
  private MainControl _mainControl;
  
  private File _dir;
  
  private Thread _thread;
  
  public ConvertDataExecutor1(MainControl mainControl, File dir) {
    _mainControl = mainControl;
    _dir = dir;
    
    _thread = new Thread(this);
    _thread.start();
  }
  
  public void run() {
    _mainControl.convertDataImpl(_dir);
  }
  
  public boolean isAlive() {
    if (_thread != null) {
      return _thread.isAlive();
    }
    return false;
  }
}
