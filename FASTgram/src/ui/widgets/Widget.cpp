#include "Widget.h"

Widget::Widget(Rectangle bounds) {
	this->bounds = bounds;
	this->visible = true;
	this->enabled = true;
	this->hovered = false;
}

void Widget::updateHover() {

	if (!visible || !enabled) {
		hovered = false;
		return;
	}

	double x = GetMouseX(); //Get mouse position X
	double y = GetMouseY(); // Get mouse position Y

	if ((x >= bounds.x && x <= bounds.x + bounds.width) && (y >= bounds.y && y <= bounds.y + bounds.height)) {
		hovered = true;
	}
	else {
		hovered = false;
	}

}

void Widget::setPosition(float x, float y) {
	bounds.x = x;
	bounds.y = y;
}

void Widget::setBounds(Rectangle rect) {
	bounds = rect;
}

Rectangle Widget::getBounds() const {
	return bounds;
}

// State getters and setters

void Widget::setVisible(bool visible) {
	if (visible) {
		this->visible = false;
	}
	else {
		this->visible = true;
	}
}

bool Widget::isVisible() const {
	return this->visible;
}

void Widget::setEnabled(bool enabled) {
	if (enabled) {
		this->enabled = false;
	}
	else {
		this->enabled = true;
	}
}

bool Widget::isEnabled() const {
	return this->enabled;
}

bool Widget::isHovered() const {
	return this->hovered;
}