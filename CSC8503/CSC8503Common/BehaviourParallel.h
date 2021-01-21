#pragma once
#include"BehaviourNodeWithChildren.h"
#include <queue>

namespace NCL {
	namespace CSC8503 {

		class BehaviourParallel : public BehaviourNodeWithChildren {
		public:
			BehaviourParallel(const std::string& nodeName) : BehaviourNodeWithChildren(nodeName) {}
			~BehaviourParallel() {}

			BehaviourState Execute(float dt) override
			{
				std::queue<BehaviourState> stateQ;

				for (auto& i : childNodes)
				{
					stateQ.emplace(i->Execute(dt));
				}

				for (auto& i : childNodes)
				{
					BehaviourState nodeState = stateQ.front();
					stateQ.pop();
					switch (nodeState)
					{
					case Success: continue;
					case Failure:
					case Ongoing:
					{
						currentState = nodeState;
						return currentState;
					}
					}
				}
				return Success;
			}
		};
	}
}