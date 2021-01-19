#pragma once
#include <vector>

namespace NCL {
	namespace Maths {
		class Vector2;
	}
	namespace CSC3222 {
		class GameMap;
		class SimObject;
		class GameSimsRenderer;
		class GameSimsPhysics;
		class TextureBase;
		class TextureManager;
		class PlayerCharacter;

		class BadFoodGame {
		public:
			BadFoodGame();
			~BadFoodGame();

			void Update(float dt);
			void RobotSpawn(float intervall, float dt);
			void AddNewObject(SimObject* object);

			void BalloonFollow(SimObject* player);
			void SpawnBalloon(PlayerCharacter* player);
			int GetBalloons();
			void FoodSpawn(float intervall, float dt);
			void CoinSpawn(float intervall, float dt);
			void FoodCollect();
			void CoinCollect();
			float IsBalloonActive() { return balloonActive; };
			void Death();
			void TradeFood();
			void TradeCoin();

			int GetFood() { return foodCount; }
			PlayerCharacter* Getplayer() { return player; }
			GameSimsPhysics* Getphysics() { return physics; }
			GameMap* GetMap() { return currentMap; }

		protected:
			void InitialiseGame();
			TextureManager*		texManager;
			GameSimsRenderer*	renderer;
			GameSimsPhysics*	physics;
			GameMap* currentMap;

			PlayerCharacter* player;

			float gameTime;
			float tr;
			float checkTimeRobot; //ROBOT SPAWNER
			float checkTimeFood; //ROBOT SPAWNER
			float laserTimer;
			float checkTimeCoin;

			int robotCounter;
			int currentScore;
			int foodCount;
			int coins;
			int balloons;
			int lives;
			float balloonActive;
			
			

			std::vector<SimObject*> gameObjects;
			std::vector<SimObject*> newObjects;
		};
	}
}

