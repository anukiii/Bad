#include "BadFoodGame.h"
#include "SimObject.h"
#include "GameMap.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "GameSimsPhysics.h"
#include "PlayerCharacter.h"
#include "BadRobot.h"
#include "Laser.h"
#include "Food.h"
#include "Coin.h"
#include "Balloon.h"
#include "EmptyColision.h"

#include "../../Common/Window.h"
#include "../../Common/TextureLoader.h"

using namespace NCL;
using namespace CSC3222;

BadFoodGame::BadFoodGame()	{
	renderer	= new GameSimsRenderer();
	texManager	= new TextureManager();
	physics		= new GameSimsPhysics(this);
	SimObject::InitObjects(this, texManager);
	InitialiseGame();
}

BadFoodGame::~BadFoodGame()	{
	delete currentMap;
	delete texManager;
	delete renderer;
	delete physics;
}

void BadFoodGame::RobotSpawn(float intervall, float dt) {
	checkTimeRobot += dt; // claculation to spawn new robot

	if (checkTimeRobot >= intervall && robotCounter<20) {
		BadRobot* tempRobot = new BadRobot(this);
		tempRobot->SetPosition(Vector2(240, 60));     //ROBOT SPAWNER EVERY 5 SECONDS
		//player->InitObjects(this, texManager);
		AddNewObject(tempRobot);
		checkTimeRobot = 0; //resets check time
		robotCounter++;
	}
}

void BadFoodGame::FoodSpawn(float intervall, float dt) {
	checkTimeFood += dt; // claculation to spawn new robot
	if (checkTimeFood >= intervall /*&& NEED TIMER*/) {
		//currentMap->TileToWorld(currentMap->GetRandomPosition());
		Food* tempFood = new Food();
		bool valid = false;
		Vector2 destination;
		while (valid == false) {
			valid = true;
			float x = (1 + (rand() % 30)) * 16;
			float y = (1 + (rand() % 20)) * 16;
			destination = Vector2{ x,y };
			CollisionVolume* temp = new CollisionVolume(&destination, 16.0f);
			for (int i = 0; i < physics->getColiders().size(); i++) {
				if (physics->getColiders().at(i)->DetectCollision(temp) > 0) {
					valid = false;
					i = physics->getColiders().size();
				}
			}
		}



		tempFood->SetPosition(destination); 


		AddNewObject(tempFood);
		checkTimeFood = 0; //resets check time

	}
}

void BadFoodGame::CoinSpawn(float intervall, float dt) {
	checkTimeCoin += dt; // claculation to spawn new robot
	if (checkTimeCoin >= intervall /*&& NEED TIMER*/) {
		//currentMap->TileToWorld(currentMap->GetRandomPosition());
		Coin* tempCoin = new Coin();
		bool valid = false;
		Vector2 destination;
		while (valid == false) {
			valid = true;
			float x = (1 + (rand() % 30)) * 16;
			float y = (1 + (rand() % 20)) * 16;
			destination = Vector2{ x,y };
			CollisionVolume* temp = new CollisionVolume(&destination, 16.0f);
			for (int i = 0; i < physics->getColiders().size(); i++) {
				if (physics->getColiders().at(i)->DetectCollision(temp) > 0) {
					valid = false;
					i = physics->getColiders().size();
				}
			}
		}



		tempCoin->SetPosition(destination);


		AddNewObject(tempCoin);
		checkTimeCoin = 0; //resets check time

	}
}


void BadFoodGame::BalloonFollow(SimObject* player) {
	for (int i = 0; i < gameObjects.size(); i++) {
		if (gameObjects.at(i)->GetIdentity() == 5) {

			float k = 5;
			Vector2 restpos = player->GetPosition() - Vector2{ 20,0 };
			Vector2 force = Vector2{ -k,-k } *(gameObjects.at(i)->GetPosition() - restpos);

			gameObjects.at(i)->AddForce(force);
		}
	}
}

void BadFoodGame::FoodCollect() {
	if (foodCount < 5) { foodCount++; }
}

void BadFoodGame::CoinCollect() {
	coins++;
}



void BadFoodGame::SpawnBalloon(PlayerCharacter* player) {
	Balloon* testBalloon = new Balloon();
	testBalloon->SetPosition(player->GetPosition() - Vector2{ 20,0 });
	AddNewObject(testBalloon);
	balloonActive = 5;
	balloons--;
}

int BadFoodGame::GetBalloons() {
	return balloons;
}

void BadFoodGame::TradeCoin() {
	if (coins > 0) {
		coins--;
		balloons++;
	}
}

void BadFoodGame::TradeFood() {

	if (foodCount == 1) { currentScore += 300; }
	else if (foodCount == 2) { currentScore += 500; }
	else if (foodCount == 3) { currentScore += 800; }
	else if (foodCount == 4) { currentScore += 1300; }
	else if (foodCount == 5) { currentScore += 2100; }
	foodCount = 0;

}

void BadFoodGame::Death() {
	std::cout << "DEATH";
	gameObjects.clear();
	physics->allBodies.clear();
	physics->allColliders.clear();
	currentMap = new GameMap("BadFoodMap.txt", gameObjects, *texManager, this);
	player = new PlayerCharacter(this);
	player->SetCharacterType(PlayerCharacter::CharacterType::TYPE_B);
	AddNewObject(player);	
	foodCount = 0;
	coins = 0;
	balloons = 1;
	lives--;
	if (lives == 0) {
		currentScore = 0;
		lives = 3;
		gameTime = 0;
	}
}

void BadFoodGame::Update(float dt) {
	balloonActive -= dt;
	CoinSpawn(15.0, dt);
	FoodSpawn(5, dt);
	RobotSpawn(15.0,dt);	

	for (auto i : newObjects) { // all of the spawned objects e.g. lasers
		gameObjects.emplace_back(i); // puts it in the vector
	}
	newObjects.clear();

	gameTime += dt; // how long game has been running for
	tr += dt;
	const float subDt = 1.0 / 120; // fixed timesteps

	renderer->Update(dt);
		
		while (tr > subDt) {
			physics->Update(subDt);
			tr -= subDt;
		}
		currentMap->DrawMap(*renderer); // tells renderer and physics to update 

		srand((int)(gameTime * 1000.0f));

		for (auto i = gameObjects.begin(); i != gameObjects.end(); ) {
			if (!(*i)->UpdateObject(dt)) { //object has said its finished with

				//delete (*i);
				i = gameObjects.erase(i);

			}
			else {
				(*i)->DrawObject(*renderer);
				++i;
			}
		}
		renderer->DrawString("B4DF00D", Vector2(420, 700));

		renderer->DrawString("Score: " + std::to_string(currentScore), Vector2(10, 10));
		renderer->DrawString("Lives left: " + std::to_string(lives), Vector2(10, 30));
		renderer->DrawString("Food: " + std::to_string(foodCount), Vector2(10, 50));
		renderer->DrawString("Coins: " + std::to_string(coins), Vector2(10, 70));
		renderer->DrawString("Balloons: " + std::to_string(balloons), Vector2(10, 90));

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
			if (GetBalloons() > 0) { // sees if player had balloons
				SpawnBalloon(player);

			}
		}


		BalloonFollow(player);




		renderer->Render();
		
	
}

void BadFoodGame::InitialiseGame() {
	delete currentMap;
	for (auto o : gameObjects) {
		delete o;
	}
	gameObjects.clear();

	int robotCounter = 0;
		currentMap = new GameMap("BadFoodMap.txt", gameObjects, *texManager, this);

	renderer->SetScreenProperties(16, currentMap->GetMapWidth(), currentMap->GetMapHeight());

	player = new PlayerCharacter(this);
	
	player->SetCharacterType(PlayerCharacter::CharacterType::TYPE_B);
	AddNewObject(player);
	
	//EmptyColision points = new emptyColission();


	gameTime		= 0;
	currentScore	= 0;
	foodCount		= 0;
	coins			= 0;
	balloons		= 1;
	lives			= 3;
}

void BadFoodGame::AddNewObject(SimObject* object) {
	newObjects.emplace_back(object);
	physics->AddRigidBody(object);
	if (object->GetCollider()) {
		physics->AddCollider(object->GetCollider());
	}
}