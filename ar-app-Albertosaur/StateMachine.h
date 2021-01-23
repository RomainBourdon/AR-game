#pragma once
#include <input/input_manager.h>
#include "StatesBase.h"
#include "Level.h"
#include "Menu.h"
#include "HowTo.h"

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	void init(gef::SpriteRenderer* spri, gef::Font* fon, gef::InputManager* inp, gef::Renderer3D* ren, PrimitiveBuilder* primi, gef::AudioManager* audio, gef::Platform &plat);
	void UpdateGameStateMachine(float frame_time);
	void Update(float frame_time);
	void Render();
	void Release();



protected:
	GameState states;
	GameState onstate;
	GameState oldGamestate;

	Menu menustate;
	Level levelstate;
	HowTo howtostate;

	StatesBase* currentstate;
	StatesBase* oldstate;

	float fps_;
};

