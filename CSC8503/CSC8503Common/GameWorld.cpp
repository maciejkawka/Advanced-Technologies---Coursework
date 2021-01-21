#include "GameWorld.h"
#include "GameObject.h"
#include "Constraint.h"
#include "CollisionDetection.h"
#include "../../Common/Camera.h"
#include <algorithm>
#include<set>

using namespace NCL;
using namespace NCL::CSC8503;

//std::vector<GameObject*> GameWorld::objectToDestroy;

GameWorld::GameWorld()	{
	mainCamera = new Camera();

	shuffleConstraints	= false;
	shuffleObjects		= false;
	worldIDCounter		= 0;
}

GameWorld::~GameWorld()	{
	ClearAndErase();
	delete mainCamera;
}

void GameWorld::Clear() {
	gameObjects.clear();
	constraints.clear();
}

void GameWorld::ClearAndErase() {
	for (auto& i : gameObjects) {
		delete i;
	}
	for (auto& i : constraints) {
		delete i;
	}
	Clear();
}

void GameWorld::AddGameObject(GameObject* o) {
	gameObjects.emplace_back(o);
	o->SetWorldID(worldIDCounter++);
}

void GameWorld::RemoveGameObject(GameObject* o, bool andDelete) {
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), o), gameObjects.end());
	if (andDelete) {
		delete o;
	}
}

void GameWorld::GetObjectIterators(
	GameObjectIterator& first,
	GameObjectIterator& last) const {

	first	= gameObjects.begin();
	last	= gameObjects.end();
}

void GameWorld::OperateOnContents(GameObjectFunc f) {
	for (GameObject* g : gameObjects) {
		f(g);
	}
}

void GameWorld::UpdateWorld(float dt) {
	if (shuffleObjects) {
		std::random_shuffle(gameObjects.begin(), gameObjects.end());
	}

	if (shuffleConstraints) {
		std::random_shuffle(constraints.begin(), constraints.end());
	}
}

//Improved Raycast use quadtree sorting, but camera cannot be very close to object. To improve this I should use OctaTree
bool GameWorld::ImprovedRaycast(Ray& r, RayCollision& closestCollision, bool closestObject, unsigned int layerMask) const
{
	QuadTree<GameObject*> tree(Vector2(100, 100), 7, 6);

	for (auto* i : gameObjects)
	{
		Vector3 halfSize;
		if (!i->GetBroadphaseAABB(halfSize))
			continue;
		Vector3 pos = i->GetTransform().GetPosition();
		tree.Insert(i, pos, halfSize);
	}

	std::set<GameObject*> rayIntersection;
	tree.OperateOnIntersectionContents([&](std::list<QuadTreeEntry<GameObject*>>& data){
			for (auto i = data.begin(); i != data.end(); ++i)
				rayIntersection.insert(i->object);
		}, r);

	//The simplest raycast just goes through each object and sees if there's a collision
	RayCollision collision;

	//Cast line 
	Debug::DrawLine(r.GetPosition(), r.GetPosition() + (r.GetDirection()) * 100, Vector4(1, 0, 0, 1), 2);

	//NarrowPhase
	for (auto& i : rayIntersection) {
		if (!i->GetBoundingVolume()) { //objects might not be collideable etc...
			continue;
		}
		if (!(i->GetLayerFlag() & layerMask)) //if layer doesn't match layerMask
			continue;

		RayCollision thisCollision;
		if (CollisionDetection::RayIntersection(r, *i, thisCollision)) {

			if (!closestObject) {
				closestCollision = collision;
				closestCollision.node = i;
				return true;
			}
			else {
				if (thisCollision.rayDistance < collision.rayDistance) {
					thisCollision.node = i;
					collision = thisCollision;
				}
			}
		}
	}
	if (collision.node) {
		closestCollision = collision;
		closestCollision.node = collision.node;
		return true;
	}
	return false;
}

bool GameWorld::Raycast(Ray& r, RayCollision& closestCollision, bool closestObject, unsigned int layerMask) const {
	//The simplest raycast just goes through each object and sees if there's a collision
	RayCollision collision;
	
	//Cast line 
	//Debug::DrawLine(r.GetPosition(), r.GetPosition() + (r.GetDirection()) * 100, Vector4(1, 0, 0, 1), 2);

	for (auto& i : gameObjects) {
		if (!i->GetBoundingVolume()) { //objects might not be collideable etc...
			continue;
		}
		if (!(i->GetLayerFlag() & layerMask)) //if layer doesn't match layerMask
			continue;

		RayCollision thisCollision;
		if (CollisionDetection::RayIntersection(r, *i, thisCollision)) {

			if (!closestObject) {	
				closestCollision		= collision;
				closestCollision.node = i;
				return true;
			}
			else {
				if (thisCollision.rayDistance < collision.rayDistance) {
					thisCollision.node = i;
					collision = thisCollision;
				}
			}
		}
	}
	if (collision.node) {
		closestCollision		= collision;
		closestCollision.node	= collision.node;
		return true;
	}
	return false;
}


/*
Constraint Tutorial Stuff
*/

void GameWorld::AddConstraint(Constraint* c) {
	constraints.emplace_back(c);
}

void GameWorld::RemoveConstraint(Constraint* c, bool andDelete) {
	constraints.erase(std::remove(constraints.begin(), constraints.end(), c), constraints.end());
	if (andDelete) {
		delete c;
	}
}

void GameWorld::GetConstraintIterators(
	std::vector<Constraint*>::const_iterator& first,
	std::vector<Constraint*>::const_iterator& last) const {
	first	= constraints.begin();
	last	= constraints.end();
}

//// M.Kawka
//void GameWorld::Destroy(GameObject* obj)
//{
//	objectToDestroy.push_back(obj);
//}
//
void GameWorld::CleanGameObjects()
{
	for (auto& i : GameObject::GetDestroyList())
		RemoveGameObject(i);
	GameObject::GetDestroyList().clear();
}
//