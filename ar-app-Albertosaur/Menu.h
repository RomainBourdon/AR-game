#pragma once
#include "StatesBase.h"
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}
class Menu :
	public StatesBase
{
public:
	Menu();
	~Menu();
	void Init(GameState state);
	void Release();
	GameState update(float frame_time, GameState state);
	bool TouchWithinButton(gef::Vector2 &screen_position, gef::Sprite* button);
	void Render();

protected:

	gef::Texture* LevelButtonTex;
	gef::Texture* HowToButtonTex;

	gef::Sprite background;
	gef::Sprite LevelButton;
	gef::Sprite HowToButton;

	gef::Texture* Background;

	Int32 UiClick;
};

