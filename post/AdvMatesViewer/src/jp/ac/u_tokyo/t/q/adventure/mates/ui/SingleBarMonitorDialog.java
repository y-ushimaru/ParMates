package jp.ac.u_tokyo.t.q.adventure.mates.ui;

import java.awt.Container;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.logging.Logger;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JProgressBar;

import jp.ac.u_tokyo.t.q.adventure.mates.resource.ResourceManager;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;
import net.miginfocom.swing.MigLayout;

public class SingleBarMonitorDialog extends JDialog implements ActionListener, MonitorDialog {
  private static final String LABEL_PRE = "dialog.singleBarMonitor.";
  
  private static final int PROGRESSBAR_MIN = 0;
  private static final int PROGRESSBAR_MAX = 100;
  private static final float PROGRESS_MIN = 0.0f;
  private static final float PROGRESS_MAX = 1.0f;
  
  /**
   * ログハンドリングクラス
   */
  private Logger _logger;
  /**
   * リソースマネージャ
   */
  private ResourceManager _rm;
  
  private boolean _isCanceled = false;
  
  private JProgressBar _progressBar;
  
  private JButton _btCancel;
  
  private JLabel _lbMessage1;
  
  private JLabel _lbMessage2;
  
  public SingleBarMonitorDialog(JFrame parent) {
    super(parent, false);
    _logger = Logger.getLogger(SingleBarMonitorDialog.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    _rm = ResourceManager.getInstance();
    setDefaultCloseOperation(JDialog.DO_NOTHING_ON_CLOSE);
    setResizable(false);
    buildUI();
  }
  
  private void buildUI() {
    setTitle(_rm.getString(LABEL_PRE + "title"));
    
    _lbMessage1 = new JLabel(" ");
    _lbMessage2 = new JLabel(" ");
    _progressBar = new JProgressBar(PROGRESSBAR_MIN, PROGRESSBAR_MAX);
    _progressBar.setStringPainted(true);
    _btCancel = new JButton(_rm.getString("button.cancel"));
    _btCancel.addActionListener(this);
    
    Container cp = getContentPane();
    cp.setLayout(new MigLayout("wrap 1", "[200:200:200]", ""));
    
    cp.add(_lbMessage1, "left");
    cp.add(_lbMessage2, "left");
    cp.add(_progressBar, "left, grow");
    cp.add(_btCancel, "center");
    pack();
  }
  
  /**
   * キャンセル状態をリセットする。
   */
  public void init() {
    _isCanceled = false;
  }
  
  public void actionPerformed(ActionEvent e) {
    Object source = e.getSource();
    if (source == _btCancel) {
      _isCanceled = true;
    }
  }
  
  public boolean isCanceled() {
    return _isCanceled;
  }
  
  public void setVisible(boolean isVisible) {
    super.setVisible(isVisible);
  }
  
  public void setIndeterminate(boolean isIndeterminate) {
    _progressBar.setIndeterminate(isIndeterminate);
  }
  
  public void setMessage1(String message) {
    _lbMessage1.setText(message);
  }
  
  public void setMessage2(String message) {
    _lbMessage2.setText(message);
  }
  
  public void setProgress(double progress) {
    int tmpProgress = 0;
    if (progress < PROGRESS_MIN) {
      tmpProgress = PROGRESSBAR_MIN;
    } else if (progress > PROGRESS_MAX) {
      tmpProgress = PROGRESSBAR_MAX;
    } else {
      double tmpRatio = (progress - PROGRESS_MIN) / (PROGRESS_MAX - PROGRESS_MIN);
      tmpProgress = (int) (PROGRESSBAR_MIN + tmpRatio * (PROGRESSBAR_MAX - PROGRESSBAR_MIN));
    }
    _progressBar.setValue(tmpProgress);
    _progressBar.setString(tmpProgress + "%");
  }
}
