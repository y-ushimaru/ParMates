package jp.ac.u_tokyo.t.q.adventure.mates.image;

import java.awt.Image;
import java.awt.Toolkit;
import java.awt.image.ImageProducer;
import java.net.URL;
import java.util.logging.Logger;

import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * �摜�t�@�C����ǂݍ��ނ��߂̃N���X
 */
public class ImageLoader {
  /**
   * ���O�n���h�����O�N���X
   */
  private Logger _logger;
  /**
   * �摜�I�u�W�F�N�g�̔z��
   */
  private Image[] _images;
  /**
   * �摜���������邽�߂�Toolkit�I�u�W�F�N�g
   */
  private Toolkit _toolkit;
  
  public ImageLoader(String[] imageFileNames) {
    init(imageFileNames);
  }
  
  public ImageLoader(String imageFileName) {
    init(new String[] { imageFileName });
  }
  
  /**
   * �^����ꂽ�t�@�C�����̔z�񂩂�摜�I�u�W�F�N�g���쐬����
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
   * �w�肵���C���f�b�N�X�̉摜��Ԃ�
   * @param index �摜�̃C���f�b�N�X
   * @return �w�肵���C���f�b�N�X�̉摜
   */
  public Image getImage(int index) {
    return _images[index];
  }
  
  /**
   * �摜�I�u�W�F�N�g�̔z���Ԃ�
   * @return �摜�I�u�W�F�N�g�̔z��
   */
  public Image[] getImages() {
    return _images;
  }
  
  /**
   * �^����ꂽ�t�@�C�����̃t�@�C������摜�f�[�^��ǂݍ���Image�I�u�W�F�N�g���쐬����
   * URL����ǂݍ��ނ̂ŁA�t�@�C����jar�Ɍł߂��Ă���ꍇ�ł��K�p�\�ł���B
   * @param imageFile �摜�t�@�C����
   * @return �摜�I�u�W�F�N�g
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
