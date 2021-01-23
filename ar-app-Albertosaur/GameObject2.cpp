#include "GameObject2.h"

#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>

#include <graphics/scene.h>
#include <maths/math_utils.h>



GameObject2::GameObject2() :
	mesh_(NULL),
	model_scene_(NULL),
	hasBouncedOff(false)
{
}


void GameObject2::InitModel(const char* model_name, gef::Platform& platform, gef::Vector4 pos, gef::Vector4 scaling, gef::Vector4 rotating)
{
	model_scene_ = new gef::Scene();
	model_scene_->ReadSceneFromFile(platform, model_name);

	model_scene_->CreateMaterials(platform);

	mesh_ = GetFirstMesh(model_scene_, platform);

	this->set_mesh(mesh_);
	scale = scaling;
	position = pos;
	rotations = rotating;
	velocity = gef::Vector4(0, 0, 0);
}

void GameObject2::InitPrimBox(gef::Vector4 size, gef::Vector4 positions, gef::Platform& platform, int ID, gef::Vector4 rotation)
{
	primitive_builder_ = new PrimitiveBuilder(platform);
	this->set_mesh(primitive_builder_->CreateBoxMesh(size, positions));
	scale = gef::Vector4(1, 1, 1);
	position = positions;
	velocity = gef::Vector4(0, 0, 0);
	rotations = rotation;
	local_transform.SetIdentity();
	WallId = ID;
}

void GameObject2::InitPrimSphere(float radius, gef::Vector4 positions, gef::Platform & platform, int ID)
{
	primitive_builder_ = new PrimitiveBuilder(platform);
	this->set_mesh(primitive_builder_->CreateSphereMesh(radius, 10, 10, positions));
	scale = gef::Vector4(1, 1, 1);
	position = positions;
	velocity = gef::Vector4(0, 0, 0);
	rotations = gef::Vector4(0, 0, 0);
	local_transform.SetIdentity();
	WallId = ID;
}

void GameObject2::InitPrimPlane(gef::Vector4 size, gef::Vector4 positions, gef::Platform & platform, int wallID, gef::Vector4 rotations)
{
	primitive_builder_ = new PrimitiveBuilder(platform);
	this->set_mesh(primitive_builder_->CreatePlaneMesh(size, positions));
	scale = gef::Vector4(1, 1, 1);
	position = positions;
	velocity = gef::Vector4(0, 0, 0);
	rotations = rotations;
	local_transform.SetIdentity();
	WallId = wallID;
}

gef::Mesh* GameObject2::GetFirstMesh(gef::Scene* scene, gef::Platform& platform)
{
	gef::Mesh* mesh = NULL;

	if (scene)
	{
		// now check to see if there is any mesh data in the file, if so lets create a mesh from it
		if (scene->mesh_data.size() > 0)
			mesh = model_scene_->CreateMesh(platform, scene->mesh_data.front());
	}

	return mesh;
}

void GameObject2::BuildTransformation()
{
	scaleMatrix.SetIdentity();
	scaleMatrix.Scale(scale);

	rotationXMatrix.SetIdentity();
	rotationYMatrix.SetIdentity();
	rotationZMatrix.SetIdentity();
	rotationXMatrix.RotationX(gef::DegToRad(rotations.x()));
	rotationYMatrix.RotationY(gef::DegToRad(rotations.y()));
	rotationZMatrix.RotationZ(gef::DegToRad(rotations.z()));
	
	translationMatrix.SetIdentity();
	translationMatrix.SetTranslation(position);

	local_transform = scaleMatrix * rotationXMatrix *  rotationYMatrix * rotationZMatrix *translationMatrix;

	WorldTransfrom = ( local_transform * markerTransform);

	
	Normal_v.set_x(local_transform.GetRow(1).x());
	Normal_v.set_y(local_transform.GetRow(1).y());
	Normal_v.set_z(0);

	this->set_transform(WorldTransfrom);
}

void GameObject2::Update(float frame_time)
{
	position += velocity * frame_time;
	BuildTransformation();
}
