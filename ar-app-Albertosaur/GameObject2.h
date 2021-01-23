#pragma once
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <maths/vector4.h>
#include <maths/matrix44.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include <system/application.h>
#include "primitive_builder.h"
#include <system/platform.h>
#include <system/application.h>
#include <graphics/scene.h>

namespace gef
{
	class Mesh;
	class Scene;
	class Skeleton;
}

class GameObject2 : public gef::MeshInstance
{

public:
	GameObject2();


	void InitModel(const char* model_name, gef::Platform& platform, gef::Vector4 pos = gef::Vector4(0.0f, 0.0f, 0.0f), gef::Vector4 scaling = gef::Vector4(1.0f, 1.0f, 1.0f), gef::Vector4 rotating = gef::Vector4(0.0f, 0.0f, 0.0f));
	void InitPrimBox(gef::Vector4 size, gef::Vector4 positions, gef::Platform& platform, int wallID, gef::Vector4 rotations = gef::Vector4(0, 0, 0));
	void InitPrimSphere(float radius, gef::Vector4 positions, gef::Platform& platform, int ID);
	void InitPrimPlane(gef::Vector4 size, gef::Vector4 positions, gef::Platform& platform, int wallID, gef::Vector4 rotations = gef::Vector4(0, 0, 0));

	void setPosition(gef::Vector4 positions) { position = positions; };
	void setScales(gef::Vector4 scale_) { scale = scale_; };
	void setRotations(gef::Vector4 rotate) { rotations = rotate; };
	void setTranslation(gef::Matrix44 transform) {
		markerTransform.SetIdentity();
		markerTransform = transform;
	};
	void setVelocity(gef::Vector4 vel) { velocity = vel; };

	gef::Vector4 getVelocity() { return velocity; };
	gef::Vector4 getPosition() { return position; };
	gef::Vector4 getRotation() { return rotations; };
	gef::Vector4 getScale() { return scale; };

	gef::Mesh* GetFirstMesh(gef::Scene* scene, gef::Platform& platform);

	void BuildTransformation();

	void Update(float frame_time);

	class gef::Mesh* mesh_;

	gef::Scene* model_scene_;

	gef::Matrix44 scaleMatrix;
	gef::Matrix44 rotationXMatrix;
	gef::Matrix44 rotationYMatrix;
	gef::Matrix44 rotationZMatrix;
	gef::Matrix44 translationMatrix;
	gef::Matrix44 local_transform;
	gef::Matrix44 markerTransform;
	gef::Matrix44 WorldTransfrom;


	gef::MeshInstance* mesh_instance;

	PrimitiveBuilder* primitive_builder_;
	gef::Vector4 Normal_v;
	
	bool isInView;
	int WallId;
	bool hasBouncedOff = false;
	 
private :
	gef::Vector4 position;
	gef::Vector4 scale;
	gef::Vector4 velocity;
	gef::Vector4 rotations;

	

};