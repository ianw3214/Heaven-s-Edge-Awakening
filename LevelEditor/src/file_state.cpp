#include "editor.hpp"

void Editor::enterFileState(FileMode mode, const std::string& dir) {
	if (!fs::is_directory(dir)) {
		ERR(dir << " isn't a directory...");
		return;
	}
	file_mode = mode;
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
			// load the file as map data
			if (file_mode == FILE_LOADMAP) {
				loadMap(files[i].name);
			}
			// load the file as a tilemap
			if (file_mode == FILE_LOADTILEMAP) {
				tilemap_source = files[i].name;
				TileMap * old = tiles;
				tiles = new TileMap(tilemap_source);
				tiles->generateTiles(tile_size, tile_size);
				delete old;
			}
			// load the file as a background source
			if (file_mode == FILE_LOADBACKGROUND) {
				background_source = files[i].name;
				QcEngine::loadTexture("background", background_source);
			}
			// change back to normal editor mode
			state = STATE_EDITOR;
			break;
		}
	}
}