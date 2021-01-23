#include "HowTo.h"



HowTo::HowTo() :
	Background(NULL)
{
}


HowTo::~HowTo()
{
}

void HowTo::Init(GameState state)
{
	
	Background = CreateTextureFromPNG("HowToBackground.png", *platform_);

	background.set_texture(Background);
	background.set_position(gef::Vector4(platform_->width()*0.5, platform_->height()*0.5, -0.99f));
	background.set_height(platform_->height());
	background.set_width(platform_->width());

}

void HowTo::Release()
{
	delete Background;
	Background = NULL;;
}

GameState HowTo::update(float frame_time, GameState state)
{
	if (input_manager)
	{
		gef::SonyControllerInputManager* controller_manager = input_manager->controller_input();
		if (controller_manager)
		{
			const gef::SonyController* controller = input_manager->controller_input()->GetController(0);
			if (controller->buttons_pressed())
			{
				state = MENU;
			}
		}
	}
	return state;
}

void HowTo::Render()
{
	sprite_renderer->Begin();

	sprite_renderer->DrawSprite(background);

	sprite_renderer->End();
}
