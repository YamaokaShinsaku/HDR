#include"mouse.h"
#include <iostream>

Mouse::Mouse()
{
	mMousePos = Vector2(0, 0);
	mCurrentButtons = 0;
	mPrevButtons = 0;
	mIsRelative = false;
}

void Mouse::SetRelativeMouseMode(bool value)
{
	SDL_bool set = value ? SDL_TRUE : SDL_FALSE;
	SDL_SetRelativeMouseMode(set);
	mIsRelative = value;
}

bool Mouse::GetButtonValue(int button) const
{
	return button & mCurrentButtons;
}

Mouse::MouseButtonState Mouse::GetButtonState(int button) const
{

	if (button & mCurrentButtons)
	{
		return (button & mPrevButtons) ? MOUSE_BUTTON_PRESSED : MOUSE_BUTTON_PUSHDOWN;
	}

	return (button & mPrevButtons) ? MOUSE_BUTTON_PULLUP : MOUSE_BUTTON_OFF;
}

void Mouse::OnMouseWheelEvent(SDL_Event& event)
{
	switch (event.type)
	{
	case SDL_MOUSEWHEEL:
		mMouseWheel = Vector2(
			static_cast<float>(event.wheel.x),
			static_cast<float>(event.wheel.y)
		);
		break;

	default:
		break;
	}
}

// 入力処理の更新。レンダリングループの先頭で1回だけ呼ぶようにする
void Mouse::Update()
{
	mPrevButtons = mCurrentButtons;

	int x = 0, y = 0;
	if (mIsRelative)
	{
		mCurrentButtons = SDL_GetRelativeMouseState(&x, &y);
	}
	else
	{
		mCurrentButtons = SDL_GetMouseState(&x, &y);
	}
	mMousePos.x = static_cast<float>(x);
	mMousePos.y = static_cast<float>(y);

	mMouseWheel = Vector2::Zero;
}
