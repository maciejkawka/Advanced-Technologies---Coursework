#pragma once
#include"GameObject.h"
namespace NCL {
	namespace CSC8503 {
		class StateMachine;
		class HierarchicalStateMachine;
		class StateGameObject : public GameObject {
		public:
			StateGameObject(string name = "");
			StateGameObject(StateMachine* stateMach, string name = "");
			StateGameObject(HierarchicalStateMachine* hstateMach, string name = "");
			~StateGameObject();

			void SetStateMachine(StateMachine* stateMachine) { this->stateMachine = stateMachine; }
			StateMachine* GetStateMachine() const { return stateMachine; }

			void SetStatHierarchicalStateMachine(HierarchicalStateMachine* hierarchicalStateMachine) { this->hierarchicalStateMachine = hierarchicalStateMachine; }
			HierarchicalStateMachine* GetStatHierarchicalStateMachine() const { return hierarchicalStateMachine; }

			virtual void Update(float dt) override;

		protected:
			void MoveLeft(float dt);
			void MoveRight(float dt);

			StateMachine* stateMachine;
			HierarchicalStateMachine* hierarchicalStateMachine;
			float counter;
		};
	}
}