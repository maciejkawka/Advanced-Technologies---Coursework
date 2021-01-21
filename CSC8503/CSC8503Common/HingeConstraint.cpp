#include "HingeConstraint.h"
#include"GameObject.h"
#include "../../Common/Maths.h"

using namespace NCL::CSC8503;

void HingeConstraint::UpdateConstraint(float dt)
{
    Vector3 direction = (objectA->GetTransform().GetPosition() - objectB->GetTransform().GetPosition()).Normalised();
    direction.y = 0;

    float aRot = -Maths::RadiansToDegrees(atan2f(direction.z, direction.x));
    float bRot = Maths::RadiansToDegrees(atan2f(-direction.z, direction.x));

    objectA->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, aRot, 0));
    objectB->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, bRot, 0));
}
