#include "StateMachine.h"
#include <input/sony_controller_input_manager.h>




StateMachine::StateMachine() 
{
}

StateMachine::~StateMachine()
{
}

void StateMachine::Release()
{
	//call the cleanup of the current state
	currentstate->Release();
}

void StateMachine::init(gef::SpriteRenderer * spr, gef::Font* fon, gef::InputManager * inp, gef::Renderer3D * ren, PrimitiveBuilder * primi, gef::AudioManager* audio, gef::Platform &plat)
{
	//set the first state to be Menu screen
	states = LEVEL;
	currentstate = &levelstate;
	currentstate->rendererInit(spr, fon, inp, ren, primi, audio, plat);
	states = MENU;
	currentstate = &menustate;
	currentstate->rendererInit(spr, fon, inp, ren, primi, audio, plat);
	states = HOWTO;
	currentstate = &howtostate;
	currentstate->rendererInit(spr, fon, inp, ren, primi, audio, plat);

	states = MENU;
	currentstate = &menustate;
	currentstate->Init(states);
}

void StateMachine::UpdateGameStateMachine(float frame_time)
{
	//save the old currentstate pointer
	oldstate = currentstate;

	//switch the states
	switch (states)
	{
	case MENU:
		currentstate = &menustate;
		
		break;
	case LEVEL:
		currentstate = &levelstate;
	
		break;
	case HOWTO:
		currentstate = &howtostate;

		break;
	
	default:
		break;
	}

	//clean up the state we were on previously
	oldstate->Release();

	
	currentstate->Init(states);
	

}

void StateMachine::Update(float frame_time)
{
	//set the null variable onstate to equal the return value of update
	onstate = currentstate->update(frame_time, states);
	//save the Gamestate we were on previously
	oldGamestate = states;
	//check if the Gamestates dont match
	if (states != onstate)
	{
		states = onstate;
		UpdateGameStateMachine(frame_time);

	}


}

void StateMachine::Render()
{
	//render the current state

	currentstate->Render();
}
