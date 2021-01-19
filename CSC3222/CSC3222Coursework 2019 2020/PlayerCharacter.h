#pragma once
#include "SimObject.h"
#include "BadFoodGame.h"

namespace NCL {
	namespace CSC3222 {
		class PlayerCharacter : public SimObject
		{
		public:
			enum class CharacterType {
				TYPE_A,
				TYPE_B
			};
			PlayerCharacter(BadFoodGame* bfg);
			~PlayerCharacter();

			bool UpdateObject(float dt) override;

			void DrawObject(GameSimsRenderer& r) override;

			void SetCharacterType(CharacterType t);

		protected:
			enum class MovementDir {
				Up,
				Down,
				Left,
				Right,
				Idle
			};
			void DrawCharacterA(GameSimsRenderer& r);
			void DrawCharacterB(GameSimsRenderer& r);

			BadFoodGame* bfg;

			MovementDir		currentAnimDir;
			CharacterType	charType;
			Vector2 hitbox = position - Vector2{ 0,-4 };
		};
	}
}

