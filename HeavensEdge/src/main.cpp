#include "QcEngine.hpp"
#include "game.hpp"

#include <memory>
#include <ctime>

// -------------------------------------------------------------------------------
// MAIN PROGRAM FOR GAME
// -------------------------------------------------------------------------------
int main(int argc, char* argv[]) {

	srand((unsigned int)time(NULL));
	
	QcEngine::createWindow("HEAVEN'S EDGE", 1280, 720, false, SDL_WINDOW_BORDERLESS);
	QcEngine::setRates(60, 44);
	QcEngine::setState(std::make_unique<Game>());

	while (QcEngine::isRunning()) {
		QcEngine::update();
	}

}