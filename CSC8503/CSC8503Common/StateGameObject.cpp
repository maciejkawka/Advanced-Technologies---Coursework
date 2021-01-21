#include"StateGameObject.h"
#include"../CSC8503Common/State.h"
#include"../CSC8503Common/StateMachine.h"
#include"../CSC8503Common/StateTransition.h"
#include"../CSC8503Common/HierarchicalStateMachine.h"

using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject(string name): GameObject(name)
{
	counter = 0.0f;
	stateMachine = new StateMachine();

	State* stateA = new State([&](float dt)->void
		{
			this->MoveLeft(dt);
		});
	State* stateB = new State([&](float dt)->void
		{
			this->MoveRight(dt);
		});

	stateMachine->AddState(stateA);
	stateMachine->AddState(stateB);

	stateMachine->AddTransition(new StateTransition(stateA, stateB, [&]()->bool
		{
			return this->counter > 1.0f;
			
		}));

	stateMachine->AddTransition(new StateTransition(stateB, stateA, [&]()->bool
		{
			return this->counter < -2.0f;
		}));

}

StateGameObject::StateGameObject(StateMachine* stateMach, string name):GameObject(name)
{
	stateMachine = stateMach;
}

StateGameObject::StateGameObject(HierarchicalStateMachine* hstateMach, string name):GameObject(name)
{
	hierarchicalStateMachine = hstateMach;
}

StateGameObject::~StateGameObject()
{
	if(stateMachine)
		delete stateMachine;

	if (hierarchicalStateMachine)
		delete hierarchicalStateMachine;
}

void StateGameObject::Update(float dt)
{
	if (stateMachine)
		stateMachine->Update(dt);
	else
		hierarchicalStateMachine->Update(dt);
}

void StateGameObject::MoveLeft(float dt)
{
	GetPhysicsObject()->AddForce({ -20, 0, 0 });
	counter += dt;
}

void StateGameObject::MoveRight(float dt)
{
	GetPhysicsObject()->AddForce({ 20,0,0 });
	counter -= dt;
}