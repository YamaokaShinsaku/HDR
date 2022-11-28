#pragma once
#include <SDL/SDL.h>

// 入力クラス
class Input
{
public:

	enum KEY_STATE_ENUM             
	{
		KEY_STATE_OFF = 0,    // 押されていない
		KEY_STATE_PUSHDOWN,   // 押下
		KEY_STATE_PRESSED,    // 押され続けている
		KEY_STATE_PULLUP      // 押離し
	};

	static Input& InputInstance()                  // インスタンス
	{
		static Input InputInstance;
		return InputInstance;
	}
	
	~Input() {};

	void Update();                                 // 入力状態の更新
	char GetInput(Uint8 SDL_SCANCODE_key);         // あるキーの入力状態の取得

	bool IsKeyPressed(Uint8 SDL_SCANCODE_key);     // あるキーが押され続けられている状態か？
	bool IsKeyPullup(Uint8 SDL_SCANCODE_key);      // あるキーが押し上げられたか？
	bool IsKeyPushdown(Uint8 SDL_SCANCODE_key);    // あるキーは押され続けられているか？
	bool IsKeyOff(Uint8 SDL_SCANCODE_key);         // あるキーは押されていないか？

private:
	Input(); // シングルトン

	char mKey[256];                                // 全キー状態取得用
	char mKeyState[2][256];                        // キー状態 [現在/過去フレーム][全キーコード]

	int  mNowKey, mPrevKey;                        // 現フレーム、過去フレームのインデックス

};

#define INPUT_INSTANCE Input::InputInstance()
