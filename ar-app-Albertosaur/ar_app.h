#ifndef _RENDER_TARGET_APP_H
#define _RENDER_TARGET_APP_H

#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include <platform/vita/graphics/texture_vita.h>
#include "primitive_builder.h"
#include "GameObject2.h"
#include "primitive_renderer.h"
#include "StateMachine.h"

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class Renderer3D;
	class Mesh;
	class RenderTarget;
	class TextureVita;
	class InputManager;
	class PrimitiveRenderer;
}


class ARApp : public gef::Application
{
public:
	ARApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
private:
	void InitFont();
	void CleanUpFont();
	

	gef::InputManager* input_manager_;
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	class gef::Renderer3D* renderer_3d_;
	PrimitiveBuilder* primitive_builder_;
	PrimitiveRenderer* primitive_renderer_;
	gef::AudioManager* audio_manager_;

	float fps_;

	StateMachine statemachine;

};




#endif // _RENDER_TARGET_APP_H