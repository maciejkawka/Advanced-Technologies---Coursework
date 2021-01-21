#pragma once
#include <stack>

namespace NCL {
	namespace CSC8503 {
		class PushdownState;

		class PushdownMachine
		{
		public:
			PushdownMachine();
			PushdownMachine(PushdownState* initialState);
			~PushdownMachine();

			bool Update(float);
			void Reset();
		protected:
			PushdownState * activeState;
			PushdownState * initialState;

			std::stack<PushdownState*> stateStack;
		};
	}
}

