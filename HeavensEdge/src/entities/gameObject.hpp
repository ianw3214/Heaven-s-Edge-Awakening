#pragma once

#include "QcEngine.hpp"

// declare the gamedata struct to use in the GameObject header
struct GameData;

enum ObjectType {
	PLAYER_OBJECT,
	ENEMY_OBJECT,
	ARROW_OBJECT
};

class GameObject {

public:

	GameObject();
	GameObject(ObjectType inp_type);
	virtual ~GameObject();

	static void setGameData(GameData * inp_data);
	bool shouldRemove() const;

	virtual void update(Uint32 delta);
	virtual void render();

protected:

	ObjectType type;
	bool remove;

	// a pointer to the game data for the entities to update against
	static GameData * data;

};