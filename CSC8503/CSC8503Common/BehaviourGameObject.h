#pragma once
#include"../CSC8503Common/BehaviourTree.h"
#include"../CSC8503Common/GameObject.h"


namespace NCL {
	namespace CSC8503 {
		class BehaviourGameObject : public GameObject {
		public:
			BehaviourGameObject(string name = "");
			BehaviourGameObject(BehaviourNode* node, string name = "");
			~BehaviourGameObject();

			void SetBehaviourTree(BehaviourNode* n) { rootNode = n; }
			BehaviourNode* GetBehaviourTree() const { return rootNode; }

			virtual void Update(float dt) override;

		protected:
			BehaviourNode* rootNode;
		};
	}
}
