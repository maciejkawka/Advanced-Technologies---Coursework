#pragma once


namespace NCL {
	namespace CSC8503 {



		class Participant {
		public:
			Participant()
			{
				moveForce = 20.0f;
				maxVelocity = 20.0f;
				jumpForce = 5.0f;
				points = 1000;
				currentTime = 0.0f;
				finished = false;
			}

			~Participant() {}

			

			inline void SetMaxVelocity(float maxV) { maxVelocity = maxV; }
			inline void SetMoveForce(float moveF) { moveForce = moveF; }
			inline void SetJumpForce(float jumpF) { jumpForce = jumpF; }

			float GetMaxVelocity() const { return maxVelocity; }
			float GetMoveForce() const { return moveForce; }
			bool IsFinished() const { return finished; }
			inline int GetPoints() const { return points; }

		protected:

			void DecreasePoints()
			{

			if (points <= 0)
			{
				points = 0;
				return;
			}

			points -= 10;
			
			}

			float moveForce;
			float maxVelocity;
			float jumpForce;
			float currentTime;
			int points;
			bool finished;
		};
	}
}