#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

#include "PhysicsObject.h"
#include "RenderObject.h"

#include <vector>

using std::vector;

namespace NCL {
	namespace CSC8503 {

		enum class Layer {
			Default = 1,
			IgnoreRayCast = 2,
			Transparent = 4,
			Water = 8,
			UI = 16,

		};

		class GameObject	{
		public:
			GameObject(string name = "");
			~GameObject();

			void SetBoundingVolume(CollisionVolume* vol) {
				boundingVolume = vol;
			}

			const CollisionVolume* GetBoundingVolume() const {
				return boundingVolume;
			}

			bool IsActive() const {
				return isActive;
			}

			Transform& GetTransform() {
				return transform;
			}

			RenderObject* GetRenderObject() const {
				return renderObject;
			}

			PhysicsObject* GetPhysicsObject() const {
				return physicsObject;
			}

			void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}

			void SetPhysicsObject(PhysicsObject* newObject) {
				physicsObject = newObject;
			}

			const string& GetName() const {
				return name;
			}

			void SetName(string name) {
				this->name = name;
			}

			virtual void OnCollisionBegin(GameObject* otherObject) {
				//std::cout << "OnCollisionBegin event occured!\n";
			}

			virtual void OnCollisionEnd(GameObject* otherObject) {
				//std::cout << "OnCollisionEnd event occured!\n";
			}

			bool GetBroadphaseAABB(Vector3&outsize) const;

			void UpdateBroadphaseAABB();

			void SetWorldID(int newID) {
				worldID = newID;
			}

			int		GetWorldID() const {
				return worldID;
			}

			Layer GetLayer() const { 
				return layer; 
			}

			unsigned int GetLayerFlag() const {
				return (unsigned int)layer;
			}

			void SetLayer(Layer layer) {
				this->layer = layer;
			}

			string GetTag() const { return tag; }

			void SetTag(string tag) { this->tag = tag; }

			virtual void Update(float dt) {}

			static void Destroy(GameObject* obj) { objectToDestroy.push_back(obj); } //Add object to list object to destory

			static std::vector<GameObject*>& GetDestroyList() { return objectToDestroy; }

		protected:
			Transform			transform;

			CollisionVolume*	boundingVolume;
			PhysicsObject*		physicsObject;
			RenderObject*		renderObject;
			Layer				layer;
			string				tag;
			bool	isActive;
			int		worldID;
			string	name;
			
			Vector3 broadphaseAABB;

			//M.Kawka
			static std::vector<GameObject*> objectToDestroy;  //List of object to destory
		};
	}
}

