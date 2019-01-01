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
 * 2D�`��p�̃N���X�ł���B
 * �e���\�b�h�͈ȉ��̏��ԂŌĂԁB
 * 
 * ��������
 * setSize()
 * setScale()
 * setRatioCoordToPixel()
 * 
 * �`�掞
 * clear()
 * setColor()
 * drawXXX()
 * ...
 * switchImage()
 */
public class Drawable2DPanel extends JPanel implements ComponentListener, MouseListener, MouseMotionListener, MouseWheelListener, KeyListener {
  //@formatter:off
  /**
   * �}�E�X����̏�Ԃ������B���삳��ĂȂ���ԁB
   */
  private static final int MODE_NORMAL                              = 100;
  /**
   * �}�E�X����̏�Ԃ������B�ړ����쒆
   */
  private static final int MODE_TRANSLATE                           = 110;
  /**
   * �}�E�X����̏�Ԃ������B�{���ύX��
   */
  private static final int MODE_SCALE                               = 120;
  
  /**
   * �}�E�X����̊��x�B�ړ�����p�B1.0f���ƁA�}�E�X�����3D�\�����قڒǐ�����
   */
  private static final float MOUSE_SENSITIVITY_TRANSLATE            = 1.0f;
  /**
   * �}�E�X����̊��x�B�{���ύX�p
   */
  private static final float MOUSE_SENSITIVITY_SCALE                = 0.01f;
  /**
   * �}�E�X����̊��x�B�}�E�X�z�C�[���ł̔{���ύX�p
   */
  private static final float MOUSE_SENSITIVITY_SCALE_BY_WHEEL       = 0.01f;
  
  private static final float[] DEFAULT_COLOR_CLEAR         = { 1.0f, 1.0f, 1.0f, 1.0f };
  private static final float[] DEFAULT_COLOR               = { 0.0f, 0.0f, 0.0f, 1.0f };
  
  private static final float DEFAULT_RADIUS                = 1.0f;
  private static final float DEFAULT_SCALE                 = 1.0f;
  //@formatter:on
  
  protected Logger _logger;
  /**
   * GUI�֘A�̃C�x���g����������N���X�B
   * MainControl�N���X�Ŏ������Ă���B
   */
  private View2DEventListener _eventListener;
  
  /**
   * ���݂̕`��p�̐F�ł���B
   */
  private Color _colorCurrent;
  /**
   * �`��̈�S�̂��N���A����Ƃ��̐F�ł���B
   */
  private Color _colorClear;
  /**
   * ��ʂ̏c���ɑ���������W�l�̑傫���ł���B
   */
  private float _radius;
  /**
   * ���W�l��1���A��ʏ�̂����̃s�N�Z���ɑΉ����邩�̔䗦��\���B
   */
  private float _ratioCoordToPixel;
  /**
   * �\���̔{���ł���B
   */
  private float _scale;
  /**
   * ���̍��W�l�̓_����ʂ̒��S�ɕ`�悷��B
   */
  private float[] _center;
  /**
   * ������Ԃł̒��S�̍��W�l�ł���B
   */
  private float[] _centerInitial;
  /**
   * ���W�l���A��ʏ�̃s�N�Z���ʒu�ɕϊ�����ۂɗp����o�b�t�@�[�ł���B
   */
  private int[] _bufferPositionOnScreen;
  /**
   * �`��p�̃o�b�t�@�[�ł���B
   */
  private int[] _bufferX;
  /**
   * �`��p�̃o�b�t�@�[�ł���B
   */
  private int[] _bufferY;
  /**
   * ��ʂ̕��ł���B
   */
  private int _width;
  /**
   * ��ʂ̍����ł���B
   */
  private int _height;
  /**
   * �_�u���o�b�t�@�����O�ɗp����BufferedImage�ł���B
   */
  private BufferedImage[] _image;
  /**
   * _imageIndex�́A�`��Ώۂ̃X�N���[���̔ԍ��ł���B
   * 1-_imageIndex�́A��ʕ\������X�N���[���̔ԍ��ł���B
   */
  private int _imageIndex;
  
  /**
   * �}�E�X��X���W
   */
  private int _mouseX;
  /**
   * �}�E�X��Y���W
   */
  private int _mouseY;
  /**
   * �}�E�X����ł̃��[�h�B
   */
  private int _mode;
  /**
   * �������ꂽ�L�[�̃R�[�h�ł���B
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
    //�L�[�C�x���g�ɔ����ł���悤�ɁAView2D���Ńt�H�[�J�X�̖ʓ|������B
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
    //�}�E�X�̉������ړ��ʁB��ʉE����
    int moveX = tmpX - _mouseX;
    //�}�E�X�̏c�����ړ��ʁB��ʏオ��
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
    //TODO �z�C�[����]�̌����Ɣ{���̕ύX�ʂ̒���
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
   * �g��{����ύX����
   * @param x ��ʉ������̈ړ���
   * @param y ��ʏc�����̈ړ���
   */
  private void scale(int x, int y) {
    scale(-(x + y) * MOUSE_SENSITIVITY_SCALE);
  }
  
  /**
   * �g��{����ύX����
   * @param scale
   */
  private void scale(float scale) {
    double tmpLog = Math.log(_scale);
    tmpLog += scale;
    setScale((float) (Math.exp(tmpLog)));
  }
  
  /**
   * ���s�ړ����s���B
   * x,y�̓}�E�X�|�C���^�́A��ʏ�ł̈ړ��ʂł���A
   * x�͉E���������Ay�͏オ�������ł���B
   * @param x �������̈ړ���
   * @param y �c�����̈ړ���
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
