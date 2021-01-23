#include "Menu.h"
#include "input/touch_input_manager.h"


Menu::Menu() :
	LevelButtonTex(NULL),
	HowToButtonTex(NULL),
	Background(NULL)
{
}


Menu::~Menu()
{
}

void Menu::Init(GameState state)
{
	//create audio manager
	//load music
	audio_manager->LoadMusic("audio_hero_Playroom_SIPML_K-0349.wav", *platform_);

	UiClick = audio_manager->LoadSample("zapsplat_multimedia_button_click_fast_plastic_49161.wav", *platform_);

	pickup_voice = -1;

	playing = false;

	//touch manager initialization
	if (input_manager && input_manager->touch_manager() && (input_manager->touch_manager()->max_num_panels() > 0))
		input_manager->touch_manager()->EnablePanel(0);

	Background = CreateTextureFromPNG("MenuBackground.png", *platform_);
	LevelButtonTex = CreateTextureFromPNG("LevelButton.png", *platform_);
	HowToButtonTex = CreateTextureFromPNG("HowToButton.png", *platform_);

	background.set_texture(Background);
	background.set_position(gef::Vector4(platform_->width()*0.5, platform_->height()*0.5, -0.99f));
	background.set_height(platform_->height());
	background.set_width(platform_->width());

	LevelButton.set_texture(LevelButtonTex);
	LevelButton.set_position(gef::Vector4(platform_->width()*0.5f, platform_->height()*0.5f - 80, -0.99f));
	LevelButton.set_height(80.0f);
	LevelButton.set_width(320.0f);

	HowToButton.set_texture(HowToButtonTex);
	HowToButton.set_position(gef::Vector4(platform_->width()*0.5f, platform_->height()*0.5f + 32, -0.99f));
	HowToButton.set_height(80.0f);
	HowToButton.set_width(320.0f);

	
}

void Menu::Release()
{
	delete LevelButtonTex;
	LevelButtonTex = NULL;

	delete HowToButtonTex;
	HowToButtonTex = NULL;

	delete Background;
	Background = NULL;

}

GameState Menu::update(float frame_time, GameState state)
{

	if (playing != true)
	{
		volume_info.volume = 1;
		audio_manager->GetMusicVolumeInfo(volume_info);
		audio_manager->PlayMusic();
		playing = true;
	}

	if (input_manager)
	{
		const gef::SonyController* controller = input_manager->controller_input()->GetController(0);
		const gef::TouchInputManager * touch_manager = input_manager->touch_manager();

		// get the active touches for this panel
		const gef::TouchContainer& panel_touches = touch_manager->touches(0);

		if (panel_touches.size() > 0)
		{
			// just grabbing the first touch for simplicity here
			// normally we go through all active touches and check the id
			gef::Touch touch = panel_touches.front();

			// only process this touch if it is NEW or ACTIVE
			if ((touch.type == gef::TT_NEW))
			{
				gef::Vector2 screen_position = touch.position;
				if (TouchWithinButton(screen_position, &LevelButton))
				{
					state = LEVEL;
					if (UiClick != -1)
						pickup_voice = audio_manager->PlaySample(UiClick, false);

					audio_manager->UnloadMusic();
				}
				if (TouchWithinButton(screen_position, &HowToButton))
				{
					state = HOWTO;
					if (UiClick != -1)
						pickup_voice = audio_manager->PlaySample(UiClick, false);
				}
			}
			else if (touch.type == gef::TT_ACTIVE)
			{

			}
			else
			{

			}
		}
	}

	return state;
}

bool Menu::TouchWithinButton(gef::Vector2 &screen_position, gef::Sprite* button)
{
	return (screen_position.y - button->position().y()) >= 0 && (screen_position.y - button->position().y()) <= button->height() && (screen_position.x - button->position().x()) >= 0 && (screen_position.x - button->position().x()) <= button->width();
}

void Menu::Render()
{
	//start sprite renderer for text and sprites
	sprite_renderer->Begin();

	sprite_renderer->DrawSprite(background);
	sprite_renderer->DrawSprite(LevelButton);
	sprite_renderer->DrawSprite(HowToButton);

	font->RenderText(
			sprite_renderer,
			gef::Vector4(platform_->width()*0.5f, platform_->height()*0.1f, -0.99f),
			2.0f,
			0xffffffff,
			gef::TJ_CENTRE,
			"Skill Shot");
	sprite_renderer->End();
}
