#pragma once
#include"BehaviourDecorator.h"

namespace NCL {
	namespace CSC8503 {

		class BehaviourInverter : public BehaviourDecorator {
		public:
			BehaviourInverter(const std::string& nodeName) : BehaviourDecorator(nodeName) {}
			~BehaviourInverter() {}

			BehaviourState Execute(float dt) override
			{
				BehaviourState nodeState = childNode->Execute(dt);
				switch (nodeState)
				{
				case Success: return Failure;
				case Failure: return Success;
				case Ongoing: return Ongoing;
				}
			}


		};
	}
}