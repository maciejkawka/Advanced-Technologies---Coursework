#pragma once
#include"Participant.h"
#include"../CSC8503Common/GameObject.h"

namespace NCL {
	namespace CSC8503 {

		class Player : public GameObject, public Participant {
		public:
			Player(string name = "Player") : GameObject(name) {}
			~Player() {}

			
			void Update(float dt) override;

			void OnCollisionBegin(GameObject* otherObject) override;
			void OnCollisionEnd(GameObject* otherObject) override;

		protected:
			void Move(float dt);
		};

	}
}