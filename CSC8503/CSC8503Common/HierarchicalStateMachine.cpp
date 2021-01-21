#include "HierarchicalStateMachine.h"
#include "StateMachine.h"
#include "HierarchicalStateTransition.h"


using namespace NCL::CSC8503;

HierarchicalStateMachine::HierarchicalStateMachine()
{
	activeState = nullptr;
}

HierarchicalStateMachine::~HierarchicalStateMachine()
{
	for (auto& i : allStates)
		delete i;
	for (auto& i : allTransitions)
		delete i.second;
}

void HierarchicalStateMachine::AddState(StateMachine* s)
{
	allStates.emplace_back(s);
	if (activeState == nullptr) {
		activeState = s;
	}
}

void HierarchicalStateMachine::AddTransition(HierarchicalStateTransition* t)
{
	allTransitions.insert(std::make_pair(t->GetSourceState(), t));
}

void HierarchicalStateMachine::Update(float dt)
{
	if (activeState) {
		

		//Get the transition set starting from this state node;
		std::pair<HierarchicalTransitionIterator, HierarchicalTransitionIterator> range = allTransitions.equal_range(activeState);

		for (auto& i = range.first; i != range.second; ++i) {
			if (i->second->CanTransition()) {
				StateMachine* newState = i->second->GetDestinationState();
				activeState = newState;
			}
		}

		activeState->Update(dt);  ///It is better to call update after transiotion. This prevent from refering to deleted memory. For instance, if we stop selecting to the object 
		//it switch firstly to the state machine which do not use pointer to selected object. We do not have to protect StateTransition from refering to unexisting memory. 
		//HierarchicalStateTransition check it before and switch StateMachie.
	}
}

