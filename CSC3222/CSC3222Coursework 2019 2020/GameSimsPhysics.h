#pragma once
#include <vector>
#include "../../Common/Vector2.h"
#include "PlayerCharacter.h"
#include "BadFoodGame.h"


namespace NCL {
	namespace CSC3222 {
		class RigidBody;
		class CollisionVolume;

		class GameSimsPhysics	{
		public:
			GameSimsPhysics(BadFoodGame* bfg);
			~GameSimsPhysics();

			void Update(float dt);

			void AddRigidBody(RigidBody* b);
			void RemoveRigidBody(RigidBody* b);

			void AddCollider(CollisionVolume* c);
			void RemoveCollider(CollisionVolume* c);
			void CalcVelocity(float dt, int position);
			//Vector2 CalcAcceleration(int position);
			//void SpringMovement(float dt, int spring,int player);
			void CollisionDetection(float dt);
			std::vector<CollisionVolume*> getColiders() { return allColliders; }
			std::vector<RigidBody*>	getBodies() { return allBodies; }

			std::vector<RigidBody*>			allBodies;
			std::vector<CollisionVolume*>	allColliders;
		protected:
			void Integration(float dt);
			BadFoodGame* bfg;
			void ImpulseCalculation(int a,int b);
			int BroadPhaseQuadrant(int obj);

		};
	}
}

