// -------------------------------------------------------------------------------
// Contains all code relating to the menu state in the editor application
// -------------------------------------------------------------------------------

#include "editor.hpp"

void Editor::renderMenu() {
	if (menu_state == MENU_SAVEMAP) {
		int x = (QcEngine::getCVARint("WINDOW WIDTH") - QcEngine::getTexture(SAVEMAP)->getWidth()) / 2;
		int y = (QcEngine::getCVARint("WINDOW HEIGHT") - QcEngine::getTexture(SAVEMAP)->getHeight()) / 2;
		QcEngine::getTexture(SAVEMAP)->render(x, y);
		// render text if there is any
		const std::string& ref = managerRef->getTextInput();
		if (ref.size() > 0) {
			SDL_Texture * text_texture = getTextTexture(ref, "default_16", { 0, 0, 0 });
			Texture * tex = new Texture(text_texture);
			tex->render(530, 350);
			delete tex;
		}
	}
	if (menu_state == MENU_EDIT_SPAWN) {
		for (unsigned int i = 0; i < player_spawns.size(); ++i) {
			// first render the tile background
			if (i == current_spawn_index) {
				QcEngine::getTexture(SPAWN_SELECT)->render(240, 100 + i * 80);
			} else {
				QcEngine::getTexture(SPAWN_BLOCK)->render(240, 100 + i * 80);
			}
			// render the tile index number
			getNumberTexture(i)->render(255, 110 + i * 80);
		}
	}
	if (menu_state == MENU_EDIT_PORTAL) {
		QcEngine::getTexture(BLANK_MENU)->render((1280 - 256) / 2, (720 - 128) / 2);
		// render the file name
		SDL_Texture * file_text = getTextTexture(portals[current_portal_index].file, "default_16", {0, 0, 0});
		Texture * file_tex = new Texture(file_text);
		file_tex->render((1280 - 240) / 2, (720 - 100) / 2);
		delete file_tex;
		// render the portal index
		SDL_Texture * num_text = getTextTexture(std::to_string(portals[current_portal_index].num), "default_16", { 0, 0, 0 });
		Texture * num_tex = new Texture(num_text);
		num_tex->render((1280 - 10) / 2, (720 - 60) / 2);
		delete num_tex;
	}
	if (menu_state == MENU_MAP_SETTINGS) {
		int anchor_x = (1280 - 256) / 2;
		int anchor_y = (720 - 128) / 2 - 100;
		// RENDER THE MAP SELECTION PANEL
		{
			QcEngine::getTexture(SMALL_MENU)->render(anchor_x, anchor_y);
			// render the title
			Texture title(getTextTexture("CURRENT MAP", "default_16", { 0, 0, 0 }));
			title.render(anchor_x + 20, anchor_y + 10);
			// render the name of the current file
			Texture file_tex(getTextTexture(current_map, "default_16", { 0, 0, 0 }));
			file_tex.render(anchor_x + 20, anchor_y + 30);
		}
		anchor_y += 66;
		// RENDER THE MAP HEIGHT/WIDTH PANEL
		{
			QcEngine::getTexture(BLANK_MENU)->render(anchor_x, anchor_y);
			QcEngine::getTexture(DIMENSION)->render(anchor_x, anchor_y);
			// render the title
			Texture title(getTextTexture("MAP DIMENSIONS", "default_16", { 0, 0, 0 }));
			title.render(anchor_x + 20, anchor_y + 20);
			// render the width / height
			Texture w(getTextTexture(std::to_string(new_width), "default_16", { 0, 0, 0 }));
			w.render(anchor_x + 180, anchor_y + 62);
			Texture h(getTextTexture(std::to_string(new_height), "default_16", { 0, 0, 0 }));
			h.render(anchor_x + 180, anchor_y + 95);
		}
		anchor_y += 130;
		// RENDER THE TILEMAP SOURCE PANEL
		{
			QcEngine::getTexture(SMALL_MENU)->render(anchor_x, anchor_y);
			// render the title
			Texture title(getTextTexture("TILEMAP SOURCE", "default_16", { 0, 0, 0 }));
			title.render(anchor_x + 20, anchor_y + 10);
			// render the name of the current tilemap file
			Texture file_tex(getTextTexture(tilemap_source, "default_16", { 0, 0, 0 }));
			file_tex.render(anchor_x + 20, anchor_y + 30);
		}
		anchor_y += 66;
		// RENDER THE BACKGROUND SOURCE PANEL
		{
			QcEngine::getTexture(SMALL_MENU)->render(anchor_x, anchor_y);
			// render the title
			Texture title(getTextTexture("BACKGROUND SOURCE", "default_16", { 0, 0, 0 }));
			title.render(anchor_x + 20, anchor_y + 10);
			// render the name of the current background file
			Texture file_tex(getTextTexture(background_source, "default_16", { 0, 0, 0 }));
			file_tex.render(anchor_x + 20, anchor_y + 30);
		}
	}
}

void Editor::handleKeyPressMenu() {
	if (keyDown(SDL_SCANCODE_RETURN)) {
		if (menu_state == MENU_SAVEMAP) {
			const std::string& ref = managerRef->getTextInput();
			if (ref.size() > 0) {
				std::string dest = DEFAULT_MAP_FOLDER + ref + ".txt";
				saveMap(dest);
			} else {
				saveMap(DEFAULT_MAP_FILE);
			}
			state = STATE_EDITOR;
			managerRef->stopTextInput();
		}
		if (menu_state == MENU_EDIT_SPAWN) {
			state = STATE_EDITOR;
		}
		if (menu_state == MENU_EDIT_PORTAL) {
			state = STATE_EDITOR;
		}
		// save the new map with new settings
		if (menu_state == MENU_MAP_SETTINGS) {
			// only update the map width/height for now
			std::vector<int> new_map(new_width * new_height, -1);
			std::vector<bool> new_collision(new_width * new_height, false);
			for (int i = 0; i < new_width; ++i) {
				for (int j = 0; j < new_height; ++j) {
					if (i < map_width && j < map_height) {
						new_map[j * new_width + i] = tilemap[tileIndex(i, j)];
						new_collision[j * new_width + i] = collisionmap[tileIndex(i, j)];
					}
				}
			}
			std::swap(new_map, tilemap);
			std::swap(new_collision, collisionmap);
			map_width = new_width;
			map_height = new_height;
			// TODO: remove entities that are out of bounds
			state = STATE_EDITOR;
		}
	}
}

void Editor::handleLeftMouseClickMenu() {
	if (menu_state == MENU_SAVEMAP) {
		Vec2 m_pos = Vec2(getMouseX(), getMouseY());
		int x = (QcEngine::getCVARint("WINDOW WIDTH") - QcEngine::getTexture(SAVEMAP)->getWidth()) / 2;
		int y = (QcEngine::getCVARint("WINDOW HEIGHT") - QcEngine::getTexture(SAVEMAP)->getHeight()) / 2;
		if (Math::isColliding(m_pos, Math::Rectangle(x, y, 256, 128))) {
			managerRef->startTextInput();
		}
	}
	if (menu_state == MENU_MAP_SETTINGS) {
		int anchor_x = (1280 - 256) / 2;
		int anchor_y = (720 - 128) / 2 - 100;
		Vec2 m_pos = Vec2(getMouseX(), getMouseY());
		// if the user clicks on the map selection, enter file menu to choose a new map file
		{
			Math::Rectangle target = Math::Rectangle(anchor_x, anchor_y, 256, 64);
			if (Math::isColliding(m_pos, target)) {
				enterFileState(FILE_LOADMAP, DEFAULT_MAP_FOLDER);
			}
		}
		anchor_y += 66;
		// if the user clicks on the map dimension buttons, update accordingly
		{
			Math::Rectangle w_left = Math::Rectangle(anchor_x + 124, anchor_y + 61, 41, 18);
			Math::Rectangle w_right = Math::Rectangle(anchor_x + 212, anchor_y + 61, 36, 18);
			Math::Rectangle h_left = Math::Rectangle(anchor_x + 124, anchor_y + 93, 41, 15);
			Math::Rectangle h_right = Math::Rectangle(anchor_x + 212, anchor_y + 93, 36, 18);
			if (Math::isColliding(m_pos, w_left)) {
				if (new_width > 0) new_width--;
			}
			if (Math::isColliding(m_pos, w_right)) {
				new_width++;
			}
			if (Math::isColliding(m_pos, h_left)) {
				if (new_height > 0) new_height--;
			}
			if (Math::isColliding(m_pos, h_right)) {
				new_height++;
			}
		}
		anchor_y += 130;
		// if the user clicks on the tilemap source, enter file menu to choose a new tilemap
		{
			Math::Rectangle target = Math::Rectangle(anchor_x, anchor_y, 256, 64);
			if (Math::isColliding(m_pos, target)) {
				enterFileState(FILE_LOADTILEMAP, DEFAULT_ASSETS);
			}
		}
		anchor_y += 66;
		// if the user clicks on the background source, enter file menu to choose a new background
		{
			Math::Rectangle target = Math::Rectangle(anchor_x, anchor_y, 256, 64);
			if (Math::isColliding(m_pos, target)) {
				enterFileState(FILE_LOADBACKGROUND, DEFAULT_ASSETS);
			}
		}
	}
}