#pragma once
#include <vector>
#include <map>

namespace NCL {
	namespace CSC8503 {

		class StateMachine;
		class HierarchicalStateTransition;

		typedef std::multimap<StateMachine*, HierarchicalStateTransition*> HierarchicalTransitionContainer;
		typedef HierarchicalTransitionContainer::iterator HierarchicalTransitionIterator;

		class HierarchicalStateMachine {
		public:
			HierarchicalStateMachine();
			~HierarchicalStateMachine();

			void AddState(StateMachine* s);
			void AddTransition(HierarchicalStateTransition* t);

			void Update(float dt);

		protected:
			StateMachine* activeState;

			std::vector<StateMachine*> allStates;

			HierarchicalTransitionContainer allTransitions;
		};
	}
}