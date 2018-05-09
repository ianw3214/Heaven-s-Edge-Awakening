#include "gameObject.hpp"

GameData * GameObject::data;

GameObject::GameObject() {
	remove = false;
}

GameObject::GameObject(ObjectType inp_type) : type(inp_type) {
	remove = false;
}

GameObject::~GameObject() {

}

void GameObject::setGameData(GameData * inp_data) {
	data = inp_data;
}

bool GameObject::shouldRemove() const {
	return remove;
}

void GameObject::update(Uint32 delta) {
	// filler function for derived classes to handle
}

void GameObject::render() {
	// filler function for derived classes to handle
}
