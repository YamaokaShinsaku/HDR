#include "FlyCamera.h"

const float initZoomDegree  =  45.0f ; // 初期ズーム画角(degree)
const float initMoveSpeed   =   5.5f ; // 初期移動速度 ( / sec単位)
const float initSensitibity =  0.25f ; // 初期ホイールスクロール感度( / frame )
const float initYawDegree   = -90.0f ; // 初期回転位置 (x軸から-90度移動 z軸反対側)

const float pitchMaxDegree  =  89.0f ; // カメラピッチ最高角度(degree)
const float pitchMinDegree  = -89.0f ; // カメラピッチ最低角度(degree)

const float zoomMaxDegree   =  90.0f ; // ズーム画角最大角度(ズームアウト)
const float zoomMinDegree   =   1.0f ; // ズーム画角最小角度(ズームイン)


FlyCamera::FlyCamera(Vector3& cameraPos)
	: mCameraPos(cameraPos)                    ,                     
  	  mCameraFront(Vector3(0.0f, 0.0f, -1.0f)) ,   
	  mCameraUp(Vector3(0.0f, 1.0f, 0.0f))     ,

	  mZoom(Math::ToRadians(initZoomDegree))   ,
	  mCameraMoveSpeed(initMoveSpeed)          ,
	  mMouseWheelSensitibity(initSensitibity)  ,
	  mYaw(Math::ToRadians(initYawDegree))     ,
	  mPitch(0.0f)                             ,
	  mScreenWidth(1024.0f)                    ,
	  mScreenHeight(768.0f)
{
	mViewMatrix       = Matrix4::CreateLookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);
	mProjectionMatrix = Matrix4::CreatePerspectiveFOV(mZoom, mScreenWidth, mScreenHeight, 0.1f, 1000.0f);
}


// カメラの更新処理
void FlyCamera::UpdateCamera(float frameDeltaTime)
{
	UpdateMouse(frameDeltaTime); // ヨーピッチ・ズームの変更
	UpdateKey(frameDeltaTime);   // カメラ位置

	mProjectionMatrix = Matrix4::CreatePerspectiveFOV(mZoom, mScreenWidth, mScreenHeight, 0.1f, 1000.0f);
    mViewMatrix       = Matrix4::CreateLookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);

}

// ヨーピッチ・ズームレベルの変更
void FlyCamera::UpdateMouse(float frameDeltaTime)
{
	// カメラのヨーピッチを変更
	Vector2 mouse = MOUSE_INSTANCE.GetPosition();
	static float sensitivity = 0.2f;
	float xoffset = mouse.x * sensitivity * frameDeltaTime;
	float yoffset = mouse.y * sensitivity * frameDeltaTime;

	mYaw -= xoffset;
	mPitch -= yoffset;

	if (mPitch > Math::ToRadians(pitchMaxDegree))
	{
		mPitch = Math::ToRadians(pitchMaxDegree);
	}
	if (mPitch < Math::ToRadians(pitchMinDegree))
	{
		mPitch = Math::ToRadians(pitchMinDegree);
	}

	// カメラのヨーピッチからカメラの前方ベクトルを求める
	Vector3 direction;
	direction.x = cosf(mYaw) * cos(mPitch);
	direction.y = sinf(mPitch);
	direction.z = sinf(mYaw) * cosf(mPitch);
	direction.Normalize();

	// カメラ前方ベクトル
	mCameraFront = direction;

	// Zoom係数
	Vector2 wheel = MOUSE_INSTANCE.GetWheel();
	mZoom += wheel.y * frameDeltaTime;

	// 最大・最小ズーム角にクランプする
	if (Math::ToDegrees(mZoom) < zoomMinDegree)
	{
		mZoom = Math::ToRadians(zoomMinDegree);
	}
	if (Math::ToDegrees(mZoom) > zoomMaxDegree)
	{
		mZoom = Math::ToRadians(zoomMaxDegree);
	}
}

// キーボード操作処理
void FlyCamera::UpdateKey(float frameDeltaTime)
{
	// カメラの前方移動速度と右方向ベクトルの取得
	const float cameraSpeed = mCameraMoveSpeed * frameDeltaTime;
	Vector3 cameraRightVec = Vector3::Cross(mCameraUp, mCameraFront);

	// キー入力からカメラの位置の計算する
	if (INPUT_INSTANCE.GetInput(SDL_SCANCODE_W) == Input::KEY_STATE_PRESSED)
	{
		mCameraPos += cameraSpeed * mCameraFront;
	}
	if (INPUT_INSTANCE.GetInput(SDL_SCANCODE_S) == Input::KEY_STATE_PRESSED)
	{
		mCameraPos -= cameraSpeed * mCameraFront;
	}
	if (INPUT_INSTANCE.GetInput(SDL_SCANCODE_D) == Input::KEY_STATE_PRESSED)
	{
		mCameraPos += cameraSpeed * cameraRightVec;
	}
	if (INPUT_INSTANCE.GetInput(SDL_SCANCODE_A) == Input::KEY_STATE_PRESSED)
	{
		mCameraPos -= cameraSpeed * cameraRightVec;
	}
}

