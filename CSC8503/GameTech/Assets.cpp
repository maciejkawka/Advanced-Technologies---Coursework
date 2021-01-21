#pragma once
#include "Assets.h"

#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"

#include"../CSC8503Common/PositionConstraint.h"
#include"../CSC8503Common/HingeConstraint.h"

#include"StateMachines.h"

#include<string>

using namespace NCL;
using namespace CSC8503;


Assets::Assets(GameWorld* w)
{
	this->w = w;

	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh", &cubeMesh);
	loadFunc("sphere.msh", &sphereMesh);
	loadFunc("Male1.msh", &charMeshA);
	loadFunc("courier.msh", &charMeshB);
	loadFunc("security.msh", &enemyMesh);
	loadFunc("coin.msh", &bonusMesh);
	loadFunc("capsule.msh", &capsuleMesh);

	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");
}

Assets::~Assets()
{
	delete cubeMesh;
	delete sphereMesh;
	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;
	delete capsuleMesh;

	delete basicTex;
	delete basicShader;
}

void Assets::InitArena()
{
	AddFirstPlatform(Vector3(0, 0, 0));
	AddSecondPlatform(Vector3(7, 0, -65));
	
	//Add bonuses
	AddBonusSet(Vector3(-17, 1, 10));
	AddBonusSet(Vector3(-31, 1, -8));
	AddBonusSet(Vector3(-3, 1, -8));
	AddBonusSet(Vector3(11, 1, 10));
	AddBonusSet(Vector3(25, 1, -8));

	AddBridge(Vector3(37, -1, -30));
	AddFinishPoint(Vector3(-6, 0, -64));

}

Player* Assets::InitPlayer(Vector3& position)
{
	Player* objectPlayer = new Player("Player");

	float halfHeight = 2.0f;
	float radius = 1.0f;
	float inverseMass = 10.0;
	float capsuleOffset = 0.3f;
	float friction = 0.2f;
	float elasticity = 0.1f;
	
	CapsuleVolume* volume = new CapsuleVolume(halfHeight - capsuleOffset, radius);
	objectPlayer->SetBoundingVolume((CollisionVolume*)volume);

	objectPlayer->GetTransform()
		.SetScale(Vector3(radius * 2, halfHeight, radius * 2))
		.SetPosition(position);

	objectPlayer->SetRenderObject(new RenderObject(&objectPlayer->GetTransform(), charMeshA, nullptr, basicShader));
	objectPlayer->SetPhysicsObject(new PhysicsObject(&objectPlayer->GetTransform(), objectPlayer->GetBoundingVolume()));

	objectPlayer->GetPhysicsObject()->SetInverseMass(inverseMass);
	objectPlayer->GetPhysicsObject()->InitCubeInertia();
	objectPlayer->GetPhysicsObject()->SetElasticity(elasticity);
	objectPlayer->GetPhysicsObject()->SetStaticFriction(friction);

	w->AddGameObject(objectPlayer);
	player = objectPlayer;
	return objectPlayer;
}

BonusGameObject* Assets::AddBonusToWorld(const Vector3& position, float radius, float inverseMass, string name)
{
	BonusGameObject* bonus = new BonusGameObject(name);

	SphereVolume* volume = new SphereVolume(radius*2);
	bonus->SetBoundingVolume((CollisionVolume*)volume);

	bonus->SetTag("Collectable");
	bonus->GetTransform()
		.SetScale(Vector3(radius, radius, radius))
		.SetPosition(position);

	bonus->SetRenderObject(new RenderObject(&bonus->GetTransform(), bonusMesh, nullptr, basicShader));
	bonus->SetPhysicsObject(new PhysicsObject(&bonus->GetTransform(), bonus->GetBoundingVolume()));
	bonus->GetPhysicsObject()->SetTrigger(true);

	bonus->GetPhysicsObject()->SetInverseMass(inverseMass);
	bonus->GetPhysicsObject()->InitSphereInertia();

	w->AddGameObject(bonus);

	return bonus;
}

void Assets::AddFloorToWorld()
{
	GameObject* object;
	object = AddCubeToWorld(Vector3(0, 0, 0), Vector3(37, 1, 30), 0, "Floor1");
	object->SetLayer(Layer::IgnoreRayCast);
	object->GetRenderObject()->SetColour(Vector4(0.2, 0.6, 0.2, 1));

	object = AddCubeToWorld(Vector3(7, 0, -64), Vector3(30, 1, 17), 0, "Floor2");
	object->SetLayer(Layer::IgnoreRayCast);
	object->GetRenderObject()->SetColour(Vector4(0.2, 0.4, 0.6, 1));
}

void Assets::AddWalls()
{
	GameObject* object;

	//Left and Right wall
	for (int i = 0; i < 2; i++)
	{
		object = AddCubeToWorld(Vector3(-39+(i*2*39), 1, 0), Vector3(2, 1, 30), 0, "Wall");
		object->SetLayer(Layer::IgnoreRayCast);
	}

	//Upper wall
	object = AddCubeToWorld(Vector3(-7, 1, -28), Vector3(30, 1, 2), 0, "Wall");
	object->SetLayer(Layer::IgnoreRayCast);

	//Lower wall
	object = AddCubeToWorld(Vector3(0, 1, 28), Vector3(37, 1, 2), 0, "Wall");
	object->SetLayer(Layer::IgnoreRayCast);

	//Inside walls
	for (int i = 0; i < 2; i++)
	{
		object = AddCubeToWorld(Vector3(-7 + (14 * 2 * i), 1, -5), Vector3(2, 1, 21), 0, "Wall");
		object->SetLayer(Layer::IgnoreRayCast);

		object = AddCubeToWorld(Vector3(-21+(14*2*i), 1, 5), Vector3(2, 1, 21), 0, "Wall");
		object->SetLayer(Layer::IgnoreRayCast);
	}


	

}

void Assets::AddFirstPlatform(Vector3& position)
{
	GameObject* object;

	//Floor
	object = AddCubeToWorld(Vector3(0, -1, 0) + position, Vector3(37, 1, 30), 0, "Floor1");
	object->SetLayer(Layer::IgnoreRayCast);
	object->GetRenderObject()->SetColour(Vector4(0.2, 0.6, 0.2, 1));

	//Left and Right wall
	for (int i = 0; i < 2; i++)
	{
		object = AddCubeToWorld(Vector3(-35 + (i * 2 * 35), 1, 0) + position, Vector3(2, 0.99, 29.99 ), 0, "Wall");
		object->SetLayer(Layer::IgnoreRayCast);
	}

	//Upper wall
	object = AddCubeToWorld(Vector3(-7, 1, -28) + position, Vector3(30, 1, 2), 0, "Wall");
	object->SetLayer(Layer::IgnoreRayCast);

	//Lower wall
	object = AddCubeToWorld(Vector3(0, 1, 28) + position, Vector3(37, 1, 2), 0, "Wall");
	object->SetLayer(Layer::IgnoreRayCast);

	//Inside walls
	for (int i = 0; i < 2; i++)
	{
		object = AddCubeToWorld(Vector3(-7 + (14 * 2 * i), 1, -5) + position, Vector3(2, 1, 21), 0, "Wall");
		object->SetLayer(Layer::IgnoreRayCast);

		object = AddCubeToWorld(Vector3(-21 + (14 * 2 * i), 1, 5) + position, Vector3(2, 1, 21), 0, "Wall");
		object->SetLayer(Layer::IgnoreRayCast);
	}


	//Add MovingCapsules
	for (int i = 0; i < 2; i++)
	{
		StateGameObject* stateObject = AddCapsuleToWorld<StateGameObject>(Vector3(-21 + 28*i, 5, -14) + position, 2, 1, 5);
		stateObject->GetPhysicsObject()->SetKinematic(true);
		StateMachines::MoveUpDown* machine = new StateMachines::MoveUpDown(stateObject);
	}

	for (int i = 0; i < 2; i++)
	{
		StateGameObject* stateObject = AddCapsuleToWorld<StateGameObject>(Vector3(-7 + 28 * i, 5, 14) + position, 2, 1, 5);
		stateObject->GetPhysicsObject()->SetKinematic(true);
		StateMachines::MoveUpDown* machine = new StateMachines::MoveUpDown(stateObject);
	}

	//Add IceFloor
	object = AddCubeToWorld(Vector3(-22, -0.25, -22) + position, Vector3(13, 0.3, 6), 0, "IceFloor");
	object->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));
	object->GetPhysicsObject()->SetStaticFriction(0.0f);

	//AddGate
	StateGameObject* stateObject = AddCubeToWorld<StateGameObject>(Vector3(-14, -2.2, 0), Vector3(5, 3, 1), 0, "Gate");
	stateObject->GetRenderObject()->SetColour(Vector4(0, 0, 0, 1));
	new StateMachines::OpenAndClose(stateObject);

	//Glue Floor
	object = AddCubeToWorld(Vector3(22, -0.25, 22) + position, Vector3(13, 0.3, 6), 0, "GlueFloor");
	object->GetRenderObject()->SetColour(Vector4(0.5, 0.8, 0, 1));
	object->GetPhysicsObject()->SetStaticFriction(0.8f);

	//AddBouncers
	for (int i = 0; i < 5; i++)
	{
		object = AddSphereToWorld(Vector3(-2 + 4*i, 1, -22) + position, 0.5, 0, "Bouncers Sphere");
		object->GetPhysicsObject()->SetElasticity(100);

	}

}

void Assets::AddSecondPlatform(Vector3& position)
{
	GameObject* object;
	//Floor
	object = AddCubeToWorld(Vector3(0, -1, 0) + position, Vector3(30, 1, 17), 0, "Floor2");
	object->SetLayer(Layer::IgnoreRayCast);
	object->GetRenderObject()->SetColour(Vector4(0.2, 0.6, 0.2, 1));

	//Left and Right wall
	for (int i = 0; i < 2; i++)
	{
		object = AddCubeOBBToWorld(Vector3(28 - (i * 2 * 28), 1, 0) + position, Vector3(2, 0.99, 16.99), 0, "Wall");
		object->SetLayer(Layer::IgnoreRayCast);
	}

	object = AddCubeToWorld(Vector3(0, 1, -15) + position, Vector3(30, 1, 2), 0, "Wall");
	object->SetLayer(Layer::IgnoreRayCast);

	object = AddCubeToWorld(Vector3(-5, 1, 15) + position, Vector3(25, 1, 2), 0, "Wall");
	object->SetLayer(Layer::IgnoreRayCast);

	object = AddCubeToWorld(Vector3(18, 1, 9) + position, Vector3(2, 0.99, 7.99), 0, "Wall");
	object->SetLayer(Layer::IgnoreRayCast);


	//AddMovingCapsules
	StateGameObject* stateObject = AddCapsuleToWorld<StateGameObject>(Vector3(18, 5, 3) + position, 2, 1, 5);
	stateObject->GetPhysicsObject()->SetKinematic(true);
	StateMachines::MoveUpDown* machine = new StateMachines::MoveUpDown(stateObject);

	//HingeConstraint
	object = AddCubeOBBToWorld(Vector3(10, 1, -6) + position, Vector3(1, 1, 2), 10.0f, "Hinge");
	object->GetPhysicsObject()->SetGravity(false);

	GameObject* object1 = AddCubeToWorld(Vector3(8, 1, -6) + position, Vector3(1, 1, 1), 0, "Hinge Wall");
	object->GetPhysicsObject()->SetGravity(false);

	HingeConstraint* hingeConstraint = new HingeConstraint(object, object1);
	PositionConstraint* positionConstraint = new PositionConstraint(object, object1,3);
	w->AddConstraint(hingeConstraint);
	w->AddConstraint(positionConstraint);

	//FreeObject to move
	for (int i = 0; i < 3; i++)
	{
		object = AddCubeToWorld(Vector3(10-i*2, 1, 6) + position, Vector3(1, 1, 1), 10.0f, "FreeCube" + std::to_string(i));
		object->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));
	}

	for (int i = 0; i < 3; i++)
	{
		object = AddSphereToWorld(Vector3(10 - i, 1, 8) + position, 1.0f, 10.0f, "FreeSphere " + std::to_string(i));
		object->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
	}
}

void NCL::CSC8503::Assets::AddBonusSet(Vector3& position)
{
	for (int i = 0; i < 3; i++)
	{
		BonusGameObject* object = AddBonusToWorld(Vector3(3*i, 1.5, 0) + position, 0.1);
		object->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));
	}
}

void Assets::AddBridge(Vector3& position)
{
	GameObject* object;

	//Bridge floor
	object = AddCubeToWorld(Vector3(-7, 0, -1) + position, Vector3(3, 1, 1), 0, "BridgeFloor");
	object->SetLayer(Layer::IgnoreRayCast);
	object->GetRenderObject()->SetColour(Vector4(0.64, 0.16, 0.16, 1));

	GameObject* previous = object;

	for (int i = 0; i < 8; i++)
	{
		GameObject* block = AddCubeToWorld(Vector3(-7,0,-4 - 1.5* i) + position, Vector3(3,1,0.5), 20, "Bridge");
		block->SetLayer(Layer::IgnoreRayCast);
		block->GetRenderObject()->SetColour(Vector4(0.64, 0.16, 0.16, 1));

		PositionConstraint* constraint = new PositionConstraint(previous, block, 1.5);
		w->AddConstraint(constraint);

		previous = block;
	}

	object = AddCubeToWorld(Vector3(-7, 0, -19) + position, Vector3(3, 0.99, 1), 0, "BridgeFloor");
	object->SetLayer(Layer::IgnoreRayCast);
	object->GetRenderObject()->SetColour(Vector4(0.64, 0.16, 0.16, 1));

	PositionConstraint* constraint = new PositionConstraint(previous, object, 1.5);
	w->AddConstraint(constraint);



}

void Assets::AddFinishPoint(Vector3& position)
{
	GameObject*	object = AddCubeToWorld(position, Vector3(2, 0.1, 15), 0, "Finish");
	object->GetRenderObject()->SetColour(Vector4(0, 1, 1, 1));
}

AIGameObject* Assets::AddAgent(Vector3& position)
{
	AIGameObject* object = new AIGameObject("AI Agent");

	float halfHeight = 2.0f;
	float radius = 1.0f;
	float inverseMass = 10.0;
	float capsuleOffset = 0.3f;
	float friction = 0.2f;
	float elasticity = 0.1f;

	CapsuleVolume* volume = new CapsuleVolume(halfHeight - capsuleOffset, radius);
	object->SetBoundingVolume((CollisionVolume*)volume);

	object->GetTransform()
		.SetScale(Vector3(radius * 2, halfHeight, radius * 2))
		.SetPosition(position);

	object->SetRenderObject(new RenderObject(&object->GetTransform(), charMeshA, nullptr, basicShader));
	object->SetPhysicsObject(new PhysicsObject(&object->GetTransform(), object->GetBoundingVolume()));

	object->GetPhysicsObject()->SetInverseMass(inverseMass);
	object->GetPhysicsObject()->InitCubeInertia();
	object->GetPhysicsObject()->SetElasticity(elasticity);
	object->GetPhysicsObject()->SetStaticFriction(friction);

	object->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	StateMachines::RacingAI* racing = new StateMachines::RacingAI(player, object, Vector3(-6, 0, -64), w);
	w->AddGameObject(object);
	return object;
}

AIGameObject* Assets::AddWaitingAgent(Vector3& position)
{
	AIGameObject* object = new AIGameObject("AI Agent");

	float halfHeight = 2.0f;
	float radius = 1.0f;
	float inverseMass = 10.0;
	float capsuleOffset = 0.3f;
	float friction = 0.2f;
	float elasticity = 0.1f;

	CapsuleVolume* volume = new CapsuleVolume(halfHeight - capsuleOffset, radius);
	object->SetBoundingVolume((CollisionVolume*)volume);

	object->GetTransform()
		.SetScale(Vector3(radius * 2, halfHeight, radius * 2))
		.SetPosition(position);

	object->SetRenderObject(new RenderObject(&object->GetTransform(), charMeshA, nullptr, basicShader));
	object->SetPhysicsObject(new PhysicsObject(&object->GetTransform(), object->GetBoundingVolume()));

	object->GetPhysicsObject()->SetInverseMass(inverseMass);
	object->GetPhysicsObject()->InitCubeInertia();
	object->GetPhysicsObject()->SetElasticity(elasticity);
	object->GetPhysicsObject()->SetStaticFriction(friction);

	object->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));

	StateMachines::WaitingRaceAT* racing = new StateMachines::WaitingRaceAT(player, object, Vector3(-6, 0, -64), w);
	w->AddGameObject(object);

	return object;
}
