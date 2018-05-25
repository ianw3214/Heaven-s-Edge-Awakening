// -------------------------------------------------------------------------------
// Contains all code relating to the editor state in the editor application
// -------------------------------------------------------------------------------

#include "editor.hpp"

void Editor::renderEditor() {
	// render the tile selection image
	QcEngine::getTexture(TILE_SELECT)->render(cur_tile_x * tile_size - camera_x, cur_tile_y * tile_size - camera_y);
	if (edit_mode == EDIT_TILE) {
		if (show_HUD) {
			// render the tile palette
			QcEngine::getTexture(PALETTE_BASE)->render(palette_x, palette_y);
			for (int i = 0; i < tiles->getNumTiles(); ++i) {
				int x = (i % 3) * tile_size + 6 + palette_x;
				int y = (i / 3) * tile_size + 6 + palette_y;
				tiles->render(x, y, i);
				if (i == current_tile) {
					QcEngine::getTexture(PALETTE_SELECT)->render(x, y);
				}
			}
		}
	}
	// render the collision boxes if user is currently editing collisions
	if (edit_mode == EDIT_COLLISION) {
		for (int i = 0; i < map_height; ++i) {
			for (int j = 0; j < map_width; ++j) {
				if (collisionmap[tileIndex(j, i)] == true) {
					QcEngine::getTexture(COLLISION)->render(j * tile_size - camera_x, i * tile_size - camera_y);
				}
			}
		}
	}
	if (edit_mode == EDIT_ENTITIES) {
		// render the player
		for (const Vec2& v : player_spawns) {
			QcEngine::getTexture(PLAYER)->render(v.x * tile_size - camera_x, v.y * tile_size - camera_y);
			// TODO: render a text to show the player spawn number as well
		}
		// render the enemies
		for (const EntityEntry& e : entities) {
			if (e.type == E_ENEMY) {
				QcEngine::getTexture(ENEMY)->render(e.x * tile_size - camera_x, e.y * tile_size - camera_y);
			}
		}
		// render the portals
		for (const Vec2& v : portals) {
			QcEngine::getTexture(PORTAL)->render(v.x * tile_size - camera_x, v.y * tile_size - camera_y);
		}
		// render the entity options
		if (show_HUD) {
			QcEngine::getTexture(PLA_ENTITY_ICON)->render(0, 0);
			QcEngine::getTexture(ENE_ENTITY_ICON)->render(0, 64);
			QcEngine::getTexture(POR_ENTITY_ICON)->render(0, 128);
			if (e_edit_mode == E_EDIT_PLAYER) {
				QcEngine::getTexture(PLA_ENTITY_SEL)->render(0, 0);
			}
			if (e_edit_mode == E_EDIT_ENEMY) {
				QcEngine::getTexture(ENE_ENTITY_SEL)->render(0, 64);
			}
			if (e_edit_mode == E_EDIT_PORTAL) {
				QcEngine::getTexture(POR_ENTITY_SEL)->render(0, 128);
			}
		}
	}
	// render editor HUD
	if (show_HUD) {
		QcEngine::getTexture(ICON_TILE)->render(0, QcEngine::getCVARint("WINDOW HEIGHT") - 64);
		QcEngine::getTexture(ICON_COLLISION)->render(64, QcEngine::getCVARint("WINDOW HEIGHT") - 64);
		QcEngine::getTexture(ICON_ENTITY)->render(128, QcEngine::getCVARint("WINDOW HEIGHT") - 64);
		if (edit_mode == EDIT_TILE) {
			QcEngine::getTexture(SELECT_TILE)->render(0, QcEngine::getCVARint("WINDOW HEIGHT") - 64);
		}
		if (edit_mode == EDIT_COLLISION) {
			QcEngine::getTexture(SELECT_COLLISION)->render(64, QcEngine::getCVARint("WINDOW HEIGHT") - 64);
		}
		if (edit_mode == EDIT_ENTITIES) {
			QcEngine::getTexture(SELECT_ENTITY)->render(128, QcEngine::getCVARint("WINDOW HEIGHT") - 64);
		}
	}
}

void Editor::handleKeyPressEditor() {
	// panning controls
	if (keyPressed(SDL_SCANCODE_SPACE)) {
		editor_state = EDITOR_PANNING;
	} else {
		if (editor_state == EDITOR_PICK) editor_state = EDITOR_PICK;
		else editor_state = EDITOR_EDIT;
		pan_started = false;
	}
	// QWERTY for common controls
	{
		if (keyDown(SDL_SCANCODE_Q)) {
			state = STATE_EDITOR;
			edit_mode = EDIT_TILE;
		}
		if (keyDown(SDL_SCANCODE_W)) {
			state = STATE_EDITOR;
			edit_mode = EDIT_COLLISION;
		}
		if (keyDown(SDL_SCANCODE_E)) {
			state = STATE_EDITOR;
			edit_mode = EDIT_ENTITIES;
		}
	}
	if (keyDown(SDL_SCANCODE_S)) {
		// TODO: move this somewhere else
		state = STATE_MENU;
		menu_state = MENU_SAVEMAP;
	}
	// RESET THE MAP
	if (keyDown(SDL_SCANCODE_DELETE)) {
		resetMap();
	}
	// pick the tile/collision/entity
	if (keyPressed(SDL_SCANCODE_LALT)) {
		if (editor_state != EDITOR_PANNING)editor_state = EDITOR_PICK;
	} else {
		if (editor_state != EDITOR_PANNING) editor_state = EDITOR_EDIT;
	}
	// handle camera movement via arrow keys
	{
		if (keyPressed(SDL_SCANCODE_UP)) {
			camera_y -= static_cast<int>(delta * PAN_SPEED / 1000.f);
		}
		if (keyPressed(SDL_SCANCODE_DOWN)) {
			camera_y += static_cast<int>(delta * PAN_SPEED / 1000.f);
		}
		if (keyPressed(SDL_SCANCODE_RIGHT)) {
			camera_x += static_cast<int>(delta * PAN_SPEED / 1000.f);
		}
		if (keyPressed(SDL_SCANCODE_LEFT)) {
			camera_x -= static_cast<int>(delta * PAN_SPEED / 1000.f);
		}
	}
	// handle tile switching via number keys
	if (edit_mode == EDIT_TILE) {
		// using numbers to select a tile
		if (keyDown(SDL_SCANCODE_1)) {
			if (tiles->getNumTiles() >= 1) {
				current_tile = 0;
			}
		}
		if (keyDown(SDL_SCANCODE_2)) {
			if (tiles->getNumTiles() >= 2) {
				current_tile = 1;
			}
		}
		if (keyDown(SDL_SCANCODE_3)) {
			if (tiles->getNumTiles() >= 3) {
				current_tile = 2;
			}
		}
		if (keyDown(SDL_SCANCODE_4)) {
			if (tiles->getNumTiles() >= 4) {
				current_tile = 3;
			}
		}
		if (keyDown(SDL_SCANCODE_5)) {
			if (tiles->getNumTiles() >= 5) {
				current_tile = 4;
			}
		}
		if (keyDown(SDL_SCANCODE_6)) {
			if (tiles->getNumTiles() >= 6) {
				current_tile = 5;
			}
		}
		if (keyDown(SDL_SCANCODE_7)) {
			if (tiles->getNumTiles() >= 7) {
				current_tile = 6;
			}
		}
		if (keyDown(SDL_SCANCODE_8)) {
			if (tiles->getNumTiles() >= 8) {
				current_tile = 7;
			}
		}
		if (keyDown(SDL_SCANCODE_9)) {
			if (tiles->getNumTiles() >= 9) {
				current_tile = 8;
			}
		}
	}
	// handle switching entity types
	if (edit_mode == EDIT_ENTITIES) {
		// edit player
		if (keyPressed(SDL_SCANCODE_1)) {
			e_edit_mode = E_EDIT_PLAYER;
		}
		if (keyPressed(SDL_SCANCODE_2)) {
			e_edit_mode = E_EDIT_ENEMY;
		}
		if (keyPressed(SDL_SCANCODE_3)) {
			e_edit_mode = E_EDIT_PORTAL;
		}
	}
}

void Editor::handleLeftMouseClickEditor() {
	// check if the mouse clicks the editor state buttons first
	Vec2 m_pos = Vec2(getMouseX(), getMouseY());
	if (Math::isColliding(m_pos, Math::Rectangle(0, QcEngine::getCVARint("WINDOW HEIGHT") - 64, 64, 64))) {
		edit_mode = EDIT_TILE;
	} else if (Math::isColliding(m_pos, Math::Rectangle(64, QcEngine::getCVARint("WINDOW HEIGHT") - 64, 64, 64))) {
		edit_mode = EDIT_COLLISION;
	} else if (Math::isColliding(m_pos, Math::Rectangle(128, QcEngine::getCVARint("WINDOW HEIGHT") - 64, 64, 64))) {
		edit_mode = EDIT_ENTITIES;
	} else if (editor_state == EDITOR_PANNING) {
		pan_started = true;
		pan_mouse_x = getMouseX();
		pan_mouse_y = getMouseY();
		pan_start_x = camera_x;
		pan_start_y = camera_y;
	} else if (editor_state == EDITOR_EDIT) {
		if (edit_mode == EDIT_ENTITIES) {
			// first check if the buttons were pressed
			if (Math::isColliding(m_pos, Math::Rectangle(0, 0, 128, 64))) {
				e_edit_mode = E_EDIT_PLAYER;
			} else if (Math::isColliding(m_pos, Math::Rectangle(0, 64, 128, 64))) {
				e_edit_mode = E_EDIT_ENEMY;
			} else if (Math::isColliding(m_pos, Math::Rectangle(0, 128, 128, 64))) {
				e_edit_mode = E_EDIT_PORTAL;
			}
			// add an entity if we are not clicking buttons
			else if (e_edit_mode == E_EDIT_PLAYER) {
				// first check to see if a player is at the current position, and if so bring up the adjustment menu
				bool clicking = false;
				for (unsigned int i = 0; i < player_spawns.size(); ++i) {
					// a player is found here
					if (cur_tile_x == player_spawns[i].x && cur_tile_y == player_spawns[i].y) {
						current_spawn_index = i;
						state = STATE_MENU;
						menu_state = MENU_EDIT_SPAWN;
						clicking = true;
						break;
					}
				}
				if (!clicking) player_spawns.push_back(Vec2(cur_tile_x, cur_tile_y));
			}
			else if (e_edit_mode == E_EDIT_ENEMY) {
				entities.push_back({ E_ENEMY, cur_tile_x, cur_tile_y });
			}
			else if (e_edit_mode == E_EDIT_PORTAL) {
				// first check to make sure a portal is not at the current position already
				bool found = false;
				for (unsigned int i = 0; i < portals.size(); ++i) {
					if (cur_tile_x == portals[i].x && cur_tile_y == portals[i].y) {
						found = true;
						break;
					}
				}
				if (!found) portals.push_back(Vec2(cur_tile_x, cur_tile_y));
			}
		}
	}
}

void Editor::handleLeftMouseHeldEditor() {
	// check if the mouse clicks the editor state buttons first
	Vec2 m_pos = Vec2(getMouseX(), getMouseY());
	if (Math::isColliding(m_pos, Math::Rectangle(0, QcEngine::getCVARint("WINDOW HEIGHT") - 64, 64, 64))) {
		edit_mode = EDIT_TILE;
	} else if (Math::isColliding(m_pos, Math::Rectangle(64, QcEngine::getCVARint("WINDOW HEIGHT") - 64, 64, 64))) {
		edit_mode = EDIT_COLLISION;
	} else if (Math::isColliding(m_pos, Math::Rectangle(128, QcEngine::getCVARint("WINDOW HEIGHT") - 64, 64, 64))) {
		edit_mode = EDIT_ENTITIES;
	}
	// the user clicked to start panning the camera around
	else if (editor_state == EDITOR_PANNING) {
		if (!pan_started) {
			pan_started = true;
			pan_mouse_x = getMouseX();
			pan_mouse_y = getMouseY();
			pan_start_x = camera_x;
			pan_start_y = camera_y;
		}
		camera_x = pan_start_x - getMouseX() + pan_mouse_x;
		camera_y = pan_start_y - getMouseY() + pan_mouse_y;
	}
	// the user clicked to pick a tile from the current map
	else if (editor_state == EDITOR_PICK) {
		int cur = cur_tile_y * map_width + cur_tile_x;
		if (edit_mode == EDIT_TILE) {
			current_tile = tilemap[cur];
		}
		if (edit_mode == EDIT_COLLISION) {
			// do nothing I guess :o
		}
	}
	// the default click is to change the current tile
	else if (editor_state == EDITOR_EDIT) {
		if (edit_mode == EDIT_TILE) {
			// the user is clicking to switch palette tiles 
			// TODO: remove magic numbers here
			if (Math::isColliding(Vec2(getMouseX(), getMouseY()), Math::Rectangle(palette_x + 6, palette_y + 6, 196, 196))) {
				int tile_x = (getMouseX() - palette_x - 6) / 64;
				int tile_y = (getMouseY() - palette_y - 6) / 64;
				int new_tile = tile_y * 3 + tile_x;
				if (new_tile < tiles->getNumTiles()) {
					current_tile = new_tile;
				}
			} else {
				// the user is clicking to edit a tile
				int current = tileIndex(cur_tile_x, cur_tile_y);
				if (current < 0 || current > map_width * map_height - 1) return;
				tilemap[current] = current_tile;
			}
		}
		if (edit_mode == EDIT_COLLISION) {
			// the user is clicking to edit a collision tile
			int current = tileIndex(cur_tile_x, cur_tile_y);
			if (current < 0 || current > map_width * map_height - 1) return;
			collisionmap[current] = true;
		}
		if (edit_mode == EDIT_ENTITIES) {
			// first check if the buttons were pressed
			if (Math::isColliding(m_pos, Math::Rectangle(0, 0, 128, 64))) {
				e_edit_mode = E_EDIT_PLAYER;
			} else if (Math::isColliding(m_pos, Math::Rectangle(0, 64, 128, 64))) {
				e_edit_mode = E_EDIT_ENEMY;
			} else if (Math::isColliding(m_pos, Math::Rectangle(0, 128, 128, 64))) {
				e_edit_mode = E_EDIT_PORTAL;
			}
			// the user is clicking to adjust the position of an entitiy
			else if (e_edit_mode == E_EDIT_PLAYER) {
				// FIND A WAY TO HANDLE THIS WELL
			}
		}
	}
}

void Editor::handleRightMouseHeldEditor() {
	if (editor_state == EDITOR_EDIT) {
		int current = tileIndex(cur_tile_x, cur_tile_y);
		if (current < 0 || current > map_width * map_height - 1) return;
		// remove a tile from the map
		if (edit_mode == EDIT_TILE) {
			tilemap[current] = -1;
		}
		// remove a collision tile from the map
		if (edit_mode == EDIT_COLLISION) {
			collisionmap[current] = false;
		}
		// remove an entity from the map
		if (edit_mode == EDIT_ENTITIES) {
			for (unsigned int i = 0; i < entities.size(); ++i) {
				if (cur_tile_x == entities[i].x && cur_tile_y == entities[i].y) {
					entities.erase(entities.begin() + i);
					break;
				}
			}
			for (unsigned int i = 0; i < player_spawns.size(); ++i) {
				if (cur_tile_x == player_spawns[i].x && cur_tile_y == player_spawns[i].y) {
					player_spawns.erase(player_spawns.begin() + i);
					break;
				}
			}
			for (unsigned int i = 0; i < portals.size(); ++i) {
				if (cur_tile_x == portals[i].x && cur_tile_y == portals[i].y) {
					portals.erase(portals.begin() + i);
					break;
				}
			}
		}
	}
}