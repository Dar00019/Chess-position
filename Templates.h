#pragma once
#include "Template.h"

class Templates : public Drawable {
	std::vector <Template*> templates;

	Template king;
	Template queen;
	Template bishop;
	Template knight;
	Template rook;
	Template pawn;

	bool isWhite = true;
	int previousPos = -1;	//предыдущая кнопка, на которую наводил курсор
	int currentPos = -1;	//текущая кнопка, на которую наводит курсор

	virtual void draw(RenderTarget& target, RenderStates states) const {
		for (Template* piece : templates) {
			target.draw(*piece);
		}
	}

public:
	Templates() {}

	void create(std::string color) {
		if (color == "Black") isWhite = false;

		king.create("png\\Chess\\" + color + "\\k.png", isWhite, "King");
		queen.create("png\\Chess\\" + color + "\\q.png", isWhite, "Queen");
		bishop.create("png\\Chess\\" + color + "\\b.png", isWhite, "Bishop");
		knight.create("png\\Chess\\" + color + "\\n.png", isWhite, "Knight");
		rook.create("png\\Chess\\" + color + "\\r.png", isWhite, "Rook");
		pawn.create("png\\Chess\\" + color + "\\p.png", isWhite, "Pawn");

		templates.push_back({ &king });
		templates.push_back({ &queen });
		templates.push_back({ &bishop });
		templates.push_back({ &knight });
		templates.push_back({ &rook });
		templates.push_back({ &pawn });
	}

	void setPosition(float x, float y) {
		Vector2f pieceSize = getButtonSize();
		for (size_t i = 0; i < templates.size(); i++) {
			templates[i]->setPosition(x, y + i * pieceSize.y);
		}
	}

	Vector2f getPosition() {
		if (templates.size() > 0) return templates[0]->getPosition();
		return Vector2f(0, 0);
	}

	Vector2f getButtonSize() {
		if (templates.size() > 0) return templates[0]->getSize();
		return Vector2f(0, 0);
	}

	void resize(float coefficient) {
		float pieceSize = getButtonSize().y * coefficient;
		Vector2f position = getPosition();
		for (size_t i = 0; i < templates.size(); i++) {
			templates[i]->resize(coefficient);
			templates[i]->setPosition(position.x, position.y + i * pieceSize);
		}
	}

	void defaultSize() {
		Vector2f position = getPosition();
		for (size_t i = 0; i < templates.size(); i++) {
			templates[i]->defaultSize();
			templates[i]->setPosition(position.x, position.y + i * templates[i]->getSize().y);
		}
	}

	void centerRelativeToTheBoard(Vector2f boardPos, Vector2f boardSize) {
		Vector2f buttonSize = getButtonSize();
		if (buttonSize == Vector2f(0, 0)) return;
		setPosition(boardPos.x + (isWhite ? -buttonSize.x : boardSize.x), boardPos.y + (boardSize.y - buttonSize.y * templates.size()) / 2);
	}

	//Взаимодействие c курсором
	bool hoverInteraction(Vector2f mousePos) {
		bool isInteraction = false;
		for (size_t i = 0; i < templates.size(); i++) {
			if (templates[i]->getGlobalBounds().contains(mousePos) and !isInteraction) {
				previousPos = currentPos;
				currentPos = i;
				templates[i]->hover();
				isInteraction = true;
			}
			else {
				int coord = isInteraction ? previousPos : currentPos;
				if (coord == i) {
					templates[i]->unhover();
				}
			}
		}
		return isInteraction;
	}

	//Вернуть фигуру, с которой производится взаимодействие
	Piece actInteraction(Vector2f mousePos) {
		for (size_t i = 0; i < templates.size(); i++) {
			if (templates[i]->getGlobalBounds().contains(mousePos)) {
				return templates[i]->getPiece();
			}
		}
		return Piece();
	}

	/*
	Piece getKing() {
		return king.getPiece();
	}

	Piece getQueen() {
		return queen.getPiece();
	}

	Piece getPawn() {
		return pawn.getPiece();
	}

	Piece getBishop() {
		return bishop.getPiece();
	}

	Piece getKnight() {
		return knight.getPiece();
	}

	Piece getRook() {
		return rook.getPiece();
	}*/

	Piece getRolePiece(std::string role) {
		if (role == "King") return king.getPiece();
		if (role == "Queen") return queen.getPiece();
		if (role == "Bishop") return bishop.getPiece();
		if (role == "Knight") return knight.getPiece();
		if (role == "Rook") return rook.getPiece();
		if (role == "Pawn") return pawn.getPiece();
		return Piece();
	}
};