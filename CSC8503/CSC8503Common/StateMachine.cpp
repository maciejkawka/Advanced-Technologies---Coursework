#include "StateMachine.h"
#include "State.h"
#include "StateTransition.h"

using namespace NCL::CSC8503;

StateMachine::StateMachine()
{
	activeState = nullptr;
}

StateMachine::~StateMachine()
{
	for (auto& i : allStates)
		delete i;
	for (auto& i : allTransitions)
		delete i.second;
}

void StateMachine::AddState(State* s) {
	allStates.emplace_back(s);
	if (activeState == nullptr) {
		activeState = s;
	}
}

void StateMachine::AddTransition(StateTransition* t) {
	allTransitions.insert(std::make_pair(t->GetSourceState(), t));
}

void StateMachine::Update(float dt) {
	if (activeState) {
		
	
		//Get the transition set starting from this state node;
		std::pair<TransitionIterator, TransitionIterator> range = allTransitions.equal_range(activeState);

		for (auto& i = range.first; i != range.second; ++i) {
			if (i->second->CanTransition()) {
				State* newState = i->second->GetDestinationState();
				activeState = newState;
			}
		}

		activeState->Update(dt); ///It is better to call update after transiotion. This prevent from refering to deleted memory. For instance, if we stop selecting to the object 
		//it switch firstly to the state machine which do not use pointer to selected object. We do not have to protect StateTransition from refering to unexisting memory. 
		//HierarchicalStateTransition check it before and switch StateMachie.
	}
}