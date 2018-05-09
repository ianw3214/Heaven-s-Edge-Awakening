#include "QcEngine.hpp"

#include <memory>

#include "game.hpp"

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

// -------------------------------------------------------------------------------
// ENTRY POINT FOR GAME CODE
// -------------------------------------------------------------------------------
int main(int argc, char* argv[]) {

	QcEngine::createWindow("HEAVEN'S EDGE: AWAKENING", DEFAULT_WIDTH, DEFAULT_HEIGHT, false, SDL_WINDOW_RESIZABLE);
	QcEngine::setRates(60, 44);
	QcEngine::setState(std::make_unique<Game>());

	while (QcEngine::isRunning()) {
		QcEngine::update();
	}

	return 0;

}