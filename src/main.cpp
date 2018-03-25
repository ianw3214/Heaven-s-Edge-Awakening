#include "QcEngine.hpp"
#include "game.hpp"

#include <memory>
#include <ctime>

int main(int argc, char* argv[]) {

	srand((unsigned int)time(NULL));
	
	QcEngine::createWindow("GMD", 1280, 720, false, NULL);
	QcEngine::setRates(60, 44);
	QcEngine::setState(std::make_unique<Game>());

	while (QcEngine::isRunning()) {
		QcEngine::update();
	}

}