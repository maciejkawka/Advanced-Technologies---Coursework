#pragma once
#include "../../Common/Vector3.h"
#include "../../Common/Matrix3.h"

using namespace NCL::Maths;

namespace NCL {
	class CollisionVolume;
	
	namespace CSC8503 {
		class Transform;

		class PhysicsObject	{
		public:
			PhysicsObject(Transform* parentTransform, const CollisionVolume* parentVolume);
			~PhysicsObject();

			Vector3 GetLinearVelocity() const {
				return linearVelocity;
			}

			Vector3 GetAngularVelocity() const {
				return angularVelocity;
			}

			Vector3 GetTorque() const {
				return torque;
			}

			Vector3 GetForce() const {
				return force;
			}

			void SetInverseMass(float invMass) {
				inverseMass = invMass;
			}

			float GetInverseMass() const {
				return inverseMass;
			}

			void ApplyAngularImpulse(const Vector3& force);
			void ApplyLinearImpulse(const Vector3& force);
			
			void AddForce(const Vector3& force);

			void AddForceAtPosition(const Vector3& force, const Vector3& position);

			void AddTorque(const Vector3& torque);


			void ClearForces();

			void SetLinearVelocity(const Vector3& v) {
				linearVelocity = v;
			}

			void SetAngularVelocity(const Vector3& v) {
				angularVelocity = v;
			}

			void InitCubeInertia();
			void InitSphereInertia();
			void InitHollowSphereInertia();

			void UpdateInertiaTensor();

			Matrix3 GetInertiaTensor() const {
				return inverseInteriaTensor;
			}

			float GetElasticity() const { return elasticity; }

			void SetElasticity(float e) { elasticity = e; }

			float GetStaticFriction() const { return staticFriction; }

			void SetStaticFriction(float f) { staticFriction = f; }

			float GetDynamicFriction() const { return dynamicFriction; }

			void SetDynamicFriction(float f) { dynamicFriction = f; }

			void SetTrigger(bool setT) { trigger = setT; }

			bool isTrigger() const { return trigger; }

			void SetKinematic(bool setK) { Kinematic = setK; }

			bool isKinematic() const { return Kinematic; }

			void SetGravity(bool setG) { gravity = setG; }

			bool GetGravity() const { return gravity; }

		protected:
			const CollisionVolume* volume;
			Transform*		transform;

			float inverseMass;
			float elasticity;
			float dynamicFriction;
			float staticFriction;
			bool trigger;
			bool Kinematic;
			bool gravity;
			//linear stuff
			Vector3 linearVelocity;
			Vector3 force;
			

			//angular stuff
			Vector3 angularVelocity;
			Vector3 torque;
			Vector3 inverseInertia;
			Matrix3 inverseInteriaTensor;
		};
	}
}

