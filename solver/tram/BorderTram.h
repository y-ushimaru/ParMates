/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __BORDER_TRAM_H__
#define __BORDER_TRAM_H__

#include "../Border.h"
#include "../AmuPoint.h"

class IntersectionTramExt;
class Connector;

class BorderTram : public Border
{
public:
    /// コネクタと線分を作成
    BorderTram(const AmuPoint begin, const AmuPoint end,
               int din, int dout,
               IntersectionTramExt* tramExt, int dir,
               double roadsideWidth,
               bool isReverse);

    /// デストラクタ
    virtual ~BorderTram();

    /// 路面電車流入点を返す
    const Connector* inPointTram();

    /// 路面電車流出点を返す
    const Connector* outPointTram();

    /// 流入・流出を合わせてidInt番目のコネクタを返す
    const Connector* connector(int idInt);

private:
    /// 路面電車流入ポイント
    /**
     * @note 1車線流入しか考慮していない
     */
    const Connector* _inPointTram;

    /// 路面電車流出ポイント
    /**
     * @note 1車線流出しか考慮していない
     */
    const Connector* _outPointTram;

protected:
    /// コネクタを作成する関数
    virtual void _createConnectors(IntersectionTramExt* tramExt,
                                   int dir);

    /// コネクタを作成する関数
    /**
     * ODNodeのSectionが接していない境界用
     */
    virtual void _createConnectorsReverse(IntersectionTramExt* tramExt,
                                          int dir);   
};

#endif //__BORDER_TRAM_H__
