#pragma once
#include "CourseworkGame.h"
#include"Assets.h"

#include <sstream>
#include<string>

using namespace NCL;
using namespace CSC8503;

CourseworkGame::CourseworkGame()
{
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);
	physics = new PhysicsSystem(*world);
	assets = new Assets(world);
	player = nullptr;
	greenAI = nullptr;
	redAI = nullptr;
	selectionObject = nullptr;

	forceMagnitude = 10.0f;
	useGravity = true;
	inDebugMode = false;

	cameraOffset = Vector3(0, 30, 30);
	selectedColour = Vector4(1, 1, 1, 1);

	Debug::SetRenderer(renderer);
}

CourseworkGame::~CourseworkGame()
{
	delete physics;
	delete renderer;
	delete world;
	delete assets;
}

void CourseworkGame::UpdateGame(float dt)
{
	UpdateDebugMode();
	
	if (player->IsFinished())
		UpdateFinish();
	else
		renderer->DrawString("Points: " + std::to_string(player->GetPoints()), Vector2(10, 20));

	if (player->GetPoints() <= 0)
	{
		renderer->DrawString("You lost!!", Vector2(20, 40), Vector4(1, 0, 0, 1), 30);
		Debug::FlushRenderables(dt);
		renderer->Render();
		return;
	}
	
	UpdateKeys();

	physics->Update(dt);
	UpdateCamera(dt, player);


	world->UpdateObjects(dt);
	world->UpdateWorld(dt);
	world->CleanGameObjects();

	Debug::FlushRenderables(dt);
	renderer->Render();

	
}

void CourseworkGame::UpdatePause(float dt)
{
	renderer->DrawString("PAUSE", Vector2(40, 50), Vector4(1, 0, 0, 1),50);
	renderer->Render();
}

void CourseworkGame::UpdateMenu(float dt)
{
	Vector4 colour(1, 1, 1, 1);
	renderer->DrawString("Welcome in coursework game", Vector2(30, 20), colour);
	renderer->DrawString("1. Singleplayer", Vector2(30, 50), colour);
	renderer->DrawString("2. Multiplayer", Vector2(30, 60), colour);
	renderer->DrawString("3. Exit", Vector2(30, 70), colour);
	renderer->Render();
}

void CourseworkGame::InitSingleplayer()
{
	InitCamera();

	world->ClearAndErase();
	physics->Clear();
	assets->InitArena();
	player = assets->InitPlayer();
	greenAI = nullptr;
	redAI = nullptr;
}

void CourseworkGame::InitMultiplayer()
{
	InitCamera();

	world->ClearAndErase();
	physics->Clear();
	assets->InitArena();
	player = assets->InitPlayer();


	redAI = assets->AddAgent(Vector3(-28, 3, 22));
	greenAI = assets->AddWaitingAgent(Vector3(-30, 3, 22));
}

void CourseworkGame::UpdateKeys()
{

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		inDebugMode = !inDebugMode;
	}
}

void CourseworkGame::UpdateCamera(float dt, GameObject* object)
{
	if (!inDebugMode&&player)
	{
		Vector3 objPos = object->GetTransform().GetPosition();
		Vector3 camPos = objPos + cameraOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}
	else 
		world->GetMainCamera()->UpdateCamera(dt);
}

void CourseworkGame::UpdateDebugMode()
{
	if (inDebugMode)
	{
		Window::GetWindow()->ShowOSPointer(true);
		Window::GetWindow()->LockMouseToWindow(false);
	}
	else
	{
		Window::GetWindow()->ShowOSPointer(false);
		Window::GetWindow()->LockMouseToWindow(true);
	}

	//SelectionObject with informations.

	if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
		if (selectionObject) {	//set colour to deselected;
			selectionObject->GetRenderObject()->SetColour(selectedColour);
			selectionObject = nullptr;
		}

		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true))
		{
			selectionObject = (GameObject*)closestCollision.node;
			selectedColour = selectionObject->GetRenderObject()->GetColour();
			selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));

			renderer->DrawString("Debug Info", Vector2(80, 10));
			renderer->DrawString("Name: " + selectionObject->GetName(), Vector2(60, 15));
			renderer->DrawString("Physics Layer: " + std::to_string((int)selectionObject->GetLayer()), Vector2(60, 20));
			renderer->DrawString("Tag name: " + selectionObject->GetTag(), Vector2(60, 25));
			renderer->DrawString("World ID: " + std::to_string(selectionObject->GetWorldID()), Vector2(60, 30));			
			
			std::stringstream position;
			position << selectionObject->GetTransform().GetPosition();

			renderer->DrawString("Transform: " + position.str(), Vector2(35, 35));

			position.str(std::string());
			position << selectionObject->GetTransform().GetOrientation().ToEuler();
			renderer->DrawString("Orientation: " + position.str(), Vector2(35, 40));

		}

	}

}

void CourseworkGame::UpdateFinish()
{
	renderer->DrawString("Player reached the finish", Vector2(20, 40), Vector4(1,0,0,1), 30);
	renderer->DrawString("Player points: " + std::to_string(player->GetPoints()), Vector2(30, 50));
	if (redAI == nullptr || greenAI == nullptr)
		return;

	renderer->DrawString("Red AI points: " + std::to_string(redAI->GetPoints()), Vector2(30, 60));
	renderer->DrawString("Red AI points: " + std::to_string(greenAI->GetPoints()), Vector2(30, 70));
}

void CourseworkGame::InitCamera()
{
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
}

