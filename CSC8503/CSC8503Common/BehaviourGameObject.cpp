#pragma once
#include"BehaviourGameObject.h"

using namespace NCL;
using namespace CSC8503;

BehaviourGameObject::BehaviourGameObject(string name): GameObject(name)
{
	rootNode = nullptr;
}

BehaviourGameObject::BehaviourGameObject(BehaviourNode* node, string name) : GameObject(name)
{
	rootNode = node;
}

BehaviourGameObject::~BehaviourGameObject()
{
	delete rootNode;
}

void BehaviourGameObject::Update(float dt)
{
	if (rootNode == nullptr)
		return;


	if (rootNode->Execute(dt) != Success)
		return;
	rootNode->Reset();
	
}
