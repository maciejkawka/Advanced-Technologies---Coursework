#pragma once
#include "BehaviourNode.h"
#include<vector>

namespace NCL {
	namespace CSC8503 {

		class BehaviourDecorator : public BehaviourNode {
		public:
			BehaviourDecorator(const std::string& nodeName) : BehaviourNode(nodeName) 
			{
				childNode = nullptr;
			}

			~BehaviourDecorator()
			{
				delete childNode;
			}

			void AddChild(BehaviourNode* n) {
				if (childNode)
					delete childNode;
				childNode = n;
			}

			void Reset() override {
					childNode->Reset();
			}

		protected:
			BehaviourNode* childNode;
		};
	}
}