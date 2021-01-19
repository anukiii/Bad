#pragma once
#include "SimObject.h"
#include "GameSimsRenderer.h"
#include "CollisionVolume.h"
namespace NCL {
	namespace CSC3222 {

		class EmptyColision : public SimObject
		{
		public:
			EmptyColision(Vector2 v, int id);
			EmptyColision(float x, int id);
			~EmptyColision();

			bool UpdateObject(float dt) override ;
			void DrawObject(GameSimsRenderer& r) override ;

		};
	}
}

