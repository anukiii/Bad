#include "BadRobot.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionVolume.h"
#include "BadFoodGame.h"
#include "GameSimsPhysics.h"
#include "PlayerCharacter.h"
#include "Laser.h"
#include "GameMap.h"
#include <vector>


using namespace NCL;
using namespace CSC3222;
std::vector<BadRobot*> BadRobot::Bots;
Vector4 animFramesGreen[] = {
	Vector4(71,142,18,18),
	Vector4(102,142,20,18),
	Vector4(135,142,19,18),
	Vector4(166,142,19,18),
	Vector4(198,140,20,18)
};

Vector4 animFramesWhite[] = {
	Vector4(71,266,17,22),
	Vector4(104,265,19,22),
	Vector4(137,266,17,22),
	Vector4(168,266,17,22),
	Vector4(200,264,17,23)
};

BadRobot::BadRobot(BadFoodGame* bfg1) : SimObject() {
	texture = texManager->GetTexture("TL_Creatures.png");
	bfg = bfg1;
	Bots.push_back(this);
	map = bfg->GetMap();
	animFrameCount	= 4;
	identity = 2;
	collider = new CollisionVolume(&position, 10);
	int r = rand() % 2;
	if (r == 0) {
		type = RobotType::Green;
	}
	else {
		type = RobotType::White;
	}
	state = 0;
	
	
}

BadRobot::~BadRobot() {

}
 
void BadRobot::Patrol(float dt) {
	if (Searching == true){
		PlayerCharacter* p = bfg->Getplayer();
		if (80 > pow(pow(p->GetPosition().x - position.x, 2) + pow(p->GetPosition().y - position.y, 2), 0.5)) {
			state = 1;//attack
		}
		

		if (abs(position.x - Destination.x)<5 && abs(position.y - Destination.y) < 5) {
			Searching = false;
		}
	}
	else {
		Searching = true;
		bool valid = false;
		while (valid == false) {	
			valid = true;
			float x = (1 + (rand() % 30)) * 16;
			float y = (1 + (rand() % 20)) * 16;
			Destination = Vector2{ x,y };
			CollisionVolume *temp = new CollisionVolume(&Destination, 16.0f);
			GameSimsPhysics* physics = bfg->Getphysics();
			for (int i = 0; i < physics->getColiders().size(); i++) {
				if (physics->getColiders().at(i)->DetectCollision(temp) > 0) {
					valid = false;
					i = physics->getColiders().size();
				}
			}
		}
		
	}
}
void BadRobot::Attack(float dt) {
	//bfg->RobotShoot(2,dt);
	PlayerCharacter* p = bfg->Getplayer();
	if (80 < pow(pow(p->GetPosition().x - position.x, 2) + pow(p->GetPosition().y - position.y, 2), 0.5)) {
		state = 0; //passive
	}
	else {
		Destination = p->GetPosition();
		RobotShoot(2, dt, p);

	}

}

void BadRobot::Protect(float dt) {
	if (!Searching2) {
		Searching2 = true;
		bool valid = false;
		while (valid == false) {
			Searching2 = true;
			valid = true;


			// door position = Vector2(30 * 11.8, 20 * 13);

			float x = (294 + (rand() % 120));
			float y = (200 + (rand() % 120));
			Destination = Vector2{ x,y };
			if (x < 30 * 16 && y < 20 * 16) {
				CollisionVolume* temp = new CollisionVolume(&Destination, 16.0f);
				GameSimsPhysics* physics = bfg->Getphysics();
				for (int i = 0; i < physics->getColiders().size(); i++) {
					if (physics->getColiders().at(i)->DetectCollision(temp) > 0) {
						valid = false;
						i = physics->getColiders().size();
					}
				}
			}
			else {
				valid = false;
			}
		}
	}
}


	void BadRobot::RobotShoot(float intervall, float dt, PlayerCharacter * p) {
		int x;
		int y;
		laserTimer += dt;
		if (laserTimer >= intervall) {
			Laser* tempLaser = new Laser(Vector2{ Destination.x - position.x,Destination.y - position.y }*3);
			tempLaser->SetPosition(position);
			bfg->AddNewObject(tempLaser);
			laserTimer = 0;
		}



	}


bool BadRobot::UpdateObject(float dt) {
	Flocking(dt);
	if (bfg->GetFood() >= 5) {
		state = 2;
		
	}
	else if (bfg->GetFood() < 5 && state == 2) {
		state = 0;
		Searching = false;
		Searching2 = false;
	}

	switch (state) {
	/*patrol*/		case 0:Patrol(dt); break;
	/*attack*/		case 1:Attack(dt); break;
	/*protectice*/	case 2:Protect(dt); break;
	}

	/*
	if (Searching2) {
		if (abs(Destination.x - position.x) < 5 && abs(Destination.y - position.y) < 5) {
			if (pathPosition == path->size()) {
				Searching2 = false;
			}
			else {
				pathPosition++;
			}
		}
		Destination = path->at(pathPosition);
	}
	*/
	AddForce(Vector2{ Destination.x - position.x,Destination.y - position.y } * 5);
	UpdateAnimFrame(dt);


		return true;
	
}

void BadRobot::DrawObject(GameSimsRenderer& r) {
	Vector2	texPos;
	Vector2 texSize;

	switch (type) {
		case RobotType::Green:{
			texPos	= Vector2(animFramesGreen[currentanimFrame].x, animFramesGreen[currentanimFrame].y);
			texSize = Vector2(animFramesGreen[currentanimFrame].z, animFramesGreen[currentanimFrame].w);
		}break;
		case RobotType::White:{
			texPos	= Vector2(animFramesWhite[currentanimFrame].x, animFramesWhite[currentanimFrame].y);
			texSize = Vector2(animFramesWhite[currentanimFrame].z, animFramesWhite[currentanimFrame].w);
		}break;
	}

	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position, false);
}


void BadRobot::Flocking(float dt) {
	for (int i = 0; i < Bots.size(); i++) {
		Vector2 dir = Alignment() + Separation() + Avoidance();


		Bots.at(i)->position += dir *dt;

	}
}

//alignment looks better
Vector2 BadRobot::Alignment() {
	Vector2 dir = this->velocity;

	for (int i = 0; i < Bots.size(); i++) {
		if (Bots.at(i) != this) {
			float distance = sqrt((dir.x * Bots.at(i)->velocity.x) + (dir.y * Bots.at(i)->velocity.y));
			if (distance <= 8) {
				dir += Bots.at(i)->velocity;
			}
		}
		return dir.Normalised();
	}
}

//so they dont bump into each other when chasing or going to protect food house
Vector2 BadRobot::Separation() {
	Vector2 dir;
	for (int i = 0; i < Bots.size(); i++) {
		if (Bots.at(i) != this) {
			float distance = sqrt(pow(Bots.at(i)->position.x - this->position.x, 2) + pow(Bots.at(i)->position.y - this->position.y, 2));
			if (distance < 8 ) {
				float strength = 1.0f - (distance / 8);
				dir += (this->position - Bots.at(i)->position).Normalised() * strength;

			}
		}return dir.Normalised();
	}

}
//cohesion doesn't seem usefull but avoidance does for when they move.
Vector2 BadRobot::Avoidance() {
	Vector2 dir;
	for (int i = 0; i < Bots.size(); i++) {
		if (Bots.at(i) != this) {
			float distance = sqrt(pow(Bots.at(i)->position.x - this->position.x, 2) + pow(Bots.at(i)->position.y - this->position.y, 2));
			if (distance < 8) {
				dir += (this->position - Bots.at(i)->position).Normalised();
			}
		}

	}
	return dir.Normalised();
}

