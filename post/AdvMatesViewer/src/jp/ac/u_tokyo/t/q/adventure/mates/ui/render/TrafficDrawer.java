package jp.ac.u_tokyo.t.q.adventure.mates.ui.render;

import java.awt.Font;
import java.util.logging.Logger;

import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;

import jp.ac.u_tokyo.t.q.adventure.mates.model.TrafficModel;
import jp.ac.u_tokyo.t.q.adventure.mates.model.VehicleFamily;
import jp.ac.u_tokyo.t.q.adventure.mates.resource.ResourceManager;
import jp.ac.u_tokyo.t.q.adventure.mates.util.Common;

/**
 * 3D画面に描画するクラス
 */
public class TrafficDrawer {
  // @formatter:off
  private static final float RATIO_HEIGHT_OF_VIEW_POINT_IN_VEHICLE    = 1.0f;
  private static final float RATIO_MARGIN                             = 5.0f;
  
  private static final float HEIGHT_INTERSECTION_ID                   = 1.0f;
  // @formatter:on
  
  /**
   * ログハンドリングクラス
   */
  private Logger _logger;
  /**
   * リソースマネージャ
   */
  private ResourceManager _rm;
  
  private ViewConfiguration _viewConfiguration;
  
  private TrafficModel _trafficModel;
  
  private Font _font;
  private TextDrawer _textDrawer;
  
  private boolean _isRoadVisible;
  private boolean _isRoadLineVisible;
  private boolean _isRoadSideVisible;
  private boolean _isSideWalkVisible;
  private boolean _isIntersectionIDVisible;
  private boolean _isVehicleVisible;
  private boolean _isVehicleIDVisible;
  private boolean _isSignalVisible;
  private boolean _isGroundVisible;
  
  /**
   * コンストラクタ
   * @param model 表示するモデル
   * @param configuration 表示の設定
   */
  public TrafficDrawer(TrafficModel model, ViewConfiguration configuration) {
    _logger = Logger.getLogger(TrafficDrawer.class.getName());
    _logger.setLevel(Common.GLOBAL_LOG_LEVEL);
    
    _rm = ResourceManager.getInstance();
    
    _font = null;
    _textDrawer = new TextDrawer();
    
    _trafficModel = model;
    setViewConfiguration(configuration);
  }
  
  public TrafficModel getTrafficModel() {
    return _trafficModel;
  }
  
  public void setViewConfiguration(ViewConfiguration configuration) {
    _viewConfiguration = configuration;
    _isRoadVisible = _viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_ROAD);
    _isRoadLineVisible = _viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_ROAD_LINE);
    _isRoadSideVisible = _viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_ROAD_SIDE);
    _isSideWalkVisible = _viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_SIDE_WALK);
    _isIntersectionIDVisible = _viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_INTERSECTION_ID);
    _isVehicleVisible = _viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_VEHICLE);
    _isVehicleIDVisible = _viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_VEHICLE_ID);
    _isSignalVisible = _viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_SIGNAL);
    _isGroundVisible = _viewConfiguration.isVisible(ViewConfiguration.RENDER_TYPE_GROUND);
  }
  
  public void setFont(Font font) {
    _font = font;
    _textDrawer.setFont(_font);
  }
  
  /**
   * 時刻ステップと車両IDで指定した車両について、位置や方向のデータをバッファーに格納する。
   * @param timeStep 時刻ステップ
   * @param SelectedVehicleID 車両番号
   * @param bufferResult 車両の位置・方向を格納するバッファー
   * @return 不正な車両番号を指定した場合などにfalseを返す
   */
  public boolean getVehicleState(int timeStep, int SelectedVehicleID, float[] bufferResult) {
    boolean isSuccess = false;
    if (timeStep >= 0 && timeStep < _trafficModel.getNumTimeSteps()) {
      int nTimeSteps = _trafficModel.getNumTimeSteps();
      if (timeStep >= 0 && timeStep < nTimeSteps) {
        int nVehicles = _trafficModel.getNumVehicles()[timeStep];
        int[] vehicleID = _trafficModel.getVehicleID()[timeStep];
        VehicleFamily[] vehicleFamily = _trafficModel.getVehicleFamily()[timeStep];
        float[] vehiclePositionX = _trafficModel.getVehiclePositionX()[timeStep];
        float[] vehiclePositionY = _trafficModel.getVehiclePositionY()[timeStep];
        float[] vehiclePositionZ = _trafficModel.getVehiclePositionZ()[timeStep];
        float[] vehicleAngleXY = _trafficModel.getVehicleAngleXY()[timeStep];
        float[] vehicleAngleZ = _trafficModel.getVehicleAngleZ()[timeStep];
        
        for (int i = 0; i < nVehicles; i++) {
          int tmpVehicleID = vehicleID[i];
          if (tmpVehicleID == SelectedVehicleID) {
            float x = vehiclePositionX[i];
            float y = vehiclePositionY[i];
            float z = vehiclePositionZ[i];
            //Y軸正方向とのなす角度の値なので、X軸正方向とのなす角に変換する
            float angleXY = (float) (Common.toRadianFromDegree(vehicleAngleXY[i]) + Math.PI / 2);
            float angleZ = Common.toRadianFromDegree(vehicleAngleZ[i]);
            float tmpHeight = vehicleFamily[i].getHeight();
            
            bufferResult[0] = x;
            bufferResult[1] = y;
            bufferResult[2] = z + tmpHeight * RATIO_HEIGHT_OF_VIEW_POINT_IN_VEHICLE;
            bufferResult[3] = angleXY;
            bufferResult[4] = angleZ;
            
            isSuccess = true;
            break;
          }
        }
      }
    }
    return isSuccess;
  }
  
  public String getListOfCurrentVehicleID(int timeStep) {
    StringBuffer sbResult = new StringBuffer();
    if (timeStep >= 0 && timeStep < _trafficModel.getNumTimeSteps()) {
      int nTimeSteps = _trafficModel.getNumTimeSteps();
      if (timeStep >= 0 && timeStep < nTimeSteps) {
        int nVehicles = _trafficModel.getNumVehicles()[timeStep];
        int[] vehicleID = _trafficModel.getVehicleID()[timeStep];
        
        for (int i = 0; i < nVehicles; i++) {
          int tmpVehicleID = vehicleID[i];
          if (sbResult.length() > 0) sbResult.append(", ");
          sbResult.append(tmpVehicleID);
        }
      }
    }
    return sbResult.toString();
  }
  
  private static GL getGL(GLAutoDrawable glAutoDrawable) {
    GL gl = null;
    if (glAutoDrawable != null) gl = glAutoDrawable.getGL();
    return gl;
  }
  
  public void draw(GLAutoDrawable glAutoDrawable, int timeStep) {
    draw(glAutoDrawable, null, timeStep);
  }
  
  public void draw(Drawable2DPanel drawable2DPanel, int timeStep) {
    draw(null, drawable2DPanel, timeStep);
  }
  
  /**
   * 全体を描画する
   * @param glAutoDrawable
   */
  private void draw(GLAutoDrawable glAutoDrawable, Drawable2DPanel drawable2DPanel, int timeStep) {
    final GL gl = getGL(glAutoDrawable);
    
    if (gl != null) gl.glEnable(GL.GL_LIGHTING);
    
    if (_isGroundVisible) {
      //地面を描画した後に、DepthBufferをクリアする
      drawGround(glAutoDrawable, drawable2DPanel);
      if (gl != null) gl.glClear(GL.GL_DEPTH_BUFFER_BIT);
    }
    
    drawTrafficMap(glAutoDrawable, drawable2DPanel);
    
    if (_isSignalVisible) {
      drawSignal(glAutoDrawable, drawable2DPanel, timeStep);
    }
    
    if (_isVehicleVisible || _isVehicleIDVisible) {
      drawTrafficModel(glAutoDrawable, drawable2DPanel, timeStep);
    }
  }
  
  private void drawGround(GLAutoDrawable glAutoDrawable, Drawable2DPanel drawable2DPanel) {
    final GL gl = getGL(glAutoDrawable);
    
    float widthOfLane = _trafficModel.getTrafficParameters().getWidthOfLane();
    
    //全体をカバーする矩形を描画する
    float xmin = _trafficModel.getFieldXMin() - widthOfLane * RATIO_MARGIN;
    float ymin = _trafficModel.getFieldYMin() - widthOfLane * RATIO_MARGIN;
    float zmin = _trafficModel.getFieldZMin();
    float xmax = _trafficModel.getFieldXMax() + widthOfLane * RATIO_MARGIN;
    float ymax = _trafficModel.getFieldYMax() + widthOfLane * RATIO_MARGIN;
    float zmax = _trafficModel.getFieldZMax();
    float base0x = xmin;
    float base0y = ymin;
    float base1x = xmax;
    float base1y = ymin;
    float base2x = xmax;
    float base2y = ymax;
    float base3x = xmin;
    float base3y = ymax;
    
    float[] colorGround = _trafficModel.getTrafficParameters().getColorForGround();
    float[] colorGroundBack = _trafficModel.getTrafficParameters().getColorForGroundBack();
    
    if (gl != null) {
      gl.glEnable(GL.GL_LIGHTING);
      gl.glBegin(GL.GL_TRIANGLES);
      
      gl.glColor4fv(colorGround, 0);
      gl.glNormal3f(0, 0, 1);
      gl.glVertex3f(base0x, base0y, 0);
      gl.glVertex3f(base1x, base1y, 0);
      gl.glVertex3f(base2x, base2y, 0);
      gl.glNormal3f(0, 0, 1);
      gl.glVertex3f(base0x, base0y, 0);
      gl.glVertex3f(base2x, base2y, 0);
      gl.glVertex3f(base3x, base3y, 0);
      
      gl.glColor4fv(colorGroundBack, 0);
      gl.glNormal3f(0, 0, -1);
      gl.glVertex3f(base0x, base0y, 0);
      gl.glVertex3f(base2x, base2y, 0);
      gl.glVertex3f(base1x, base1y, 0);
      gl.glNormal3f(0, 0, -1);
      gl.glVertex3f(base0x, base0y, 0);
      gl.glVertex3f(base3x, base3y, 0);
      gl.glVertex3f(base2x, base2y, 0);
      
      gl.glEnd();
    }
    
    if (drawable2DPanel != null) {
      drawable2DPanel.setColor(colorGround);
      drawable2DPanel.drawTriangle(base0x, base0y, 0, base1x, base1y, 0, base2x, base2y, 0);
      drawable2DPanel.drawTriangle(base0x, base0y, 0, base2x, base2y, 0, base3x, base3y, 0);
    }
  }
  
  private void drawTrafficMap(GLAutoDrawable glAutoDrawable, Drawable2DPanel drawable2DPanel) {
    drawRoad(glAutoDrawable, drawable2DPanel);
    drawIntersection(glAutoDrawable, drawable2DPanel);
  }
  
  private void drawIntersection(GLAutoDrawable glAutoDrawable, Drawable2DPanel drawable2DPanel) {
    final GL gl = getGL(glAutoDrawable);
    
    float MOVE = 0.0f;
    
    int nIntersections = _trafficModel.getNumIntersections();
    int[] nShapePoints = _trafficModel.getIntersectionNumShapePoints();
    int[] intersectionID = _trafficModel.getIntersectionID();
    String[] intersectionIDString = _trafficModel.getIntersectionIDString();
    float[] positionCenterX = _trafficModel.getIntersectionCenterPositionX();
    float[] positionCenterY = _trafficModel.getIntersectionCenterPositionY();
    float[] positionCenterZ = _trafficModel.getIntersectionCenterPositionZ();
    float[][] positionShapeX = _trafficModel.getIntersectionShapePositionX();
    float[][] positionShapeY = _trafficModel.getIntersectionShapePositionY();
    float[][] positionShapeZ = _trafficModel.getIntersectionShapePositionZ();
    
    if (_isRoadVisible) {
      float[] colorRoad = _trafficModel.getTrafficParameters().getColorForRoad();
      
      if (gl != null) {
        gl.glEnable(GL.GL_LIGHTING);
        gl.glColor4fv(colorRoad, 0);
        gl.glBegin(GL.GL_TRIANGLES);
      }
      if (drawable2DPanel != null) {
        drawable2DPanel.setColor(colorRoad);
      }
      for (int i = 0; i < nIntersections; i++) {
        int nShapePoint = nShapePoints[i];
        
        float cx = positionCenterX[i];
        float cy = positionCenterY[i];
        float cz = positionCenterZ[i];
        
        for (int j = 0; j < nShapePoint; j++) {
          int p0 = j;
          int p1 = j + 1;
          if (p1 >= nShapePoint) p1 = 0;
          
          float p0x = positionShapeX[i][p0];
          float p0y = positionShapeY[i][p0];
          float p0z = positionShapeZ[i][p0];
          float p1x = positionShapeX[i][p1];
          float p1y = positionShapeY[i][p1];
          float p1z = positionShapeZ[i][p1];
          if (gl != null) {
            gl.glNormal3f(0, 0, 1);
            gl.glVertex3f(cx, cy, cz + MOVE);
            gl.glVertex3f(p0x, p0y, p0z + MOVE);
            gl.glVertex3f(p1x, p1y, p1z + MOVE);
            gl.glNormal3f(0, 0, -1);
            gl.glVertex3f(cx, cy, cz - MOVE);
            gl.glVertex3f(p1x, p1y, p1z - MOVE);
            gl.glVertex3f(p0x, p0y, p0z - MOVE);
          }
          if (drawable2DPanel != null) {
            drawable2DPanel.drawTriangle(cx, cy, cz + MOVE, p0x, p0y, p0z + MOVE, p1x, p1y, p1z + MOVE);
          }
        }
      }
      
      if (gl != null) {
        gl.glEnd();
      }
    }
    
    //##############################################
    if (glAutoDrawable != null || drawable2DPanel != null) {
      if (_isIntersectionIDVisible) {
        //交差点ID
        for (int i = 0; i < nIntersections; i++) {
          //int tmpIntersectionID = intersectionID[i];
          String strIntersectionID = intersectionIDString[i];
          float cx = positionCenterX[i];
          float cy = positionCenterY[i];
          float cz = positionCenterZ[i];
          
          //_textDrawer.setText("[" + tmpIntersectionID + "]");
          _textDrawer.setText("[id=" + strIntersectionID + "]");
          _textDrawer.setPosition3D(cx, cy, cz + HEIGHT_INTERSECTION_ID);
          if (glAutoDrawable != null) {
            _textDrawer.draw(glAutoDrawable);
          }
          if (drawable2DPanel != null) {
            _textDrawer.draw(drawable2DPanel);
          }
        }
      }
    }
  }
  
  private void drawRoad(GLAutoDrawable glAutoDrawable, Drawable2DPanel drawable2DPanel) {
    final GL gl = getGL(glAutoDrawable);
    
    int nRoads = _trafficModel.getNumRoads();
    int[] nLanesRight = _trafficModel.getRoadNumLanesRight();
    int[] nLanesLeft = _trafficModel.getRoadNumLanesLeft();
    int[] nPointsCenter = _trafficModel.getRoadNumPointsCenter();
    int[] nPointsRight = _trafficModel.getRoadNumPointsRight();
    int[] nPointsLeft = _trafficModel.getRoadNumPointsLeft();
    
    float[][][] positionLanesRightX = _trafficModel.getRoadPositionLanesRightX();
    float[][][] positionLanesRightY = _trafficModel.getRoadPositionLanesRightY();
    float[][][] positionLanesRightZ = _trafficModel.getRoadPositionLanesRightZ();
    float[][][] positionLanesLeftX = _trafficModel.getRoadPositionLanesLeftX();
    float[][][] positionLanesLeftY = _trafficModel.getRoadPositionLanesLeftY();
    float[][][] positionLanesLeftZ = _trafficModel.getRoadPositionLanesLeftZ();
    
    float[][][] positionRoadSideRightX = _trafficModel.getRoadPositionRoadSideRightX();
    float[][][] positionRoadSideRightY = _trafficModel.getRoadPositionRoadSideRightY();
    float[][][] positionRoadSideRightZ = _trafficModel.getRoadPositionRoadSideRightZ();
    float[][][] positionRoadSideLeftX = _trafficModel.getRoadPositionRoadSideLeftX();
    float[][][] positionRoadSideLeftY = _trafficModel.getRoadPositionRoadSideLeftY();
    float[][][] positionRoadSideLeftZ = _trafficModel.getRoadPositionRoadSideLeftZ();
    
    float[][][] positionSideWalkRightX = _trafficModel.getRoadPositionSideWalkRightX();
    float[][][] positionSideWalkRightY = _trafficModel.getRoadPositionSideWalkRightY();
    float[][][] positionSideWalkRightZ = _trafficModel.getRoadPositionSideWalkRightZ();
    float[][][] positionSideWalkLeftX = _trafficModel.getRoadPositionSideWalkLeftX();
    float[][][] positionSideWalkLeftY = _trafficModel.getRoadPositionSideWalkLeftY();
    float[][][] positionSideWalkLeftZ = _trafficModel.getRoadPositionSideWalkLeftZ();
    
    float[][] positionCenterX = _trafficModel.getRoadPositionCenterX();
    float[][] positionCenterY = _trafficModel.getRoadPositionCenterY();
    float[][] positionCenterZ = _trafficModel.getRoadPositionCenterZ();
    
    float[][] positionRightX = _trafficModel.getRoadPositionRightX();
    float[][] positionRightY = _trafficModel.getRoadPositionRightY();
    float[][] positionRightZ = _trafficModel.getRoadPositionRightZ();
    
    float[][] positionLeftX = _trafficModel.getRoadPositionLeftX();
    float[][] positionLeftY = _trafficModel.getRoadPositionLeftY();
    float[][] positionLeftZ = _trafficModel.getRoadPositionLeftZ();
    
    float[][] vectorPerpendicularX = _trafficModel.getRoadVectorPerpendicularX();
    float[][] vectorPerpendicularY = _trafficModel.getRoadVectorPerpendicularY();
    float[][] vectorPerpendicularZ = _trafficModel.getRoadVectorPerpendicularZ();
    
    float[][] signalCoords = _trafficModel.getSignalShapeCoords();
    int[][] signalConnectivity = _trafficModel.getSignalShapeConnectivity();
    float[][] signalNormals = _trafficModel.getSignalShapeNormals();
    float[][] signalColors = _trafficModel.getSignalShapeColors();
    
    float[] colorRoad = _trafficModel.getTrafficParameters().getColorForRoad();
    float[] colorRoadSide = _trafficModel.getTrafficParameters().getColorForRoadSide();
    float[] colorRoadCenterLine1 = _trafficModel.getTrafficParameters().getColorForRoadCenterLine1();
    float[] colorRoadCenterLine2 = _trafficModel.getTrafficParameters().getColorForRoadCenterLine2();
    float[] colorSideWalk = _trafficModel.getTrafficParameters().getColorForSideWalk();
    
    if (gl != null) {
      gl.glLineWidth(Drawable3DPanel.DEFAULT_THICKNESS_LINE);
    }
    
    for (int i = 0; i < nRoads; i++) {
      int tmpNumPoints = nPointsCenter[i];
      int tmpNumLanesRight = nLanesRight[i];
      int tmpNumLanesLeft = nLanesLeft[i];
      
      if (_isRoadVisible || _isRoadLineVisible) {
        boolean isRoadVisible = _isRoadVisible;
        boolean isRoadLineVisible = _isRoadLineVisible;
        for (int k = 0; k < tmpNumLanesRight; k++) {
          float[] colorSideLineRight = colorRoadCenterLine2;
          float[] colorSideLineLeft = colorRoadCenterLine2;
          if (k == 0) colorSideLineRight = null;
          if (k == tmpNumLanesRight - 1) colorSideLineLeft = colorRoadCenterLine1;
          drawRoadSub(glAutoDrawable, drawable2DPanel, isRoadVisible, isRoadLineVisible, colorRoad, colorSideLineRight, colorSideLineLeft,
              tmpNumPoints, positionLanesRightX[i][k], positionLanesRightY[i][k], positionLanesRightZ[i][k], positionLanesRightX[i][k + 1],
              positionLanesRightY[i][k + 1], positionLanesRightZ[i][k + 1]);
        }
        
        for (int k = 0; k < tmpNumLanesLeft; k++) {
          float[] colorSideLineRight = colorRoadCenterLine2;
          float[] colorSideLineLeft = colorRoadCenterLine2;
          if (k == 0) colorSideLineRight = colorRoadCenterLine1;
          if (k == tmpNumLanesLeft - 1) colorSideLineLeft = null;
          drawRoadSub(glAutoDrawable, drawable2DPanel, isRoadVisible, isRoadLineVisible, colorRoad, colorSideLineRight, colorSideLineLeft,
              tmpNumPoints, positionLanesLeftX[i][k], positionLanesLeftY[i][k], positionLanesLeftZ[i][k], positionLanesLeftX[i][k + 1],
              positionLanesLeftY[i][k + 1], positionLanesLeftZ[i][k + 1]);
        }
      }
      
      if (_isRoadSideVisible) {
        boolean isRoadVisible = _isRoadSideVisible;
        boolean isRoadLineVisible = false;
        //路側
        drawRoadSub(glAutoDrawable, drawable2DPanel, isRoadVisible, isRoadLineVisible, colorRoadSide, null, null, tmpNumPoints,
            positionRoadSideRightX[i][0], positionRoadSideRightY[i][0], positionRoadSideRightZ[i][0], positionRoadSideRightX[i][1],
            positionRoadSideRightY[i][1], positionRoadSideRightZ[i][1]);
        drawRoadSub(glAutoDrawable, drawable2DPanel, isRoadVisible, isRoadLineVisible, colorRoadSide, null, null, tmpNumPoints,
            positionRoadSideLeftX[i][0], positionRoadSideLeftY[i][0], positionRoadSideLeftZ[i][0], positionRoadSideLeftX[i][1],
            positionRoadSideLeftY[i][1], positionRoadSideLeftZ[i][1]);
            
      }
      
      if (_isSideWalkVisible) {
        boolean isRoadVisible = _isSideWalkVisible;
        boolean isRoadLineVisible = false;
        //歩道
        drawRoadSub(glAutoDrawable, drawable2DPanel, isRoadVisible, isRoadLineVisible, colorSideWalk, null, null, tmpNumPoints,
            positionSideWalkRightX[i][0], positionSideWalkRightY[i][0], positionSideWalkRightZ[i][0], positionSideWalkRightX[i][1],
            positionSideWalkRightY[i][1], positionSideWalkRightZ[i][1]);
        drawRoadSub(glAutoDrawable, drawable2DPanel, isRoadVisible, isRoadLineVisible, colorSideWalk, null, null, tmpNumPoints,
            positionSideWalkLeftX[i][0], positionSideWalkLeftY[i][0], positionSideWalkLeftZ[i][0], positionSideWalkLeftX[i][1],
            positionSideWalkLeftY[i][1], positionSideWalkLeftZ[i][1]);
      }
    }
  }
  
  private void drawRoadSub(GLAutoDrawable glAutoDrawable, Drawable2DPanel drawable2DPanel, boolean isRoadVisible, boolean isRoadLineVisible,
      float[] colorRoad, float[] colorSideLineRight, float[] colorSideLineLeft, int nPoints, float[] position1X, float[] position1Y,
      float[] position1Z, float[] position2X, float[] position2Y, float[] position2Z) {
    final GL gl = getGL(glAutoDrawable);
    
    float MOVE1 = 0.0f;
    float MOVE2 = 0.0f;
    for (int j = 0; j < nPoints - 1; j++) {
      float p0x = position1X[j];
      float p0y = position1Y[j];
      float p0z = position1Z[j];
      float p1x = position1X[j + 1];
      float p1y = position1Y[j + 1];
      float p1z = position1Z[j + 1];
      float p2x = position2X[j];
      float p2y = position2Y[j];
      float p2z = position2Z[j];
      float p3x = position2X[j + 1];
      float p3y = position2Y[j + 1];
      float p3z = position2Z[j + 1];
      float p02x = (p0x + p2x) / 2;
      float p02y = (p0y + p2y) / 2;
      float p02z = (p0z + p2z) / 2;
      float p13x = (p1x + p3x) / 2;
      float p13y = (p1y + p3y) / 2;
      float p13z = (p1z + p3z) / 2;
      
      if (isRoadVisible) {
        //道路の面
        if (gl != null) {
          gl.glEnable(GL.GL_LIGHTING);
          gl.glColor4fv(colorRoad, 0);
          gl.glBegin(GL.GL_TRIANGLES);
          gl.glNormal3f(0, 0, 1);
          gl.glVertex3f(p0x, p0y, p0z + MOVE1);
          gl.glVertex3f(p1x, p1y, p1z + MOVE1);
          gl.glVertex3f(p3x, p3y, p3z + MOVE1);
          gl.glNormal3f(0, 0, 1);
          gl.glVertex3f(p0x, p0y, p0z + MOVE1);
          gl.glVertex3f(p3x, p3y, p3z + MOVE1);
          gl.glVertex3f(p2x, p2y, p2z + MOVE1);
          gl.glNormal3f(0, 0, -1);
          gl.glVertex3f(p0x, p0y, p0z - MOVE1);
          gl.glVertex3f(p3x, p3y, p3z - MOVE1);
          gl.glVertex3f(p1x, p1y, p1z - MOVE1);
          gl.glNormal3f(0, 0, -1);
          gl.glVertex3f(p0x, p0y, p0z - MOVE1);
          gl.glVertex3f(p2x, p2y, p2z - MOVE1);
          gl.glVertex3f(p3x, p3y, p3z - MOVE1);
          gl.glEnd();
        }
        if (drawable2DPanel != null) {
          drawable2DPanel.setColor(colorRoad);
          drawable2DPanel.drawTriangle(p0x, p0y, p0z + MOVE1, p1x, p1y, p1z + MOVE1, p3x, p3y, p3z + MOVE1);
          drawable2DPanel.drawTriangle(p0x, p0y, p0z + MOVE1, p3x, p3y, p3z + MOVE1, p2x, p2y, p2z + MOVE1);
        }
      }
      
      if (isRoadLineVisible) {
        //道路の輪郭
        if (gl != null) {
          gl.glDisable(GL.GL_LIGHTING);
          gl.glBegin(GL.GL_LINES);
          if (colorSideLineRight != null) {
            gl.glColor4fv(colorSideLineRight, 0);
            gl.glVertex3f(p0x, p0y, p0z + MOVE2);
            gl.glVertex3f(p1x, p1y, p1z + MOVE2);
          }
          if (colorSideLineLeft != null) {
            gl.glColor4fv(colorSideLineLeft, 0);
            gl.glVertex3f(p2x, p2y, p2z + MOVE2);
            gl.glVertex3f(p3x, p3y, p3z + MOVE2);
          }
          //道路の各レーンの中心線
          //gl.glColor4fv(COLOR_BLUE, 0);
          //gl.glVertex3f(p02x, p02y, p02z + MOVE2);
          //gl.glColor4fv(COLOR_RED, 0);
          //gl.glVertex3f(p13x, p13y, p13z + MOVE2);
          gl.glEnd();
        }
        if (drawable2DPanel != null) {
          if (colorSideLineRight != null) {
            drawable2DPanel.setColor(colorSideLineRight);
            drawable2DPanel.drawLine(p0x, p0y, p0z + MOVE2, p1x, p1y, p1z + MOVE2, 1);
          }
          if (colorSideLineLeft != null) {
            drawable2DPanel.setColor(colorSideLineLeft);
            drawable2DPanel.drawLine(p2x, p2y, p2z + MOVE2, p3x, p3y, p3z + MOVE2, 1);
          }
        }
      }
    }
  }
  
  private void drawSignal(GLAutoDrawable glAutoDrawable, Drawable2DPanel drawable2DPanel, int timeStep) {
    final GL gl = getGL(glAutoDrawable);
    
    int nRoads = _trafficModel.getNumRoads();
    int[] nPointsCenter = _trafficModel.getRoadNumPointsCenter();
    int[] nPointsRight = _trafficModel.getRoadNumPointsRight();
    int[] nPointsLeft = _trafficModel.getRoadNumPointsLeft();
    
    boolean[] existSignalStart = _trafficModel.getRoadExistSignalStart();
    boolean[] existSignalEnd = _trafficModel.getRoadExistSignalEnd();
    int[][] signalStateStart = _trafficModel.getSignalStateOfStartOfRoad();
    int[][] signalStateEnd = _trafficModel.getSignalStateOfEndOfRoad();
    
    //float[][] positionCenterX = _trafficModel.getRoadPositionCenterX();
    //float[][] positionCenterY = _trafficModel.getRoadPositionCenterY();
    //float[][] positionCenterZ = _trafficModel.getRoadPositionCenterZ();
    
    float[][] positionRightX = _trafficModel.getRoadPositionRightX();
    float[][] positionRightY = _trafficModel.getRoadPositionRightY();
    float[][] positionRightZ = _trafficModel.getRoadPositionRightZ();
    
    float[][] positionLeftX = _trafficModel.getRoadPositionLeftX();
    float[][] positionLeftY = _trafficModel.getRoadPositionLeftY();
    float[][] positionLeftZ = _trafficModel.getRoadPositionLeftZ();
    
    float[][] vectorPerpendicularX = _trafficModel.getRoadVectorPerpendicularX();
    float[][] vectorPerpendicularY = _trafficModel.getRoadVectorPerpendicularY();
    float[][] vectorPerpendicularZ = _trafficModel.getRoadVectorPerpendicularZ();
    
    float[][] signalCoords = _trafficModel.getSignalShapeCoords();
    int[][] signalConnectivity = _trafficModel.getSignalShapeConnectivity();
    float[][] signalNormals = _trafficModel.getSignalShapeNormals();
    float[][] signalColors = _trafficModel.getSignalShapeColors();
    
    float[][] signalSimpleCoords = _trafficModel.getSignalSimpleShapeCoords();
    int[][] signalSimpleConnectivity = _trafficModel.getSignalSimpleShapeConnectivity();
    float[][] signalSimpleNormals = _trafficModel.getSignalSimpleShapeNormals();
    float[][] signalSimpleColors = _trafficModel.getSignalSimpleShapeColors();
    
    for (int i = 0; i < nRoads; i++) {
      int tmpNumPoints = nPointsCenter[i];
      
      int tmpSignalStateStart = TrafficModel.SIGNAL_STATE_DEFAULT;
      int tmpSignalStateEnd = TrafficModel.SIGNAL_STATE_DEFAULT;
      //_logger.info("step=" + timeStep + " : " + tmpSignalStateStart + " : " + tmpSignalStateEnd);
      
      //信号
      //道路の、進行方向に垂直なベクトルは、進行方向に対して左側を向いている。
      if (existSignalStart[i]) {
        tmpSignalStateStart = signalStateStart[i][timeStep];
        float signalRightX = positionRightX[i][0];
        float signalRightY = positionRightY[i][0];
        float signalRightZ = positionRightZ[i][0];
        float signalRightDirectionX = vectorPerpendicularX[i][0];
        float signalRightDirectionY = vectorPerpendicularY[i][0];
        float signalRightDirectionZ = vectorPerpendicularZ[i][0];
        drawSignalSub(glAutoDrawable, null, tmpSignalStateStart, signalRightX, signalRightY, signalRightZ, signalRightDirectionX,
            signalRightDirectionY, signalRightDirectionZ, signalCoords, signalConnectivity, signalNormals, signalColors);
        drawSignalSub(null, drawable2DPanel, tmpSignalStateStart, signalRightX, signalRightY, signalRightZ, signalRightDirectionX,
            signalRightDirectionY, signalRightDirectionZ, signalSimpleCoords, signalSimpleConnectivity, signalSimpleNormals, signalSimpleColors);
      }
      
      if (existSignalEnd[i]) {
        tmpSignalStateEnd = signalStateEnd[i][timeStep];
        float signalLeftX = positionLeftX[i][tmpNumPoints - 1];
        float signalLeftY = positionLeftY[i][tmpNumPoints - 1];
        float signalLeftZ = positionLeftZ[i][tmpNumPoints - 1];
        float signalLeftDirectionX = -vectorPerpendicularX[i][tmpNumPoints - 1];
        float signalLeftDirectionY = -vectorPerpendicularY[i][tmpNumPoints - 1];
        float signalLeftDirectionZ = -vectorPerpendicularZ[i][tmpNumPoints - 1];
        drawSignalSub(glAutoDrawable, null, tmpSignalStateEnd, signalLeftX, signalLeftY, signalLeftZ, signalLeftDirectionX, signalLeftDirectionY,
            signalLeftDirectionZ, signalCoords, signalConnectivity, signalNormals, signalColors);
        drawSignalSub(null, drawable2DPanel, tmpSignalStateEnd, signalLeftX, signalLeftY, signalLeftZ, signalLeftDirectionX, signalLeftDirectionY,
            signalLeftDirectionZ, signalSimpleCoords, signalSimpleConnectivity, signalSimpleNormals, signalSimpleColors);
      }
    }
  }
  
  private void drawSignalSub(GLAutoDrawable glAutoDrawable, Drawable2DPanel drawable2DPanel, int signalState, float positionX, float positionY,
      float positionZ, float directionX, float directionY, float directionZ, float[][] shapeCoords, int[][] shapeConnectivity, float[][] shapeNormals,
      float[][] shapeColors) {
    final GL gl = getGL(glAutoDrawable);
    
    //float angleXY = Common.toRadianFromDegree(vehicleAngleXY[i]);
    //float angleZ = Common.toRadianFromDegree(vehicleAngleZ[i]);
    //_logger.info("#               " + x + ", " + y + ", " + z);
    
    float cos1 = directionX;
    float sin1 = directionY;
    //float cos2 = (float) (Math.cos(angleZ));
    //float sin2 = (float) (Math.sin(angleZ));
    
    float[] colorSignalGreen = _trafficModel.getTrafficParameters().getColorForSignalGreen();
    float[] colorSignalYellow = _trafficModel.getTrafficParameters().getColorForSignalYellow();
    float[] colorSignalRed = _trafficModel.getTrafficParameters().getColorForSignalRed();
    float[] colorSignalDirection = _trafficModel.getTrafficParameters().getColorForSignalDirection();
    
    if (gl != null) {
      gl.glEnable(GL.GL_LIGHTING);
      gl.glBegin(GL.GL_TRIANGLES);
    }
    
    for (int j = 0; j < shapeConnectivity.length; j++) {
      float[] color = shapeColors[j];
      if (j == TrafficModel.INDEX_SIGNAL_GREEN) {
        if ((signalState & TrafficModel.MASK_SIGNAL_GREEN) != 0) {
          color = colorSignalGreen;
        }
      }
      if (j == TrafficModel.INDEX_SIGNAL_YELLOW) {
        if ((signalState & TrafficModel.MASK_SIGNAL_YELLOW) != 0) {
          color = colorSignalYellow;
        }
      }
      if (j == TrafficModel.INDEX_SIGNAL_RED) {
        if ((signalState & TrafficModel.MASK_SIGNAL_RED) != 0) {
          color = colorSignalRed;
        }
      }
      if (j == TrafficModel.INDEX_SIGNAL_LEFT) {
        if ((signalState & TrafficModel.MASK_SIGNAL_LEFT) != 0) {
          color = colorSignalDirection;
        }
      }
      if (j == TrafficModel.INDEX_SIGNAL_STRAIGHT) {
        if ((signalState & TrafficModel.MASK_SIGNAL_STRAIGHT) != 0) {
          color = colorSignalDirection;
        }
      }
      if (j == TrafficModel.INDEX_SIGNAL_RIGHT) {
        if ((signalState & TrafficModel.MASK_SIGNAL_RIGHT) != 0) {
          color = colorSignalDirection;
        }
      }
      
      if (gl != null) {
        gl.glColor4fv(color, 0);
      }
      if (drawable2DPanel != null) {
        drawable2DPanel.setColor(color);
      }
      
      for (int k = 0, kx3 = 0; kx3 < shapeConnectivity[j].length; k++, kx3 += 3) {
        int p0x3 = shapeConnectivity[j][kx3 + 0] * 3;
        int p1x3 = shapeConnectivity[j][kx3 + 1] * 3;
        int p2x3 = shapeConnectivity[j][kx3 + 2] * 3;
        float p0x = shapeCoords[j][p0x3 + 0];
        float p0y = shapeCoords[j][p0x3 + 1];
        float p0z = shapeCoords[j][p0x3 + 2];
        float p1x = shapeCoords[j][p1x3 + 0];
        float p1y = shapeCoords[j][p1x3 + 1];
        float p1z = shapeCoords[j][p1x3 + 2];
        float p2x = shapeCoords[j][p2x3 + 0];
        float p2y = shapeCoords[j][p2x3 + 1];
        float p2z = shapeCoords[j][p2x3 + 2];
        float nx = shapeNormals[j][kx3 + 0];
        float ny = shapeNormals[j][kx3 + 1];
        float nz = shapeNormals[j][kx3 + 2];
        
        float pp0x = p0x * cos1 - p0y * sin1;
        float pp0y = p0x * sin1 + p0y * cos1;
        float pp0z = p0z;
        float pp1x = p1x * cos1 - p1y * sin1;
        float pp1y = p1x * sin1 + p1y * cos1;
        float pp1z = p1z;
        float pp2x = p2x * cos1 - p2y * sin1;
        float pp2y = p2x * sin1 + p2y * cos1;
        float pp2z = p2z;
        float nnx = nx * cos1 - ny * sin1;
        float nny = nx * sin1 + ny * cos1;
        float nnz = nz;
        
        if (gl != null) {
          gl.glNormal3f(nnx, nny, nnz);
          gl.glVertex3f(pp0x + positionX, pp0y + positionY, pp0z + positionZ);
          gl.glVertex3f(pp1x + positionX, pp1y + positionY, pp1z + positionZ);
          gl.glVertex3f(pp2x + positionX, pp2y + positionY, pp2z + positionZ);
        }
        if (drawable2DPanel != null) {
          drawable2DPanel.drawTriangle(pp0x + positionX, pp0y + positionY, pp0z + positionZ, pp1x + positionX, pp1y + positionY, pp1z + positionZ,
              pp2x + positionX, pp2y + positionY, pp2z + positionZ);
        }
      }
    }
    
    if (gl != null) {
      gl.glEnd();
    }
  }
  
  private void drawTrafficModel(GLAutoDrawable glAutoDrawable, Drawable2DPanel drawable2DPanel, int timeStep) {
    final GL gl = getGL(glAutoDrawable);
    
    if (gl != null) {
      gl.glEnable(GL.GL_LIGHTING);
    }
    
    //float[][] shapeCoords = _trafficModel.getShapeCoords();
    //int[][] shapeConnectivity = _trafficModel.getShapeConnectivity();
    //float[][] shapeNormals = _trafficModel.getShapeNormals();
    //float[][] shapeColors = _trafficModel.getShapeColors();
    
    int nTimeSteps = _trafficModel.getNumTimeSteps();
    if (timeStep >= 0 && timeStep < nTimeSteps) {
      int nVehicles = _trafficModel.getNumVehicles()[timeStep];
      int[] vehicleID = _trafficModel.getVehicleID()[timeStep];
      int[] vehicleFamilyID = _trafficModel.getVehicleFamilyID()[timeStep];
      VehicleFamily[] vehicleFamily = _trafficModel.getVehicleFamily()[timeStep];
      float[] vehiclePositionX = _trafficModel.getVehiclePositionX()[timeStep];
      float[] vehiclePositionY = _trafficModel.getVehiclePositionY()[timeStep];
      float[] vehiclePositionZ = _trafficModel.getVehiclePositionZ()[timeStep];
      float[] vehicleAngleXY = _trafficModel.getVehicleAngleXY()[timeStep];
      float[] vehicleAngleZ = _trafficModel.getVehicleAngleZ()[timeStep];
      float[] vehicleVelocity = _trafficModel.getVehicleVelocity()[timeStep];
      float[] vehicleAcceleration = _trafficModel.getVehicleAcceleration()[timeStep];
      int[] vehicleIntersectionID = _trafficModel.getVehicleIntersectionID()[timeStep];
      int[] vehicleRoadID = _trafficModel.getVehicleRoadID()[timeStep];
      
      //######################################################
      if (_isVehicleVisible) {
        //車両の形状
        if (gl != null) {
          gl.glBegin(GL.GL_TRIANGLES);
        }
        
        for (int i = 0; i < nVehicles; i++) {
          float x = vehiclePositionX[i];
          float y = vehiclePositionY[i];
          float z = vehiclePositionZ[i];
          //Y軸正方向とのなす角度の値なので、X軸正方向とのなす角に変換する
          float angleXY = (float) (Common.toRadianFromDegree(vehicleAngleXY[i]) + Math.PI / 2);
          float angleZ = Common.toRadianFromDegree(vehicleAngleZ[i]);
          
          float cos1 = (float) (Math.cos(angleXY));
          float sin1 = (float) (Math.sin(angleXY));
          float cos2 = (float) (Math.cos(angleZ));
          float sin2 = (float) (Math.sin(angleZ));
          
          if (gl != null) {
            float[][] shapeCoords = vehicleFamily[i].getShapeCoords();
            int[][] shapeConnectivity = vehicleFamily[i].getShapeConnectivity();
            float[][] shapeNormals = vehicleFamily[i].getShapeNormals();
            float[][] shapeColors = vehicleFamily[i].getShapeColors();
            
            for (int j = 0; j < shapeConnectivity.length; j++) {
              float[] color = shapeColors[j];
              gl.glColor4fv(color, 0);
              for (int k = 0, kx3 = 0; kx3 < shapeConnectivity[j].length; k++, kx3 += 3) {
                int p0x3 = shapeConnectivity[j][kx3 + 0] * 3;
                int p1x3 = shapeConnectivity[j][kx3 + 1] * 3;
                int p2x3 = shapeConnectivity[j][kx3 + 2] * 3;
                float p0x = shapeCoords[j][p0x3 + 0];
                float p0y = shapeCoords[j][p0x3 + 1];
                float p0z = shapeCoords[j][p0x3 + 2];
                float p1x = shapeCoords[j][p1x3 + 0];
                float p1y = shapeCoords[j][p1x3 + 1];
                float p1z = shapeCoords[j][p1x3 + 2];
                float p2x = shapeCoords[j][p2x3 + 0];
                float p2y = shapeCoords[j][p2x3 + 1];
                float p2z = shapeCoords[j][p2x3 + 2];
                float nx = shapeNormals[j][kx3 + 0];
                float ny = shapeNormals[j][kx3 + 1];
                float nz = shapeNormals[j][kx3 + 2];
                
                float pp0x = p0x * cos2 - p0z * sin2;
                float pp0y = p0y;
                float pp0z = p0x * sin2 + p0z * cos2;
                float pp1x = p1x * cos2 - p1z * sin2;
                float pp1y = p1y;
                float pp1z = p1x * sin2 + p1z * cos2;
                float pp2x = p2x * cos2 - p2z * sin2;
                float pp2y = p2y;
                float pp2z = p2x * sin2 + p2z * cos2;
                float nnx = nx * cos2 - nx * sin2;
                float nny = ny;
                float nnz = ny * sin2 + nz * cos2;
                
                float ppp0x = pp0x * cos1 - pp0y * sin1;
                float ppp0y = pp0x * sin1 + pp0y * cos1;
                float ppp0z = pp0z;
                float ppp1x = pp1x * cos1 - pp1y * sin1;
                float ppp1y = pp1x * sin1 + pp1y * cos1;
                float ppp1z = pp1z;
                float ppp2x = pp2x * cos1 - pp2y * sin1;
                float ppp2y = pp2x * sin1 + pp2y * cos1;
                float ppp2z = pp2z;
                float nnnx = nnx * cos1 - nny * sin1;
                float nnny = nnx * sin1 + nny * cos1;
                float nnnz = nnz;
                
                gl.glNormal3f(nnnx, nnny, nnnz);
                gl.glVertex3f(ppp0x + x, ppp0y + y, ppp0z + z);
                gl.glVertex3f(ppp1x + x, ppp1y + y, ppp1z + z);
                gl.glVertex3f(ppp2x + x, ppp2y + y, ppp2z + z);
              }
            }
          }
          
          if (drawable2DPanel != null) {
            VehicleFamily tmpVehicleFamily = vehicleFamily[i];
            float width = tmpVehicleFamily.getWidth();
            float length = tmpVehicleFamily.getLength();
            float[] color = tmpVehicleFamily.getColorForBody();
            float widthHalf = width / 2;
            float lengthHalf = length / 2;
            
            float p0x = lengthHalf;
            float p0y = widthHalf;
            float p0z = 0;
            float p1x = -lengthHalf;
            float p1y = widthHalf;
            float p1z = 0;
            float p2x = -lengthHalf;
            float p2y = -widthHalf;
            float p2z = 0;
            float p3x = lengthHalf;
            float p3y = -widthHalf;
            float p3z = 0;
            
            float pp0x = p0x * cos1 - p0y * sin1;
            float pp0y = p0x * sin1 + p0y * cos1;
            float pp0z = p0z;
            float pp1x = p1x * cos1 - p1y * sin1;
            float pp1y = p1x * sin1 + p1y * cos1;
            float pp1z = p1z;
            float pp2x = p2x * cos1 - p2y * sin1;
            float pp2y = p2x * sin1 + p2y * cos1;
            float pp2z = p2z;
            float pp3x = p3x * cos1 - p3y * sin1;
            float pp3y = p3x * sin1 + p3y * cos1;
            float pp3z = p3z;
            
            drawable2DPanel.setColor(color);
            drawable2DPanel.drawTriangle(pp0x + x, pp0y + y, pp0z + z, pp1x + x, pp1y + y, pp1z + z, pp2x + x, pp2y + y, pp2z + z);
            drawable2DPanel.drawTriangle(pp0x + x, pp0y + y, pp0z + z, pp2x + x, pp2y + y, pp2z + z, pp3x + x, pp3y + y, pp3z + z);
          }
        }
        
        if (gl != null) {
          gl.glEnd();
        }
      }
      
      //#####################################################
      if (glAutoDrawable != null || drawable2DPanel != null) {
        if (_isVehicleIDVisible) {
          //車や車両IDなどの文字列
          for (int i = 0; i < nVehicles; i++) {
            float x = vehiclePositionX[i];
            float y = vehiclePositionY[i];
            float z = vehiclePositionZ[i];
            int tmpVehicleID = vehicleID[i];
            int tmpFamilyID = vehicleFamilyID[i];
            float tmpHeight = vehicleFamily[i].getHeight() * 1.05f;
            _textDrawer.setText("[id=" + tmpVehicleID + ", family=" + tmpFamilyID + "]");
            _textDrawer.setPosition3D(x, y, z + tmpHeight);
            if (glAutoDrawable != null) {
              _textDrawer.draw(glAutoDrawable);
            }
            if (drawable2DPanel != null) {
              _textDrawer.draw(drawable2DPanel);
            }
          }
        }
      }
    }
  }
}
