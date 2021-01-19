#pragma once
#include "SimObject.h"
#include <vector>
#include "BadFoodGame.h"
#include "GameMap.h"

namespace NCL {
	namespace CSC3222 {
		class BadRobot : public SimObject
		{
		public:
			BadRobot(BadFoodGame* bfg1);
			~BadRobot();
			
			bool UpdateObject(float dt) override;
			void DrawObject(GameSimsRenderer& r) override;
			void RobotShoot(float intervall, float dt, PlayerCharacter* p);
			
		protected:
			void Patrol(float dt);
			void Attack(float dt);
			void Protect(float dt);

			Vector2 Alignment();
			Vector2 Separation();
			Vector2 Avoidance();
			void Flocking(float dt);

			enum class RobotType {
				Green,
				White,
			};
			static std::vector<BadRobot*> Bots;
			BadFoodGame* bfg;
			GameMap* map;
			RobotType type;
			//std::vector<Vector2>* path;
			int pathPosition = 0;
			int state;
			bool Searching = false;
			bool Searching2 = false;
			Vector2 Destination;
			float laserTimer = 0;
		};
	}
}

