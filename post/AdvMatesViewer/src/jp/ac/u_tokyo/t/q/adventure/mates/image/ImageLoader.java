package jp.ac.u_tokyo.t.q.adventure.mates.image;

import java.awt.Image;
import java.awt.Toolkit;
import java.awt.image.ImageProducer;
import java.net.URL;
import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * 画像ファイルを読み込むためのクラス
 */
public class ImageLoader {
  /**
   * ログハンドリングクラス
   */
  private Logger _logger;
  /**
   * 画像オブジェクトの配列
   */
  private Image[] _images;
  /**
   * 画像を処理するためのToolkitオブジェクト
   */
  private Toolkit _toolkit;
  
  public ImageLoader(String[] imageFileNames) {
    init(imageFileNames);
  }
  
  public ImageLoader(String imageFileName) {
    init(new String[] { imageFileName });
  }
  
  /**
   * 与えられたファイル名の配列から画像オブジェクトを作成する
   * @param imageFileNames
   */
  private void init(String[] imageFileNames) {
    _logger = Logger.getLogger(ImageLoader.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    _images = new Image[imageFileNames.length];
    
    _toolkit = Toolkit.getDefaultToolkit();
    
    for (int i = 0; i < _images.length; i++) {
      _images[i] = loadImage(imageFileNames[i]);
    }
  }
  
  /**
   * 指定したインデックスの画像を返す
   * @param index 画像のインデックス
   * @return 指定したインデックスの画像
   */
  public Image getImage(int index) {
    return _images[index];
  }
  
  /**
   * 画像オブジェクトの配列を返す
   * @return 画像オブジェクトの配列
   */
  public Image[] getImages() {
    return _images;
  }
  
  /**
   * 与えられたファイル名のファイルから画像データを読み込みImageオブジェクトを作成する
   * URLから読み込むので、ファイルがjarに固められている場合でも適用可能である。
   * @param imageFile 画像ファイル名
   * @return 画像オブジェクト
   */
  private Image loadImage(String imageFileName) {
    URL url = getClass().getResource(imageFileName);
    StringBuffer strbuff = new StringBuffer();
    strbuff.append("Load image").append(Common.EOL);
    strbuff.append("imageFileName : ").append(imageFileName).append(Common.EOL);
    strbuff.append("URL           : ").append(url);
    //_logger.info(strbuff.toString());
    
    Image image = null;
    try {
      image = _toolkit.createImage((ImageProducer) url.getContent());
    } catch (Exception e) {
      _logger.info(" failed to load : " + url);
      e.printStackTrace();
    }
    return image;
  }
}
