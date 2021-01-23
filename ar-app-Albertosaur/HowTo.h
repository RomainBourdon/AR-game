#pragma once
#include "StatesBase.h"
class HowTo :
	public StatesBase
{
public:
	HowTo();
	~HowTo();

	void Init(GameState state);
	void Release();
	GameState update(float frame_time, GameState state);
	void Render();

	gef::Sprite background;

	gef::Texture* Background;
};

