#include"../CSC8503Common/GameObject.h"

namespace NCL {
	namespace CSC8503 {

		class BonusGameObject : public GameObject {
		public:
			BonusGameObject(string name="Bonus", float rotateTime = 3): GameObject(name)
			{
				this->rotateTime = rotateTime;
				
			}
			~BonusGameObject();

			void Update(float dt) override
			{
				this->GetTransform().Rotate(0, 180 * dt/rotateTime, 0);
			}

			void SetRotateTime(float t) { rotateTime = t; }

		protected:
			float rotateTime;
			




		};
	}
}
