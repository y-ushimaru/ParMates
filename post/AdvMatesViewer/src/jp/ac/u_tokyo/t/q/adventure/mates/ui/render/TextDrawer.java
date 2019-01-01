package jp.ac.u_tokyo.t.q.adventure.mates.ui.render;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.awt.image.Raster;
import java.nio.ByteBuffer;
import java.util.logging.Logger;

import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import javax.swing.JLabel;

import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

public class TextDrawer {
  private static final int COLOR_TYPE_BUFFERED_IMAGE = BufferedImage.TYPE_INT_ARGB;
  private static final int COLOR_TYPE_GL_DRAW_PIXELS = GL.GL_RGBA;
  
  private static final Color COLOR_TRANSPARENT = new Color(0, 0, 0, 0);
  
  /**
   * ログハンドリングクラス
   */
  private Logger _logger;
  /**
   * 描画する文字列
   */
  private String _text;
  
  private Font _font;
  
  private int _width;
  
  private int _height;
  
  private float _positionX;
  
  private float _positionY;
  
  private float _positionZ;
  
  private Color _color;
  
  private ByteBuffer _bufferTextImage;
  
  public TextDrawer() {
    _logger = Logger.getLogger(TextDrawer.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    _bufferTextImage = null;
    _text = null;
    _font = null;
    _width = -1;
    _height = -1;
    _positionX = -1;
    _positionY = -1;
    _positionZ = -1;
    
    _color = new Color(0, 0, 0, 255);
  }
  
  public int getWidth() {
    return _width;
  }
  
  public int getHeight() {
    return _height;
  }
  
  public void setText(String text) {
    _text = text;
    getTextImageBuffer(_text);
  }
  
  public void setFont(Font font) {
    _font = font;
    getTextImageBuffer(_text);
  }
  
  public void setColor(Color color) {
    _color = color;
    getTextImageBuffer(_text);
  }
  
  public void setColor(float[] colorFloat) {
    if (colorFloat != null && colorFloat.length >= 4) {
      Color color = new Color(colorFloat[0], colorFloat[1], colorFloat[2], colorFloat[3]);
      setColor(color);
    }
  }
  
  public void draw(GLAutoDrawable glAutoDrawable) {
    if (_width > 0 && _height > 0 && _bufferTextImage != null) {
      final GL gl = glAutoDrawable.getGL();
      //gl.glRasterPos3f(_positionX, _positionY, 0);
      gl.glRasterPos3f(_positionX, _positionY, _positionZ);
      gl.glDrawPixels(_width, _height, COLOR_TYPE_GL_DRAW_PIXELS, GL.GL_UNSIGNED_BYTE, _bufferTextImage);
    }
  }
  
  public void draw(Drawable2DPanel drawable2DPanel) {
    if (_width > 0 && _height > 0 && _bufferTextImage != null) {
      drawable2DPanel.setColor(_color);
      drawable2DPanel.drawString(_positionX, _positionY, _positionZ, _text);
    }
  }
  
  public void draw2D(Drawable2DPanel drawable2DPanel) {
    if (_width > 0 && _height > 0 && _bufferTextImage != null) {
      drawable2DPanel.setColor(_color);
      drawable2DPanel.drawString2D(_positionX, _positionY, _positionZ, _text);
    }
  }
  
  public void setPosition(int x, int y) {
    _positionX = x;
    _positionY = y;
    _positionZ = 0;
  }
  
  public void setPosition3D(float x, float y, float z) {
    _positionX = x;
    _positionY = y;
    _positionZ = z;
  }
  
  private void getTextImageBuffer(String text) {
    if (_text == null || _text.equals(Common.BLANK) || _font == null) {
      _width = 0;
      _height = 0;
      _bufferTextImage = null;
    } else {
      //文字列を描画した結果の幅と高さを得るためにJLabelを用いる
      JLabel label = new JLabel(text);
      label.setFont(_font);
      
      int tmpWidth = label.getPreferredSize().width;
      int tmpHeight = label.getPreferredSize().height;
      int imageWidth = tmpWidth;
      int imageHeight = tmpHeight * 2;
      BufferedImage image = new BufferedImage(imageWidth, imageHeight, COLOR_TYPE_BUFFERED_IMAGE);
      
      //TODO 描画もJLabelに行わせたい
      //image.getGraphics().***()という形式でメソッドを呼ぶと設定や描画が正常に行われない
      Graphics g = image.getGraphics();
      g.setFont(_font);
      g.setColor(COLOR_TRANSPARENT);
      g.fillRect(0, 0, imageWidth, imageHeight);
      g.setColor(_color);
      g.drawString(_text, 0, tmpHeight);
      
      Raster raster = image.getRaster();
      int minX = imageWidth;
      int maxX = -1;
      int minY = imageHeight;
      int maxY = -1;
      int[] tmpColor = new int[4];
      for (int i = 0; i < imageWidth; i++) {
        for (int j = 0; j < imageHeight; j++) {
          //指定した位置のピクセルの色情報を取得する
          //raster.getPixel(i, j, array);
          //arrayの長さは4必要で、
          //array[0]=red, array[1]=green, array[2]=blue, array[3]=alpha
          //として色情報を取得できる
          raster.getPixel(i, j, tmpColor);
          boolean flgRed = (tmpColor[0] == COLOR_TRANSPARENT.getRed());
          boolean flgGreen = (tmpColor[1] == COLOR_TRANSPARENT.getGreen());
          boolean flgBlue = (tmpColor[2] == COLOR_TRANSPARENT.getBlue());
          boolean flgAlpha = (tmpColor[3] == COLOR_TRANSPARENT.getAlpha());
          
          if (!flgAlpha || !flgRed || !flgGreen || !flgBlue) {
            if (minX > i) minX = i;
            if (maxX < i) maxX = i;
            if (minY > j) minY = j;
            if (maxY < j) maxY = j;
          }
        }
      }
      
      _width = maxX - minX + 1;
      _height = maxY - minY + 1;
      byte[] array = new byte[_width * _height * 4];
      int[] arrayInt = new int[_width * _height];
      
      for (int i = 0; i < _height; i++) {
        image.getRGB(minX, maxY - i, _width, 1, arrayInt, _width * i, 0);
      }
      
      //intの色情報をbyteに変換する。値の順番は注意が必要
      int ix4 = 0;
      for (int i = 0; i < arrayInt.length; i++) {
        array[ix4 + 3] = (byte) (arrayInt[i] >> 24);
        array[ix4 + 0] = (byte) (arrayInt[i] >> 16);
        array[ix4 + 1] = (byte) (arrayInt[i] >> 8);
        array[ix4 + 2] = (byte) (arrayInt[i]);
        ix4 += 4;
      }
      _bufferTextImage = ByteBuffer.wrap(array);
    }
  }
}
