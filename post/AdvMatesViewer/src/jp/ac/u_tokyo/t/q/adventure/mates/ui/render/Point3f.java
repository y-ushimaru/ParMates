package jp.ac.u_tokyo.t.q.adventure.mates.ui.render;

/**
 * 3������Ԃł̍��W�l��ێ����邾���̃N���X�ł���B
 * Point3f��f�́Afloat�ō��W�l�������Ƃ��Ӗ����Ă���B
 */
public class Point3f {
  public float x;
  public float y;
  public float z;
  
  public Point3f() {
    x = 0;
    y = 0;
    z = 0;
  }
  
  public Point3f(float x, float y, float z) {
    this.x = x;
    this.y = y;
    this.z = z;
  }
  
  public void init(float x, float y, float z) {
    this.x = x;
    this.y = y;
    this.z = z;
  }
  
  public void add(Point3f point, float coefficient) {
    this.x += point.x * coefficient;
    this.y += point.y * coefficient;
    this.z += point.z * coefficient;
  }
  
  public void copy(Point3f point) {
    this.x = point.x;
    this.y = point.y;
    this.z = point.z;
  }
}
