#pragma once
#include "SimObject.h"

namespace NCL {
	namespace CSC3222 {
		class Balloon : public SimObject
		{
		public:
			Balloon();
			~Balloon();

			bool UpdateObject(float dt) override;
			void DrawObject(GameSimsRenderer& r) override;

		protected:

			float Lifetime =5;
		};
	}
}

