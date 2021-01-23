
#pragma once
#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include <platform/vita/graphics/texture_vita.h>
#include "primitive_builder.h"
#include "GameObject2.h"
#include "primitive_renderer.h"
#include "AnimatedSprite.h"


// Vita AR includes
#include <camera.h>
#include <gxm.h>
#include <motion.h>
#include <libdbg.h>
#include <libsmart.h>

#include "StatesBase.h"

class Level :
	public StatesBase
{
public:
	Level();
	~Level();

	void Init(GameState state);
	void Release();
	GameState update(float frame_time, GameState state);
	void Reset();
	void Render();
	
private:
	void RenderOverlay(gef::Platform& plat);
	void SetupLights();
	void DrawHUD();

	void SetMarkerPos(GameObject2* object, int Markerid, float frame_time);
	bool isCollidingSphere(const gef::MeshInstance& meshinstance1, const gef::MeshInstance& meshinstance2);
	bool isCollidingAABB(const gef::MeshInstance& meshinstance1, const gef::MeshInstance& meshinstance2);
	gef::Vector4 ConvertToScreenSpace(const gef::Matrix44 & projection, const gef::Matrix44& view, GameObject2 Object, float width, float height);

	void DrawAxes(PrimitiveRenderer* primitive_renderer, const gef::Matrix44& transform, const float size);

	float fps_;
	PrimitiveRenderer* primitive_renderer_;

	gef::Sprite cameraFeed;

	gef::TextureVita camera_texture;

	GameObject2 Archer;
	GameObject2 reboundCube[5];
	GameObject2 Arrow;
	GameObject2 Objective;

	int markerid;
	int score;
	bool exploded;

	gef::Vector4 velocity;
	gef::Vector4 position;

	bool isColliding_;
	bool hitObjective;
	float camera_aspect_ratio;
	float display_aspect_ratio;
	float vertical_image_scale;
	bool spawnArrow;

	gef::Sprite WinScreen;
	gef::Sprite LoseScreen;
	gef::Sprite pauseScreen;

	AnimatedSprite explosion;

	gef::Texture* winTex;
	gef::Texture* explosionTex;
	gef::Texture* loseTex;
	gef::Texture* pauseTex;


	gef::Matrix44 Final_Proj_Matrix;
	gef::Matrix44 viewMatrix;
	bool startGame;
	bool isPaused;
	bool hasLost;
	bool hasWon;
	int numberOfTries;

	gef::Material colours[6];
	gef::Matrix44 proj_matrix2d;

	Int32 redirectSE;
	Int32 ExplosionSE;
	Int32 LaunchSE;
};

