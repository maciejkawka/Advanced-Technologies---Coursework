#include "PushdownMachine.h"
#include "PushdownState.h"
using namespace NCL::CSC8503;

PushdownMachine::PushdownMachine()
{
	activeState = nullptr;
}

PushdownMachine::PushdownMachine(PushdownState* initialState)
{
	this->initialState = initialState;
	activeState = nullptr;
}

PushdownMachine::~PushdownMachine()
{
	if (stateStack.size())
	{
		while (stateStack.size() > 1)
		{
			delete activeState;
			stateStack.pop();
			activeState = stateStack.top();

		}

		delete activeState;

	}

}
void PushdownMachine::Reset()
{
	
	while(stateStack.size()>1)
	{
		delete activeState;
		stateStack.pop();
		activeState = stateStack.top();
		
	}

	activeState->OnAwake();
}
bool PushdownMachine::Update(float dt) {
	if (activeState) {
		PushdownState* newState = nullptr;
		PushdownState::PushdownResult result = activeState->OnUpdate(dt, &newState);

		switch (result) {
			case PushdownState::Pop: {
				activeState->OnSleep();
				delete activeState;
				stateStack.pop();
				if (stateStack.empty()) {
					return false;
				}
				else {
					activeState = stateStack.top();
					activeState->OnAwake();
				}
			}break;
			case PushdownState::Push: {
				activeState->OnSleep();
				stateStack.push(newState);
				activeState = newState;
				newState->OnAwake();
			}break;
		}
	}
	else {
		stateStack.push(initialState);
		activeState = initialState;
		activeState->OnAwake();
	}

	return true;

}