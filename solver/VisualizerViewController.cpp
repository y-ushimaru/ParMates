/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Visualizer.h"
#include "GVManager.h"
#include <autogl.h>
#include <cassert>
#include <iostream>

/* OpenCV用 */
#ifdef USE_OPENCV
#include <cv.h>
#include <highgui.h>
#endif //USE_OPENCV

using namespace std;

//======================================================================
void Visualizer::drawButtonCallback()
{
    // 描画・出力フラグを更新する
    renewFlag();
    clearTempFlag();

    // シンプル地図モードでは視線方向をリセット
    if (GVManager::getFlag("VIS_SIMPLE_MAP"))
    {
        AutoGL_SetViewDirection(0, 0, 1);
        AutoGL_SetViewUpVector(0, 1, 0);
    }
    AutoGL_DrawView();
}

//======================================================================
void Visualizer::printViewingParamsCallback()
{
    double size;
    double cx, cy, cz;
    double dx, dy, dz;
    double ux, uy, uz;

    size = AutoGL_GetViewSize();
    AutoGL_GetViewCenter(&cx, &cy, &cz);
    AutoGL_GetViewDirection(&dx, &dy, &dz);
    AutoGL_GetViewUpVector(&ux, &uy, &uz);

    cout << "*** Viewing Params ***" << endl;
    cout << "ViewSize: " << size << endl;
    cout << "ViewCenter: ("
         << cx << ", " << cy << ", " << cz << ")" << endl;
    cout << "ViewDirection: ("
         << dx << ", " << dy << ", " << dz << ")" << endl;
    cout << "ViewUpVector: ("
         << ux << ", " << uy << ", " << uz << ")" << endl;
}

//======================================================================
void Visualizer::setViewingParamsCallback()
{
    _viewSize = _svpSize;

    _viewPositionX  = _svpPositionX;
    _viewPositionY  = _svpPositionY;
    _viewPositionZ  = _svpPositionZ;

    _viewDirectionX = _svpDirectionX;
    _viewDirectionY = _svpDirectionY;
    _viewDirectionZ = _svpDirectionZ;

    _viewUpVectorX  = _svpUpVectorX;
    _viewUpVectorY  = _svpUpVectorY;
    _viewUpVectorZ  = _svpUpVectorZ;

    /* 更新されたパラメータで再描画する */
    AutoGL_SetViewSize(_viewSize);
    AutoGL_SetViewCenter(_viewPositionX,
                         _viewPositionY,
                         _viewPositionZ);
    AutoGL_SetViewDirection(_viewDirectionX,
                            _viewDirectionY,
                            _viewDirectionZ);
    AutoGL_SetViewUpVector(_viewUpVectorX,
                           _viewUpVectorY,
                           _viewUpVectorY);

    clearTempFlag();
    AutoGL_DrawView();
}

//======================================================================
void Visualizer::updateBackgroundTexture()
{
#ifdef USE_OPENCV
    // テクスチャの読み込み
    string fullName = GVManager::getString("VIS_TEXTURE_DIR");
    string fileName(_bgFileName);

    fullName += fileName;

    IplImage* bg =cvLoadImage(fullName.c_str(),
                              CV_LOAD_IMAGE_ANYDEPTH
                              | CV_LOAD_IMAGE_ANYCOLOR);
    if(!bg)
    {
        cerr << "ERROR: cannot load image file: "<< fullName;
        return;
    }

    // 色の変更
    if (_bgGrayscaleIsOn)
    {
        IplImage* bgGray = cvCreateImage(cvGetSize(bg), IPL_DEPTH_8U, 1);
        cvCvtColor(bg, bgGray, CV_BGR2GRAY);
        cvCvtColor(bgGray, bg, CV_GRAY2RGB);
        cvReleaseImage(&bgGray);
    }
    else
    {
        cvCvtColor(bg, bg, CV_BGR2RGB);
    }

    // 上下反転
    cvFlip(bg,NULL,0);

    // テクスチャ作成
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
                      bg->width, bg->height,
                      GL_RGB, GL_UNSIGNED_BYTE,
                      bg->imageData);

    cvReleaseImage(&bg);

    _textureIsShown = true;
    clearTempFlag();
    AutoGL_DrawView();

#else //USE_OPENCV not defined

    cerr << "ERROR: cannot use texture without opencv." << endl;

#endif //USE_OPENCV
}

//======================================================================
void Visualizer::clearBackgroundTexture()
{
#ifdef USE_OPENCV
    _textureIsShown = false;
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &_texture);

    clearTempFlag();
    AutoGL_DrawView();
#endif //USE_OPENCV
}
