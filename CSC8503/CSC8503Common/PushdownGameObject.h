#pragma once
#include"../CSC8503Common/GameObject.h"
namespace NCL {
	namespace CSC8503 {
		class PushdownMachine;

		class PushdownGameObject : public GameObject {
		public:
			PushdownGameObject(string name = "");
			PushdownGameObject(PushdownMachine* pushdownMachine, string name = "");
			~PushdownGameObject();

			void SetPushdownMachine(PushdownMachine* pushdownMachine);

			PushdownMachine* GetPushdownMachine() { return pushdownMachine; }

			virtual void Update(float dt) override;

		protected:
			PushdownMachine* pushdownMachine;
			float counter;
		};
	}
}