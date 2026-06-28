/////////////////////////////////////////////////////////////////////////////
//  PROGRAM      : MzPoint.h                                      
//  FUNCTION     : CMzPoint クラスの定義                            
//  SYSTEM       : Visual C++                                     
//  DEFINE_TABLE : ***                                                                                             */
//  DATA/AUTHOR  : 02/06/12 Y.Izutsu                              
//  MODIFY 0.00  : 00/00/00                                       
/////////////////////////////////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CMzPoint クラスの定義

#include <math.h>

#define MZ_DIMENSION_XY 0
#define MZ_DIMENSION_YZ 1
#define MZ_DIMENSION_XZ 2
#define EPS6 1.0E-6

class CMzPoint {
public:

    // メンバ
    double  x;   // X座標値
    double  y;   // Y座標値
    double  z;   // Z座標値 
    
    // コンストラクタ
    CMzPoint() {}
    CMzPoint( const double x, 
              const double y,              
              const double z )        { Set( x,y,z ); }   // 座標値の初期値設定
    CMzPoint( const float*  point )   { Set( point ); }   // 座標値の初期値設定
    CMzPoint( const double* point )   { Set( point ); }   // 座標値の初期値設定
    CMzPoint( const CMzPoint& point ) { Set( point ); }   // 座標値の初期値設定

    CMzPoint( const int mode )                                 { Set2D( mode ); }         // 座標値の初期値設定
    CMzPoint( const double a, const double b, const int mode ) { Set2D( a,b,mode ); }     // 座標値の初期値設定
    CMzPoint( const float* point, const int mode )             { Set2D( point,mode ); }   // 座標値の初期値設定
    CMzPoint( const double* point, const int mode )            { Set2D( point,mode ); }   // 座標値の初期値設定
    CMzPoint( const CMzPoint& point, const int mode )          { Set2D( point,mode ); }   // 座標値の初期値設定

    // デストラクタ
    //~CMzPoint() {}

    // メソッド
    CMzPoint& Set();                                                // 座標値の初期化  
    CMzPoint& Set( const double&, const double&, const double& );   // 座標値の設定 
    CMzPoint& Set( const float* );                                  // 座標値の設定
    CMzPoint& Set( const double* );                                 // 座標値の設定
    CMzPoint& Set( const CMzPoint& );                               // 座標値の設定

    CMzPoint& SetXY();                                 // 座標値の初期化(XY)
    CMzPoint& SetXY( const double&, const double& );   // 座標値の設定(XY) 
    CMzPoint& SetXY( const float* );                   // 座標値の設定(XY)
    CMzPoint& SetXY( const double* );                  // 座標値の設定(XY)
    CMzPoint& SetXY( const CMzPoint& );                // 座標値の設定(XY)

    CMzPoint& SetYZ();                                 // 座標値の初期化(YZ)
    CMzPoint& SetYZ( const double&, const double& );   // 座標値の設定(YZ) 
    CMzPoint& SetYZ( const float* );                   // 座標値の設定(YZ)
    CMzPoint& SetYZ( const double* );                  // 座標値の設定(YZ)
    CMzPoint& SetYZ( const CMzPoint& );                // 座標値の設定(YZ)
        
    CMzPoint& SetXZ();                                 // 座標値の初期化(XZ)
    CMzPoint& SetXZ( const double&, const double& );   // 座標値の設定(XZ)
    CMzPoint& SetXZ( const float* );                   // 座標値の設定(XZ)
    CMzPoint& SetXZ( const double* );                  // 座標値の設定(XZ)    
    CMzPoint& SetXZ( const CMzPoint& );                // 座標値の設定(XZ)

    CMzPoint& Set2D( const int );                                 // 座標値の初期化(2D)
    CMzPoint& Set2D( const double&, const double&, const int );   // 座標値の設定(2D)
    CMzPoint& Set2D( const float*, const int );                   // 座標値の設定(2D)
    CMzPoint& Set2D( const double*, const int );                  // 座標値の設定(2D)    
    CMzPoint& Set2D( const CMzPoint&, const int );                // 座標値の設定(2D)

    CMzPoint& Add( const double& );                                 // 座標値に加算
    CMzPoint& Add( const double&, const double&, const double& );   // 座標値に加算
    CMzPoint& Add( const float* );                                  // 座標値に加算
    CMzPoint& Add( const double* );                                 // 座標値に加算
    CMzPoint& Add( const CMzPoint& );                               // 座標値に加算

    CMzPoint& AddXY( const double& );                  // 座標値に加算(XY)
    CMzPoint& AddXY( const double&, const double& );   // 座標値に加算(XY)
    CMzPoint& AddXY( const float* );                   // 座標値に加算(XY)
    CMzPoint& AddXY( const double* );                  // 座標値に加算(XY)
    CMzPoint& AddXY( const CMzPoint& );                // 座標値に加算(XY)

    CMzPoint& AddYZ( const double& );                  // 座標値に加算(YZ)    
    CMzPoint& AddYZ( const double&, const double& );   // 座標値に加算(YZ)
    CMzPoint& AddYZ( const float* );                   // 座標値に加算(YZ)
    CMzPoint& AddYZ( const double* );                  // 座標値に加算(YZ)
    CMzPoint& AddYZ( const CMzPoint& );                // 座標値に加算(YZ)

    CMzPoint& AddXZ( const double& );                  // 座標値に加算(XZ) 
    CMzPoint& AddXZ( const double&, const double& );   // 座標値に加算(XZ)
    CMzPoint& AddXZ( const float* );                   // 座標値に加算(XZ)
    CMzPoint& AddXZ( const double* );                  // 座標値に加算(XZ)
    CMzPoint& AddXZ( const CMzPoint& );                // 座標値に加算(XZ)

    CMzPoint& Add2D( const double&, const int );                  // 座標値に加算(2D) 
    CMzPoint& Add2D( const double&, const double&, const int );   // 座標値に加算(2D)
    CMzPoint& Add2D( const float*, const int );                   // 座標値に加算(2D)
    CMzPoint& Add2D( const double*, const int );                  // 座標値に加算(2D)
    CMzPoint& Add2D( const CMzPoint&, const int );                // 座標値に加算(2D)

    CMzPoint& Sub( const double& );                                 // 座標値から減算
    CMzPoint& Sub( const double&, const double&, const double& );   // 座標値から減算
    CMzPoint& Sub( const float* );                                  // 座標値から減算
    CMzPoint& Sub( const double* );                                 // 座標値から減算
    CMzPoint& Sub( const CMzPoint& );                               // 座標値から減算

    CMzPoint& SubXY( const double& );                  // 座標値から減算(XY)
    CMzPoint& SubXY( const double&, const double& );   // 座標値から減算(XY)
    CMzPoint& SubXY( const float* );                   // 座標値から減算(XY)
    CMzPoint& SubXY( const double* );                  // 座標値から減算(XY)
    CMzPoint& SubXY( const CMzPoint& );                // 座標値から減算(XY)

    CMzPoint& SubYZ( const double& );                  // 座標値から減算(YZ)
    CMzPoint& SubYZ( const double&, const double& );   // 座標値から減算(YZ)
    CMzPoint& SubYZ( const float* );                   // 座標値から減算(YZ)
    CMzPoint& SubYZ( const double* );                  // 座標値から減算(YZ)
    CMzPoint& SubYZ( const CMzPoint& );                // 座標値から減算(YZ)

    CMzPoint& SubXZ( const double& );                  // 座標値から減算(XZ)    
    CMzPoint& SubXZ( const double&, const double& );   // 座標値から減算(XZ)
    CMzPoint& SubXZ( const float* );                   // 座標値から減算(XZ)
    CMzPoint& SubXZ( const double* );                  // 座標値から減算(XZ)
    CMzPoint& SubXZ( const CMzPoint& );                // 座標値から減算(XZ)

    CMzPoint& Sub2D( const double&, const int );                  // 座標値から減算(2D)    
    CMzPoint& Sub2D( const double&, const double&, const int );   // 座標値から減算(2D)
    CMzPoint& Sub2D( const float*, const int );                   // 座標値から減算(2D)
    CMzPoint& Sub2D( const double*, const int );                  // 座標値から減算(2D)
    CMzPoint& Sub2D( const CMzPoint&, const int );                // 座標値から減算(2D)

    CMzPoint GetAdd( const double& ) const;                                 // 加算した座標値を取得する
    CMzPoint GetAdd( const double&, const double&, const double& ) const;   // 加算した座標値を取得する
    CMzPoint GetAdd( const float* ) const;                                  // 加算した座標値を取得する
    CMzPoint GetAdd( const double* ) const;                                 // 加算した座標値を取得する
    CMzPoint GetAdd( const CMzPoint& ) const;                               // 加算した座標値を取得する

    CMzPoint GetAddXY( const double& ) const;                  // 加算した座標値を取得する(XY)
    CMzPoint GetAddXY( const double&, const double& ) const;   // 加算した座標値を取得する(XY)
    CMzPoint GetAddXY( const float* ) const;                   // 加算した座標値を取得する(XY)
    CMzPoint GetAddXY( const double* ) const;                  // 加算した座標値を取得する(XY)
    CMzPoint GetAddXY( const CMzPoint& ) const;                // 加算した座標値を取得する(XY)

    CMzPoint GetAddYZ( const double& ) const;                  // 加算した座標値を取得する(YZ)
    CMzPoint GetAddYZ( const double&, const double& ) const;   // 加算した座標値を取得する(YZ)
    CMzPoint GetAddYZ( const float* ) const;                   // 加算した座標値を取得する(YZ)
    CMzPoint GetAddYZ( const double* ) const;                  // 加算した座標値を取得する(YZ)
    CMzPoint GetAddYZ( const CMzPoint& ) const;                // 加算した座標値を取得する(YZ)
  
    CMzPoint GetAddXZ( const double& ) const;                  // 加算した座標値を取得する(XZ)
    CMzPoint GetAddXZ( const double&, const double& ) const;   // 加算した座標値を取得する(XZ)
    CMzPoint GetAddXZ( const float* ) const;                   // 加算した座標値を取得する(XZ)
    CMzPoint GetAddXZ( const double* ) const;                  // 加算した座標値を取得する(XZ)
    CMzPoint GetAddXZ( const CMzPoint& ) const;                // 加算した座標値を取得する(XZ)

    CMzPoint GetAdd2D( const double&, const int ) const;                  // 加算した座標値を取得する(2D)
    CMzPoint GetAdd2D( const double&, const double&, const int ) const;   // 加算した座標値を取得する(2D)
    CMzPoint GetAdd2D( const float*, const int ) const;                   // 加算した座標値を取得する(2D)
    CMzPoint GetAdd2D( const double*, const int ) const;                  // 加算した座標値を取得する(2D)
    CMzPoint GetAdd2D( const CMzPoint&, const int ) const;                // 加算した座標値を取得する(2D)

    CMzPoint GetSub( const double& ) const;                                 // 減算した座標値を取得する
    CMzPoint GetSub( const double&, const double&, const double& ) const;   // 減算した座標値を取得する
    CMzPoint GetSub( const float* ) const;                                  // 減算した座標値を取得する
    CMzPoint GetSub( const double* ) const;                                 // 減算した座標値を取得する
    CMzPoint GetSub( const CMzPoint& ) const;                               // 減算した座標値を取得する

    CMzPoint GetSubXY( const double& ) const;                  // 減算した座標値を取得する(XY)
    CMzPoint GetSubXY( const double&, const double& ) const;   // 減算した座標値を取得する(XY)
    CMzPoint GetSubXY( const float* ) const;                   // 減算した座標値を取得する(XY)
    CMzPoint GetSubXY( const double* ) const;                  // 減算した座標値を取得する(XY)
    CMzPoint GetSubXY( const CMzPoint& ) const;                // 減算した座標値を取得する(XY)

    CMzPoint GetSubYZ( const double& ) const;                  // 減算した座標値を取得する(YZ)
    CMzPoint GetSubYZ( const double&, const double& ) const;   // 減算した座標値を取得する(YZ)
    CMzPoint GetSubYZ( const float* ) const;                   // 減算した座標値を取得する(YZ)
    CMzPoint GetSubYZ( const double* ) const;                  // 減算した座標値を取得する(YZ)
    CMzPoint GetSubYZ( const CMzPoint& ) const;                // 減算した座標値を取得する(YZ)

    CMzPoint GetSubXZ( const double& ) const;                  // 減算した座標値を取得する(XZ)
    CMzPoint GetSubXZ( const double&, const double& ) const;   // 減算した座標値を取得する(XZ)
    CMzPoint GetSubXZ( const float* ) const;                   // 減算した座標値を取得する(XZ)
    CMzPoint GetSubXZ( const double* ) const;                  // 減算した座標値を取得する(XZ)
    CMzPoint GetSubXZ( const CMzPoint& ) const;                // 減算した座標値を取得する(XZ)

    CMzPoint GetSub2D( const double&, const int ) const;                  // 減算した座標値を取得する(2D)
    CMzPoint GetSub2D( const double&, const double&, const int ) const;   // 減算した座標値を取得する(2D)
    CMzPoint GetSub2D( const float*, const int ) const;                   // 減算した座標値を取得する(2D)
    CMzPoint GetSub2D( const double*, const int ) const;                  // 減算した座標値を取得する(2D)
    CMzPoint GetSub2D( const CMzPoint&, const int ) const;                // 減算した座標値を取得する(2D)

    double LengthSquared() const;                      // ベクトルの大きさの２乗
    double LengthSquaredXY() const;                    // ベクトルの大きさの２乗(XY)
    double LengthSquaredYZ() const;                    // ベクトルの大きさの２乗(YZ)
    double LengthSquaredXZ() const;                    // ベクトルの大きさの２乗(XZ)
    double LengthSquared2D( const int ) const;         // ベクトルの大きさの２乗(2D)
    
    double Length() const;                             // ベクトルの大きさ
    double LengthXY() const;                           // ベクトルの大きさ(XY)
    double LengthYZ() const;                           // ベクトルの大きさ(YZ)
    double LengthXZ() const;                           // ベクトルの大きさ(XZ)
    double Length2D( const int ) const;                // ベクトルの大きさ(2D)

    CMzPoint& Normalize();                             // ベクトル正規化
    CMzPoint& Normalize( const double& );              // ベクトル正規化

    CMzPoint& NormalizeXY();                           // ベクトル正規化(XY)
    CMzPoint& NormalizeXY( const double& );            // ベクトル正規化(XY)

    CMzPoint& NormalizeYZ();                           // ベクトル正規化(YZ)
    CMzPoint& NormalizeYZ( const double& );            // ベクトル正規化(YZ)

    CMzPoint& NormalizeXZ();                           // ベクトル正規化(XZ)
    CMzPoint& NormalizeXZ( const double& );            // ベクトル正規化(XZ)

    CMzPoint& Normalize2D( const int );                           // ベクトル正規化(2D)
    CMzPoint& Normalize2D( const double&, const int );            // ベクトル正規化(2D)

    CMzPoint GetNormalized() const;                    // 正規化したベクトルを取得
    CMzPoint GetNormalized( const double& ) const;     // ｗ正規化したベクトルを取得

    CMzPoint GetNormalizedXY() const;                  // 正規化したベクトルを取得(XY)
    CMzPoint GetNormalizedXY( const double& ) const;   // 正規化したベクトルを取得(XY)

    CMzPoint GetNormalizedYZ() const;                  // 正規化したベクトルを取得(YZ)
    CMzPoint GetNormalizedYZ( const double& ) const;   // 正規化したベクトルを取得(YZ)

    CMzPoint GetNormalizedXZ() const;                  // 正規化したベクトルを取得(XZ)
    CMzPoint GetNormalizedXZ( const double& ) const;   // 正規化したベクトルを取得(XZ)

    CMzPoint GetNormalized2D( const int ) const;                  // 正規化したベクトルを取得(2D)
    CMzPoint GetNormalized2D( const double&, const int ) const;   // 正規化したベクトルを取得(2D)

    double DistanceSquared( const double&, const double&, const double& ) const;   // 2点間の距離の2乗
    double DistanceSquared( const float* ) const;                                  // 2点間の距離の2乗
    double DistanceSquared( const double* ) const;                                 // 2点間の距離の2乗 
    double DistanceSquared( const CMzPoint& ) const;                               // 2点間の距離の2乗

    double DistanceSquaredXY( const double&, const double& ) const;   // 2点間の距離の2乗(XY)
    double DistanceSquaredXY( const float* ) const;                   // 2点間の距離の2乗(XY)
    double DistanceSquaredXY( const double* ) const;                  // 2点間の距離の2乗(XY)       
    double DistanceSquaredXY( const CMzPoint& ) const;                // 2点間の距離の2乗(XY)

    double DistanceSquaredYZ( const double&, const double& ) const;   // 2点間の距離の2乗(YZ)
    double DistanceSquaredYZ( const float* ) const;                   // 2点間の距離の2乗(YZ)
    double DistanceSquaredYZ( const double* ) const;                  // 2点間の距離の2乗(YZ)       
    double DistanceSquaredYZ( const CMzPoint& ) const;                // 2点間の距離の2乗(YZ)

    double DistanceSquaredXZ( const double&, const double& ) const;   // 2点間の距離の2乗(XZ)
    double DistanceSquaredXZ( const float* ) const;                   // 2点間の距離の2乗(XZ)
    double DistanceSquaredXZ( const double* ) const;                  // 2点間の距離の2乗(XZ)       
    double DistanceSquaredXZ( const CMzPoint& ) const;                // 2点間の距離の2乗(XZ)

    double DistanceSquared2D( const double&, const double&, const int ) const;   // 2点間の距離の2乗(2D)
    double DistanceSquared2D( const float*, const int ) const;                   // 2点間の距離の2乗(2D)
    double DistanceSquared2D( const double*, const int ) const;                  // 2点間の距離の2乗(2D)       
    double DistanceSquared2D( const CMzPoint&, const int ) const;                // 2点間の距離の2乗(2D)

    double Distance( const double&, const double&, const double& ) const;   // 2点間の距離
    double Distance( const float* ) const;                                  // 2点間の距離
    double Distance( const double* ) const;                                 // 2点間の距離
    double Distance( const CMzPoint& ) const;                               // 2点間の距離

    double DistanceXY( const double&, const double& ) const;   // 2点間の距離(XY)
    double DistanceXY( const float* ) const;                   // 2点間の距離(XY)
    double DistanceXY( const double* ) const;                  // 2点間の距離(XY)
    double DistanceXY( const CMzPoint& ) const;                // 2点間の距離(XY)

    double DistanceYZ( const double&, const double& ) const;   // 2点間の距離(YZ)
    double DistanceYZ( const float* ) const;                   // 2点間の距離(YZ)
    double DistanceYZ( const double* ) const;                  // 2点間の距離(YZ)
    double DistanceYZ( const CMzPoint& ) const;                // 2点間の距離(YZ)

    double DistanceXZ( const double&, const double& ) const;   // 2点間の距離(XZ)
    double DistanceXZ( const float* ) const;                   // 2点間の距離(XZ)
    double DistanceXZ( const double* ) const;                  // 2点間の距離(XZ)
    double DistanceXZ( const CMzPoint& ) const;                // 2点間の距離(XZ)

    double Distance2D( const double&, const double&, const int ) const;   // 2点間の距離(2D)
    double Distance2D( const float*, const int ) const;                   // 2点間の距離(2D)
    double Distance2D( const double*, const int ) const;                  // 2点間の距離(2D)
    double Distance2D( const CMzPoint&, const int ) const;                // 2点間の距離(2D)

    double DotProduct( const double&, const double&, const double& ) const;   // 内積
    double DotProduct( const float* ) const;                                  // 内積
    double DotProduct( const double* ) const;                                 // 内積
    double DotProduct( const CMzPoint& ) const;                               // 内積

    double DotProductXY( const double&, const double& ) const;   // 内積(XY)
    double DotProductXY( const float* ) const;                   // 内積(XY)
    double DotProductXY( const double* ) const;                  // 内積(XY)
    double DotProductXY( const CMzPoint& ) const;                // 内積(XY)

    double DotProductYZ( const double&, const double& ) const;   // 内積(YZ)
    double DotProductYZ( const float* ) const;                   // 内積(YZ)
    double DotProductYZ( const double* ) const;                  // 内積(YZ)
    double DotProductYZ( const CMzPoint& ) const;                // 内積(YZ)

    double DotProductXZ( const double&, const double& ) const;   // 内積(XZ)
    double DotProductXZ( const float* ) const;                   // 内積(XZ)
    double DotProductXZ( const double* ) const;                  // 内積(XZ)
    double DotProductXZ( const CMzPoint& ) const;                // 内積(XZ)
    
    double DotProduct2D( const double&, const double&, const int ) const;   // 内積(2D)
    double DotProduct2D( const float*, const int ) const;                   // 内積(2D)
    double DotProduct2D( const double*, const int ) const;                  // 内積(2D)
    double DotProduct2D( const CMzPoint&, const int ) const;                // 内積(2D)    

    CMzPoint CrossProduct( const double&, const double&, const double& ) const;   // 外積
    CMzPoint CrossProduct( const float* ) const;                                  // 外積
    CMzPoint CrossProduct( const double* ) const;                                 // 外積
    CMzPoint CrossProduct( const CMzPoint& ) const;                               // 外積 
    
    bool IsSamePoint( const double&, const double&, const double& ) const;   // 2つの点が等しければ真 
    bool IsSamePoint( const float* ) const;                                  // 2つの点が等しければ真
    bool IsSamePoint( const double* ) const;                                 // 2つの点が等しければ真
    bool IsSamePoint( const CMzPoint& ) const;                               // 2つの点が等しければ真

    bool IsSamePointXY( const double&, const double& ) const;   // 2つの点が等しければ真(XY) 
    bool IsSamePointXY( const float* ) const;                   // 2つの点が等しければ真(XY)
    bool IsSamePointXY( const double* ) const;                  // 2つの点が等しければ真(XY)
    bool IsSamePointXY( const CMzPoint& ) const;                // 2つの点が等しければ真(XY)

    bool IsSamePointYZ( const double&, const double& ) const;   // 2つの点が等しければ真(YZ) 
    bool IsSamePointYZ( const float* ) const;                   // 2つの点が等しければ真(YZ)
    bool IsSamePointYZ( const double* ) const;                  // 2つの点が等しければ真(YZ)
    bool IsSamePointYZ( const CMzPoint& ) const;                // 2つの点が等しければ真(YZ)

    bool IsSamePointXZ( const double&, const double& ) const;   // 2つの点が等しければ真(XZ) 
    bool IsSamePointXZ( const float* ) const;                   // 2つの点が等しければ真(XZ)
    bool IsSamePointXZ( const double* ) const;                  // 2つの点が等しければ真(XZ)
    bool IsSamePointXZ( const CMzPoint& ) const;                // 2つの点が等しければ真(XZ)

    bool IsSamePoint2D( const double&, const double&, const int ) const;   // 2つの点が等しければ真(2D) 
    bool IsSamePoint2D( const float*, const int ) const;                   // 2つの点が等しければ真(2D)
    bool IsSamePoint2D( const double*, const int ) const;                  // 2つの点が等しければ真(2D)
    bool IsSamePoint2D( const CMzPoint&, const int ) const;                // 2つの点が等しければ真(2D)

    CMzPoint& Reverse();                // ベクトル反転
    CMzPoint& ReverseXY();              // ベクトル反転(XY)
    CMzPoint& ReverseYZ();              // ベクトル反転(YZ)
    CMzPoint& ReverseXZ();              // ベクトル反転(XZ)
    CMzPoint& Reverse2D( const int );   // ベクトル反転(2D)

    CMzPoint GetReversed() const;                // 反転したベクトルを取得
    CMzPoint GetReversedXY() const;              // 反転したベクトルを取得(XY)
    CMzPoint GetReversedYZ() const;              // 反転したベクトルを取得(YZ)
    CMzPoint GetReversedXZ() const;              // 反転したベクトルを取得(XZ)
    CMzPoint GetReversed2D( const int ) const;   // 反転したベクトルを取得(2D)

    // 演算子関数
    bool operator == ( const CMzPoint& ) const;       // 2つの点が等しければ真 ( IsSamePoint )
    bool operator != ( const CMzPoint& ) const;       // 2つの点が等しくなければ真 ( !IsSamePoint )
  
    CMzPoint& operator += ( const CMzPoint& );        // 現在の座標値に加算 ( Add )
    CMzPoint& operator += ( const double& );          // 現在の座標値に加算 ( Add )
    CMzPoint& operator -= ( const CMzPoint& );        // 現在の座標値から減算 ( Sub )
    CMzPoint& operator -= ( const double& );          // 現在の座標値から減算 ( Sub )
    CMzPoint& operator /= ( const double& );          // 現在のベクトル正規化 ( Normalize )
    
    CMzPoint  operator + ( const CMzPoint& ) const;   // 加算 ( GetAdd )
    CMzPoint  operator + ( const double& ) const;     // 加算 ( GetAdd )
    CMzPoint  operator - ( const CMzPoint& ) const;   // 減算 ( GetSub )
    CMzPoint  operator - ( const double& ) const;     // 減算 ( GetSub )
    double    operator * ( const CMzPoint& ) const;   // 内積 ( DotProduct )
    CMzPoint  operator * ( const double& ) const;     // 単位ベクトル → ベクトル
    CMzPoint  operator ^ ( const CMzPoint& ) const;   // 外積 ( CrossProduct )
    CMzPoint  operator / ( const double& ) const;     // ベクトル正規化 ( GetNormalized )
    double&   operator []( const int& );              // 配列
};

/////////////////////////////////////////////////////////////////////////////
// Set : 座標値の初期化

inline CMzPoint& CMzPoint::Set()
{
    this->x = 0.0; 
    this->y = 0.0; 
    this->z = 0.0;
    return *this;
}

inline CMzPoint& CMzPoint::Set( const double& x, const double& y, const double& z )
{
    this->x = x;
    this->y = y;
    this->z = z;
    return *this;
}

inline CMzPoint& CMzPoint::Set( const float* point )
{
    this->x = point[0]; 
    this->y = point[1]; 
    this->z = point[2];
    return *this;
}

inline CMzPoint& CMzPoint::Set( const double* point )
{
    this->x = point[0]; 
    this->y = point[1]; 
    this->z = point[2];
    return *this;
}

inline CMzPoint& CMzPoint::Set( const CMzPoint& point )
{
    this->x = point.x;
    this->y = point.y;
    this->z = point.z;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// SetXY : 座標値の初期化(XY)

inline CMzPoint& CMzPoint::SetXY()
{
    this->x = 0.0; 
    this->y = 0.0; 
    return *this;
}

inline CMzPoint& CMzPoint::SetXY( const double& x, const double& y )
{  
    this->x = x; 
    this->y = y; 
    return *this;
}

inline CMzPoint& CMzPoint::SetXY( const float* point )
{
    this->x = point[0]; 
    this->y = point[1]; 
    return *this;
}

inline CMzPoint& CMzPoint::SetXY( const double* point )
{
    this->x = point[0]; 
    this->y = point[1]; 
    return *this;
}

inline CMzPoint& CMzPoint::SetXY( const CMzPoint& point )
{
    this->x = point.x;
    this->y = point.y;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// SetYZ : 座標値の初期化(YZ)

inline CMzPoint& CMzPoint::SetYZ()
{
    this->y = 0.0; 
    this->z = 0.0; 
    return *this;
}

inline CMzPoint& CMzPoint::SetYZ( const double& y, const double& z )
{
    this->y = y; 
    this->z = z; 
    return *this;
}

inline CMzPoint& CMzPoint::SetYZ( const float* point )
{
    this->y = point[1]; 
    this->z = point[2]; 
    return *this;
}

inline CMzPoint& CMzPoint::SetYZ( const double* point )
{
    this->y = point[1]; 
    this->z = point[2]; 
    return *this;
}

inline CMzPoint& CMzPoint::SetYZ( const CMzPoint& point )
{
    this->y = point.y;
    this->z = point.z;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// SetXZ : 座標値の初期化(XZ)

inline CMzPoint& CMzPoint::SetXZ()
{
    this->x = 0.0; 
    this->z = 0.0; 
    return *this;
}

inline CMzPoint& CMzPoint::SetXZ( const double& x, const double& z )
{
    this->x = x; 
    this->z = z; 
    return *this;
}

inline CMzPoint& CMzPoint::SetXZ( const float* point )
{
    this->x = point[0]; 
    this->z = point[2]; 
    return *this;
}

inline CMzPoint& CMzPoint::SetXZ( const double* point )
{
    this->x = point[0]; 
    this->z = point[2]; 
    return *this;
}

inline CMzPoint& CMzPoint::SetXZ( const CMzPoint& point )
{
    this->x = point.x;
    this->z = point.z;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// Set2D : 座標値の初期化(2D)

inline CMzPoint& CMzPoint::Set2D( const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return SetXY();
        case MZ_DIMENSION_YZ: return SetYZ();
        case MZ_DIMENSION_XZ: return SetXZ();
        default             : return *this;
    }
}

inline CMzPoint& CMzPoint::Set2D( const double& a, const double& b, const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return SetXY(a,b);
        case MZ_DIMENSION_YZ: return SetYZ(a,b);
        case MZ_DIMENSION_XZ: return SetXZ(a,b);
        default             : return *this;
    }
}

inline CMzPoint& CMzPoint::Set2D( const float* point, const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return SetXY(point);
        case MZ_DIMENSION_YZ: return SetYZ(point);
        case MZ_DIMENSION_XZ: return SetXZ(point);
        default             : return *this;
    } 
}

inline CMzPoint& CMzPoint::Set2D( const double* point, const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return SetXY(point);
        case MZ_DIMENSION_YZ: return SetYZ(point);
        case MZ_DIMENSION_XZ: return SetXZ(point);
        default             : return *this;
    }   
}

inline CMzPoint& CMzPoint::Set2D( const CMzPoint& point, const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return SetXY(point);
        case MZ_DIMENSION_YZ: return SetYZ(point);
        case MZ_DIMENSION_XZ: return SetXZ(point);
        default             : return *this;
    }   
}

/////////////////////////////////////////////////////////////////////////////
// Add : 座標値に加算

inline CMzPoint& CMzPoint::Add( const double& value )
{
    this->x += value;
    this->y += value;
    this->z += value;
    return *this;
}

inline CMzPoint& CMzPoint::Add( const double& x, const double& y, const double& z )
{
    this->x += x;
    this->y += y;
    this->z += z;
    return *this;
}

inline CMzPoint& CMzPoint::Add( const float* size )
{
    this->x += size[0];
    this->y += size[1];
    this->z += size[2];
    return *this;
}

inline CMzPoint& CMzPoint::Add( const double* size )
{
    this->x += size[0];
    this->y += size[1];
    this->z += size[2];
    return *this;
}

inline CMzPoint& CMzPoint::Add( const CMzPoint& size )
{
    this->x += size.x;
    this->y += size.y;
    this->z += size.z;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// AddXY : 座標値に加算(XY)

inline CMzPoint& CMzPoint::AddXY( const double& value )
{
    this->x += value;
    this->y += value;
    return *this;
}

inline CMzPoint& CMzPoint::AddXY( const double& x, const double& y )
{
    this->x += x;
    this->y += y;
    return *this;
}

inline CMzPoint& CMzPoint::AddXY( const float* size )
{
    this->x += size[0];
    this->y += size[1];
    return *this;
}

inline CMzPoint& CMzPoint::AddXY( const double* size )
{
    this->x += size[0];
    this->y += size[1];
    return *this;
}

inline CMzPoint& CMzPoint::AddXY( const CMzPoint& size )
{
    this->x += size.x;
    this->y += size.y;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// AddYZ : 座標値に加算(YZ)

inline CMzPoint& CMzPoint::AddYZ( const double& value )
{
    this->y += value;
    this->z += value;
    return *this;
}

inline CMzPoint& CMzPoint::AddYZ( const double& y, const double& z )
{
    this->y += y;
    this->z += z;
    return *this;
}

inline CMzPoint& CMzPoint::AddYZ( const float* size )
{
    this->y += size[1];
    this->z += size[2];
    return *this;
}

inline CMzPoint& CMzPoint::AddYZ( const double* size )
{
    this->y += size[1];
    this->z += size[2];
    return *this;
}

inline CMzPoint& CMzPoint::AddYZ( const CMzPoint& size )
{
    this->y += size.y;
    this->z += size.z;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// AddXZ : 座標値に加算(XZ)

inline CMzPoint& CMzPoint::AddXZ( const double& value )
{
    this->x += value;
    this->z += value;
    return *this;
}

inline CMzPoint& CMzPoint::AddXZ( const double& x, const double& z )
{
    this->x += x;
    this->z += z;
    return *this;
}

inline CMzPoint& CMzPoint::AddXZ( const float* size )
{
    this->x += size[0];
    this->z += size[2];
    return *this;
}

inline CMzPoint& CMzPoint::AddXZ( const double* size )
{
    this->x += size[0];
    this->z += size[2];
    return *this;
}

inline CMzPoint& CMzPoint::AddXZ( const CMzPoint& size )
{
    this->x += size.x;
    this->z += size.z;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// Add2D : 座標値に加算(2D)

inline CMzPoint& CMzPoint::Add2D( const double& value, const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return AddXY(value);
        case MZ_DIMENSION_YZ: return AddYZ(value);
        case MZ_DIMENSION_XZ: return AddXZ(value);
        default             : return *this;
    }
}

inline CMzPoint& CMzPoint::Add2D( const double& a, const double& b, const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return AddXY(a,b);
        case MZ_DIMENSION_YZ: return AddYZ(a,b);
        case MZ_DIMENSION_XZ: return AddXZ(a,b);
        default             : return *this;                           
    }
}

inline CMzPoint& CMzPoint::Add2D( const float* size, const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return AddXY(size);
        case MZ_DIMENSION_YZ: return AddYZ(size);
        case MZ_DIMENSION_XZ: return AddXZ(size);
        default             : return *this;
    }
}

inline CMzPoint& CMzPoint::Add2D( const double* size, const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return AddXY(size);
        case MZ_DIMENSION_YZ: return AddYZ(size);
        case MZ_DIMENSION_XZ: return AddXZ(size);
        default             : return *this;
    }
}

inline CMzPoint& CMzPoint::Add2D( const CMzPoint& size, const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return AddXY(size);
        case MZ_DIMENSION_YZ: return AddYZ(size);
        case MZ_DIMENSION_XZ: return AddXZ(size);
        default             : return *this;
    }
}

/////////////////////////////////////////////////////////////////////////////
// Sub : 座標値から減算

inline CMzPoint& CMzPoint::Sub( const double& value )
{
    this->x -= value;
    this->y -= value;
    this->z -= value;
    return *this;
}

inline CMzPoint& CMzPoint::Sub( const double& x, const double& y, const double& z )
{
    this->x -= x;
    this->y -= y;
    this->z -= z;
    return *this;
}

inline CMzPoint& CMzPoint::Sub( const float* size )
{
    this->x -= size[0];
    this->y -= size[1];
    this->z -= size[2];
    return *this;
}

inline CMzPoint& CMzPoint::Sub( const double* size )
{
    this->x -= size[0];
    this->y -= size[1];
    this->z -= size[2];
    return *this;
}

inline CMzPoint& CMzPoint::Sub( const CMzPoint& size )
{
    this->x -= size.x;
    this->y -= size.y;
    this->z -= size.z;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// SubXY : 座標値から減算(XY)

inline CMzPoint& CMzPoint::SubXY( const double& value )
{
    this->x -= value;
    this->y -= value;
    return *this;
}

inline CMzPoint& CMzPoint::SubXY( const double& x, const double& y )
{
    this->x -= x;
    this->y -= y;
    return *this;
}

inline CMzPoint& CMzPoint::SubXY( const float* size )
{
    this->x -= size[0];
    this->y -= size[1];
    return *this;
}

inline CMzPoint& CMzPoint::SubXY( const double* size )
{
    this->x -= size[0];
    this->y -= size[1];
    return *this;
}

inline CMzPoint& CMzPoint::SubXY( const CMzPoint& size )
{
    this->x -= size.x;
    this->y -= size.y;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// SubYZ : 座標値から減算(YZ)

inline CMzPoint& CMzPoint::SubYZ( const double& value )
{
    this->y -= value;
    this->z -= value;
    return *this;
}

inline CMzPoint& CMzPoint::SubYZ( const double& y, const double& z )
{
    this->y -= y;
    this->z -= z;
    return *this;
}

inline CMzPoint& CMzPoint::SubYZ( const float* size )
{
    this->y -= size[1];
    this->z -= size[2];
    return *this;
}

inline CMzPoint& CMzPoint::SubYZ( const double* size )
{
    this->y -= size[1];
    this->z -= size[2];
    return *this;
}

inline CMzPoint& CMzPoint::SubYZ( const CMzPoint& size )
{
    this->y -= size.y;
    this->z -= size.z;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// SubXZ : 座標値から減算(XZ)

inline CMzPoint& CMzPoint::SubXZ( const double& value )
{
    this->x -= value;
    this->z -= value;
    return *this;
}

inline CMzPoint& CMzPoint::SubXZ( const double& x, const double& z )
{
    this->x -= x;
    this->z -= z;
    return *this;
}

inline CMzPoint& CMzPoint::SubXZ( const float* size )
{
    this->x -= size[0];
    this->z -= size[2];
    return *this;
}

inline CMzPoint& CMzPoint::SubXZ( const double* size )
{
    this->x -= size[0];
    this->z -= size[2];
    return *this;
}

inline CMzPoint& CMzPoint::SubXZ( const CMzPoint& size )
{
    this->x -= size.x;
    this->z -= size.z;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// Sub2D : 座標値から減算(2D)

inline CMzPoint& CMzPoint::Sub2D( const double& value, const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return SubXY(value);
        case MZ_DIMENSION_YZ: return SubYZ(value);
        case MZ_DIMENSION_XZ: return SubXZ(value);
        default             : return *this;
    }
}

inline CMzPoint& CMzPoint::Sub2D( const double& a, const double& b, const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return SubXY(a,b);
        case MZ_DIMENSION_YZ: return SubYZ(a,b);
        case MZ_DIMENSION_XZ: return SubXZ(a,b);
        default             : return *this;
    }
}

inline CMzPoint& CMzPoint::Sub2D( const float* size, const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return SubXY(size);
        case MZ_DIMENSION_YZ: return SubYZ(size);
        case MZ_DIMENSION_XZ: return SubXZ(size);
        default             : return *this;
    }
}

inline CMzPoint& CMzPoint::Sub2D( const double* size, const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return SubXY(size);
        case MZ_DIMENSION_YZ: return SubYZ(size);
        case MZ_DIMENSION_XZ: return SubXZ(size);
        default             : return *this;
    }
}

inline CMzPoint& CMzPoint::Sub2D( const CMzPoint& size, const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return SubXY(size);
        case MZ_DIMENSION_YZ: return SubYZ(size);
        case MZ_DIMENSION_XZ: return SubXZ(size);
        default             : return *this;
    }
}

/////////////////////////////////////////////////////////////////////////////
// GetAdd : 加算した座標値を取得する

inline CMzPoint CMzPoint::GetAdd( const double& value ) const
{
    CMzPoint point = *this;
    point.Add(value);
    return point;
}

inline CMzPoint CMzPoint::GetAdd( const double& x, const double& y, const double& z ) const
{
    CMzPoint point = *this;
    point.Add(x,y,z);
    return point;
}

inline CMzPoint CMzPoint::GetAdd( const float* size ) const
{
    CMzPoint point = *this;
    point.Add(size);
    return point;
}

inline CMzPoint CMzPoint::GetAdd( const double* size ) const
{
    CMzPoint point = *this;
    point.Add(size);
    return point;
}

inline CMzPoint CMzPoint::GetAdd( const CMzPoint& size ) const
{
    CMzPoint point = *this;
    point.Add(size);
    return point;
}

/////////////////////////////////////////////////////////////////////////////
// GetAddXY : 加算した座標値を取得する(XY)

inline CMzPoint CMzPoint::GetAddXY( const double& value ) const
{
    CMzPoint point = *this;
    point.AddXY(value);
    return point;
}

inline CMzPoint CMzPoint::GetAddXY( const double& x, const double& y ) const
{
    CMzPoint point = *this;
    point.AddXY(x,y);
    return point;
}

inline CMzPoint CMzPoint::GetAddXY( const float* size ) const
{
    CMzPoint point = *this;
    point.AddXY(size);
    return point;
}

inline CMzPoint CMzPoint::GetAddXY( const double* size ) const
{
    CMzPoint point = *this;
    point.AddXY(size);
    return point;
}

inline CMzPoint CMzPoint::GetAddXY( const CMzPoint& size ) const
{
    CMzPoint point = *this;
    point.AddXY(size);
    return point;
}

/////////////////////////////////////////////////////////////////////////////
// GetAddYZ : 加算した座標値を取得する(YZ)

inline CMzPoint CMzPoint::GetAddYZ( const double& value ) const
{
    CMzPoint point = *this;
    point.AddYZ(value);
    return point;
}

inline CMzPoint CMzPoint::GetAddYZ( const double& y, const double& z ) const
{
    CMzPoint point = *this;
    point.AddYZ(y,z);
    return point;
}

inline CMzPoint CMzPoint::GetAddYZ( const float* size ) const
{
    CMzPoint point = *this;
    point.AddYZ(size);
    return point;
}

inline CMzPoint CMzPoint::GetAddYZ( const double* size ) const
{
    CMzPoint point = *this;
    point.AddYZ(size);
    return point;
}

inline CMzPoint CMzPoint::GetAddYZ( const CMzPoint& size ) const
{
    CMzPoint point = *this;
    point.AddYZ(size);
    return point;
}

/////////////////////////////////////////////////////////////////////////////
// GetAddXZ : 加算した座標値を取得する(XZ)

inline CMzPoint CMzPoint::GetAddXZ( const double& value ) const
{
    CMzPoint point = *this;
    point.AddXZ(value);
    return point;
}

inline CMzPoint CMzPoint::GetAddXZ( const double& x, const double& z ) const
{
    CMzPoint point = *this;
    point.AddXZ(x,z);
    return point;
}

inline CMzPoint CMzPoint::GetAddXZ( const float* size ) const
{
    CMzPoint point = *this;
    point.AddXZ(size);
    return point;
}

inline CMzPoint CMzPoint::GetAddXZ( const double* size ) const
{
    CMzPoint point = *this;
    point.AddXZ(size);
    return point;
}

inline CMzPoint CMzPoint::GetAddXZ( const CMzPoint& size ) const
{
    CMzPoint point = *this;
    point.AddXZ(size);
    return point;
}

/////////////////////////////////////////////////////////////////////////////
// GetAdd2D : 加算した座標値を取得する(2D)

inline CMzPoint CMzPoint::GetAdd2D( const double& value, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return GetAddXY(value);
        case MZ_DIMENSION_YZ: return GetAddYZ(value);
        case MZ_DIMENSION_XZ: return GetAddXZ(value);
        default             : return *this;
    }
}

inline CMzPoint CMzPoint::GetAdd2D( const double& a, const double& b, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return GetAddXY(a,b);
        case MZ_DIMENSION_YZ: return GetAddYZ(a,b);
        case MZ_DIMENSION_XZ: return GetAddXZ(a,b);
        default             : return *this;
    }
}

inline CMzPoint CMzPoint::GetAdd2D( const float* size, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return GetAddXY(size);
        case MZ_DIMENSION_YZ: return GetAddYZ(size);
        case MZ_DIMENSION_XZ: return GetAddXZ(size);
        default             : return *this;
    }
}

inline CMzPoint CMzPoint::GetAdd2D( const double* size, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return GetAddXY(size);
        case MZ_DIMENSION_YZ: return GetAddYZ(size);
        case MZ_DIMENSION_XZ: return GetAddXZ(size);
        default             : return *this;
    }
}

inline CMzPoint CMzPoint::GetAdd2D( const CMzPoint& size, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return GetAddXY(size);
        case MZ_DIMENSION_YZ: return GetAddYZ(size);
        case MZ_DIMENSION_XZ: return GetAddXZ(size);
        default             : return *this;
    }
}

/////////////////////////////////////////////////////////////////////////////
// GetSub : 減算した座標値を取得する

inline CMzPoint CMzPoint::GetSub( const double& value ) const
{
    CMzPoint point = *this;
    point.Sub(value);
    return point;
}

inline CMzPoint CMzPoint::GetSub( const double& x, const double& y, const double& z ) const
{
    CMzPoint point = *this;
    point.Sub(x,y,z);
    return point;
}

inline CMzPoint CMzPoint::GetSub( const float* size ) const
{
    CMzPoint point = *this;
    point.Sub(size);
    return point;
}

inline CMzPoint CMzPoint::GetSub( const double* size ) const
{
    CMzPoint point = *this;
    point.Sub(size);
    return point;
}

inline CMzPoint CMzPoint::GetSub( const CMzPoint& size ) const
{
    CMzPoint point = *this;
    point.Sub(size);
    return point;
}

/////////////////////////////////////////////////////////////////////////////
// GetSubXY : 減算した座標値を取得する(XY)

inline CMzPoint CMzPoint::GetSubXY( const double& value ) const
{
    CMzPoint point = *this;
    point.SubXY(value);
    return point;
}

inline CMzPoint CMzPoint::GetSubXY( const double& x, const double& y ) const
{
    CMzPoint point = *this;
    point.SubXY(x,y);
    return point;
}

inline CMzPoint CMzPoint::GetSubXY( const float* size ) const
{
    CMzPoint point = *this;
    point.SubXY(size);
    return point;
}

inline CMzPoint CMzPoint::GetSubXY( const double* size ) const
{
    CMzPoint point = *this;
    point.SubXY(size);
    return point;
}

inline CMzPoint CMzPoint::GetSubXY( const CMzPoint& size ) const
{
    CMzPoint point = *this;
    point.SubXY(size);
    return point;
}

/////////////////////////////////////////////////////////////////////////////
// GetSubYZ : 減算した座標値を取得する(YZ)

inline CMzPoint CMzPoint::GetSubYZ( const double& value ) const
{
    CMzPoint point = *this;
    point.SubYZ(value);
    return point;
}

inline CMzPoint CMzPoint::GetSubYZ( const double& y, const double& z ) const
{
    CMzPoint point = *this;
    point.SubYZ(y,z);
    return point;
}

inline CMzPoint CMzPoint::GetSubYZ( const float* size ) const
{
    CMzPoint point = *this;
    point.SubYZ(size);
    return point;
}

inline CMzPoint CMzPoint::GetSubYZ( const double* size ) const
{
    CMzPoint point = *this;
    point.SubYZ(size);
    return point;
}

inline CMzPoint CMzPoint::GetSubYZ( const CMzPoint& size ) const
{
    CMzPoint point = *this;
    point.SubYZ(size);
    return point;
}

/////////////////////////////////////////////////////////////////////////////
// GetSubXZ : 減算した座標値を取得する(XZ)

inline CMzPoint CMzPoint::GetSubXZ( const double& value ) const
{
    CMzPoint point = *this;
    point.SubXZ(value);
    return point;
}

inline CMzPoint CMzPoint::GetSubXZ( const double& x, const double& z ) const
{
    CMzPoint point = *this;
    point.SubXZ(x,z);
    return point;
}

inline CMzPoint CMzPoint::GetSubXZ( const float* size ) const
{
    CMzPoint point = *this;
    point.SubXZ(size);
    return point;
}

inline CMzPoint CMzPoint::GetSubXZ( const double* size ) const
{
    CMzPoint point = *this;
    point.SubXZ(size);
    return point;
}

inline CMzPoint CMzPoint::GetSubXZ( const CMzPoint& size ) const
{
    CMzPoint point = *this;
    point.SubXZ(size);
    return point;
}

/////////////////////////////////////////////////////////////////////////////
// GetSub2D : 減算した座標値を取得する(2D)

inline CMzPoint CMzPoint::GetSub2D( const double& value, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return GetSubXY(value);
        case MZ_DIMENSION_YZ: return GetSubYZ(value);
        case MZ_DIMENSION_XZ: return GetSubXZ(value);
        default             : return *this;
    }
}

inline CMzPoint CMzPoint::GetSub2D( const double& a, const double& b, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return GetSubXY(a,b);
        case MZ_DIMENSION_YZ: return GetSubYZ(a,b);
        case MZ_DIMENSION_XZ: return GetSubXZ(a,b);
        default             : return *this;
    }
}

inline CMzPoint CMzPoint::GetSub2D( const float* size, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return GetSubXY(size);
        case MZ_DIMENSION_YZ: return GetSubYZ(size);
        case MZ_DIMENSION_XZ: return GetSubXZ(size);
        default             : return *this;
    }
}

inline CMzPoint CMzPoint::GetSub2D( const double* size, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return GetSubXY(size);
        case MZ_DIMENSION_YZ: return GetSubYZ(size);
        case MZ_DIMENSION_XZ: return GetSubXZ(size);
        default             : return *this;
    }
}

inline CMzPoint CMzPoint::GetSub2D( const CMzPoint& size, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return GetSubXY(size);
        case MZ_DIMENSION_YZ: return GetSubYZ(size);
        case MZ_DIMENSION_XZ: return GetSubXZ(size);
        default             : return *this;
    }
}

/////////////////////////////////////////////////////////////////////////////
// LengthSquared : ベクトルの大きさの２乗

inline double CMzPoint::LengthSquared() const
{
    return x*x + y*y + z*z;
}

/////////////////////////////////////////////////////////////////////////////
// LengthSquaredXY : ベクトルの大きさの２乗(XY)

inline double CMzPoint::LengthSquaredXY() const
{
    return x*x + y*y;
}

/////////////////////////////////////////////////////////////////////////////
// LengthSquaredYZ : ベクトルの大きさの２乗(YZ)

inline double CMzPoint::LengthSquaredYZ() const
{
    return y*y + z*z;
}

/////////////////////////////////////////////////////////////////////////////
// LengthSquaredXZ : ベクトルの大きさの２乗(XZ)

inline double CMzPoint::LengthSquaredXZ() const
{
    return x*x + z*z;
}

/////////////////////////////////////////////////////////////////////////////
// LengthSquared2D : ベクトルの大きさの２乗(2D)

inline double CMzPoint::LengthSquared2D( const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return LengthSquaredXY();
        case MZ_DIMENSION_YZ: return LengthSquaredYZ();
        case MZ_DIMENSION_XZ: return LengthSquaredXZ();
        default             : return 0.0;
    }
}

/////////////////////////////////////////////////////////////////////////////
// Length : ベクトルの大きさ

inline double CMzPoint::Length() const
{
    return sqrt( LengthSquared() );
}

/////////////////////////////////////////////////////////////////////////////
// LengthXY : ベクトルの大きさ(XY)

inline double CMzPoint::LengthXY() const
{
    return sqrt( LengthSquaredXY() );
}

/////////////////////////////////////////////////////////////////////////////
// LengthYZ : ベクトルの大きさ(YZ)

inline double CMzPoint::LengthYZ() const
{
    return sqrt( LengthSquaredYZ() );
}

/////////////////////////////////////////////////////////////////////////////
// LengthXZ : ベクトルの大きさ(XZ)

inline double CMzPoint::LengthXZ() const
{   
    return sqrt( LengthSquaredXZ() );
}

/////////////////////////////////////////////////////////////////////////////
// Length2D : ベクトルの大きさ(2D)

inline double CMzPoint::Length2D( const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return LengthXY();
        case MZ_DIMENSION_YZ: return LengthYZ();
        case MZ_DIMENSION_XZ: return LengthXZ();
        default             : return 0.0;
    }
}

/////////////////////////////////////////////////////////////////////////////
// Normalize : ベクトル正規化

inline CMzPoint& CMzPoint::Normalize()
{
    double length = Length();
    this->x /= length;
    this->y /= length;
    this->z /= length;
    return *this;
}

inline CMzPoint& CMzPoint::Normalize( const double& length )
{
    this->x /= length;
    this->y /= length;
    this->z /= length;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// NormalizeXY : ベクトル正規化(XY)

inline CMzPoint& CMzPoint::NormalizeXY()
{
    double length = LengthXY();
    this->x /= length;
    this->y /= length;
    return *this;
}

inline CMzPoint& CMzPoint::NormalizeXY( const double& length )
{
    this->x /= length;
    this->y /= length;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// NormalizeYZ : ベクトル正規化(YZ)

inline CMzPoint& CMzPoint::NormalizeYZ()
{
    double length = LengthYZ();
    this->y /= length;
    this->z /= length;
    return *this;
}

inline CMzPoint& CMzPoint::NormalizeYZ( const double& length )
{
    this->y /= length;
    this->z /= length;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// NormalizeXZ : ベクトル正規化(XZ)

inline CMzPoint& CMzPoint::NormalizeXZ()
{
    double length = LengthXZ();
    this->x /= length;
    this->z /= length;
    return *this;
}

inline CMzPoint& CMzPoint::NormalizeXZ( const double& length )
{
    this->x /= length;
    this->z /= length;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// Normalize2D : ベクトル正規化(2D)

inline CMzPoint& CMzPoint::Normalize2D( const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return NormalizeXY();
        case MZ_DIMENSION_YZ: return NormalizeYZ();
        case MZ_DIMENSION_XZ: return NormalizeXZ();
        default             : return *this;
    }
}

inline CMzPoint& CMzPoint::Normalize2D( const double& length, const int mode )
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return NormalizeXY(length);
        case MZ_DIMENSION_YZ: return NormalizeYZ(length);
        case MZ_DIMENSION_XZ: return NormalizeXZ(length);
        default             : return *this;
    }
}

/////////////////////////////////////////////////////////////////////////////
// GetNormalized : 正規化したベクトルを取得

inline CMzPoint CMzPoint::GetNormalized() const
{
    CMzPoint vector = *this;
    vector.Normalize( vector.Length() );    
    return vector;
}

inline CMzPoint CMzPoint::GetNormalized( const double& length ) const
{
    CMzPoint vector = *this;
    vector.Normalize( length );
    return vector;
}

/////////////////////////////////////////////////////////////////////////////
// GetNormalizedXY : 正規化したベクトルを取得(XY)

inline CMzPoint CMzPoint::GetNormalizedXY() const
{
    CMzPoint vector = *this;
    vector.NormalizeXY( vector.LengthXY() );    
    return vector;
}

inline CMzPoint CMzPoint::GetNormalizedXY( const double& length ) const
{
    CMzPoint vector = *this;
    vector.NormalizeXY( length );    
    return vector;
}

/////////////////////////////////////////////////////////////////////////////
// GetNormalizedYZ : 正規化したベクトルを取得(YZ)

inline CMzPoint CMzPoint::GetNormalizedYZ() const
{
    CMzPoint vector = *this;
    vector.NormalizeYZ( vector.LengthYZ() );    
    return vector;
}

inline CMzPoint CMzPoint::GetNormalizedYZ( const double& length ) const
{
    CMzPoint vector = *this;
    vector.NormalizeYZ( length );    
    return vector;
}

/////////////////////////////////////////////////////////////////////////////
// GetNormalizedXZ : 正規化したベクトルを取得(XZ)

inline CMzPoint CMzPoint::GetNormalizedXZ() const
{
    CMzPoint vector = *this;
    vector.NormalizeXZ( vector.LengthXZ() );    
    return vector;
}

inline CMzPoint CMzPoint::GetNormalizedXZ( const double& length ) const
{
    CMzPoint vector = *this;
    vector.NormalizeXZ( length );    
    return vector;
}

/////////////////////////////////////////////////////////////////////////////
// GetNormalized2D : 正規化したベクトルを取得(2D)

inline CMzPoint CMzPoint::GetNormalized2D( const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return GetNormalizedXY();
        case MZ_DIMENSION_YZ: return GetNormalizedYZ();
        case MZ_DIMENSION_XZ: return GetNormalizedXZ();
        default             : return *this;
    }
}

inline CMzPoint CMzPoint::GetNormalized2D( const double& length, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return GetNormalizedXY(length);
        case MZ_DIMENSION_YZ: return GetNormalizedYZ(length);
        case MZ_DIMENSION_XZ: return GetNormalizedXZ(length);
        default             : return *this;
    }
}

/////////////////////////////////////////////////////////////////////////////
// DistanceSquared : 2点間の距離の2乗

inline double CMzPoint::DistanceSquared( const double& x, const double& y, const double& z ) const
{
    CMzPoint vector = GetSub(x,y,z);
    return vector.LengthSquared();
}

inline double CMzPoint::DistanceSquared( const float* point ) const
{
    CMzPoint vector = GetSub(point);
    return vector.LengthSquared();
}

inline double CMzPoint::DistanceSquared( const double* point ) const
{
    CMzPoint vector = GetSub(point);
    return vector.LengthSquared();
}

inline double CMzPoint::DistanceSquared( const CMzPoint& point ) const
{
    CMzPoint vector = GetSub(point);
    return vector.LengthSquared();
}

/////////////////////////////////////////////////////////////////////////////
// DistanceSquaredXY : 2点間の距離の2乗(XY)

inline double CMzPoint::DistanceSquaredXY( const double& x, const double& y ) const
{
    CMzPoint vector = GetSubXY(x,y);
    return vector.LengthSquaredXY();
}

inline double CMzPoint::DistanceSquaredXY( const float* point ) const
{
    CMzPoint vector = GetSubXY(point);
    return vector.LengthSquaredXY();
}

inline double CMzPoint::DistanceSquaredXY( const double* point ) const
{
    CMzPoint vector = GetSubXY(point);
    return vector.LengthSquaredXY();
}

inline double CMzPoint::DistanceSquaredXY( const CMzPoint& point ) const
{
    CMzPoint vector = GetSubXY(point);
    return vector.LengthSquaredXY();
}

/////////////////////////////////////////////////////////////////////////////
// DistanceSquaredYZ : 2点間の距離の2乗(YZ)

inline double CMzPoint::DistanceSquaredYZ( const double& y, const double& z ) const
{
    CMzPoint vector = GetSubYZ(y,z);
    return vector.LengthSquaredYZ();
}

inline double CMzPoint::DistanceSquaredYZ( const float* point ) const
{
    CMzPoint vector = GetSubYZ(point);
    return vector.LengthSquaredYZ();
}

inline double CMzPoint::DistanceSquaredYZ( const double* point ) const
{
    CMzPoint vector = GetSubYZ(point);
    return vector.LengthSquaredYZ();
}

inline double CMzPoint::DistanceSquaredYZ( const CMzPoint& point ) const
{
    CMzPoint vector = GetSubYZ(point);
    return vector.LengthSquaredYZ();
}

/////////////////////////////////////////////////////////////////////////////
// DistanceSquaredXZ : 2点間の距離の2乗(XZ)

inline double CMzPoint::DistanceSquaredXZ( const double& x, const double& z ) const
{
    CMzPoint vector = GetSubXZ(x,z);
    return vector.LengthSquaredXZ();
}

inline double CMzPoint::DistanceSquaredXZ( const float* point ) const
{
    CMzPoint vector = GetSubXZ(point);
    return vector.LengthSquaredXZ();
}

inline double CMzPoint::DistanceSquaredXZ( const double* point ) const
{
    CMzPoint vector = GetSubXZ(point);
    return vector.LengthSquaredXZ();
}

inline double CMzPoint::DistanceSquaredXZ( const CMzPoint& point ) const
{
    CMzPoint vector = GetSubXZ(point);
    return vector.LengthSquaredXZ();
}

/////////////////////////////////////////////////////////////////////////////
// DistanceSquared2D : 2点間の距離の2乗(2D)

inline double CMzPoint::DistanceSquared2D( const double& a, const double& b, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return DistanceSquaredXZ(a,b);
        case MZ_DIMENSION_YZ: return DistanceSquaredYZ(a,b);
        case MZ_DIMENSION_XZ: return DistanceSquaredXZ(a,b);
        default             : return 0.0;
    }
}

inline double CMzPoint::DistanceSquared2D( const float* point, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return DistanceSquaredXZ(point);
        case MZ_DIMENSION_YZ: return DistanceSquaredYZ(point);
        case MZ_DIMENSION_XZ: return DistanceSquaredXZ(point);
        default             : return 0.0;
    }
}

inline double CMzPoint::DistanceSquared2D( const double* point, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return DistanceSquaredXZ(point);
        case MZ_DIMENSION_YZ: return DistanceSquaredYZ(point);
        case MZ_DIMENSION_XZ: return DistanceSquaredXZ(point);
        default             : return 0.0;
    }
}

inline double CMzPoint::DistanceSquared2D( const CMzPoint& point, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return DistanceSquaredXZ(point);
        case MZ_DIMENSION_YZ: return DistanceSquaredYZ(point);
        case MZ_DIMENSION_XZ: return DistanceSquaredXZ(point);
        default             : return 0.0;
    }
}

/////////////////////////////////////////////////////////////////////////////
// Distance : 2点間の距離

inline double CMzPoint::Distance( const double& x, const double& y, const double& z ) const
{
    return sqrt( DistanceSquared(x,y,z) );
}

inline double CMzPoint::Distance( const float* point ) const
{
    return sqrt( DistanceSquared(point) );
}

inline double CMzPoint::Distance( const double* point ) const
{
    return sqrt( DistanceSquared(point) );
}

inline double CMzPoint::Distance( const CMzPoint& point ) const
{
    return sqrt( DistanceSquared(point) );
}

/////////////////////////////////////////////////////////////////////////////
// DistanceXY : 2点間の距離(XY)

inline double CMzPoint::DistanceXY( const double& x, const double& y ) const
{
    return sqrt( DistanceSquaredXY(x,y) );
}

inline double CMzPoint::DistanceXY( const float* point ) const
{
    return sqrt( DistanceSquaredXY(point) );
}

inline double CMzPoint::DistanceXY( const double* point ) const
{
    return sqrt( DistanceSquaredXY(point) );
}

inline double CMzPoint::DistanceXY( const CMzPoint& point ) const
{
    return sqrt( DistanceSquaredXY(point) );
}

/////////////////////////////////////////////////////////////////////////////
// DistanceYZ : 2点間の距離(YZ)

inline double CMzPoint::DistanceYZ( const double& y, const double& z ) const
{
    return sqrt( DistanceSquaredYZ(y,z) );
}

inline double CMzPoint::DistanceYZ( const float* point ) const
{
    return sqrt( DistanceSquaredYZ(point) );
}

inline double CMzPoint::DistanceYZ( const double* point ) const
{
    return sqrt( DistanceSquaredYZ(point) );
}

inline double CMzPoint::DistanceYZ( const CMzPoint& point ) const
{
    return sqrt( DistanceSquaredYZ(point) );
}

/////////////////////////////////////////////////////////////////////////////
// DistanceXZ : 2点間の距離(XZ)

inline double CMzPoint::DistanceXZ( const double& x, const double& z ) const
{
    return sqrt( DistanceSquaredXZ(x,z) );
}

inline double CMzPoint::DistanceXZ( const float* point ) const
{
    return sqrt( DistanceSquaredXZ(point) );
}

inline double CMzPoint::DistanceXZ( const double* point ) const
{
    return sqrt( DistanceSquaredXZ(point) );
}

inline double CMzPoint::DistanceXZ( const CMzPoint& point ) const
{
    return sqrt( DistanceSquaredXZ(point) );
}

/////////////////////////////////////////////////////////////////////////////
// Distance2D : 2点間の距離(2D)

inline double CMzPoint::Distance2D( const double& a, const double& b, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return DistanceXY(a,b);
        case MZ_DIMENSION_YZ: return DistanceYZ(a,b);
        case MZ_DIMENSION_XZ: return DistanceXZ(a,b);
        default             : return 0.0;
    }
}

inline double CMzPoint::Distance2D( const float* point, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return DistanceXY(point);
        case MZ_DIMENSION_YZ: return DistanceYZ(point);
        case MZ_DIMENSION_XZ: return DistanceXZ(point);
        default             : return 0.0;
    }
}

inline double CMzPoint::Distance2D( const double* point, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return DistanceXY(point);
        case MZ_DIMENSION_YZ: return DistanceYZ(point);
        case MZ_DIMENSION_XZ: return DistanceXZ(point);
        default             : return 0.0;
    }
}

inline double CMzPoint::Distance2D( const CMzPoint& point, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return DistanceXY(point);
        case MZ_DIMENSION_YZ: return DistanceYZ(point);
        case MZ_DIMENSION_XZ: return DistanceXZ(point);
        default             : return 0.0;
    }
}
    
/////////////////////////////////////////////////////////////////////////////
// DotProduct : 内積

inline double CMzPoint::DotProduct( const double& x, const double& y, const double& z ) const
{
    return this->x*x + this->y*y + this->z*z;
}

inline double CMzPoint::DotProduct( const float* vector ) const
{
    return x*vector[0] + y*vector[1] + z*vector[2];
}
    
inline double CMzPoint::DotProduct( const double* vector ) const
{
    return x*vector[0] + y*vector[1] + z*vector[2];
}

inline double CMzPoint::DotProduct( const CMzPoint& vector ) const
{
    return x*vector.x + y*vector.y + z*vector.z;
}

/////////////////////////////////////////////////////////////////////////////
// DotProductXY : 内積(XY)

inline double CMzPoint::DotProductXY( const double& x, const double& y ) const
{
    return this->x*x + this->y*y;
}

inline double CMzPoint::DotProductXY( const float* vector ) const
{
    return x*vector[0] + y*vector[1];
}
    
inline double CMzPoint::DotProductXY( const double* vector ) const
{
    return x*vector[0] + y*vector[1];
}

inline double CMzPoint::DotProductXY( const CMzPoint& vector ) const
{
    return x*vector.x + y*vector.y;
}

/////////////////////////////////////////////////////////////////////////////
// DotProductYZ : 内積(YZ)

inline double CMzPoint::DotProductYZ( const double& y, const double& z ) const
{
    return this->y*y + this->z*z;
}

inline double CMzPoint::DotProductYZ( const float* vector ) const
{
    return y*vector[1] + z*vector[2];
}
    
inline double CMzPoint::DotProductYZ( const double* vector ) const
{
    return y*vector[1] + z*vector[2];
}

inline double CMzPoint::DotProductYZ( const CMzPoint& vector ) const
{
    return y*vector.y + z*vector.z;
}

/////////////////////////////////////////////////////////////////////////////
// DotProductXZ : 内積(XZ)

inline double CMzPoint::DotProductXZ( const double& x, const double& z ) const
{
    return this->x*x + this->z*z;
}

inline double CMzPoint::DotProductXZ( const float* vector ) const
{
    return x*vector[0] + z*vector[2];
}
    
inline double CMzPoint::DotProductXZ( const double* vector ) const
{
    return x*vector[0] + z*vector[2];
}

inline double CMzPoint::DotProductXZ( const CMzPoint& vector ) const
{
    return x*vector.x + z*vector.z;
}

/////////////////////////////////////////////////////////////////////////////
// DotProduct2D : 内積(2D)

inline double CMzPoint::DotProduct2D( const double& a, const double& b, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return DistanceXY(a,b);
        case MZ_DIMENSION_YZ: return DistanceYZ(a,b);
        case MZ_DIMENSION_XZ: return DistanceXZ(a,b);
        default             : return 0.0;
    }
}

inline double CMzPoint::DotProduct2D( const float* vector, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return DistanceXY(vector);
        case MZ_DIMENSION_YZ: return DistanceYZ(vector);
        case MZ_DIMENSION_XZ: return DistanceXZ(vector);
        default             : return 0.0;
    }
}
    
inline double CMzPoint::DotProduct2D( const double* vector, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return DistanceXY(vector);
        case MZ_DIMENSION_YZ: return DistanceYZ(vector);
        case MZ_DIMENSION_XZ: return DistanceXZ(vector);
        default             : return 0.0;
    }
}

inline double CMzPoint::DotProduct2D( const CMzPoint& vector, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return DistanceXY(vector);
        case MZ_DIMENSION_YZ: return DistanceYZ(vector);
        case MZ_DIMENSION_XZ: return DistanceXZ(vector);
        default             : return 0.0;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CrossProduct : 外積

inline CMzPoint CMzPoint::CrossProduct( const double& x, const double& y, const double& z ) const
{
    return CMzPoint( (this->y * z) - (this->z * y),
                     (this->z * x) - (this->x * z),
                     (this->x * y) - (this->y * x) );
}

inline CMzPoint CMzPoint::CrossProduct( const float* vector ) const
{
    return CMzPoint( (y * vector[2]) - (z * vector[1]),
                     (z * vector[0]) - (x * vector[2]),
                     (x * vector[1]) - (y * vector[0]) );
}
    
inline CMzPoint CMzPoint::CrossProduct( const double* vector ) const
{
    return CMzPoint( (y * vector[2]) - (z * vector[1]),
                     (z * vector[0]) - (x * vector[2]),
                     (x * vector[1]) - (y * vector[0]) );
}

inline CMzPoint CMzPoint::CrossProduct( const CMzPoint& vector ) const
{
    return CMzPoint( (y * vector.z) - (z * vector.y),
                     (z * vector.x) - (x * vector.z),
                     (x * vector.y) - (y * vector.x) );
}

/////////////////////////////////////////////////////////////////////////////
// IsSamePoint : 2つの点が等しければ真

inline bool CMzPoint::IsSamePoint( const double& x, const double& y, const double& z ) const
{
    if ( DistanceSquared(x,y,z) < EPS6 ) return true;
    else                                 return false;
}

inline bool CMzPoint::IsSamePoint( const float* point ) const
{
    if ( DistanceSquared(point) < EPS6 ) return true;
    else                                 return false;
}

inline bool CMzPoint::IsSamePoint( const double* point ) const
{
    if ( DistanceSquared(point) < EPS6 ) return true;
    else                                 return false;
}

inline bool CMzPoint::IsSamePoint( const CMzPoint& point ) const
{
    if ( DistanceSquared(point) < EPS6 ) return true;
    else                                 return false;   
}

/////////////////////////////////////////////////////////////////////////////
// IsSamePointXY : 2つの点が等しければ真(XY)

inline bool CMzPoint::IsSamePointXY( const double& x, const double& y ) const
{
    if ( DistanceSquaredXY(x,y) < EPS6 ) return true;
    else                                 return false;
}

inline bool CMzPoint::IsSamePointXY( const float* point ) const
{
    if ( DistanceSquaredXY(point) < EPS6 ) return true;
    else                                   return false;
}

inline bool CMzPoint::IsSamePointXY( const double* point ) const
{
    if ( DistanceSquaredXY(point) < EPS6 ) return true;
    else                                   return false;
}

inline bool CMzPoint::IsSamePointXY( const CMzPoint& point ) const
{
    if ( DistanceSquaredXY(point) < EPS6 ) return true;
    else                                   return false;   
}

/////////////////////////////////////////////////////////////////////////////
// IsSamePointYZ : 2つの点が等しければ真(YZ)

inline bool CMzPoint::IsSamePointYZ( const double& y, const double& z ) const
{
    if ( DistanceSquaredYZ(y,z) < EPS6 ) return true;
    else                                 return false;
}

inline bool CMzPoint::IsSamePointYZ( const float* point ) const
{
    if ( DistanceSquaredYZ(point) < EPS6 ) return true;
    else                                   return false;
}

inline bool CMzPoint::IsSamePointYZ( const double* point ) const
{
    if ( DistanceSquaredYZ(point) < EPS6 ) return true;
    else                                   return false;
}

inline bool CMzPoint::IsSamePointYZ( const CMzPoint& point ) const
{
    if ( DistanceSquaredYZ(point) < EPS6 ) return true;
    else                                   return false;   
}

/////////////////////////////////////////////////////////////////////////////
// IsSamePointXZ : 2つの点が等しければ真(XZ)

inline bool CMzPoint::IsSamePointXZ( const double& x, const double& z ) const
{
    if ( DistanceSquaredXZ(x,z) < EPS6 ) return true;
    else                                 return false;
}

inline bool CMzPoint::IsSamePointXZ( const float* point ) const
{
    if ( DistanceSquaredXZ(point) < EPS6 ) return true;
    else                                   return false;
}

inline bool CMzPoint::IsSamePointXZ( const double* point ) const
{
    if ( DistanceSquaredXZ(point) < EPS6 ) return true;
    else                                   return false;
}

inline bool CMzPoint::IsSamePointXZ( const CMzPoint& point ) const
{
    if ( DistanceSquaredXZ(point) < EPS6 ) return true;
    else                                   return false;   
}

/////////////////////////////////////////////////////////////////////////////
// IsSamePoint2D : 2つの点が等しければ真(2D)

inline bool CMzPoint::IsSamePoint2D( const double& a, const double& b, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return IsSamePointXY(a,b);
        case MZ_DIMENSION_YZ: return IsSamePointYZ(a,b);
        case MZ_DIMENSION_XZ: return IsSamePointXZ(a,b);
        default             : return false;
    }
}

inline bool CMzPoint::IsSamePoint2D( const float* point, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return IsSamePointXY(point);
        case MZ_DIMENSION_YZ: return IsSamePointYZ(point);
        case MZ_DIMENSION_XZ: return IsSamePointXZ(point);
        default             : return false;
    }
}

inline bool CMzPoint::IsSamePoint2D( const double* point, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return IsSamePointXY(point);
        case MZ_DIMENSION_YZ: return IsSamePointYZ(point);
        case MZ_DIMENSION_XZ: return IsSamePointXZ(point);
        default             : return false;
    }
}

inline bool CMzPoint::IsSamePoint2D( const CMzPoint& point, const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return IsSamePointXY(point);
        case MZ_DIMENSION_YZ: return IsSamePointYZ(point);
        case MZ_DIMENSION_XZ: return IsSamePointXZ(point);
        default             : return false;
    }   
}

/////////////////////////////////////////////////////////////////////////////
// Reverse : ベクトル反転

inline CMzPoint& CMzPoint::Reverse() 
{
    x = -x;
    y = -y;
    z = -z;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// ReverseXY : ベクトル反転(XY)

inline CMzPoint& CMzPoint::ReverseXY() 
{
    x = -x;
    y = -y;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// ReverseYZ : ベクトル反転(YZ)

inline CMzPoint& CMzPoint::ReverseYZ() 
{
    y = -y;
    z = -z;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// ReverseXZ : ベクトル反転(XZ)

inline CMzPoint& CMzPoint::ReverseXZ() 
{
    x = -x;
    z = -z;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// Reverse2D : ベクトル反転(2D)

inline CMzPoint& CMzPoint::Reverse2D( const int mode ) 
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return ReverseXY();
        case MZ_DIMENSION_YZ: return ReverseYZ();
        case MZ_DIMENSION_XZ: return ReverseXZ();
        default             : return *this;
    }
}

/////////////////////////////////////////////////////////////////////////////
// GetReversed : 反転したベクトルを取得

inline CMzPoint CMzPoint::GetReversed() const
{
    CMzPoint vector = *this;
    vector.Reverse();
    return vector;
}

/////////////////////////////////////////////////////////////////////////////
// GetReversedXY : 反転したベクトルを取得(XY)

inline CMzPoint CMzPoint::GetReversedXY() const
{
    CMzPoint vector = *this;
    vector.ReverseXY();
    return vector;
}

/////////////////////////////////////////////////////////////////////////////
// GetReversedYZ : 反転したベクトルを取得(YZ)

inline CMzPoint CMzPoint::GetReversedYZ() const
{
    CMzPoint vector = *this;
    vector.ReverseYZ();
    return vector;
}

/////////////////////////////////////////////////////////////////////////////
// GetReversedXZ : 反転したベクトルを取得(XZ)

inline CMzPoint CMzPoint::GetReversedXZ() const
{
    CMzPoint vector = *this;
    vector.ReverseXZ();
    return vector;
}

/////////////////////////////////////////////////////////////////////////////
// GetReversed2D : 反転したベクトルを取得(2D)

inline CMzPoint CMzPoint::GetReversed2D( const int mode ) const
{
    ASSERT( mode == MZ_DIMENSION_XY || 
            mode == MZ_DIMENSION_YZ || 
            mode == MZ_DIMENSION_XZ );

    switch( mode ) {
        case MZ_DIMENSION_XY: return GetReversedXY();
        case MZ_DIMENSION_YZ: return GetReversedYZ();
        case MZ_DIMENSION_XZ: return GetReversedXZ();
        default             : return *this;
    }
}

/////////////////////////////////////////////////////////////////////////////
// operator == : 2つの点が等しければ真

inline bool CMzPoint::operator == ( const CMzPoint& point ) const
{
    return IsSamePoint(point);
}

/////////////////////////////////////////////////////////////////////////////
// operator != : 2つの点が等しくなければ真

inline bool CMzPoint::operator != ( const CMzPoint& point ) const
{
    return !IsSamePoint(point);
}

/////////////////////////////////////////////////////////////////////////////
// operator += : 現在の座標値に加算

inline CMzPoint& CMzPoint::operator += ( const CMzPoint& point )
{
    return Add(point);
}

inline CMzPoint& CMzPoint::operator += ( const double& value )
{
    return Add(value);
}

/////////////////////////////////////////////////////////////////////////////
// operator -= : 現在の座標値から減算

inline CMzPoint& CMzPoint::operator -= ( const CMzPoint& point )
{
    return Sub(point);
}

inline CMzPoint& CMzPoint::operator -= ( const double& value )
{
    return Sub(value);
}

/////////////////////////////////////////////////////////////////////////////
// operator /= : 現在のベクトル正規化

inline CMzPoint& CMzPoint::operator /= ( const double& length )
{ 
    return Normalize(length);
}

/////////////////////////////////////////////////////////////////////////////
// operator + : 加算

inline CMzPoint CMzPoint::operator + ( const CMzPoint& point ) const
{
    return GetAdd(point);
}

inline CMzPoint CMzPoint::operator + ( const double& value ) const
{
    return GetAdd(value);
}

/////////////////////////////////////////////////////////////////////////////
// operator - : 減算

inline CMzPoint CMzPoint::operator - ( const CMzPoint& point ) const
{
    return GetSub(point);
}

inline CMzPoint CMzPoint::operator - ( const double& value ) const
{
    return GetSub(value);
}

/////////////////////////////////////////////////////////////////////////////
// operator * : 内積

inline double CMzPoint::operator * ( const CMzPoint& vector ) const
{
    return DotProduct(vector);
}

/////////////////////////////////////////////////////////////////////////////
// operator * : 単位ベクトル → ベクトル

inline CMzPoint CMzPoint::operator * ( const double& length ) const
{
    CMzPoint vec = *this;
    vec.x = vec.x * length;
    vec.y = vec.y * length;
    vec.z = vec.z * length;
    return vec;
}

/////////////////////////////////////////////////////////////////////////////
// operator ^ : 外積

inline CMzPoint CMzPoint::operator ^ ( const CMzPoint& vector ) const
{
    return CrossProduct(vector);
}

/////////////////////////////////////////////////////////////////////////////
// operator / : ベクトル正規化

inline CMzPoint CMzPoint::operator / ( const double& length ) const
{
    return GetNormalized(length);
}

/////////////////////////////////////////////////////////////////////////////
// operator [] : 配列

inline double& CMzPoint::operator []( const int& no )
{
    ASSERT( no >= 0 && no <= 2 );
    switch( no ) {
       case  0: return this->x;
       case  1: return this->y;
       case  2: return this->z;
       default: return this->x;
    } 
    //
    // ### 注意 ###
    //
    // 予定外の番号を受け取った時でも何かメンバ変数を返さないと
    // コンパイル出来ないので,しぶしぶｘを返すようにしています。
    //
    // 従って,予定外の番号の時は…
    //    参照：ｘの値が参照されます。
    //    代入：ｘに値が代入されます。(ｘの値が壊れます)
    //
}
