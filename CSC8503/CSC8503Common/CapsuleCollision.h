#pragma once
#include"CapsuleVolume.h"
#include"SphereVolume.h"
#include"AABBVolume.h"

namespace NCL {
	class CapsuleCollision : public CapsuleVolume {
	public:
		CapsuleCollision(float halfHeight, float radius) : CapsuleVolume(halfHeight, radius)
		{
			sphereBottom = new SphereVolume(radius);
			sphereTop = new SphereVolume(radius);
			box = new AABBVolume(halfHeight);
		}

		~CapsuleCollision()
		{
			delete sphereBottom;
			delete sphereTop;
			delete box;
		}


	protected:
		SphereVolume* sphereBottom;
		SphereVolume* sphereTop;
		AABBVolume* box;

	};
}
