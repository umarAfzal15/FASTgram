#include "Theme.h"

bool Theme::darkMode = false;

void Theme::toggle() {
	if (darkMode) {
		darkMode = false;
	}
	else {
		darkMode = true;
	}
}

void  Theme::setDark(bool dark) {
	darkMode = dark;
}
bool  Theme::isDark() {
	return darkMode;
}

// Core UI Palette
Color Theme::outerBackground() {
	if (darkMode) {
		return GetColor(0x334155FF);
	}
	else {
		return GetColor(0xF1F5F9FF);
	}
}
Color Theme::background() {
	if (darkMode) {
		return GetColor(0x334155FF);
	}
	else {
		return GetColor(0xF8FAFCFF);
	}
}

Color Theme::surface() {
	if (darkMode) {
		return GetColor(0x1E293BFF);
	}
	else {
		return GetColor(0xFFFFFFFF);
	}
}
//Color Theme:: surfaceHover(); 
//Color Theme:: primaryText(); 
//Color Theme:: secondaryText();
//Color Theme:: border(); 
//Color Theme:: accent();
//Color Theme:: accentHover(); 
//Color Theme:: danger();
//Color Theme:: success(); 