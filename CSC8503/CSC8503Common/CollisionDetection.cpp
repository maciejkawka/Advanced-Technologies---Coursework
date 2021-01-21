#include "CollisionDetection.h"
#include "CollisionVolume.h"
#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "../../Common/Vector2.h"
#include "../../Common/Window.h"
#include "../../Common/Maths.h"
#include "Debug.h"

#include <list>

using namespace NCL;

bool CollisionDetection::RayPlaneIntersection(const Ray&r, const Plane&p, RayCollision& collisions) {
	float ln = Vector3::Dot(p.GetNormal(), r.GetDirection());

	if (ln == 0.0f) {
		return false; //direction vectors are perpendicular!
	}
	
	Vector3 planePoint = p.GetPointOnPlane();

	Vector3 pointDir = planePoint - r.GetPosition();

	float d = Vector3::Dot(pointDir, p.GetNormal()) / ln;

	collisions.collidedAt = r.GetPosition() + (r.GetDirection() * d);

	return true;
}

bool CollisionDetection::RayIntersection(const Ray& r,GameObject& object, RayCollision& collision) {
	bool hasCollided = false;

	const Transform& worldTransform = object.GetTransform();
	const CollisionVolume* volume	= object.GetBoundingVolume();

	if (!volume) {
		return false;
	}

	switch (volume->type) {
		case VolumeType::AABB:		hasCollided = RayAABBIntersection(r, worldTransform, (const AABBVolume&)*volume	, collision); break;
		case VolumeType::OBB:		hasCollided = RayOBBIntersection(r, worldTransform, (const OBBVolume&)*volume	, collision); break;
		case VolumeType::Sphere:	hasCollided = RaySphereIntersection(r, worldTransform, (const SphereVolume&)*volume	, collision); break;
		case VolumeType::Capsule:	hasCollided = RayCapsuleIntersection(r, worldTransform, (const CapsuleVolume&)*volume, collision); break;
	}

	return hasCollided;
}

bool CollisionDetection::RayBoxIntersection(const Ray&r, const Vector3& boxPos, const Vector3& boxSize, RayCollision& collision) {
	Vector3 boxMin = boxPos - boxSize;
	Vector3 boxMax = boxPos + boxSize;

	Vector3 rayPos = r.GetPosition();
	Vector3 rayDir = r.GetDirection();

	Vector3 tVals(-1, -1, -1);

	for (int i = 0; i < 3; ++i)
	{
		if (rayDir[i] > 0)
			tVals[i] = (boxMin[i] - rayPos[i]) / rayDir[i];
		else if (rayDir[i] < 0)
			tVals[i] = (boxMax[i] - rayPos[i]) / rayDir[i];
	}

	float bestT = tVals.GetMaxElement();
	if (bestT < 0.0f)
		return false;
	
	Vector3 intersection = rayPos + (rayDir * bestT);
	const float epsilon = 0.0001f; //an amount of leeway in out calcs
	for (int i = 0; i < 3; ++i)
		if (intersection[i] + epsilon<boxMin[i] || intersection[i] - epsilon>boxMax[i])
			return false;

	collision.collidedAt = intersection;
	collision.rayDistance = bestT;

	return true;
}

bool CollisionDetection::RayAABBIntersection(const Ray&r, const Transform& worldTransform, const AABBVolume& volume, RayCollision& collision) {
	
	Vector3 boxPos = worldTransform.GetPosition();
	Vector3 boxSize = volume.GetHalfDimensions();
	return RayBoxIntersection(r, boxPos, boxSize, collision);
}

bool CollisionDetection::RayOBBIntersection(const Ray&r, const Transform& worldTransform, const OBBVolume& volume, RayCollision& collision) {
	
	Quaternion orientation = worldTransform.GetOrientation();
	Vector3 position = worldTransform.GetPosition();

	Matrix3 transform = Matrix3(orientation);
	Matrix3 invTransform = Matrix3(orientation.Conjugate());

	Vector3 localRayPos = r.GetPosition() - position;

	Ray tempRay(invTransform * localRayPos, invTransform * r.GetDirection());

	bool collided = RayBoxIntersection(tempRay, Vector3(), volume.GetHalfDimensions(), collision);

	if (collided)
		collision.collidedAt = transform * collision.collidedAt + position;
	return collided;
}

bool CollisionDetection::RayCapsuleIntersection(const Ray& r, const Transform& worldTransform, const CapsuleVolume& volume, RayCollision& collision) {

	//Auxiliary variables
	Vector3 capsulePos = worldTransform.GetPosition();
	Quaternion capsuleOrientation = worldTransform.GetOrientation();
	Vector3 upVector = (capsuleOrientation * Vector3(0, 1, 0)).Normalised();
	
	//Points to find a plane
	Vector3 directionToRay = r.GetPosition() - capsulePos;
	Vector3 secondVector =  upVector;
	Vector3 thirdVector = Vector3::Cross(secondVector, directionToRay);

	//Plane facing ray position
	Vector3 planeNormal = Vector3::Cross(thirdVector, upVector).Normalised();
	float planeD = -Vector3::Dot(planeNormal, capsulePos);

	//Calculating where the ray intersects the plane.
	if (!RayPlaneIntersection(r, Plane(planeNormal, planeD), collision))
		return false;
	Vector3 p = collision.collidedAt;

	//float t = -(Vector3::Dot(r.GetPosition(), planeNormal) + planeD) / Vector3::Dot(r.GetDirection(), planeNormal);
	//Vector3 p = r.GetPosition() + r.GetDirection()*t;
	//if (t < 0)
	//	return false;

	//Case when ray intersects top hemisphere
	Vector3 topSpherePos = capsulePos + upVector * (volume.GetHalfHeight() - volume.GetRadius());
	if (Vector3::Dot(p - topSpherePos, -upVector) < 0)
	{
		Transform topSphereTransform;
		topSphereTransform.SetOrientation(worldTransform.GetOrientation());
		topSphereTransform.SetPosition(topSpherePos);
		topSphereTransform.SetScale(worldTransform.GetScale());

		SphereVolume topSphereVolume(volume.GetRadius());

		return RaySphereIntersection(r, topSphereTransform, topSphereVolume, collision);
	}

	//Case when ray intersects bottom hemisphere
	Vector3 bottomSpherePos = capsulePos - upVector * (volume.GetHalfHeight() - volume.GetRadius());
	if (Vector3::Dot(p - bottomSpherePos, upVector) < 0)
	{
		Transform bottomSphereTransform;
		bottomSphereTransform.SetOrientation(worldTransform.GetOrientation());
		bottomSphereTransform.SetPosition(bottomSpherePos);
		bottomSphereTransform.SetScale(worldTransform.GetScale());

		SphereVolume bottomSphereVolume(volume.GetRadius());

		return RaySphereIntersection(r, bottomSphereTransform, bottomSphereVolume, collision);
	}

	//Case when ray intersetcs cylinder between two hemispheres
	 Vector3 d = capsulePos + upVector * Vector3::Dot(p - capsulePos, upVector);
	 float length = (p - d).Length();
	if (length < volume.GetRadius())
	{
		Vector3 cylinderSphere = d;
		Transform cylinderSphereTransform;
		cylinderSphereTransform.SetOrientation(worldTransform.GetOrientation());
		cylinderSphereTransform.SetPosition(cylinderSphere);
		cylinderSphereTransform.SetScale(worldTransform.GetScale());

		SphereVolume cylinderSphereVolume(volume.GetRadius());

		return RaySphereIntersection(r, cylinderSphereTransform, cylinderSphereVolume, collision);
	}
	 
	return false;
}

bool CollisionDetection::RaySphereIntersection(const Ray&r, const Transform& worldTransform, const SphereVolume& volume, RayCollision& collision) {
	Vector3 spherePos = worldTransform.GetPosition();
	float sphereRadius = volume.GetRadius();

	//Get the direction between the ray orgin and the sphere origin
	Vector3 dir = (spherePos - r.GetPosition());

	//Then project the sphere's origin onto our ray direction vector
	float sphereProj = Vector3::Dot(dir, r.GetDirection());

	if (sphereProj < 0.0f)
		return false;

	//Get closest point on ray line to sphere
	Vector3 point = r.GetPosition() + (r.GetDirection() * sphereProj);

	float sphereDist = (point - spherePos).Length();

	if (sphereDist > sphereRadius)
		return false;

	float offset = sqrt((sphereRadius * sphereRadius) - (sphereDist * sphereDist));

	collision.rayDistance = sphereProj - offset;
	collision.collidedAt = r.GetPosition()+(r.GetDirection()*collision.rayDistance);

	return true;
}

Matrix4 GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
		Matrix4::Translation(position) *
		Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
		Matrix4::Rotation(-pitch, Vector3(-1, 0, 0));

	return iview;
}

Vector3 CollisionDetection::Unproject(const Vector3& screenPos, const Camera& cam) {
	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	float aspect	= screenSize.x / screenSize.y;
	float fov		= cam.GetFieldOfVision();
	float nearPlane = cam.GetNearPlane();
	float farPlane  = cam.GetFarPlane();

	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(cam) * GenerateInverseProjection(aspect, fov, nearPlane, farPlane);

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(screenPos.x / (float)screenSize.x) * 2.0f - 1.0f,
		(screenPos.y / (float)screenSize.y) * 2.0f - 1.0f,
		(screenPos.z),
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

Ray CollisionDetection::BuildRayFromMouse(const Camera& cam) {
	Vector2 screenMouse = Window::GetMouse()->GetAbsolutePosition();
	Vector2 screenSize	= Window::GetWindow()->GetScreenSize();

	//We remove the y axis mouse position from height as OpenGL is 'upside down',
	//and thinks the bottom left is the origin, instead of the top left!
	Vector3 nearPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		-0.99999f
	);

	//We also don't use exactly 1.0 (the normalised 'end' of the far plane) as this
	//causes the unproject function to go a bit weird. 
	Vector3 farPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		0.99999f
	);

	Vector3 a = Unproject(nearPos, cam);
	Vector3 b = Unproject(farPos, cam);
	Vector3 c = b - a;

	c.Normalise();

	//std::cout << "Ray Direction:" << c << std::endl;

	return Ray(cam.GetPosition(), c);
}

//http://bookofhook.com/mousepick.pdf
Matrix4 CollisionDetection::GenerateInverseProjection(float aspect, float fov, float nearPlane, float farPlane) {
	Matrix4 m;

	float t = tan(fov*PI_OVER_360);

	float neg_depth = nearPlane - farPlane;

	const float h = 1.0f / t;

	float c = (farPlane + nearPlane) / neg_depth;
	float e = -1.0f;
	float d = 2.0f*(nearPlane*farPlane) / neg_depth;

	m.array[0]  = aspect / h;
	m.array[5]  = tan(fov*PI_OVER_360);

	m.array[10] = 0.0f;
	m.array[11] = 1.0f / d;

	m.array[14] = 1.0f / e;

	m.array[15] = -c / (d*e);

	return m;
}

/*
And here's how we generate an inverse view matrix. It's pretty much
an exact inversion of the BuildViewMatrix function of the Camera class!
*/
Matrix4 CollisionDetection::GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
Matrix4::Translation(position) *
Matrix4::Rotation(yaw, Vector3(0, 1, 0)) *
Matrix4::Rotation(pitch, Vector3(1, 0, 0));

return iview;
}


/*
If you've read through the Deferred Rendering tutorial you should have a pretty
good idea what this function does. It takes a 2D position, such as the mouse
position, and 'unprojects' it, to generate a 3D world space position for it.

Just as we turn a world space position into a clip space position by multiplying
it by the model, view, and projection matrices, we can turn a clip space
position back to a 3D position by multiply it by the INVERSE of the
view projection matrix (the model matrix has already been assumed to have
'transformed' the 2D point). As has been mentioned a few times, inverting a
matrix is not a nice operation, either to understand or code. But! We can cheat
the inversion process again, just like we do when we create a view matrix using
the camera.

So, to form the inverted matrix, we need the aspect and fov used to create the
projection matrix of our scene, and the camera used to form the view matrix.

*/
Vector3	CollisionDetection::UnprojectScreenPosition(Vector3 position, float aspect, float fov, const Camera &c) {
	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(c) * GenerateInverseProjection(aspect, fov, c.GetNearPlane(), c.GetFarPlane());

	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(position.x / (float)screenSize.x) * 2.0f - 1.0f,
		(position.y / (float)screenSize.y) * 2.0f - 1.0f,
		(position.z) - 1.0f,
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

bool CollisionDetection::ObjectIntersection(GameObject* a, GameObject* b, CollisionInfo& collisionInfo) {
	const CollisionVolume* volA = a->GetBoundingVolume();
	const CollisionVolume* volB = b->GetBoundingVolume();

	if (!volA || !volB) {
		return false;
	}

	collisionInfo.a = a;
	collisionInfo.b = b;

	Transform& transformA = a->GetTransform();
	Transform& transformB = b->GetTransform();
	
	VolumeType pairType = (VolumeType)((int)volA->type | (int)volB->type);

	if (pairType == VolumeType::AABB) {
		return AABBIntersection((AABBVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);
	}

	if (pairType == VolumeType::Sphere) {
		return SphereIntersection((SphereVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}

	if (pairType == VolumeType::OBB) {
		return OBBIntersection((OBBVolume&)*volA, transformA, (OBBVolume&)*volB, transformB, collisionInfo);
	}

	if (pairType == VolumeType::Capsule){
		return CapsuleIntersection((CapsuleVolume&)*volA, transformA, (CapsuleVolume&)*volB, transformB, collisionInfo);
	}

	if (volA->type == VolumeType::AABB && volB->type == VolumeType::Sphere) {
		return AABBSphereIntersection((AABBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}

	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::AABB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBSphereIntersection((AABBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	if (volA->type == VolumeType::OBB && volB->type == VolumeType::Sphere){
		return OBBSphereIntersection((AABBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}

	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::OBB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return OBBSphereIntersection((AABBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::Sphere) {
		return SphereCapsuleIntersection((CapsuleVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}

	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::Capsule) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return SphereCapsuleIntersection((CapsuleVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::AABB) {
		return AABBCapsuleIntersection((CapsuleVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);
	}

	if (volA->type == VolumeType::AABB && volB->type == VolumeType::Capsule) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBCapsuleIntersection((CapsuleVolume&)*volB, transformB, (AABBVolume&)*volA, transformA, collisionInfo);
	}

	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::OBB) {
		return OBBCapsuleIntersection((CapsuleVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);
	}

	if (volA->type == VolumeType::OBB && volB->type == VolumeType::Capsule) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return OBBCapsuleIntersection((CapsuleVolume&)*volB, transformB, (AABBVolume&)*volA, transformA, collisionInfo);
	}

	return false;
}

bool CollisionDetection::AABBTest(const Vector3& posA, const Vector3& posB, const Vector3& halfSizeA, const Vector3& halfSizeB) {

	Vector3 delta = posB - posA;
	Vector3 totalSize = halfSizeA + halfSizeB;

	if (abs(delta.x) < totalSize.x && abs(delta.y) < totalSize.y && abs(delta.z) < totalSize.z)
		return true;
	return false;
}

//AABB/AABB Collisions
bool CollisionDetection::AABBIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {

	Vector3 boxAPos = worldTransformA.GetPosition();
	Vector3 boxBPos = worldTransformB.GetPosition();

	Vector3 boxASize = volumeA.GetHalfDimensions();
	Vector3 boxBSize = volumeB.GetHalfDimensions();

	bool overlap = AABBTest(boxAPos, boxBPos, boxASize, boxBSize);

	if (overlap)
	{
		static const Vector3 faces[6] =
		{
			Vector3(-1,0,0), Vector3(1,0,0),
			Vector3(0,-1,0), Vector3(0,1,0),
			Vector3(0,0,-1), Vector3(0,0,1)
		}; //It shoud be switched.

		Vector3 maxA = boxAPos + boxASize;
		Vector3 minA = boxAPos - boxASize;

		Vector3 maxB = boxBPos + boxBSize;
		Vector3 minB = boxBPos - boxBSize;

		float distances[6] =
		{
			(maxB.x - minA.x),
			(maxA.x - minB.x),
			(maxB.y - minA.y),
			(maxA.y - minB.y),
			(maxB.z - minA.z),
			(maxA.z - minB.z)
		};
	
		float penetration = FLT_MAX;
		Vector3 bestAxis;

		for (int i = 0; i < 6; i++)
		{
			if (distances[i] < penetration)
			{
				penetration = distances[i];
				bestAxis = faces[i];
			}
		}
		collisionInfo.AddContactPoint(Vector3(), Vector3(), bestAxis, penetration);
		return true;
	}
	return false;
}

//Sphere / Sphere Collision
bool CollisionDetection::SphereIntersection(const SphereVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {

	float radii = volumeA.GetRadius() + volumeB.GetRadius();
	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();

	float deltaLength = delta.Length();

	if (deltaLength < radii)
	{
		float penetration = (radii - deltaLength);
		Vector3 normal = delta.Normalised();
		Vector3 localA = normal * volumeA.GetRadius();
		Vector3 localB = -normal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, normal, penetration);
		return true;
	}
	return false;
}

//AABB - Sphere Collision
bool CollisionDetection::AABBSphereIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {

	Vector3 boxSize = volumeA.GetHalfDimensions();
	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();

	Vector3 closestPointOnBox = Maths::Clamp(delta, -boxSize, boxSize);

	Vector3 localPoint = delta - closestPointOnBox;
	float distance = localPoint.Length();

	if (distance < volumeB.GetRadius())
	{
		Vector3 collisionNormal = localPoint.Normalised();
		float penetration = (volumeB.GetRadius() - distance);

		Vector3 localA = Vector3();
		Vector3 localB = -collisionNormal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);
		return true;
	}
	return false;
}

//OBB - Sphere intersection
bool CollisionDetection::OBBSphereIntersection(const AABBVolume& volumeA, const Transform& worldTransformA, const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	Quaternion orientationA = worldTransformA.GetOrientation();
	Quaternion invOrientationA = orientationA.Conjugate();

	Vector3 localPosB = worldTransformB.GetPosition() - worldTransformA.GetPosition();
	Vector3 rotatedLocalPosB = invOrientationA * localPosB;

	Transform tempBTransform;
	tempBTransform.SetOrientation(worldTransformB.GetOrientation());
	tempBTransform.SetPosition(rotatedLocalPosB + worldTransformA.GetPosition());
	tempBTransform.SetScale(worldTransformB.GetScale());

	if (AABBSphereIntersection(volumeA, worldTransformA, volumeB, tempBTransform, collisionInfo))
	{
		Vector3 boxSize = volumeA.GetHalfDimensions();
		Vector3 delta = tempBTransform.GetPosition() - worldTransformA.GetPosition();
		Vector3 closestPointOnBox = Maths::Clamp(delta, -boxSize, boxSize);

		collisionInfo.point.normal = orientationA * collisionInfo.point.normal;
		collisionInfo.point.localB = orientationA * collisionInfo.point.localB;
		collisionInfo.point.localA = orientationA * closestPointOnBox;

		return true;
	}
	return false;
}

bool CollisionDetection::OBBIntersection(
	const OBBVolume& volumeA, const Transform& worldTransformA,
	const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	return false;
}

bool CollisionDetection::SphereCapsuleIntersection(
	const CapsuleVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {

	Vector3 capsulePos = worldTransformA.GetPosition();
	Vector3 upVector = (worldTransformA.GetOrientation() * Vector3(0, 1, 0)).Normalised();

	Vector3 spherePos = worldTransformB.GetPosition();


	//Case when ray intersects top hemisphere
	Vector3 topSpherePos = capsulePos + upVector * (volumeA.GetHalfHeight() - volumeA.GetRadius());
	if (Vector3::Dot(spherePos - topSpherePos, -upVector) < 0)
	{
		//TODO
		Transform topSphereTransform; // To switch to copy constructor
		topSphereTransform.SetOrientation(worldTransformA.GetOrientation());
		topSphereTransform.SetPosition(topSpherePos);
		topSphereTransform.SetScale(worldTransformA.GetScale());

		SphereVolume topSphereVolume(volumeA.GetRadius());

		bool isColliding = SphereIntersection(topSphereVolume, topSphereTransform, volumeB, worldTransformB, collisionInfo);
		collisionInfo.point.localA = Vector3();
		return isColliding;
	}

	//Case when ray intersects bottom hemisphere
	Vector3 bottomSpherePos = capsulePos - upVector * (volumeA.GetHalfHeight() - volumeA.GetRadius());
	if (Vector3::Dot(spherePos - bottomSpherePos, upVector) < 0)
	{
		Transform bottomSphereTransform;
		bottomSphereTransform.SetOrientation(worldTransformA.GetOrientation());
		bottomSphereTransform.SetPosition(bottomSpherePos);
		bottomSphereTransform.SetScale(worldTransformA.GetScale());

		SphereVolume bottomSphereVolume(volumeA.GetRadius());

		bool isColliding = SphereIntersection(bottomSphereVolume, bottomSphereTransform, volumeB, worldTransformB, collisionInfo);
		collisionInfo.point.localA = Vector3();
		return isColliding;
	}


	Vector3 d = capsulePos + upVector * Vector3::Dot(spherePos - capsulePos, upVector);
	float length = (spherePos - d).Length();
	if (length < (volumeA.GetRadius() + volumeB.GetRadius()))
	{
		Vector3 cylinderSphere = d;
		Transform cylinderSphereTransform;
		cylinderSphereTransform.SetOrientation(worldTransformA.GetOrientation());
		cylinderSphereTransform.SetPosition(cylinderSphere);
		cylinderSphereTransform.SetScale(worldTransformA.GetScale());

		SphereVolume cylinderSphereVolume(volumeA.GetRadius());

		bool isColliding = SphereIntersection(cylinderSphereVolume, cylinderSphereTransform, volumeB, worldTransformB, collisionInfo);
		collisionInfo.point.localA = Vector3();
		return isColliding;
	}

	return false;
}

bool CollisionDetection::AABBCapsuleIntersection(
	const CapsuleVolume& volumeA, const Transform& worldTransformA,
	const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {

	Vector3 capsulePos = worldTransformA.GetPosition();
	Vector3 upVector = (worldTransformA.GetOrientation() * Vector3(0, 1, 0)).Normalised();

	Vector3 AABBPos = worldTransformB.GetPosition();


	//Case when ray intersects top hemisphere
	Vector3 topSpherePos = capsulePos + upVector * (volumeA.GetHalfHeight() - volumeA.GetRadius());
	if (Vector3::Dot(AABBPos - topSpherePos, -upVector) < 0)
	{
		Transform topSphereTransform;
		topSphereTransform.SetOrientation(worldTransformA.GetOrientation());
		topSphereTransform.SetPosition(topSpherePos);
		topSphereTransform.SetScale(worldTransformA.GetScale());

		SphereVolume topSphereVolume(volumeA.GetRadius());

		GameObject* tempA = collisionInfo.a;
		collisionInfo.a = collisionInfo.b;
		collisionInfo.b = tempA;
		bool isColliding = AABBSphereIntersection(volumeB, worldTransformB, topSphereVolume, topSphereTransform, collisionInfo);
		collisionInfo.point.localB = Vector3();
		return isColliding;
	}

	//Case when ray intersects bottom hemisphere
	Vector3 bottomSpherePos = capsulePos - upVector * (volumeA.GetHalfHeight() - volumeA.GetRadius());
	if (Vector3::Dot(AABBPos - bottomSpherePos, upVector) < 0)
	{
		Transform bottomSphereTransform;
		bottomSphereTransform.SetOrientation(worldTransformA.GetOrientation());
		bottomSphereTransform.SetPosition(bottomSpherePos);
		bottomSphereTransform.SetScale(worldTransformA.GetScale());

		SphereVolume bottomSphereVolume(volumeA.GetRadius());

		GameObject* tempA = collisionInfo.a;
		collisionInfo.a = collisionInfo.b;
		collisionInfo.b = tempA;
		bool isColliding = AABBSphereIntersection(volumeB, worldTransformB, bottomSphereVolume, bottomSphereTransform, collisionInfo);
		collisionInfo.point.localB = Vector3();
		return isColliding;
	}

	//Case when ray intersects cylinder
	Vector3 d = capsulePos + upVector * Vector3::Dot(AABBPos - capsulePos, upVector);
	float length = (AABBPos - d).Length();

	Vector3 cylinderSphere = d;
	Transform cylinderSphereTransform;
	cylinderSphereTransform.SetOrientation(worldTransformA.GetOrientation());
	cylinderSphereTransform.SetPosition(cylinderSphere);
	cylinderSphereTransform.SetScale(worldTransformA.GetScale());

	SphereVolume cylinderSphereVolume(volumeA.GetRadius());

	GameObject* tempA = collisionInfo.a;
	collisionInfo.a = collisionInfo.b;
	collisionInfo.b = tempA;
	bool isColliding = AABBSphereIntersection(volumeB, worldTransformB, cylinderSphereVolume, cylinderSphereTransform, collisionInfo);
	collisionInfo.point.localB = Vector3();
	return isColliding;
}

bool CollisionDetection::OBBCapsuleIntersection(
	const CapsuleVolume& volumeA, const Transform& worldTransformA,
	const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {

	Vector3 capsulePos = worldTransformA.GetPosition();
	Vector3 upVector = (worldTransformA.GetOrientation() * Vector3(0, 1, 0)).Normalised();

	Vector3 OBBPos = worldTransformB.GetPosition();


	//Case when ray intersects top hemisphere
	Vector3 topSpherePos = capsulePos + upVector * (volumeA.GetHalfHeight() - volumeA.GetRadius());
	if (Vector3::Dot(OBBPos - topSpherePos, -upVector) < 0)
	{
		Transform topSphereTransform;
		topSphereTransform.SetOrientation(worldTransformA.GetOrientation());
		topSphereTransform.SetPosition(topSpherePos);
		topSphereTransform.SetScale(worldTransformA.GetScale());

		SphereVolume topSphereVolume(volumeA.GetRadius());

		GameObject* tempA = collisionInfo.a;
		collisionInfo.a = collisionInfo.b;
		collisionInfo.b = tempA;
		bool isColliding = OBBSphereIntersection(volumeB, worldTransformB, topSphereVolume, topSphereTransform, collisionInfo);
		collisionInfo.point.localB = Vector3();
		return isColliding;
	}

	//Case when ray intersects bottom hemisphere
	Vector3 bottomSpherePos = capsulePos - upVector * (volumeA.GetHalfHeight() - volumeA.GetRadius());
	if (Vector3::Dot(OBBPos - bottomSpherePos, upVector) < 0)
	{
		Transform bottomSphereTransform;
		bottomSphereTransform.SetOrientation(worldTransformA.GetOrientation());
		bottomSphereTransform.SetPosition(bottomSpherePos);
		bottomSphereTransform.SetScale(worldTransformA.GetScale());

		SphereVolume bottomSphereVolume(volumeA.GetRadius());

		GameObject* tempA = collisionInfo.a;
		collisionInfo.a = collisionInfo.b;
		collisionInfo.b = tempA;
		bool isColliding = OBBSphereIntersection(volumeB, worldTransformB, bottomSphereVolume, bottomSphereTransform, collisionInfo);
		collisionInfo.point.localB = Vector3();
		return isColliding;
	}

	//Case when ray intersects cylinder
	Vector3 d = capsulePos + upVector * Vector3::Dot(OBBPos - capsulePos, upVector);
	float length = (OBBPos - d).Length();

	Vector3 cylinderSphere = d;
	Transform cylinderSphereTransform;
	cylinderSphereTransform.SetOrientation(worldTransformA.GetOrientation());
	cylinderSphereTransform.SetPosition(cylinderSphere);
	cylinderSphereTransform.SetScale(worldTransformA.GetScale());

	SphereVolume cylinderSphereVolume(volumeA.GetRadius());

	GameObject* tempA = collisionInfo.a;
	collisionInfo.a = collisionInfo.b;
	collisionInfo.b = tempA;
	bool isColliding = OBBSphereIntersection(volumeB, worldTransformB, cylinderSphereVolume, cylinderSphereTransform, collisionInfo);
	collisionInfo.point.localB = Vector3();
	return isColliding;
}

bool NCL::CollisionDetection::CapsuleIntersection(const CapsuleVolume& volumeA, const Transform& worldTransformA, const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	Vector3 aPos = worldTransformA.GetPosition();
	Vector3 aUpVector = (worldTransformA.GetOrientation() * Vector3(0, 1, 0)).Normalised();
	Vector3 aTopSphere = aPos + aUpVector * (volumeA.GetHalfHeight() - volumeA.GetRadius());
	Vector3 aBottomSphere = aPos - aUpVector * (volumeA.GetHalfHeight() - volumeA.GetRadius());

	Vector3 bPos = worldTransformB.GetPosition();
	Vector3 bUpVector = (worldTransformB.GetOrientation() * Vector3(0, 1, 0)).Normalised();
	Vector3 bTopSphere = bPos + bUpVector * (volumeB.GetHalfHeight() - volumeB.GetRadius());
	Vector3 bBottomSphere = bPos - bUpVector * (volumeB.GetHalfHeight() - volumeB.GetRadius());

	//Distance between edge spheres
	float d0 = (bBottomSphere - aBottomSphere).Length();
	float d1 = (bTopSphere - aBottomSphere).Length();
	float d2 = (bBottomSphere - aTopSphere).Length();
	float d3 = (bTopSphere - aTopSphere).Length();

	Vector3 aLocalSphere;
	if (d2 < d1 || d3 < d1 || d2 < d0 || d3 < d0)
		aLocalSphere = aTopSphere;
	else
		aLocalSphere = aBottomSphere;

	Vector3 bLocalSphere = bPos + bUpVector * Vector3::Dot(aLocalSphere - bPos, bUpVector);
	bLocalSphere = Maths::Clamp(bLocalSphere, bBottomSphere, bTopSphere);

	aLocalSphere = aPos + aUpVector * Vector3::Dot(bLocalSphere - aPos, aUpVector);
	aLocalSphere = Maths::Clamp(aLocalSphere, aBottomSphere, aTopSphere);

	Transform aTransform(worldTransformA);
	aTransform.SetPosition(aLocalSphere);
	
	SphereVolume aSphereVolume(volumeA.GetRadius());

	Transform bTransform(worldTransformB);
	bTransform.SetPosition(bLocalSphere);

	SphereVolume bSphereVolume(volumeB.GetRadius());
	bool isCollide = SphereIntersection(aSphereVolume, aTransform, bSphereVolume, bTransform, collisionInfo);
	collisionInfo.point.localA = Vector3();
	collisionInfo.point.localB = Vector3();

	return isCollide;
}
