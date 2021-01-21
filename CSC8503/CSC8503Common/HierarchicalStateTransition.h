#pragma once
#include<functional>
namespace NCL {
	namespace CSC8503 {

		class StateMachine;
		typedef std::function<bool()> StateTransitionFunction;

		class HierarchicalStateTransition
		{
		public:
			HierarchicalStateTransition(StateMachine* source, StateMachine* dest, StateTransitionFunction f) {
				sourceState = source;
				destinationState = dest;
				function = f;
			}
			bool CanTransition() const {
				return function();
			}

			StateMachine* GetDestinationState() const { return destinationState; }
			StateMachine* GetSourceState() const { return sourceState; }

		protected:
			StateMachine* sourceState;
			StateMachine* destinationState;
			StateTransitionFunction function;
		};
	}
}

