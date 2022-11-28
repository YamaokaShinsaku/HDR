#pragma once
#include <SDL/SDL.h>
#include "Math.h"
#include "Input.h"
#include "mouse.h"

class FlyCamera
{
public:
	FlyCamera(Vector3& cameraPos);
	~FlyCamera() {};

	// getter/setter
	void        SetZoom(float zoom)                  { mZoom = zoom; }
	void        SetCameraMoveSpeed(float speed)      { mCameraMoveSpeed = speed; }
	void        SetSensitivity(float sensitibity)    { mMouseWheelSensitibity = sensitibity; }
	void        SetScreenWidth(float screenw)        { mScreenWidth = screenw; }
	void        SetScreenHeight(float screenh)       { mScreenHeight = screenh; }

	const       Matrix4& GetViewMatrix() const       { return mViewMatrix; } // カメラ（ビュー行列)の取得
	const       Matrix4& GetProjectionMatrix() const { return mProjectionMatrix; }     // プロジェクション行列の取得
	const       Vector3& GetFrontVec() const         { return mCameraFront; }
	const       Vector3& GetPositionVec() const      { return mCameraPos; }

	void        UpdateCamera(float frameDeltaTime); // カメラの移動計算

private:
	Vector3     mCameraPos;              // カメラ位置
	Vector3     mCameraFront;            // カメラ前方方向
	Vector3     mCameraUp;               // カメラ上方向
		        
	Matrix4     mViewMatrix;             // カメラ行列
	Matrix4     mProjectionMatrix;       // プロジェクション行列

	float       mZoom;                   // ズームレベル（画角)
	float       mCameraMoveSpeed;        // カメラ移動速度 (/ sec)
	float       mMouseWheelSensitibity;  // マウスホイール係数 (/ frame)

	float       mYaw;                    // ヨー(オイラー角）
	float       mPitch;                  // ピッチ(オイラー角）

	float       mScreenWidth;            // スクリーン幅
	float       mScreenHeight;           // スクリーン高さ

	void        UpdateMouse(float frameDeltaTime);
	void        UpdateKey(float frameDeltaTime);

};