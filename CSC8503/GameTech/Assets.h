#pragma once
#include "GameTechRenderer.h"

#include"Player.h"
#include"BonusGameObject.h"
#include"../CSC8503Common/StateGameObject.h"
#include"AIGameObject.h"



namespace NCL {
	namespace CSC8503 {

		class Assets {
		public:
			Assets(GameWorld* w);
			~Assets();

			void			InitArena();
			Player*			InitPlayer(Vector3& position = Vector3(-32, 3, 22));
			AIGameObject*	AddAgent(Vector3& position);
			AIGameObject*	AddWaitingAgent(Vector3& position);

		protected:

			void		AddFloorToWorld();
			void		AddWalls();
			void		AddFirstPlatform(Vector3& position);
			void		AddSecondPlatform(Vector3& position);
			void		AddBonusSet(Vector3& position);
			void		AddBridge(Vector3& position);
			void		AddFinishPoint(Vector3& position);

			GameWorld* w;
			Player* player;

			OGLMesh* capsuleMesh = nullptr;
			OGLMesh* cubeMesh = nullptr;
			OGLMesh* sphereMesh = nullptr;
			OGLTexture* basicTex = nullptr;
			OGLShader* basicShader = nullptr;

			//Coursework Meshes
			OGLMesh* charMeshA = nullptr;
			OGLMesh* charMeshB = nullptr;
			OGLMesh* enemyMesh = nullptr;
			OGLMesh* bonusMesh = nullptr;

			BonusGameObject* AddBonusToWorld(const Vector3& position, float radius, float inverseMass = 0.0f, string name = "Bonus");

			//Templated GameObject creation
			template<class T = GameObject>
			T* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f, string name = "Cube")
			{
				T* cube = new T(name);

				AABBVolume* volume = new AABBVolume(dimensions);
				cube->SetBoundingVolume((CollisionVolume*)volume);

				cube->GetTransform()
					.SetPosition(position)
					.SetScale(dimensions * 2);

				cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
				cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

				cube->GetPhysicsObject()->SetInverseMass(inverseMass);
				cube->GetPhysicsObject()->InitCubeInertia();
				w->AddGameObject(cube);

				return cube;
			}

			template<class T = GameObject>
			T* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f, string name = "Capsule")
			{
				T* capsule = new T(name);

				CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
				capsule->SetBoundingVolume((CollisionVolume*)volume);

				capsule->GetTransform()
					.SetScale(Vector3(radius * 2, halfHeight, radius * 2))
					.SetPosition(position);

				capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
				capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

				capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
				capsule->GetPhysicsObject()->InitCubeInertia();
				w->AddGameObject(capsule);

				return capsule;
			}

			template<class T = GameObject>
			T* AddCubeOBBToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f, string name = "Cube OBB")
			{
				T* cube = new T(name);

				OBBVolume* volume = new OBBVolume(dimensions);
				cube->SetBoundingVolume((CollisionVolume*)volume);

				cube->GetTransform()
					.SetPosition(position)
					.SetScale(dimensions * 2);

				cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
				cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

				cube->GetPhysicsObject()->SetInverseMass(inverseMass);
				cube->GetPhysicsObject()->InitCubeInertia();

				w->AddGameObject(cube);

				return cube;
			}

			template<class T = GameObject>
			T* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f, string name = "Sphere")
			{
				{
					T* sphere = new T(name);

					Vector3 sphereSize = Vector3(radius, radius, radius);
					SphereVolume* volume = new SphereVolume(radius);
					sphere->SetBoundingVolume((CollisionVolume*)volume);

					sphere->GetTransform()
						.SetScale(sphereSize)
						.SetPosition(position);

					sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
					sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

					sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
					sphere->GetPhysicsObject()->InitHollowSphereInertia();

					w->AddGameObject(sphere);

					return sphere;
				}
			}
		};
	}
}
