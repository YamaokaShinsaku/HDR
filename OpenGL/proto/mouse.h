#pragma once
#include<SDL/SDL.h>
#include"Math.h"

class Mouse
{
public:
	typedef enum MouseButtonState
	{
		MOUSE_BUTTON_OFF,
		MOUSE_BUTTON_PUSHDOWN,
		MOUSE_BUTTON_PRESSED,
		MOUSE_BUTTON_PULLUP
	}MouseButtonState;

	// インスタンス
	static Mouse& MouseInstance()
	{
		static Mouse MouseInstance;
		return MouseInstance;
	}

	~Mouse() {};

	void             SetRelativeMouseMode(bool value);
	const Vector2&   GetPosition() const { return mMousePos; }
	const Vector2& GetWheel() const { return mMouseWheel; }
	bool             GetButtonValue(int button) const;
	MouseButtonState GetButtonState(int button) const;
	void             OnMouseWheelEvent(SDL_Event& event);
	void             Update();

private:
	Mouse(); //シングルトン

	Vector2 mMousePos;
	Vector2 mMouseWheel;

	//ボタンのデータ
	Uint32 mCurrentButtons;
	Uint32 mPrevButtons;

	// 相対モード
	bool   mIsRelative;
};

#define MOUSE_INSTANCE Mouse::MouseInstance()
