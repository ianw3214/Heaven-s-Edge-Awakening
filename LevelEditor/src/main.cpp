#include "QcEngine.hpp"
#include "editor.hpp"

#include <memory>

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

// -------------------------------------------------------------------------------
// MAIN PROGRAM FOR EDITOR CODE
// -------------------------------------------------------------------------------
int main(int argc, char* argv[]) {

	QcEngine::createWindow("EDITOR", DEFAULT_WIDTH, DEFAULT_HEIGHT, false, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALWAYS_ON_TOP);
	QcEngine::setRates(60, 44);
	QcEngine::setState(std::make_unique<Editor>());

	QcEngine::setCVAR("WINDOW WIDTH", DEFAULT_WIDTH);
	QcEngine::setCVAR("WINDOW HEIGHT", DEFAULT_HEIGHT);

	while (QcEngine::isRunning()) {
		QcEngine::update();
	}

	return 0;

}