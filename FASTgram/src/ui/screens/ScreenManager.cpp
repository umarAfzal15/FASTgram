#include "ScreenManager.h"

ScreenManager::ScreenManager() {
	this->currentScreen = nullptr;
}

void ScreenManager::setScreen(Screen* newScreen) {
	if (newScreen == this->currentScreen || newScreen == nullptr) {
		return;
	}

	if (this->currentScreen != nullptr) {
		this->currentScreen->onExit();
		delete this->currentScreen;
	}
	this->currentScreen = newScreen;
	this->currentScreen->onEnter();
}

void ScreenManager::update() {
	if (this->currentScreen != nullptr) {
		this->currentScreen->update();
	}
}

void ScreenManager::draw() const {
	if (this->currentScreen != nullptr) {
		this->currentScreen->draw();
	}
}

bool ScreenManager::hasScreen() const {

	if (this->currentScreen) {
		return true;
	}
	else {
		return false;
	}
}

ScreenManager::~ScreenManager() {
	if (this->currentScreen != nullptr) {
		this->currentScreen->onExit();
		delete this->currentScreen;
		this->currentScreen = nullptr;
	}
}