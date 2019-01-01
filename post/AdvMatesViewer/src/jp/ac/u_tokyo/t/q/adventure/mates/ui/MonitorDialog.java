package jp.ac.u_tokyo.t.q.adventure.mates.ui;

public interface MonitorDialog {
  public boolean isCanceled();
  
  public void setMessage1(String message);
  
  public void setMessage2(String message);
  
  public void setProgress(double progress);
  
  public void setVisible(boolean isVisible);
}
