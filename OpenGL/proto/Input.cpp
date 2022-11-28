#include "Input.h"
#include <memory.h>

Input::Input()
{
	mNowKey = 0;
	mPrevKey = 1;

	for (int i = 0; i < 256; i++)
	{
		mKey[i] = 0;
		mKeyState[mNowKey][i] = KEY_STATE_OFF;
		mKeyState[mPrevKey][i] = KEY_STATE_OFF;
	}
}

void Input::Update()
{
	// 現在のキーと1つ前のキーの状態を入れ替える  0を１に、1を0に
	mNowKey ^= 1;
	mPrevKey = mNowKey ^ 1;

	//キー状態取得
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	memcpy(mKeyState[mNowKey], keys, sizeof(char) * 256);

	for (int i = 0; i < 256; i++)
	{
		char nowInput  = mKeyState[mNowKey][i];
		char prevInput = mKeyState[mPrevKey][i];

		if (nowInput) //現在押されている
		{
			//前のフレームも押されていた？
			if (prevInput)
			{
				mKey[i] = KEY_STATE_PRESSED; // 押されっぱなし
			}
			else {
				mKey[i] = KEY_STATE_PUSHDOWN;// 今押された
			}

		}
		else // 現在キーは押されていない
		{
			// 前のフレームで押されていた？
			if (prevInput)
			{
				mKey[i] = KEY_STATE_PULLUP;
			}
			else {
				mKey[i] = KEY_STATE_OFF;
			}
		}
	}
}

char Input::GetInput(Uint8 SDL_SCANCODE_key)
{
	return mKey[SDL_SCANCODE_key];
}

bool Input::IsKeyPressed(Uint8 SDL_SCANCODE_key)
{
	return mKey[SDL_SCANCODE_key] == KEY_STATE_PRESSED;
}

bool Input::IsKeyPullup(Uint8 SDL_SCANCODE_key)
{
	return mKey[SDL_SCANCODE_key] == KEY_STATE_PULLUP;
}

bool Input::IsKeyPushdown(Uint8 SDL_SCANCODE_key)
{
	return mKey[SDL_SCANCODE_key] == KEY_STATE_PUSHDOWN;
}

bool Input::IsKeyOff(Uint8 SDL_SCANCODE_key)
{
	return mKey[SDL_SCANCODE_key] == KEY_STATE_OFF;
}
