#pragma once
#include "SimObject.h"

namespace NCL {
	namespace CSC3222 {
		class Food : public SimObject
		{
		public:
			Food();
			~Food();

			bool UpdateObject(float dt) override;
			void DrawObject(GameSimsRenderer& r) override;

		protected:
			int foodItem;
			float Lifetime = 12; // how long each food lives for
		};
	}
}

