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