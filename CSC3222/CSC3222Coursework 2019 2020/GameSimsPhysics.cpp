#include "GameSimsPhysics.h"
#include "RigidBody.h"
#include "CollisionVolume.h"
#include "../../Common/Vector2.h"
#include <numeric>

using namespace NCL;
using namespace CSC3222;

GameSimsPhysics::GameSimsPhysics(BadFoodGame* bfg)	{
	this->bfg = bfg;
}

GameSimsPhysics::~GameSimsPhysics()	{

}

void GameSimsPhysics::Update(float dt) {
	Integration(dt);
	CollisionDetection(dt);
}

void GameSimsPhysics::AddRigidBody(RigidBody* b) {
	allBodies.emplace_back(b);
}

void GameSimsPhysics::RemoveRigidBody(RigidBody* b) {
	auto at = std::find(allBodies.begin(), allBodies.end(), b);

	if (at != allBodies.end()) {
		allBodies.erase(at);
	}
}

void GameSimsPhysics::AddCollider(CollisionVolume* c) {
	allColliders.emplace_back(c);
}

void GameSimsPhysics::RemoveCollider(CollisionVolume* c) {
	auto at = std::find(allColliders.begin(), allColliders.end(), c);

	if (at != allColliders.end()) {
		//maybe delete too?
		allColliders.erase(at);
	}
}



void GameSimsPhysics::CalcVelocity(float dt,int position) {
	allBodies.at(position)->velocity += allBodies.at(position)->force * allBodies.at(position)->inverseMass * dt;
	
}


void GameSimsPhysics::Integration(float dt) {
	
	for (int i = 0; i < allBodies.size(); i++) {
		if (allBodies.at(i)->inverseMass > 0 && allBodies.at(i)->identity!=5) {
			CalcVelocity(dt, i);
			allBodies.at(i)->position += allBodies.at(i)->velocity * dt;
			allBodies.at(i)->velocity *= 0.95f;
			allBodies.at(i)->force = { 0,0 };
		}
		else if (allBodies.at(i)->identity == 5){ // for baloons i wanted to incorporate dampening rather than air resistance since
			float  c = 0.3;// dampening constant
			allBodies.at(i)->force -= Vector2(c,c) *allBodies.at(i)->velocity;
			CalcVelocity(dt, i);				  // they're suposed to represent a spring
			allBodies.at(i)->position += allBodies.at(i)->velocity * dt;
			allBodies.at(i)->force = { 0,0 };

		}
	}
	


}

//4 sections for borad phase are {0<=x<=160,0<=y<=240} , {0<=x<=160,240<=y<480}, {160<=x<=320,0<=y<=240}, {160<=x<320,240<=y<=480}
//this also avoids collisions outside of game map. Due to the way in which objects have weight, no separate list is needed to be made
//if the object is static, thus having an inverse mass of 0, the current object inst done narrow or borad phase. If it's dynamic and thus has a 
//inverse mass >0, broad phase then narrow pase is done.
void GameSimsPhysics::CollisionDetection(float dt) {
	//since all boddies and all coliders are added at the same time we can assume theye paralel
	int iQuadrant; //1,2,3,4  0 is off screen
	int jQuadrant; //1,2,3,4  0 is off screen

	for (int i = 0; i < allColliders.size(); i++) { //I IS OUTER
		

			for (int j = i + 1; j < allColliders.size(); j++) {//J IS INNER
				
				if (allBodies.at(j)->inverseMass > 0 || allBodies.at(i)->inverseMass > 0){// checks if both j and i are static
					
					if (!(allBodies.at(i)->identity==6 && allBodies.at(j)->identity==2) && !(allBodies.at(i)->identity == 2 && allBodies.at(j)->identity == 6)) {
					//^^ done so that robots and lasers dont colide																									^^done so lasers dont cloise																		
						jQuadrant = BroadPhaseQuadrant(j);
						iQuadrant = BroadPhaseQuadrant(i);
						
						//remove food if picked up
						if (iQuadrant == jQuadrant && allColliders.at(j)->DetectCollision(allColliders.at(i)) > 0 && iQuadrant != 0 && jQuadrant != 0) { // if broadphase is passed, do narrow phase
							
							if ((allBodies.at(i)->identity == 1 && allBodies.at(j)->identity == 4)) {
								bfg->FoodCollect();
								allBodies.at(j)->remainingBounce --;
								allColliders.erase(allColliders.begin() + j);
								allBodies.erase(allBodies.begin() + j);

							}

							else if ((allBodies.at(i)->identity == 1 && allBodies.at(j)->identity == 3)) {
								bfg->CoinCollect();
								allBodies.at(j)->remainingBounce--;
								allColliders.erase(allColliders.begin() + j);
								allBodies.erase(allBodies.begin() + j);

							}
							else if ((allBodies.at(i)->identity == 1 && allBodies.at(j)->identity == 6)|| (allBodies.at(i)->identity == 1 && allBodies.at(j)->identity == 2)) {
								std::cout << bfg->IsBalloonActive();
								if (bfg->IsBalloonActive() <=0) {
									bfg->Death();
								}

							}

							else if ((allBodies.at(j)->identity == 1 && allBodies.at(i)->identity == 20)) {
								bfg->TradeFood();

							}
							else if ((allBodies.at(j)->identity == 1 && allBodies.at(i)->identity == 21)) {
								bfg->TradeCoin();

							}

							else {
								ImpulseCalculation(j, i);

								if (allBodies.at(j)->identity == 6) {
									allBodies.at(j)->remainingBounce--;
									
									if (allBodies.at(j)->remainingBounce <= 0) {
										//removes laser from physics engine if bounced enough
										allColliders.erase(allColliders.begin() + j);
										allBodies.erase(allBodies.begin() + j);
									}


								}
							}

						}
					}
				}
			}
		

	}
}

void GameSimsPhysics::ImpulseCalculation(int a, int b) {
	float e = 0.8;
	Vector2 normal;
	//players use hitbox rather than position
	if (allBodies.at(b)->identity == 1) {
		normal = { allBodies.at(b)->position.x - allBodies.at(a)->position.x,(allBodies.at(b)->position.y + 4) - allBodies.at(a)->position.y };
	}
	else if (allBodies.at(a)->identity == 1) {
		normal = { allBodies.at(b)->position.x - allBodies.at(a)->position.x,allBodies.at(b)->position.y - (allBodies.at(a)->position.y+4) };

	}
	else {
		normal = { allBodies.at(b)->position.x - allBodies.at(a)->position.x,allBodies.at(b)->position.y - allBodies.at(a)->position.y };
	}
	normal.Normalise();
	Vector2 relativeVelocity = allBodies.at(b)->GetVelocity() - allBodies.at(a)->GetVelocity();
	float NormalVelocity = normal.Dot(normal, relativeVelocity);
	if (NormalVelocity > 0) {
		return;
	}
	float j = (-(1 + e) * NormalVelocity) / (allBodies.at(a)->inverseMass + allBodies.at(b)->inverseMass);

	Vector2 impulse = Vector2{ j * normal.x,j * normal.y };
	allBodies.at(a)->SetVelocity(allBodies.at(a)->GetVelocity() - Vector2{ allBodies.at(a)->inverseMass * impulse.x,allBodies.at(a)->inverseMass * impulse.y });
	allBodies.at(b)->SetVelocity(allBodies.at(b)->GetVelocity() + Vector2{ allBodies.at(b)->inverseMass * impulse.x,allBodies.at(b)->inverseMass * impulse.y });

	//fixes sinking
	float pen = allColliders.at(a)->DetectCollision(allColliders.at(b)); 
	Vector2 fix = { pen / (allBodies.at(a)->inverseMass + allBodies.at(b)->inverseMass) * 0.1f * normal.x ,pen / (allBodies.at(a)->inverseMass + allBodies.at(b)->inverseMass) * 0.1f * normal.y };
	allBodies.at(a)->SetPosition (allBodies.at(a)->GetPosition() - Vector2{ allBodies.at(a)->inverseMass * fix.x, allBodies.at(a)->inverseMass * fix.y });
	allBodies.at(b)->SetPosition ( allBodies.at(b)->GetPosition() + Vector2{ allBodies.at(b)->inverseMass * fix.x, allBodies.at(b)->inverseMass * fix.y });
	
	//for lasers with constant velocity
	if (allBodies.at(a)->identity == 6) {
		allBodies.at(a)->constVelocity = allBodies.at(a)->GetVelocity();
	}
	else if (allBodies.at(b)->identity == 6) {
		allBodies.at(b)->constVelocity = allBodies.at(b)->GetVelocity();
	}

}

int GameSimsPhysics::BroadPhaseQuadrant(int obj) {
	Vector2 pos = allBodies.at(obj)->GetPosition();
	if (0 <= pos.x <= 160 && 0 <= pos.y <= 240) {
		return 1;
	}
	else if (0 <= pos.x <= 160 && 240 <= pos.y < 480) {
		return 2;
	}
	else if (160 <= pos.x <= 320 && 0 <= pos.y <= 240) {
		return 3;
	}
	else if (160 <= pos.x <= 320 && 240 <= pos.y < 480) {
		return 4;
	}
	else {
		return 0; // outside of map boundaries
	}
}