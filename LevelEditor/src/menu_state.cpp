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
}