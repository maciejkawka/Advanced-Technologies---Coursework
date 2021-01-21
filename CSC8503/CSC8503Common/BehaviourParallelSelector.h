#pragma once
#include"BehaviourNodeWithChildren.h"
#include <queue>

namespace NCL {
	namespace CSC8503 {

		class BehaviourParallelSelector : public BehaviourNodeWithChildren {
		public:
			BehaviourParallelSelector(const std::string& nodeName) : BehaviourNodeWithChildren(nodeName) {}
			~BehaviourParallelSelector() {}

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
					case Failure: continue;
					case Success:
					case Ongoing:
					{
						currentState = nodeState;
						return currentState;
					}
					}
				}
				return Failure;
			}
		};
	}
}