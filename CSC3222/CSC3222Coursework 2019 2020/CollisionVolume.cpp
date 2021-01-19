#include "CollisionVolume.h"
#include "RigidBody.h"
#include <algorithm>
using namespace std;

using namespace NCL;
using namespace CSC3222;

CollisionVolume::CollisionVolume(){
	type = 0;
	radius = 0;
	position = new Vector2{ 0,0 };
}


CollisionVolume::CollisionVolume(Vector2* v, float r) {
	radius = r;
	type = 1;
	position = v;
	xy = { 0,0 };
	//std::cout << "CIRCLE 2" << '\n';
}

CollisionVolume::CollisionVolume(Vector2* v, float x, float y) {
	type = 2;
	xy = { x,y };
	position = v;
	radius = 0;

}

CollisionVolume::~CollisionVolume()
{
	delete position;
}

float CollisionVolume::DetectCollision(CollisionVolume* cv) {

	if ((1 <= type <= 2) && (1 <= cv->type <= 2)) {

		if (type == 1) {///////////////////////////////  CIRCLE ON ...

			if (cv->type == 1) {///////////////////////////////  CIRCLE ON CIRCLE

				float distance = pow(pow(position->x - cv->position->x, 2) + pow(position->y - cv->position->y, 2), 0.5);
				if (distance > radius + cv->radius) {
					return 0;
				}
				else {
					return  radius + cv->radius- distance;
				}

			}
			else if (cv->type == 2) {///////////////////////////////  CIRCLE ON SQUARE
				return SquareOnCircle(cv, this);

			}

		}
		else if (type == 2) {///////////////////////////////  SQARE ON ...
			if (cv->type == 1) {///////////////////////////////  SQUARE ON CIRCLE
				return SquareOnCircle(this, cv);

			}
			else if (cv->type == 2) {///////////////////////////////  SQUARE ON SQUARE
				float distancex = abs(position->x - cv->position->x);
				float distancey = abs(position->y - cv->position->y);

				if ((distancex > (xy.x + cv->xy.x)) || (distancey > (xy.y + cv->xy.y))) {
					return 0;
				}
				else {
					return pow(pow((xy.x + cv->xy.x) - distancex,2) + pow ((xy.y + cv->xy.y) - distancey,2),0.5);
				}



			}

		}
	}
}

float CollisionVolume::SquareOnCircle(CollisionVolume* square, CollisionVolume* circle) {
	float clampedX = circle->position->x;
	clampedX = std::min(clampedX, square->position->x + (square->xy.x));
	clampedX = std::max(clampedX, square->position->x - (square->xy.x));

	float clampedY = circle->position->y;
	clampedY = std::min(clampedY, square->position->y + (square->xy.y));
	clampedY = std::max(clampedY, square->position->y - (square->xy.y));

	float distance = pow(pow(clampedX - circle->position->x, 2) + pow(clampedY - circle->position->y, 2), 0.5);
	if (distance > circle->radius) {
		return 0;
	}
	else {
		return circle->radius- distance;
	}

}
