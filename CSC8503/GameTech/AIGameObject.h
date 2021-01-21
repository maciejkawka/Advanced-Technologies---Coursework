#pragma once
#include"../CSC8503Common/StateGameObject.h"
#include"../CSC8503Common/NavigationGrid.h"
#include"Participant.h"


namespace NCL {
	namespace CSC8503 {



		class AIGameObject : public StateGameObject, public Participant{
		public:
			AIGameObject(string name = "Runner") : StateGameObject(name)
			{
				grid = new NavigationGrid("arena.txt");
			}

			AIGameObject(StateMachine* node, string name = "Runner") : StateGameObject(node, name)
			{
				grid = new NavigationGrid("arena.txt");
			}

			~AIGameObject() {}

			void Update(float dt) override;
			void OnCollisionBegin(GameObject* otherObject) override;

			const std::vector<Vector3>& GetPath() const { return path; }

			void FindPath(Vector3& position);
			void DrawPath();
			void FollowPath();

		protected:
			std::vector<Vector3>  path;
			NavigationGrid* grid;

		};
	}
}