#pragma once
#include"Constraint.h"
#include "../../Common/Vector3.h"



namespace NCL {
	namespace CSC8503 {

		using namespace Maths;
		class GameObject;

		class OrientationConstraint : public Constraint {
		public:
			OrientationConstraint(GameObject* a, GameObject* b, Vector3 o)
			{
				objectA = a;
				objectB = b;
				orientation = o;

			}
			~OrientationConstraint() {}

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* objectA;
			GameObject* objectB;

			Vector3 orientation;
		};
	}
}