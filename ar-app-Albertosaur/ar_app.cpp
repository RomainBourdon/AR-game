#include "ar_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/font.h>
#include <input/touch_input_manager.h>
#include <maths/vector2.h>
#include <input/input_manager.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>
#include <graphics/render_target.h>

#include <sony_sample_framework.h>
#include <sony_tracking.h>
#include <input\sony_controller_input_manager.h>


ARApp::ARApp(gef::Platform& platform) :
	Application(platform),
	input_manager_(NULL),
	sprite_renderer_(NULL),
	font_(NULL),
	renderer_3d_(NULL),
	primitive_builder_(NULL)
{
}

void ARApp::Init()
{
	input_manager_ = gef::InputManager::Create(platform_);
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	renderer_3d_ = gef::Renderer3D::Create(platform_);
	primitive_builder_ = new PrimitiveBuilder(platform_);
	primitive_renderer_ = new PrimitiveRenderer(platform_);
	//initialize audio manager
	audio_manager_ = gef::AudioManager::Create();

	InitFont();


	// initialise sony framework
	sampleInitialize();
	smartInitialize();

	statemachine.init(sprite_renderer_, font_, input_manager_, renderer_3d_, primitive_builder_, audio_manager_, platform_);
	
}

void ARApp::CleanUp()
{
	delete primitive_renderer_;
	primitive_renderer_ = NULL;

	delete primitive_builder_;
	primitive_builder_ = NULL;

	smartRelease();
	sampleRelease();

	CleanUpFont();
	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete input_manager_;
	input_manager_ = NULL;

	statemachine.Release();
}

bool ARApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;

	input_manager_->Update();

	statemachine.Update(frame_time);
	
	return true;
}



void ARApp::Render()
{
	statemachine.Render();
}



void ARApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void ARApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}



