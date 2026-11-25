#include "Theme.h"
#include <fstream>

bool Theme::darkMode = false;
int Theme::windowWidth = 480;
int Theme::windowHeight = 800;
Font Theme::appFont = { 0 };

void Theme::toggle() {
	if (darkMode) {
		darkMode = false;
	}
	else {
		darkMode = true;
	}
}

void Theme::setDark(bool dark) {
	darkMode = dark;
}

bool Theme::isDark() {
	return darkMode;
}

void Theme::setWindowSize(int width, int height) {
	windowWidth = width;
	windowHeight = height;
}

int Theme::getWindowWidth() {
	return windowWidth;
}

int Theme::getWindowHeight() {
	return windowHeight;
}

void Theme::loadConfig(int& w, int& h) {
	loadConfig("data/config.txt");
	w = windowWidth;
	h = windowHeight;
}

void Theme::saveConfig(int w, int h) {
	windowWidth = w;
	windowHeight = h;
	saveConfig("data/config.txt");
}

void Theme::loadConfig(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) return;

	int savedDark = 0;
	int savedW = 480;
	int savedH = 800;

	if (file >> savedDark >> savedW >> savedH) {
		darkMode = (savedDark == 1);
		if (savedW >= 480) windowWidth = savedW;
		if (savedH >= 600) windowHeight = savedH;
	}
}

void Theme::saveConfig(const std::string& filePath) {
	std::ofstream file(filePath);
	if (!file.is_open()) return;

	file << (darkMode ? 1 : 0) << " " << windowWidth << " " << windowHeight << "\n";
}

// Core UI Palette
Color Theme::outerBackground() {
	if (darkMode) {
		return GetColor(0x05070BFF);
	}
	else {
		return GetColor(0xE8ECF4FF);
	}
}

Color Theme::background() {
	if (darkMode) {
		return GetColor(0x0B0F19FF);
	}
	else {
		return GetColor(0xF7F9FCFF);
	}
}

Color Theme::surface() {
	if (darkMode) {
		return GetColor(0x1B2438FF);
	}
	else {
		return GetColor(0xF0F4FAFF);
	}
}

Color Theme::surfaceHover() {
	if (darkMode) {
		return GetColor(0x243049FF);
	}
	else {
		return GetColor(0xE2E8F3FF);
	}
}

Color Theme::primaryText() {
	if (darkMode) {
		return GetColor(0xF8FAFCFF);
	}
	else {
		return GetColor(0x0B1120FF);
	}
}

Color Theme::secondaryText() {
	if (darkMode) {
		return GetColor(0xCBD5E1FF);
	}
	else {
		return GetColor(0x334155FF);
	}
}

Color Theme::border() {
	if (darkMode) {
		return GetColor(0x232F46FF);
	}
	else {
		return GetColor(0xDCE3EEFF);
	}
}

Color Theme::accent() {
	if (darkMode) {
		return GetColor(0x6366F1FF);
	}
	else {
		return GetColor(0x4F46E5FF);
	}
}

Color Theme::accentHover() {
	if (darkMode) {
		return GetColor(0x818CF8FF);
	}
	else {
		return GetColor(0x4338CAFF);
	}
}

Color Theme::danger() {
	if (darkMode) {
		return GetColor(0xFEE2E2FF);
	}
	else {
		return GetColor(0xEF4444FF);
	}
}

Color Theme::success() {
	if (darkMode) {
		return GetColor(0xD1FAE5FF);
	}
	else {
		return GetColor(0x10B981FF);
	}
}