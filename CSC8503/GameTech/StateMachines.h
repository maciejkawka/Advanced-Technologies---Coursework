#pragma once

#include"../CSC8503Common/StateGameObject.h"
#include"../CSC8503Common/StateMachine.h"
#include"../CSC8503Common/State.h"
#include"../CSC8503Common/StateTransition.h"

#include"../CSC8503Common/NavigationGrid.h"
#include "../../Common/Maths.h"
#include"Player.h"
#include"AIGameObject.h"





namespace NCL {

	namespace CSC8503 {

		namespace StateMachines {

			class MoveUpDown : public StateMachine {
			public:
				MoveUpDown(StateGameObject* stateObject)
				{
					this->object = stateObject;


					timer = 1.0f;
					force = 0.3;

					State* moveUp = new State([&](float dt)->void {

						object->GetPhysicsObject()->AddForce(Vector3(0, 1, 0) * force);
						timer += dt;
						});

					State* moveDown = new State([&](float dt)->void {

						object->GetPhysicsObject()->AddForce(Vector3(0, -1, 0) * force);
						timer -= dt;
						});


					AddState(moveUp);
					AddState(moveDown);

					AddTransition(new StateTransition(moveDown, moveUp, [&]()->bool {

						return timer <= 0.0f;
						}));

					AddTransition(new StateTransition(moveUp, moveDown, [&]()->bool {

						return timer >= 2.0f;
						}));

					object->SetStateMachine(this);
				}
				~MoveUpDown() {}

			protected:
				float timer;
				float force;
				StateGameObject* object;

			};




			class OpenAndClose : public StateMachine {
			public:
				OpenAndClose(StateGameObject* stateObject)
				{
					object = stateObject;

					openTime = 5.0f;
					closeTime = 5.0f;
					velocty = 3.0f;
					timer = 0.0f;

					State* moveUp = new State([&](float dt)->void {

						object->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 1, 0) * velocty);
						
						});

					State* moveDown = new State([&](float dt)->void {

						object->GetPhysicsObject()->SetLinearVelocity(Vector3(0, -1, 0) * velocty);
						});

					State* waitClose = new State([&](float dt)->void {
						
						timer += dt;
						});

					State* waitOpen = new State([&](float dt)->void {

						timer += dt;
						});
					AddState(moveUp);
					AddState(moveDown);
					AddState(waitOpen);
					AddState(waitClose);

					AddTransition(new StateTransition(waitOpen, moveUp, [&]()->bool {

						return timer >= openTime;
						}));

					AddTransition(new StateTransition(moveUp, waitClose, [&]()->bool {

						if (object->GetTransform().GetPosition().y >= -0.7)
						{
							timer = 0;
							object->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0) * velocty);
							object->GetTransform().SetPosition(Vector3(-14, -0.7, 0));
							return true;
						}

						return false;

						}));

					AddTransition(new StateTransition(waitClose, moveDown, [&]()->bool {

						return timer >= closeTime;
						}));

					AddTransition(new StateTransition(moveDown, waitOpen, [&]()->bool {

						if (object->GetTransform().GetPosition().y <= -3.1)
						{
							timer = 0;
							object->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0) * velocty);
							object->GetTransform().SetPosition(Vector3(-14, -3.1, 0));
							return true;
						}

						return false;

						}));

					object->SetStateMachine(this);
				}
			protected:
				float openTime;
				float closeTime;
				float velocty;
				float timer;
				StateGameObject* object;
			};



			class RacingAI : public StateMachine {
			public:

				RacingAI(Player* _player, AIGameObject* _AI, Vector3& _finish, GameWorld* _world)
				{
		
					this->player = _player;
					this->AI = _AI;
					this->finish = _finish;
					this->world = _world;
					waitTime = 10.0f;
					timer = 0.0f;
				
				State* generatePath = new State([&](float dt)->void {

					AI->FindPath(finish);
					});

				State* followPath = new State([&](float dt)->void {

					AI->FollowPath();
					});

				State* wait = new State([&](float dt)->void {

					timer += dt;
					});

				State* teleportToPlayer = new State([&](float dt)->void {

					Vector3 playerPos = player->GetTransform().GetPosition();
					Vector3 playerOrient = player->GetTransform().GetOrientation() * Vector3(0, 0, -1);
					AI->GetTransform().SetPosition(playerPos - playerOrient * 2);
					});

				AddState(generatePath);
				AddState(followPath);
				AddState(wait);
				AddState(teleportToPlayer);

				AddTransition(new StateTransition(generatePath, followPath, [&](void)->bool {
						
					return AI->GetPath().size() > 0;				
					}));

				AddTransition(new StateTransition(followPath, wait, [&](void)->bool {

					Ray r = Ray(AI->GetTransform().GetPosition(), AI->GetTransform().GetOrientation() * Vector3(0, 0, -1));
					RayCollision closestCollision;
					if (!world->Raycast(r, closestCollision, true))
						return false;

					return closestCollision.rayDistance < 2.0f;
					}));

				AddTransition(new StateTransition(wait, teleportToPlayer, [&](void)->bool {

					return timer >= waitTime;
					}));

				AddTransition(new StateTransition(wait, followPath, [&](void)->bool {

					Ray r = Ray(AI->GetTransform().GetPosition(), AI->GetTransform().GetOrientation() * Vector3(0, 0, -1));
					RayCollision closestCollision;
					if (world->Raycast(r, closestCollision, true))
						return true;

					return closestCollision.rayDistance > 2.0f;
					}));

				AddTransition(new StateTransition(teleportToPlayer, generatePath, [&](void)->bool {

					return true;
					}));


				AI->SetStateMachine(this);

			}
				~RacingAI() {}

			protected:
				float waitTime;
				float timer; 
				Player* player;
				Vector3 finish;
				AIGameObject* AI;
				GameWorld* world;
			};


			class WaitingRaceAT : public StateMachine {
			public:

				WaitingRaceAT(Player* _player, AIGameObject* _AI, Vector3& _finish, GameWorld*	_world)
				{

					this->player = _player;
					this->AI = _AI;
					this->finish = _finish;
					this->world = _world;
					waitTime = 10.0f;
					timer = 0.0f;

					State* generatePath = new State([&](float dt)->void {

						AI->FindPath(finish);
						});

					State* followPath = new State([&](float dt)->void {

						AI->FollowPath();
						});

					State* wait = new State([&](float dt)->void {

						timer += dt;
						});

					State* teleportToPlayer = new State([&](float dt)->void {

						Vector3 playerPos = player->GetTransform().GetPosition();
						Vector3 playerOrient = player->GetTransform().GetOrientation() * Vector3(0, 0, -1);
						AI->GetTransform().SetPosition(playerPos - playerOrient * 2);
						});

					State* moveSlower = new State([&](float dt)->void {

						AI->FollowPath();
						Vector3 velocity = AI->GetPhysicsObject()->GetLinearVelocity();
						AI->GetPhysicsObject()->SetLinearVelocity(velocity * 0.8f);
						});

					
					AddState(generatePath);
					AddState(followPath);
					AddState(wait);
					AddState(teleportToPlayer);
					AddState(moveSlower);

					AddTransition(new StateTransition(generatePath, followPath, [&](void)->bool {

						return AI->GetPath().size() > 0;
						}));

					AddTransition(new StateTransition(followPath, wait, [&](void)->bool {

						Ray r = Ray(AI->GetTransform().GetPosition(), AI->GetTransform().GetOrientation()*Vector3(0,0,-1));
						RayCollision closestCollision;
						if (!world->Raycast(r, closestCollision, true))
							return false;

						return closestCollision.rayDistance < 2.0f;
						}));

					AddTransition(new StateTransition(moveSlower, wait, [&](void)->bool {

						Ray r = Ray(AI->GetTransform().GetPosition(), AI->GetTransform().GetOrientation() * Vector3(0, 0, -1));
						RayCollision closestCollision;
						if (!world->Raycast(r, closestCollision, true))
							return false;

						return closestCollision.rayDistance < 2.0f;
						}));

					AddTransition(new StateTransition(wait, teleportToPlayer, [&](void)->bool {

						return timer >= waitTime;
						}));

					AddTransition(new StateTransition(wait, followPath, [&](void)->bool {

						Ray r = Ray(AI->GetTransform().GetPosition(), AI->GetTransform().GetOrientation() * Vector3(0, 0, -1));
						RayCollision closestCollision;
						if (!world->Raycast(r, closestCollision, true))
							return true;

						return closestCollision.rayDistance > 2.0f;
						}));

					AddTransition(new StateTransition(teleportToPlayer, generatePath, [&](void)->bool {

						return true;
						}));

					AddTransition(new StateTransition(followPath, moveSlower, [&](void)->bool {

						Vector3 dir = player->GetTransform().GetPosition() - AI->GetTransform().GetPosition();
						Vector3 forwardDir = AI->GetTransform().GetOrientation() * Vector3(0, 0, -1);

						return Vector3::Dot(dir.Normalised(), forwardDir.Normalised()) < -0.707 && dir.Length() > 5;
						}));

					AddTransition(new StateTransition(moveSlower, followPath, [&](void)->bool {

						Vector3 dir = player->GetTransform().GetPosition() - AI->GetTransform().GetPosition();
						Vector3 forwardDir = AI->GetTransform().GetOrientation() * Vector3(0, 0, -1);

						return Vector3::Dot(dir.Normalised(), forwardDir.Normalised()) > 0;
						}));

					AI->SetStateMachine(this);

				}
				~WaitingRaceAT() {}

			protected:
				float waitTime;
				float timer;
				Player* player;
				Vector3 finish;
				AIGameObject* AI;
				GameWorld* world;
			};
		}
	}
}