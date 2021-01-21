#include"PushdownGameObject.h"
#include"../CSC8503Common/PushdownMachine.h"

using namespace NCL;
using namespace CSC8503;

PushdownGameObject::PushdownGameObject(string name): GameObject(name)
{
	pushdownMachine = nullptr;
}

PushdownGameObject::PushdownGameObject(PushdownMachine* pushdownMachine, string name):GameObject(name)
{
	this->pushdownMachine = pushdownMachine;
}

NCL::CSC8503::PushdownGameObject::~PushdownGameObject()
{
	delete pushdownMachine;
}

void PushdownGameObject::SetPushdownMachine(PushdownMachine* pushdownMachine)
{
	this->pushdownMachine = pushdownMachine;
}

void NCL::CSC8503::PushdownGameObject::Update(float dt)
{
	if (pushdownMachine)
	{
		pushdownMachine->Update(dt);
	}
}

