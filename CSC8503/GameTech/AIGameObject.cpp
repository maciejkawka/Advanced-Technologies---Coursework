#include "AIGameObject.h"
#include "../../Common/Window.h"
#include "../../Common/Maths.h"
#include"../CSC8503Common/Debug.h"


using namespace NCL;
using namespace CSC8503;

void AIGameObject::Update(float dt)
{
	if (finished)
		return;

	if (currentTime >= 1)
	{
		DecreasePoints();
		currentTime = 0;
	}

	StateGameObject::Update(dt);

	currentTime += dt;
}

void AIGameObject::OnCollisionBegin(GameObject* otherObject)
{
	if (otherObject->GetName() == "Finish")
	{
		finished = true;
		this->SetLayer(Layer::IgnoreRayCast);
	}

	if (otherObject->GetTag() == "Collectable")
	{
		Destroy(otherObject);
		points += 25;
	}
}

void AIGameObject::FindPath(Vector3& position)
{
	path.clear();
	NavigationPath outPath;

	Vector3 pathOffset = Vector3(36, 0, 78);
	Vector3 startPos(this->GetTransform().GetPosition() + pathOffset);
	Vector3 endPos(position + pathOffset);

	bool found = grid->FindPath(startPos, endPos, outPath);

	Vector3 pos;
	while (outPath.PopWaypoint(pos))
		path.push_back(pos - pathOffset);
}

void AIGameObject::DrawPath()
{
	for (int i = 1; i < path.size(); ++i)
	{
		Vector3 a = path[i - 1];
		Vector3 b = path[i];

		Debug::DrawLine(a, b, Vector4(1, 0, 0, 1));
	}
}

void NCL::CSC8503::AIGameObject::FollowPath()
{
	if (path.size() < 2)
		return;

	Quaternion agentOrient = this->GetTransform().GetOrientation();
	Vector3 agentPos = this->GetTransform().GetPosition();

	Vector3 dirToNode = (path[0] - agentPos);
	dirToNode.y = 0;
	float length = dirToNode.Length();

	if (length < 2)
	{
		Vector3 dirToNextNode = (path[1] - path[0]).Normalised();
		dirToNextNode.y = 0.0f;

		float cos = Vector3::Dot(Vector3(0, 0, -1), dirToNextNode);
		float angle = std::acos(cos);
		angle = Maths::RadiansToDegrees(angle) * -(dirToNextNode.x + dirToNextNode.z);
		Quaternion lookAt = Quaternion::EulerAnglesToQuaternion(0, angle, 0);

		this->GetTransform().SetOrientation(Quaternion::Lerp(agentOrient, lookAt, 0.05));
		this->GetPhysicsObject()->AddForce(dirToNextNode * moveForce);
		path.erase(path.begin());

	}
	else
	{
		dirToNode.Normalise();
		float cos = Vector3::Dot(Vector3(0, 0, -1), dirToNode);
		float angle = std::acos(cos);
		angle = Maths::RadiansToDegrees(angle) * -(dirToNode.x + dirToNode.z);
		Quaternion lookAt = Quaternion::EulerAnglesToQuaternion(0, angle, 0);

		this->GetTransform().SetOrientation(Quaternion::Lerp(agentOrient, lookAt, 0.05));
		this->GetPhysicsObject()->AddForce(dirToNode * moveForce);
	}

	Vector3 velocity = this->GetPhysicsObject()->GetLinearVelocity();
	Vector3 vel = Maths::Clamp(velocity, Vector3(-5, -5, -5), Vector3(5, 5, 5));
	physicsObject->SetLinearVelocity(vel);
}
