#include "editor.hpp"

void Editor::enterFileState(const std::string& dir) {
	if (!fs::is_directory(dir)) {
		ERR(dir << " isn't a directory...");
		return;
	}
	state = STATE_FILE;
	files.clear();
	// initialize file entries in the target directory
	int cur_y = 10;
	for (auto& p : fs::directory_iterator(dir)) {
		if (!fs::is_directory(p)) {
			std::string entry = p.path().string();
			Math::Rectangle rect = Math::Rectangle(20, cur_y, 256, 24);
			cur_y += 24;
			files.push_back({ entry, rect, new Texture(getTextTexture(entry, "default_16",{ 0, 0, 0 })) });
		}
	}
}

void Editor::renderFile() {
	// render the background menu image
	QcEngine::getTexture(LARGE_MENU)->render(0, 0);
	// render the file items
	for (unsigned int i = 0; i < files.size(); ++i) {
		// if the mouse is hovering over current item, render white overlay
		if (getMouseX() > files[i].collision.pos.x && getMouseX() < files[i].collision.pos.x + files[i].collision.w &&
			getMouseY() > files[i].collision.pos.y && getMouseY() < files[i].collision.pos.y + files[i].collision.h) {
			QcEngine::getTexture(WHITE_BAR)->render(20, 10 + 24 * i);
		}
		files[i].tex->render(20, 10 + 24 * i);
	}
}

void Editor::handleKeyPressFile() {

}

void Editor::handleLeftMouseClickFile() {
	for (unsigned int i = 0; i < files.size(); ++i) {
		if (getMouseX() > files[i].collision.pos.x && getMouseX() < files[i].collision.pos.x + files[i].collision.w &&
			getMouseY() > files[i].collision.pos.y && getMouseY() < files[i].collision.pos.y + files[i].collision.h) {
			// SAVE THE FOUND FILE SOMEHOW
			/*
			TileMap * old = tiles;
			tiles = new TileMap(files[i].name);
			delete old;
			state = STATE_EDITOR;
			// TODO: fix metadata loading when opening file
			tiles->generateTiles(DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE);
			*/
			if (file_mode == FILE_LOADMAP) {
				loadMap(files[i].name);
			}
			// change back to normal editor mode
			state = STATE_EDITOR;
			break;
		}
	}
}