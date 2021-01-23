#pragma once
#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <input/input_manager.h>
#include <graphics/sprite_renderer.h>
#include <graphics/renderer_3d.h>
#include <graphics/font.h>
#include <graphics\primitive.h>
#include "input\sony_controller_input_manager.h"
#include "graphics\sprite.h"
#include "maths\math_utils.h"
#include "GameState.h"
#include <audio/audio_manager.h>
#include <input/input_manager.h>
#include "load_texture.h"
#include "input\sony_controller_input_manager.h"
#include "graphics\sprite.h"
#include "maths\math_utils.h"
#include "GameState.h"
#include <audio/audio_manager.h>
#include <input/input_manager.h>
#include "GameObject2.h"


class StatesBase
{
public:
	StatesBase();

	virtual GameState update(float frame_time, GameState state) = 0;
	virtual void Release() = 0;
	virtual void Init(GameState state) = 0;
	virtual void Render() = 0;

	void Releases();
	void rendererInit(gef::SpriteRenderer* spr, gef::Font* font, gef::InputManager* inp, gef::Renderer3D* ren, PrimitiveBuilder* primi, gef::AudioManager* audiomanager, gef::Platform& plat);

protected:
	gef::SpriteRenderer* sprite_renderer;

	gef::InputManager* input_manager;

	gef::Renderer3D* renderer_3d;

	PrimitiveBuilder* primitive_builder;

	gef::Font* font;

	gef::AudioManager* audio_manager;

	gef::Platform* platform_;

	Int32 UIsfx;
	Int32 pickup_voice;
	gef::VolumeInfo volume_info;
	bool playing;

};

