package jp.ac.u_tokyo.t.q.adventure.mates.ui.render;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.awt.image.BufferedImage;
import java.util.logging.Logger;

import javax.swing.JPanel;
import javax.swing.SwingUtilities;

import jp.ac.u_tokyo.t.q.adventure.mates.control.View2DEventListener;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * 2D描画用のクラスである。
 * 各メソッドは以下の順番で呼ぶ。
 * 
 * 初期化時
 * setSize()
 * setScale()
 * setRatioCoordToPixel()
 * 
 * 描画時
 * clear()
 * setColor()
 * drawXXX()
 * ...
 * switchImage()
 */
public class Drawable2DPanel extends JPanel implements ComponentListener, MouseListener, MouseMotionListener, MouseWheelListener, KeyListener {
  //@formatter:off
  /**
   * マウス操作の状態を示す。操作されてない状態。
   */
  private static final int MODE_NORMAL                              = 100;
  /**
   * マウス操作の状態を示す。移動操作中
   */
  private static final int MODE_TRANSLATE                           = 110;
  /**
   * マウス操作の状態を示す。倍率変更中
   */
  private static final int MODE_SCALE                               = 120;
  
  /**
   * マウス操作の感度。移動操作用。1.0fだと、マウス操作に3D表示がほぼ追随する
   */
  private static final float MOUSE_SENSITIVITY_TRANSLATE            = 1.0f;
  /**
   * マウス操作の感度。倍率変更用
   */
  private static final float MOUSE_SENSITIVITY_SCALE                = 0.01f;
  /**
   * マウス操作の感度。マウスホイールでの倍率変更用
   */
  private static final float MOUSE_SENSITIVITY_SCALE_BY_WHEEL       = 0.01f;
  
  private static final float[] DEFAULT_COLOR_CLEAR         = { 1.0f, 1.0f, 1.0f, 1.0f };
  private static final float[] DEFAULT_COLOR               = { 0.0f, 0.0f, 0.0f, 1.0f };
  
  private static final float DEFAULT_RADIUS                = 1.0f;
  private static final float DEFAULT_SCALE                 = 1.0f;
  //@formatter:on
  
  protected Logger _logger;
  /**
   * GUI関連のイベントを処理するクラス。
   * MainControlクラスで実装している。
   */
  private View2DEventListener _eventListener;
  
  /**
   * 現在の描画用の色である。
   */
  private Color _colorCurrent;
  /**
   * 描画領域全体をクリアするときの色である。
   */
  private Color _colorClear;
  /**
   * 画面の縦幅に相当する座標値の大きさである。
   */
  private float _radius;
  /**
   * 座標値の1が、画面上のいくつのピクセルに対応するかの比率を表す。
   */
  private float _ratioCoordToPixel;
  /**
   * 表示の倍率である。
   */
  private float _scale;
  /**
   * この座標値の点を画面の中心に描画する。
   */
  private float[] _center;
  /**
   * 初期状態での中心の座標値である。
   */
  private float[] _centerInitial;
  /**
   * 座標値を、画面上のピクセル位置に変換する際に用いるバッファーである。
   */
  private int[] _bufferPositionOnScreen;
  /**
   * 描画用のバッファーである。
   */
  private int[] _bufferX;
  /**
   * 描画用のバッファーである。
   */
  private int[] _bufferY;
  /**
   * 画面の幅である。
   */
  private int _width;
  /**
   * 画面の高さである。
   */
  private int _height;
  /**
   * ダブルバッファリングに用いるBufferedImageである。
   */
  private BufferedImage[] _image;
  /**
   * _imageIndexは、描画対象のスクリーンの番号である。
   * 1-_imageIndexは、画面表示するスクリーンの番号である。
   */
  private int _imageIndex;
  
  /**
   * マウスのX座標
   */
  private int _mouseX;
  /**
   * マウスのY座標
   */
  private int _mouseY;
  /**
   * マウス操作でのモード。
   */
  private int _mode;
  /**
   * 押下されたキーのコードである。
   */
  private int _pressedKey;
  
  public Drawable2DPanel(View2DEventListener eventListener) {
    super();
    _logger = Logger.getLogger(Drawable2DPanel.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    
    _eventListener = eventListener;
    
    setClearColor(DEFAULT_COLOR_CLEAR);
    setColor(DEFAULT_COLOR);
    
    setRadius(DEFAULT_RADIUS);
    setScale(DEFAULT_SCALE);
    
    _center = new float[3];
    _centerInitial = new float[3];
    _bufferPositionOnScreen = new int[2];
    _bufferX = new int[3];
    _bufferY = new int[3];
    _image = new BufferedImage[2];
    _imageIndex = 0;
    
    addComponentListener(this);
    addMouseListener(this);
    addMouseMotionListener(this);
    addMouseWheelListener(this);
    //キーイベントに反応できるように、View2D側でフォーカスの面倒を見る。
    addKeyListener(this);
  }
  
  public void paintComponent(Graphics g) {
    Image img = _image[1 - _imageIndex];
    if (img != null) {
      g.drawImage(img, 0, 0, this);
    }
  }
  
  public void componentHidden(ComponentEvent e) {
  }
  
  public void componentMoved(ComponentEvent e) {
  }
  
  public void componentResized(ComponentEvent e) {
    resizeImage();
  }
  
  public void componentShown(ComponentEvent e) {
  }
  
  public void mouseClicked(MouseEvent e) {
  }
  
  public void mousePressed(MouseEvent e) {
    _mouseX = e.getX();
    _mouseY = e.getY();
    
    if (SwingUtilities.isLeftMouseButton(e)) {
      if (_pressedKey == KeyEvent.VK_C) {
      } else if (_pressedKey == KeyEvent.VK_S) {
        _mode = MODE_SCALE;
      } else {
        _mode = MODE_TRANSLATE;
      }
    } else if (SwingUtilities.isRightMouseButton(e)) {
      if (e.isShiftDown()) {
        //_mode = MODE_MOVE_CENTER_OF_ROTATION;
      } else {
        _mode = MODE_TRANSLATE;
      }
    } else {
      if (e.isShiftDown()) {
      } else {
        _mode = MODE_SCALE;
      }
    }
  }
  
  public void mouseReleased(MouseEvent e) {
    _mode = MODE_NORMAL;
  }
  
  public void mouseEntered(MouseEvent e) {
  }
  
  public void mouseExited(MouseEvent e) {
  }
  
  public void mouseDragged(MouseEvent e) {
    int tmpX = e.getX();
    int tmpY = e.getY();
    //マウスの横方向移動量。画面右が正
    int moveX = tmpX - _mouseX;
    //マウスの縦方向移動量。画面上が正
    int moveY = _mouseY - tmpY;
    
    if (_mode == MODE_SCALE) {
      scale(moveX, moveY);
      _eventListener.reDraw();
    } else if (_mode == MODE_TRANSLATE) {
      translate(moveX, moveY);
      _eventListener.reDraw();
    }
    _mouseX = tmpX;
    _mouseY = tmpY;
  }
  
  public void mouseMoved(MouseEvent e) {
  }
  
  public void mouseWheelMoved(MouseWheelEvent e) {
    //TODO ホイール回転の向きと倍率の変更量の調整
    if (_mode == MODE_NORMAL) {
      int tmpScroll = e.getUnitsToScroll();
      scale(-tmpScroll * MOUSE_SENSITIVITY_SCALE_BY_WHEEL);
      _eventListener.reDraw();
    }
  }
  
  public void keyTyped(KeyEvent e) {
  }
  
  public void keyPressed(KeyEvent e) {
    _pressedKey = e.getKeyCode();
    
    boolean toReDraw = true;
    if (_pressedKey == KeyEvent.VK_R) {
      reset();
    } else if (_pressedKey == KeyEvent.VK_N) {
      _eventListener.nextTrafficTime();
    } else if (_pressedKey == KeyEvent.VK_P) {
      _eventListener.previousTrafficTime();
    } else {
      toReDraw = false;
    }
    
    if (toReDraw) {
      _eventListener.reDraw();
    }
  }
  
  public void keyReleased(KeyEvent e) {
    _pressedKey = -1;
  }
  
  public void setClearColor(Color color) {
    _colorClear = color;
  }
  
  public void setColor(Color color) {
    _colorCurrent = color;
  }
  
  public void setClearColor(float red, float green, float blue, float alpha) {
    _colorClear = new Color(red, green, blue, alpha);
  }
  
  public void setColor(float red, float green, float blue, float alpha) {
    _colorCurrent = new Color(red, green, blue, alpha);
  }
  
  public void setClearColor(float[] color) {
    _colorClear = new Color(color[0], color[1], color[2], color[3]);
  }
  
  public void setColor(float[] color) {
    _colorCurrent = new Color(color[0], color[1], color[2], color[3]);
  }
  
  public void setClearColor(double red, double green, double blue, double alpha) {
    _colorClear = new Color((float) red, (float) green, (float) blue, (float) alpha);
  }
  
  public void setColor(double red, double green, double blue, double alpha) {
    _colorCurrent = new Color((float) red, (float) green, (float) blue, (float) alpha);
  }
  
  public float[] getCenter() {
    float[] array = new float[3];
    array[0] = _center[0];
    array[1] = _center[1];
    array[2] = _center[2];
    return array;
  }
  
  public void setCenter(float x, float y, float z) {
    _centerInitial[0] = x;
    _centerInitial[1] = y;
    _centerInitial[2] = z;
    _center[0] = _centerInitial[0];
    _center[1] = _centerInitial[1];
    _center[2] = _centerInitial[2];
  }
  
  private void resizeImage() {
    int width = getWidth();
    int height = getHeight();
    setSize(width, height);
  }
  
  public void setSize(int width, int height) {
    _width = width;
    _height = height;
    _imageIndex = 0;
    _image[0] = new BufferedImage(_width, _height, BufferedImage.TYPE_INT_RGB);
    _image[1] = new BufferedImage(_width, _height, BufferedImage.TYPE_INT_RGB);
    clear(_image[0].getGraphics());
    clear(_image[1].getGraphics());
    computeRatioCoordToPixel();
  }
  
  public void setRadius(float radius) {
    _radius = radius;
    computeRatioCoordToPixel();
  }
  
  public void computeRatioCoordToPixel() {
    _ratioCoordToPixel = _height / _radius;
  }
  
  public void setScale(float scale) {
    _scale = scale;
  }
  
  public void switchImage() {
    _imageIndex = 1 - _imageIndex;
    repaint();
  }
  
  public Graphics getGraphicsToDraw() {
    Graphics g = null;
    if (_image[_imageIndex] != null) {
      g = _image[_imageIndex].getGraphics();
    }
    return g;
  }
  
  public void reset() {
    _center[0] = _centerInitial[0];
    _center[1] = _centerInitial[1];
    _center[2] = _centerInitial[2];
    setScale(DEFAULT_SCALE);
  }
  
  /**
   * 拡大倍率を変更する
   * @param x 画面横方向の移動量
   * @param y 画面縦方向の移動量
   */
  private void scale(int x, int y) {
    scale(-(x + y) * MOUSE_SENSITIVITY_SCALE);
  }
  
  /**
   * 拡大倍率を変更する
   * @param scale
   */
  private void scale(float scale) {
    double tmpLog = Math.log(_scale);
    tmpLog += scale;
    setScale((float) (Math.exp(tmpLog)));
  }
  
  /**
   * 平行移動を行う。
   * x,yはマウスポインタの、画面上での移動量であり、
   * xは右が正方向、yは上が正方向である。
   * @param x 横方向の移動量
   * @param y 縦方向の移動量
   */
  public void translate(int x, int y) {
    float tmpX = x / _ratioCoordToPixel / _scale;
    float tmpY = y / _ratioCoordToPixel / _scale;
    _center[0] -= tmpX;
    _center[1] -= tmpY;
  }
  
  public void clear() {
    if (_image[_imageIndex] != null) {
      clear(_image[_imageIndex].getGraphics());
    }
  }
  
  private void clear(Graphics g) {
    g.setColor(_colorClear);
    g.fillRect(0, 0, _width, _height);
  }
  
  public void drawPoint(float x, float y, float z, int size) {
    Graphics g = getGraphicsToDraw();
    if (g != null) {
      g.setColor(_colorCurrent);
      int sizeHalf = size / 2;
      int[] position = toCoordinateOnScreen(x, y, z);
      int px0 = position[0] - sizeHalf;
      int py0 = position[1] - sizeHalf;
      g.fillRect(px0, py0, size, size);
    }
  }
  
  public void drawLine(float x0, float y0, float z0, float x1, float y1, float z1, int size) {
    Graphics g = getGraphicsToDraw();
    if (g != null) {
      g.setColor(_colorCurrent);
      int[] position = toCoordinateOnScreen(x0, y0, z0);
      int px0 = position[0];
      int py0 = position[1];
      position = toCoordinateOnScreen(x1, y1, z1);
      int px1 = position[0];
      int py1 = position[1];
      g.drawLine(px0, py0, px1, py1);
    }
  }
  
  public void drawTriangle(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2) {
    Graphics g = getGraphicsToDraw();
    if (g != null) {
      g.setColor(_colorCurrent);
      int[] position = toCoordinateOnScreen(x0, y0, z0);
      _bufferX[0] = position[0];
      _bufferY[0] = position[1];
      position = toCoordinateOnScreen(x1, y1, z1);
      _bufferX[1] = position[0];
      _bufferY[1] = position[1];
      position = toCoordinateOnScreen(x2, y2, z2);
      _bufferX[2] = position[0];
      _bufferY[2] = position[1];
      g.fillPolygon(_bufferX, _bufferY, 3);
    }
  }
  
  public void drawString(float x, float y, float z, String str) {
    Graphics g = getGraphicsToDraw();
    if (g != null) {
      int[] position = toCoordinateOnScreen(x, y, z);
      g.setColor(_colorCurrent);
      g.drawString(str, position[0], position[1]);
    }
  }
  
  public void drawString2D(float x, float y, float z, String str) {
    Graphics g = getGraphicsToDraw();
    if (g != null) {
      int xInt = (int) (x);
      int yInt = _height - (int) (y);
      g.setColor(_colorCurrent);
      g.drawString(str, xInt, yInt);
    }
  }
  
  public int[] toCoordinateOnScreen(float x, float y, float z) {
    float diffx = x - _center[0];
    float diffy = y - _center[1];
    
    diffx *= _scale;
    diffy *= _scale;
    
    _bufferPositionOnScreen[0] = _width / 2;
    _bufferPositionOnScreen[1] = _height / 2;
    
    _bufferPositionOnScreen[0] += diffx * _ratioCoordToPixel;
    _bufferPositionOnScreen[1] -= diffy * _ratioCoordToPixel;
    
    return _bufferPositionOnScreen;
  }
}
