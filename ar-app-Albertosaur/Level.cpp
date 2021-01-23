#include "Level.h"
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



Level::Level() :
	winTex(NULL),
	explosionTex(NULL),
	loseTex(NULL),
	pauseTex(NULL),
	startGame(false),
	isPaused(false),
	score(0),
	exploded(false),
	isColliding_(false),
	spawnArrow(false),
	numberOfTries(3),
	hasLost(false),
	hasWon(false)
{
}


Level::~Level()
{
}

void Level::Init(GameState state)
{
	
	primitive_renderer_ = new PrimitiveRenderer(*platform_);

	audio_manager->LoadMusic("audio_hero_Marching-Disorders_SIPML_K-04-41-01.wav", *platform_);
	ExplosionSE = audio_manager->LoadSample("esm_8bit_explosion_bomb_boom_blast_cannon_retro_old_school_classic_cartoon.wav", *platform_);
	redirectSE = audio_manager->LoadSample("zapsplat_multimedia_game_jump_24928.wav", *platform_);
	LaunchSE = audio_manager->LoadSample("zapsplat_warfare_mortar_projectile_launch_001_25231.wav", *platform_);

	pickup_voice = -1;

	volume_info.volume = 1;

	audio_manager->SetSampleVoiceVolumeInfo(ExplosionSE, volume_info);
	playing = false;

	SetupLights();

	// reset marker tracking
	AppData* dat = sampleUpdateBegin();
	smartTrackingReset();
	sampleUpdateEnd(dat);

	position = gef::Vector4(0, 0, -0.2);

	Archer.InitPrimBox(gef::Vector4(0.015, 0.015, 0.05), gef::Vector4(0, 0, 0.05), *platform_, 0);

	Arrow.InitPrimSphere(0.02, gef::Vector4(0, 0, 0.06), *platform_, 0);

	for(int i = 0; i < 5; i++)
		reboundCube[i].InitPrimBox(gef::Vector4(0.03, 0.005, 0.03), gef::Vector4(0, 0, 0.05), *platform_, i+1, gef::Vector4(0, 0, 0));
		
	winTex = CreateTextureFromPNG("VictoryScreen.png", *platform_);
	explosionTex = CreateTextureFromPNG("exp3.png", *platform_);
	pauseTex = CreateTextureFromPNG("PauseScreen.png", *platform_);
	loseTex = CreateTextureFromPNG("lostScreen.png", *platform_);
	
	WinScreen.set_texture(winTex);
	WinScreen.set_position(gef::Vector4(platform_->width()*0.5, platform_->height()*0.5, -0.8f));
	WinScreen.set_height(platform_->height());
	WinScreen.set_width(platform_->width());

	LoseScreen.set_texture(loseTex);
	LoseScreen.set_position(gef::Vector4(platform_->width()*0.5, platform_->height()*0.5, -0.8f));
	LoseScreen.set_height(platform_->height());
	LoseScreen.set_width(platform_->width());

	pauseScreen.set_texture(pauseTex);
	pauseScreen.set_position(gef::Vector4(platform_->width()*0.5, platform_->height()*0.5, -0.8f));
	pauseScreen.set_height(platform_->height());
	pauseScreen.set_width(platform_->width());

	explosion.set_texture(explosionTex);
	explosion.set_position(gef::Vector4(0,0,-0.99));
	explosion.set_height(256.0f);
	explosion.set_width(256.0f);
	
	Objective.InitModel("ring.scn", *platform_, gef::Vector4(0.2,0,0.1), gef::Vector4(0.005, 0.005, 0.005));

	Objective.WallId = 6;

	gef::Matrix44 ortho = platform_->OrthographicFrustum(-1,	1,-1, 1, -1, 1);

	velocity = gef::Vector4(0.0f, 0.0f, -2.0f);
	isColliding_ = false;
	spawnArrow = false;

	score = 0;
	exploded = false;
	numberOfTries = 3;
	hasLost = false;
	hasWon = false;

	colours[0] = primitive_builder->blue_material();
	colours[1] = primitive_builder->red_material();
	colours[2] = primitive_builder->green_material();
	colours[3] = primitive_builder->purple_material();
	colours[4] = primitive_builder->orange_material();
	colours[5] = primitive_builder->yellow_material();
}

void Level::Release()
{
	delete winTex;
	winTex = NULL;

	delete explosionTex;
	explosionTex = NULL;

	delete loseTex;
	loseTex = NULL;

	delete pauseTex;
	pauseTex = NULL;

	audio_manager->UnloadMusic();
}

GameState Level::update(float frame_time, GameState state)
{
	fps_ = 1.0f / frame_time;
	
	AppData* dat = sampleUpdateBegin();

	if (input_manager)
	{

		// controller input
		gef::SonyControllerInputManager* controller_manager = input_manager->controller_input();
		if (controller_manager)
		{
			const gef::SonyController* controller = input_manager->controller_input()->GetController(0);
			if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS)
			{
				startGame = true;
			}
		}
	}
	if (playing != true)
	{
		audio_manager->PlayMusic();
		playing = true;
	}

	// use the tracking library to try and find markers
	smartUpdate(dat->currentImage);

	if (startGame)
	{
		
		if(isPaused == false)
		{
			

			// read input devices

			if (input_manager)
			{

				// controller input
				gef::SonyControllerInputManager* controller_manager = input_manager->controller_input();
				if (controller_manager)
				{
					const gef::SonyController* controller = input_manager->controller_input()->GetController(0);
					if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE)
					{
						if (numberOfTries > 0)
						{
							spawnArrow = true;
							Arrow.setVelocity(gef::Vector4(0, -0.2, 0));
							Arrow.setPosition(gef::Vector4(0, 0, 0.06));
							Reset();
							numberOfTries--;
							if (LaunchSE != -1)
								pickup_voice = audio_manager->PlaySample(LaunchSE, false);
						}
						else
						{
							hasLost = true;
						}
					}
					else if (controller->buttons_pressed() & gef_SONY_CTRL_SELECT)
					{
						isPaused = true;
					}
				}
			}

			SetMarkerPos(&Archer, 0, frame_time);

			if (spawnArrow)
				SetMarkerPos(&Arrow, markerid, frame_time);

			for (int i = 0; i < 5; i++)
				SetMarkerPos(&reboundCube[i], i + 1, frame_time);

			SetMarkerPos(&Objective, 0, frame_time);

			for (int i = 0; i < 5; i++)
			{

				isColliding_ = isCollidingAABB(Arrow, reboundCube[i]);

				if (isColliding_)
				{
					if (Arrow.WallId == (reboundCube[i].WallId - 1) || Arrow.WallId == reboundCube[i].WallId)
					{
						Arrow.WallId = reboundCube[i].WallId;
						markerid = reboundCube[i].WallId;
						Arrow.setVelocity(gef::Vector4(0, -0.4, 0));

						if (reboundCube[i].hasBouncedOff == false)
						{
							Arrow.setPosition(gef::Vector4(0, 0, 0.06));
							reboundCube[i].hasBouncedOff = true;
							score += 50;
							if (redirectSE != -1)
								pickup_voice = audio_manager->PlaySample(redirectSE, false);
						}

					}
					else if (Arrow.WallId > reboundCube[i].WallId)
					{
						score += 100;
						//destroy wall
						if (ExplosionSE != -1)
							pickup_voice = audio_manager->PlaySample(ExplosionSE, false);

						explosion.set_position(ConvertToScreenSpace(Final_Proj_Matrix, viewMatrix, reboundCube[i], platform_->width(), platform_->height()));

						exploded = true;
						explosion.init();
						reboundCube[i].setPosition(gef::Vector4(3, 0, 0, 0));

					}
					else
					{
						////destroy ball
						explosion.set_position(ConvertToScreenSpace(Final_Proj_Matrix, viewMatrix, Arrow, platform_->width(), platform_->height()));

						explosion.set_position(gef::Vector4(explosion.position().x(), explosion.position().y() *0.8, -0.99));
						
						if (ExplosionSE != -1)
							pickup_voice = audio_manager->PlaySample(ExplosionSE, false);

						Arrow.setPosition(gef::Vector4(2, 0, 0));
						Arrow.setVelocity(gef::Vector4(0, 0, 0));
						explosion.init();
						exploded = true;
						if (numberOfTries == 0)
						{
							hasLost = true;
						}

					}
				}
			}

			if (exploded)
			{
				if (explosion.Animate(4, 4) != true)
				{
					exploded = false;

				}
			}

			hitObjective = isCollidingAABB(Arrow, Objective);

			if (hitObjective)
			{
				if (Arrow.WallId == (Objective.WallId - 1) || Arrow.WallId == Objective.WallId)
				{
					//victory screen
					if (Objective.hasBouncedOff == false)
					{
						score += 200;
						Objective.hasBouncedOff = true;
						hasWon = true;
					}
				}
			}
			if (hasWon || hasLost)
			{
				if (input_manager)
				{

					// controller input
					gef::SonyControllerInputManager* controller_manager = input_manager->controller_input();
					if (controller_manager)
					{
						const gef::SonyController* controller = input_manager->controller_input()->GetController(0);
						if (controller->buttons_pressed() & gef_SONY_CTRL_SELECT)
						{
							isPaused = false;
							hasWon = false;
							hasLost = false;
							Arrow.setPosition(gef::Vector4(2, 0, 0));
							numberOfTries = 3;
							Reset();
						}
						else if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE)
						{
							state = MENU;
							startGame = false;
							Reset();
						}
					}
				}
			}
			
		}
		else
		{
			if (input_manager)
			{

				// controller input
				gef::SonyControllerInputManager* controller_manager = input_manager->controller_input();
				if (controller_manager)
				{
					const gef::SonyController* controller = input_manager->controller_input()->GetController(0);
					if (controller->buttons_pressed() & gef_SONY_CTRL_SELECT)
					{
						isPaused = false;
					}
					else if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE)
					{
						state = MENU;
						startGame = false;
						Reset();
					}
				}
			}
		}
	}
	sampleUpdateEnd(dat);
	return state;
}

void Level::Reset()
{
	markerid = 0;
	Arrow.WallId = 0;
	for (int i = 0; i < 5; i++)
	{
		reboundCube[i].hasBouncedOff = false;
		reboundCube[i].setPosition(gef::Vector4(0, 0, 0.05));
	}
	Objective.hasBouncedOff = false;
	exploded = false;
	score = 0;
	
}


void Level::Render()
{
	AppData* dat = sampleRenderBegin();

	// Render the camera feed
	camera_aspect_ratio = CAMERA_WIDTH / CAMERA_HEIGHT;

	display_aspect_ratio = DISPLAY_WIDTH / DISPLAY_HEIGHT;

	vertical_image_scale = display_aspect_ratio / camera_aspect_ratio;

	cameraFeed.set_position(gef::Vector4(0, 0, 1, 1));
	cameraFeed.set_width(2.0f);
	cameraFeed.set_height(2 * vertical_image_scale);


	// SET THE PROJECTION MATRIX
	gef::Matrix44 ortho = platform_->OrthographicFrustum(-1, 1, -1, 1, -1, 1);
	sprite_renderer->set_projection_matrix(ortho);



	sprite_renderer->Begin(true);

	// DRAW CAMERA FEED 
	if (dat->currentImage)
	{
		camera_texture.set_texture(dat->currentImage->tex_yuv);
		cameraFeed.set_texture(&camera_texture);
		sprite_renderer->DrawSprite(cameraFeed);
	}

	if (exploded)
		sprite_renderer->DrawSprite(explosion); 

	sprite_renderer->End();

	//
	// Render 3D scene
	//
   
	// SET VIEW AND PROJECTION MATRIX HERE
	gef::Matrix44 ProjMatrix = platform_->PerspectiveProjectionFov(SCE_SMART_IMAGE_FOV, (float)SCE_SMART_IMAGE_WIDTH / (float)SCE_SMART_IMAGE_HEIGHT, 0.1, 10);

	gef::Matrix44 ScaleProj;
	ScaleProj.Scale(gef::Vector4(1, vertical_image_scale, 1, 1));

	Final_Proj_Matrix = ProjMatrix * ScaleProj;

	viewMatrix;
	viewMatrix.SetIdentity();

	renderer_3d->set_view_matrix(viewMatrix);
	renderer_3d->set_projection_matrix(Final_Proj_Matrix);

	// Begin rendering 3D meshes, don't clear the frame buffer
	renderer_3d->Begin(false);

	// DRAW 3D MESHES
	if (startGame)
	{
		if (isPaused == false)
		{
			if (Archer.isInView)
			{
				renderer_3d->DrawMesh(Archer);
			}

			for (int i = 0; i < 5; i++)
			{
				if (reboundCube[i].isInView)
				{
					renderer_3d->set_override_material(&colours[i]);
					renderer_3d->DrawMesh(reboundCube[i]);
					renderer_3d->set_override_material(NULL);
				}
			}

			if (Arrow.isInView)
			{
				renderer_3d->set_override_material(&colours[Arrow.WallId]);
				renderer_3d->DrawMesh(Arrow);
				renderer_3d->set_override_material(NULL);
			}

			if (Objective.isInView)
			{
				renderer_3d->set_override_material(&primitive_builder->yellow_material());
				renderer_3d->DrawMesh(Objective);
				renderer_3d->set_override_material(NULL);
			}
		}
	}

	renderer_3d->End();


	RenderOverlay(*platform_);
	

	sampleRenderEnd();
}
void Level::SetMarkerPos(GameObject2* object, int Markerid, float frame_time)
{
	if (sampleIsMarkerFound(Markerid))
	{

		// marker is being tracked, get its transform
		gef::Matrix44 marker_transform;
		sampleGetTransform(Markerid, &marker_transform);

		// set the transform of the 3D mesh instance to draw on
		// top of the marker
		object->setTranslation(marker_transform);
		object->Update(frame_time);
		object->isInView = true;
	}
	else
		object->isInView = false;
}

bool Level::isCollidingSphere(const gef::MeshInstance & meshinstance1, const gef::MeshInstance & meshinstance2)
{
	gef::Sphere sphere1 = meshinstance1.mesh()->bounding_sphere();
	gef::Sphere sphere2 = meshinstance2.mesh()->bounding_sphere();

	gef::Sphere sphere1_transform = sphere1.Transform(meshinstance1.transform());
	gef::Sphere sphere2_transform = sphere2.Transform(meshinstance2.transform());

	gef::Vector4 offset = sphere1_transform.position() - sphere2_transform.position();
	float dis = std::sqrtf(offset.x() * offset.x() + offset.y() * offset.y() + offset.z() * offset.z());
	float combined_radii = sphere1_transform.radius() + sphere2_transform.radius();

	return dis < combined_radii;
}

bool Level::isCollidingAABB(const gef::MeshInstance & meshinstance1, const gef::MeshInstance & meshinstance2)
{
	gef::Aabb Aabb1 = meshinstance1.mesh()->aabb();
	gef::Aabb Aabb2 = meshinstance2.mesh()->aabb();

	gef::Matrix44 local_copied1 = meshinstance1.transform();
	gef::Matrix44 local_copied2 = meshinstance2.transform();

	gef::Matrix44 scale_copied;
	scale_copied.SetIdentity();
	scale_copied.Scale(gef::Vector4(0.003, 0.003, 0.003));
	
	gef::Aabb Aabb1_transformed = Aabb1.Transform(local_copied1 * scale_copied);
	gef::Aabb Aabb2_transformed = Aabb2.Transform(local_copied2 * scale_copied);

	if (Aabb1_transformed.max_vtx().x() > Aabb2_transformed.min_vtx().x() &&
		Aabb1_transformed.min_vtx().x() < Aabb2_transformed.max_vtx().x() &&
		Aabb1_transformed.max_vtx().y() > Aabb2_transformed.min_vtx().y() &&
		Aabb1_transformed.min_vtx().y() < Aabb2_transformed.max_vtx().y() &&
		Aabb1_transformed.max_vtx().z() > Aabb2_transformed.min_vtx().z() &&
		Aabb1_transformed.min_vtx().z() < Aabb2_transformed.max_vtx().z())
	{
		return true;
	}

	return false;
}

gef::Vector4 Level::ConvertToScreenSpace(const gef::Matrix44 & projection, const gef::Matrix44& view, GameObject2 Object, float width, float height)
{
	
		gef::Matrix44 vp = view * projection;

		/// convert world space position (spawn) to ndc
		gef::Vector4 pp = Object.WorldTransfrom.GetTranslation();
		pp.set_w(1.f);
		pp = pp.TransformW(vp);

		/// convert ndc to screen position
		pp = pp / pp.w();
		return gef::Vector4((pp.x() + 1) / 2.f * width, (1 - pp.y()) / 2.f * height, -0.79f, 0.0f);
	
}

void Level::DrawAxes(PrimitiveRenderer* primitive_renderer, const gef::Matrix44& transform, const float size)
{
	gef::Vector4 x_axis(1.0f, 0.0f, 0.0f);
	gef::Vector4 y_axis(0.0f, 1.0f, 0.0f);
	gef::Vector4 z_axis(0.0f, 0.0f, 1.0f);
	gef::Vector4 origin(0.0f, 0.0, 0.0f);

	x_axis = x_axis * size;
	y_axis = y_axis * size;
	z_axis = z_axis * size;

	x_axis = x_axis.Transform(transform);
	y_axis = y_axis.Transform(transform);
	z_axis = z_axis.Transform(transform);
	origin = origin.Transform(transform);

	primitive_renderer->AddLine(origin, x_axis, gef::Colour(1.0f, 0.0f, 0.0f));
	primitive_renderer->AddLine(origin, y_axis, gef::Colour(0.0f, 1.0f, 0.0f));
	primitive_renderer->AddLine(origin, z_axis, gef::Colour(0.0f, 0.0f, 1.0f));
}

void Level::RenderOverlay(gef::Platform& platform_)
{
	//
	// render 2d hud on top
	//
	

	proj_matrix2d = platform_.OrthographicFrustum(0.0f, platform_.width(), 0.0f, platform_.height(), -1.0f, 1.0f);
	sprite_renderer->set_projection_matrix(proj_matrix2d);

	sprite_renderer->Begin(false);

	if (startGame)
	{
		

		if (hasWon)
			sprite_renderer->DrawSprite(WinScreen);

		if (hasLost)
			sprite_renderer->DrawSprite(LoseScreen);

		if (isPaused)
			sprite_renderer->DrawSprite(pauseScreen);
		
		if (exploded)
			sprite_renderer->DrawSprite(explosion);
		
		DrawHUD();
	}

	

	

	sprite_renderer->End();

}

void Level::SetupLights()
{
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-300.0f, -500.0f, 100.0f));

	gef::Default3DShaderData& default_shader_data = renderer_3d->default_shader_data();
	default_shader_data.set_ambient_light_colour(gef::Colour(0.5f, 0.5f, 0.5f, 1.0f));
	default_shader_data.AddPointLight(default_point_light);
}

void Level::DrawHUD()
{
	if (font)
	{
		
				
			if(hasLost)
			{
				
				font->RenderText(sprite_renderer, gef::Vector4(480.0f, 172.0f, -0.9), 2.0f, 0xffffffff, gef::TJ_CENTRE, "Score: %i", score);
				font->RenderText(sprite_renderer, gef::Vector4(480.0f, 238.0f, -0.9), 2.0f, 0xffffffff, gef::TJ_CENTRE, "Circle to head back to Main Menu");
				font->RenderText(sprite_renderer, gef::Vector4(480.0f, 280.0f, -0.9), 2.0f, 0xffffffff, gef::TJ_CENTRE, "Select to play again");
			}
			else if (hasWon)
			{
				
				font->RenderText(sprite_renderer, gef::Vector4(480.0f, 172.0f, -0.9), 2.0f, 0xffffffff, gef::TJ_CENTRE, "Score: %i", score);
				font->RenderText(sprite_renderer, gef::Vector4(480.0f, 238.0f, -0.9), 2.0f, 0xffffffff, gef::TJ_CENTRE, "Circle to head back to Main Menu");
				font->RenderText(sprite_renderer, gef::Vector4(480.0f, 280.0f, -0.9), 2.0f, 0xffffffff, gef::TJ_CENTRE, "Select to play again");
			}
			else
			{
				font->RenderText(sprite_renderer, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
				font->RenderText(sprite_renderer, gef::Vector4(50.0f, 10.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Score: %i", score);
				font->RenderText(sprite_renderer, gef::Vector4(200.0f, 10.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Number of Tries: %i", numberOfTries);
			}
			if (isPaused)
			{
				
				font->RenderText(sprite_renderer, gef::Vector4(480.0f, 238.0f, -0.9), 2.0f, 0xffffffff, gef::TJ_CENTRE, "Circle to head back to Main Menu");
				font->RenderText(sprite_renderer, gef::Vector4(480.0f, 280.0f, -0.9), 2.0f, 0xffffffff, gef::TJ_CENTRE, "Select to continue");

			}
		
	}
}
