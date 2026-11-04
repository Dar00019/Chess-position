#pragma once
#include "Piece.h"

class Template : public Drawable {
	Piece piece;
	RectangleShape button;
	Color baseColor = Color(120, 80, 50);
	Color outlineColor = Color(220, 190, 110);
	Color selectColor = Color(10, 200, 10);		//цвет выделения кнопки

	virtual void draw(RenderTarget& target, RenderStates states) const {
		target.draw(button);
		target.draw(piece);
	}

public:
	Template() {}

	void create(std::string texturePath, bool isWhite, std::string role) {
		piece.create(texturePath, isWhite, role);
		FloatRect pieceSize = piece.getSize();
		button.setSize(Vector2f(pieceSize.width, pieceSize.height));
		button.setFillColor(baseColor);
		button.setOutlineColor(outlineColor);
		button.setOutlineThickness(1);
	}

	void setPosition(float x, float y) {
		button.setPosition(x, y);
		piece.setPosition(x, y);
	}

	Vector2f getPosition() {
		return button.getPosition();
	}

	Vector2f getSize() {
		return button.getSize();
	}

	Piece getPiece() {
		return piece;
	}

	//изменить размер доски (на коэффициент)
	void resize(float coefficient) {
		piece.scale(coefficient);
		FloatRect pieceSize = piece.getSize();
		button.setSize(Vector2f(pieceSize.width, pieceSize.height));
	}

	//вернуть изначальный размер доски
	void defaultSize() {
		piece.defaultScale();
		FloatRect pieceSize = piece.getSize();
		button.setSize(Vector2f(pieceSize.width, pieceSize.height));
	}

	FloatRect getGlobalBounds() {
		return button.getGlobalBounds();
	}

	//наведение курсора на клетку
	void hover() {
		button.setFillColor(selectColor);
	}

	//сведение курсора с клетки
	void unhover() {
		button.setFillColor(baseColor);
	}
};