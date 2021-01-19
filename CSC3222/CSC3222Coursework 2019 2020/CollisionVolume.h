#pragma once
#include "RigidBody.h"

namespace NCL {
	namespace CSC3222 {
		class CollisionVolume {
		public:
			CollisionVolume();
			CollisionVolume(Vector2* v, float r);
			CollisionVolume(Vector2* v, float x, float y);
			~CollisionVolume();
			float DetectCollision(CollisionVolume* cv); // {penetration distance, collision normal, collision manifold}


		protected:
			float SquareOnCircle(CollisionVolume* square, CollisionVolume* circle);
			

			int type; //1 is circle, 2 is box
			float radius;
			Vector2* position;
			Vector2 xy;

		};
	}
}

