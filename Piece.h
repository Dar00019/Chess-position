#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Piece : public Drawable {
	Texture texture;
	Sprite sprite;
	Sprite defaultSprite;
	bool setup = false;
	bool isWhite = true;
	std::string role = "";

	virtual void draw(RenderTarget& target, RenderStates states) const {
		target.draw(sprite);
	}
public:
	Piece() {}

	Piece(std::string texturePath, bool isWhite, std::string role) {
		create(texturePath, isWhite, role);
	}

	void create(std::string texturePath, bool isWhite, std::string role) {
		texture.loadFromFile(texturePath);
		sprite.setTexture(texture);
		sprite.scale(0.8, 0.8);
		defaultSprite = sprite;
		this->role = role;
		this->isWhite = isWhite;
		setup = true;
	}

	void setPosition(float x, float y) {
		sprite.setPosition(x, y);
	}

	void setPosition(Vector2f pos) {
		sprite.setPosition(pos);
	}

	Vector2f getPosition() {
		return sprite.getPosition();
	}

	FloatRect getSize() {
		return sprite.getGlobalBounds();
	}

	std::string getRole() {
		return role;
	}

	std::string getColor() {
		if (isWhite) return "White";
		return "Black";
	}

	bool isSetup() {
		return setup;
	}

	void scale(float coefficient) {
		sprite.scale(coefficient, coefficient);
	}

	void defaultScale() {
		sprite = defaultSprite;
	}
};