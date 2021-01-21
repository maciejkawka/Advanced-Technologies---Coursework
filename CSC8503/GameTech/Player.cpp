#include"Player.h"
#include "../../Common/Window.h"
#include "../../Common/Maths.h"

#include"../CSC8503Common/Debug.h"

using namespace NCL;
using namespace CSC8503;

void Player::Move(float dt)
{
	Vector3 forward = Vector3(0, 0, -1);
	Vector3 right = Vector3(1, 0, 0);
	Vector3 up = Vector3(0, 1, 0);
	float rotateTime = 0.05f;

	PhysicsObject* phys = this->GetPhysicsObject();

	Quaternion playerOrient = this->GetTransform().GetOrientation();
	Quaternion lookAt;

	Vector3 velocity = phys->GetLinearVelocity();
	float velocityMag = velocity.Length();
	if (velocityMag>maxVelocity)
	{
		Vector3 maxVelocityVector = velocity.Normalised()* maxVelocity;
		maxVelocityVector.y = velocity.y;
		phys->SetLinearVelocity(maxVelocityVector);
		return;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		phys->AddForce(-right * moveForce);


		lookAt = Quaternion::EulerAnglesToQuaternion(0, 90, 0);
		this->GetTransform().SetOrientation(Quaternion::Lerp(playerOrient, lookAt, rotateTime));
	}

	else if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		phys->AddForce(right * moveForce);

		lookAt = Quaternion::EulerAnglesToQuaternion(0, -90, 0);
		this->GetTransform().SetOrientation(Quaternion::Lerp(playerOrient, lookAt, rotateTime));
	}

	else if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		phys->AddForce(forward * moveForce);
		lookAt = Quaternion::EulerAnglesToQuaternion(0, 0, 0);
		this->GetTransform().SetOrientation(Quaternion::Lerp(playerOrient, lookAt, rotateTime));
	}

	else if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		phys->AddForce(-forward * moveForce);
		lookAt = Quaternion::EulerAnglesToQuaternion(0, 180, 0);
		this->GetTransform().SetOrientation(Quaternion::Lerp(playerOrient, lookAt, rotateTime));
	}

	else if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE)) {
		//	this->GetPhysicsObject()->ApplyLinearImpulse(up * moveForce);
	}

	Vector3 vel = Maths::Clamp(velocity, Vector3(-5, -5, -5), Vector3(5, 5, 5));
	physicsObject->SetLinearVelocity(vel);
}

void Player::Update(float dt)
{
	if (finished)
		return;

	Move(dt);

	if (currentTime >= 1)
	{
		DecreasePoints();
		currentTime = 0;
	}

	currentTime += dt;
}

void Player::OnCollisionBegin(GameObject* otherObject)
{
	if (otherObject->GetName() == "Finish")
		finished = true;

	if (otherObject->GetTag() == "Collectable")
	{
		Destroy(otherObject);
		points+=25;
	}
}

void Player::OnCollisionEnd(GameObject* otherObject)
{
}
