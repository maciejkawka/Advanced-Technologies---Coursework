#include "OrientationConstraint.h"
#include"GameObject.h"

using namespace NCL::CSC8503;
using namespace NCL::Maths;

void OrientationConstraint::UpdateConstraint(float dt)
{
	Quaternion relativeOrient = objectA->GetTransform().GetOrientation().Conjugate() * objectB->GetTransform().GetOrientation();
	Vector3 offset = orientation - relativeOrient.ToEuler();

	Vector3 relativePos = objectA->GetTransform().GetPosition() - objectB->GetTransform().GetPosition();
	float currentDistance = relativePos.Length();

	if (offset.Length() != 0)
	{
		Vector3 offsetDir = relativePos.Normalised();

		PhysicsObject* physA = objectA->GetPhysicsObject();
		PhysicsObject* physB = objectB->GetPhysicsObject();
	
		Vector3 relativeAngularVelocity = physA->GetAngularVelocity() - physB->GetAngularVelocity();
		float velocityDotA = Vector3::Dot(Vector3::Cross(relativeAngularVelocity,offsetDir), physA->GetAngularVelocity());
		float velocityDotB = Vector3::Dot(Vector3::Cross(relativeAngularVelocity,offsetDir), physA->GetAngularVelocity());

	}
}
