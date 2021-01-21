#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"

namespace NCL {
	namespace CSC8503 {
		
		class Player;
		class Assets;
		class AIGameObject;

		class CourseworkGame {
		public:
			CourseworkGame();
			~CourseworkGame();

			virtual void UpdateGame(float dt);
			virtual void UpdatePause(float dt);
			virtual void UpdateMenu(float dt);

			void InitSingleplayer();
			void InitMultiplayer();
			 
		protected:
			void UpdateKeys();
			void UpdateCamera(float dt, GameObject* object = nullptr);
			void UpdateDebugMode();
			void UpdateFinish();

			void InitCamera();	
			

			GameTechRenderer* renderer;
			PhysicsSystem* physics;
			GameWorld* world;

			bool useGravity;
			bool inDebugMode;
			float forceMagnitude;

			Vector3 cameraOffset;
			Vector4 selectedColour;
			//GameObjects
			Player* player;
			Assets* assets;
			AIGameObject* redAI;
			AIGameObject* greenAI;
			GameObject* selectionObject;

		};
	}
}