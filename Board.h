#pragma once
#include "Templates.h"
#include "Buttons.h"
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include "json.hpp"
using json = nlohmann::json;

const int NUMBER_OF_CELLS = 8; //количество клеток на одной стороне

//Класс, отвечающий за доску
class Board : public Drawable {
	//структура шахматной клетки
	struct Cell {
		RectangleShape field;
		Piece piece;
	};

	//один ход
	struct Step {
		Vector2i previousPos = Vector2i(-1, -1);
		Vector2i currentPos = Vector2i(-1, -1);
		Piece previousPieceInCurrentPos;
		Piece newPiece;
	};

	RectangleShape base;		//основа
	RectangleShape outline;		//обводка
	Cell** boardArray;			//массив с клетками
	Text* numbersColumn;		//цифры на доске
	Text* letterRow;			//буквы на доске
	Text* numbersColumn2;		//цифры на доске
	Text* letterRow2;			//буквы на доске

	const int DEFAULT_CHARACTER_SIZE = 20;		//размер символов по умолчанию
	const float DEFAULT_CELL_SIZE = 75;			//размер клетки по умолчанию
	const float DEFAULT_BASE_SIZE = 30;			//размер основы по умолчанию
	const float DEFAULT_OUTLINE_SIZE = 2;		//размер обводки по умолчанию

	int characterSize = DEFAULT_CHARACTER_SIZE;	//текущий размер символов
	float cellSize = DEFAULT_CELL_SIZE;			//текущий размер клетки
	float baseWidth = DEFAULT_BASE_SIZE;		//текущий размер основы
	float outlineWidth = DEFAULT_OUTLINE_SIZE;	//текущий размер обводки

	Color blackColor = Color(120, 80, 50);		//тёмный цвет клетки
	Color whiteColor = Color(220, 190, 110);	//светлый цвет клетки
	Color selectColor = Color(10, 200, 10);		//цвет выделения клетки
	Color textColor = Color::Black;				//цвет текста

	Vector2i previousMousePos = Vector2i(-1, -1);	//предыдущая клетка, на которую наводил курсор
	Vector2i currentMousePos = Vector2i(-1, -1);	//текущая клетка, на которую наводит курсор

	bool isMove = false;			//если взяли фигуру с доски
	std::vector<Step> history;		//история действий
	int currentStep = 0;			//текущий ход

	bool isStandartPosition = true;	//прямое или обратное положение доски

	SoundBuffer buffer;
	Sound sound;
	bool isSound = true;


	//отрисовать доску
	virtual void draw(RenderTarget& target, RenderStates states) const {
		target.draw(base);
		target.draw(outline);
		for (size_t i = 0; i < NUMBER_OF_CELLS; i++) {
			for (size_t j = 0; j < NUMBER_OF_CELLS; j++) {
				target.draw(boardArray[i][j].field);
				target.draw(boardArray[i][j].piece);
			}
			target.draw(numbersColumn[i]);
			target.draw(letterRow[i]);
			target.draw(numbersColumn2[i]);
			target.draw(letterRow2[i]);
		}
	}

	//выровнять фигуру по центру клетки
	void centerPiece(size_t i, size_t j) {
		Vector2f cellPos = boardArray[i][j].field.getPosition();
		FloatRect pieceSize = boardArray[i][j].piece.getSize();
		boardArray[i][j].piece.setPosition(cellPos.x + (cellSize - pieceSize.width) / 2, cellPos.y + (cellSize - pieceSize.height) / 2);
	}

	//обновить размер доски
	void updateSize(const float COEFFICIENT = 1) {
		base.setSize(Vector2f(cellSize * NUMBER_OF_CELLS + baseWidth * 2, cellSize * NUMBER_OF_CELLS + baseWidth * 2));
		outline.setSize(Vector2f(cellSize * NUMBER_OF_CELLS + outlineWidth * 2, cellSize * NUMBER_OF_CELLS + outlineWidth * 2));
		outline.setPosition(baseWidth + base.getPosition().x, baseWidth + base.getPosition().y);

		float x = base.getPosition().x + baseWidth + outlineWidth;
		float y = base.getPosition().y + baseWidth + outlineWidth;
		for (size_t i = 0; i < NUMBER_OF_CELLS; i++) {
			for (size_t j = 0; j < NUMBER_OF_CELLS; j++) {
				boardArray[i][j].field.setSize(Vector2f(cellSize, cellSize));
				boardArray[i][j].field.setPosition(i * cellSize + x, j * cellSize + y);
				if (COEFFICIENT != 1) {
					boardArray[i][j].piece.scale(COEFFICIENT);
				}
				else {
					boardArray[i][j].piece.defaultScale();
				}
				centerPiece(i, j);
			}
		}
	}

	//выравнивание меток
	void alignSymbol(size_t i) {
		float x = base.getPosition().x;
		float y = base.getPosition().y;
		Vector2f boardSize = base.getSize();
		auto numberWidth = numbersColumn[i].getGlobalBounds().width;
		auto letterWidth = letterRow[i].getGlobalBounds().width;
		numbersColumn[i].setPosition((baseWidth - numberWidth) / 2 + x, i * cellSize + (cellSize - characterSize) / 2 + y + baseWidth + outlineWidth);
		letterRow[i].setPosition(i * cellSize + (cellSize - letterWidth) / 2 + x + baseWidth + outlineWidth, base.getSize().y - baseWidth + (baseWidth - characterSize) / 2 + y);
		numbersColumn2[i].setPosition(boardSize.x - baseWidth + (baseWidth - numberWidth) / 2 + x, numbersColumn[i].getPosition().y);
		letterRow2[i].setPosition(letterRow[i].getPosition().x, (baseWidth - characterSize) / 2 + y);
	}

	//наведение курсора на клетку
	void hover(size_t i, size_t j) {
		boardArray[i][j].field.setFillColor(selectColor);
	}

	//сведение курсора с клетки
	void unhover(size_t i, size_t j) {
		if ((i + j) % 2 == 0)
			boardArray[i][j].field.setFillColor(whiteColor);
		else
			boardArray[i][j].field.setFillColor(blackColor);
	}

	//Изменить размер фигурок в истории
	void resizeHistoryPieces(const float COEFFICIENT) {
		for (size_t i = 0; i < history.size(); i++) {
			if (history[i].newPiece.isSetup()) {
				history[i].newPiece.scale(COEFFICIENT);
			}
			if (history[i].previousPieceInCurrentPos.isSetup()) {
				history[i].previousPieceInCurrentPos.scale(COEFFICIENT);
			}
		}
	}

	//Вернуть прежний размер фигурам в истории
	void defaultSizeHistoryPieces() {
		for (size_t i = 0; i < history.size(); i++) {
			if (history[i].newPiece.isSetup()) {
				history[i].newPiece.defaultScale();
			}
			if (history[i].previousPieceInCurrentPos.isSetup()) {
				history[i].previousPieceInCurrentPos.defaultScale();
			}
		}
	}

public:
	//инициализация доски
	Board() {
		base.setSize(Vector2f(cellSize * NUMBER_OF_CELLS + baseWidth * 2, cellSize * NUMBER_OF_CELLS + baseWidth * 2));
		outline.setSize(Vector2f(cellSize * NUMBER_OF_CELLS + outlineWidth * 2, cellSize * NUMBER_OF_CELLS + outlineWidth * 2));
		outline.setPosition(baseWidth, baseWidth);

		base.setFillColor(whiteColor);
		outline.setFillColor(blackColor);

		//численные и буквенные метки
		numbersColumn = new Text[NUMBER_OF_CELLS];
		letterRow = new Text[NUMBER_OF_CELLS];
		numbersColumn2 = new Text[NUMBER_OF_CELLS];
		letterRow2 = new Text[NUMBER_OF_CELLS];

		//доска
		boardArray = new Cell * [NUMBER_OF_CELLS] {};

		for (size_t i = 0; i < NUMBER_OF_CELLS; i++) {

			//настройка численных и буквенных меток
			numbersColumn[i].setFont(fontUsual);
			letterRow[i].setFont(fontUsual);
			numbersColumn[i].setString(std::to_string(NUMBER_OF_CELLS - i));
			letterRow[i].setString((char)((int)'A' + i));
			numbersColumn[i].setCharacterSize(characterSize);
			letterRow[i].setCharacterSize(characterSize);
			numbersColumn[i].setFillColor(textColor);
			letterRow[i].setFillColor(textColor);
			numbersColumn2[i] = numbersColumn[i];
			letterRow2[i] = letterRow[i];
			alignSymbol(i);

			//настройка самих клеток
			boardArray[i] = new Cell [NUMBER_OF_CELLS];
			for (size_t j = 0; j < NUMBER_OF_CELLS; j++) {
				boardArray[i][j].field.setSize(Vector2f(cellSize, cellSize));
				boardArray[i][j].field.setPosition(i * cellSize + baseWidth + outlineWidth, j * cellSize + baseWidth + outlineWidth);
				if ((i + j) % 2 == 0)
					boardArray[i][j].field.setFillColor(whiteColor);
				else
					boardArray[i][j].field.setFillColor(blackColor);
			}
		}

		//Загрузка звука
		if (buffer.loadFromFile("wav\\thud.wav")) {
			sound.setBuffer(buffer);
		}
	}

	//выбор позиции
	void setPosition(float x, float y) {
		base.setPosition(x, y);
		outline.setPosition(x + baseWidth, y + baseWidth);
		for (size_t i = 0; i < NUMBER_OF_CELLS; i++) {
			alignSymbol(i);
			for (size_t j = 0; j < NUMBER_OF_CELLS; j++) {
				boardArray[i][j].field.setPosition(i * cellSize + x + baseWidth + outlineWidth, j * cellSize + y + baseWidth + outlineWidth);
				centerPiece(i, j);
			}
		}
	}

	//получить позицию доски
	Vector2f getPosition() {
		return base.getPosition();
	}

	//получить размеры доски
	Vector2f getSize() {
		return base.getSize();
	}

	//разместить доску по центру
	void center(const int DIMENSION_SCREEN_X, const int DIMENSION_SCREEN_Y) {
		setPosition((DIMENSION_SCREEN_X - base.getSize().x) / 2.0, (DIMENSION_SCREEN_Y - base.getSize().y) / 2.0);
	}

	//изменить размер доски (на коэффициент)
	void resize(const float COEFFICIENT) {
		cellSize *= COEFFICIENT;
		baseWidth *= COEFFICIENT;
		outlineWidth *= COEFFICIENT;
		resizeHistoryPieces(COEFFICIENT);
		updateSize(COEFFICIENT);
	}

	//вернуть изначальный размер доски
	void defaultSize() {
		cellSize = DEFAULT_CELL_SIZE;
		baseWidth = DEFAULT_BASE_SIZE;
		outlineWidth = DEFAULT_OUTLINE_SIZE;
		defaultSizeHistoryPieces();
		updateSize();
	}

	//Взаимодействие c курсором
	bool hoverInteraction(Vector2f mousePos) {
		bool isInteraction = false;
		for (size_t i = 0; i < NUMBER_OF_CELLS; i++) {
			for (size_t j = 0; j < NUMBER_OF_CELLS; j++) {
				if (boardArray[i][j].field.getGlobalBounds().contains(mousePos) and !isInteraction) {
					previousMousePos = currentMousePos;
					currentMousePos = Vector2i(i, j);
					hover(i, j);
					isInteraction = true;
				}
				else {
					auto coord = isInteraction ? previousMousePos : currentMousePos;
					if (coord.x == i and coord.y == j) {
						unhover(i, j);
					}
				}
			}
		}
		return isInteraction;
	}

	//Взаимодействие с курсором только если есть фигура
	bool speciallyHoverInteraction(Vector2f mousePos) {
		bool isInteraction = false;
		for (size_t i = 0; i < NUMBER_OF_CELLS; i++) {
			for (size_t j = 0; j < NUMBER_OF_CELLS; j++) {
				if (boardArray[i][j].field.getGlobalBounds().contains(mousePos) && !isInteraction && boardArray[i][j].piece.isSetup()) {
					previousMousePos = currentMousePos;
					currentMousePos = Vector2i(i, j);
					hover(i, j);
					isInteraction = true;
				}
				else {
					auto coord = isInteraction ? previousMousePos : currentMousePos;
					if (coord.x == i && coord.y == j) {
						unhover(i, j);
					}
				}
			}
		}
		return isInteraction;
	}

	//Положить в клетку фигуру
	bool placePiece(Vector2f mousePos, Piece piece) {
		for (size_t i = 0; i < NUMBER_OF_CELLS; i++) {
			for (size_t j = 0; j < NUMBER_OF_CELLS; j++) {
				if (boardArray[i][j].field.getGlobalBounds().contains(mousePos)) {
					Vector2f cellPos = boardArray[i][j].field.getPosition();
					piece.setPosition(cellPos.x + (cellSize - piece.getSize().width) / 2, cellPos.y + (cellSize - piece.getSize().height) / 2);

					if (isMove) {
						isMove = false;
						if (history[currentStep].previousPos != Vector2i(i, j)) {
							history[currentStep].currentPos = Vector2i(i, j);

							std::cout << "Переставлена\n";
							for (size_t i = 0; i < history.size(); i++) {
								std::cout << i << ". (" << history[i].previousPos.x << ", " << history[i].previousPos.y << ") ->";
								std::cout << " (" << history[i].currentPos.x << ", " << history[i].currentPos.y << ")\n";
								std::cout << "newPiece: " << history[i].newPiece.getRole() << "\t";
								std::cout << "prePiece: " << history[i].previousPieceInCurrentPos.getRole() << "\n";
							}
							std::cout << "history size: " << history.size() << std::endl;
							std::cout << "currentStep: " << currentStep << "\n\n";
						}
						else {
							history.pop_back();
							boardArray[i][j].piece = piece;
							unhover(i, j);
							return false;
						}
					}
					else {
						//Очищаем историю
						if (currentStep < history.size()) {
							//std::cout << "Очищаем историю\n";
							history.erase(history.begin() + currentStep, history.end());
						}

						/*
						std::cout << std::endl;
						for (size_t i = 0; i < history.size(); i++) {
							std::cout << i << ". prePos: (" << history[i].previousPos.x << ", " << history[i].previousPos.y << ")\t";
							std::cout << ". curPos: (" << history[i].currentPos.x << ", " << history[i].currentPos.y << ")\n";
						}
						std::cout << "history size: " << history.size() << std::endl;*/

						Step newStep;
						newStep.currentPos = Vector2i(i, j);
						newStep.newPiece = piece;
						history.push_back(newStep);
						
						std::cout << "Поставлена\n";

						for (size_t i = 0; i < history.size(); i++) {
							std::cout << i << ". (" << history[i].previousPos.x << ", " << history[i].previousPos.y << ") ->";
							std::cout << " (" << history[i].currentPos.x << ", " << history[i].currentPos.y << ")\n";
							std::cout << "newPiece: " << history[i].newPiece.getRole() << "\t";
							std::cout << "prePiece: " << history[i].previousPieceInCurrentPos.getRole() << "\n";
						}
						std::cout << "history size: " << history.size() << std::endl;
						std::cout << "currentStep: " << currentStep << "\n\n";
					}
					history[currentStep].previousPieceInCurrentPos = boardArray[i][j].piece;
					boardArray[i][j].piece = piece;
					currentStep++;
					//std::cout << "currentStep: " << currentStep << "\n";

					if (isSound) sound.play();
					
					unhover(i, j);
					return true;
				}
			}
		}
		if (isMove) {
			std::cout << "Выкинута\n";
			isMove = false;
			history[currentStep].newPiece = piece;
			currentStep++;
			
			for (size_t i = 0; i < history.size(); i++) {
				std::cout << i << ". (" << history[i].previousPos.x << ", " << history[i].previousPos.y << ") ->";
				std::cout << " (" << history[i].currentPos.x << ", " << history[i].currentPos.y << ")\n";
				std::cout << "newPiece: " << history[i].newPiece.getRole() << "\t";
				std::cout << "prePiece: " << history[i].previousPieceInCurrentPos.getRole() << "\n";
			}
			std::cout << "history size: " << history.size() << std::endl;
			std::cout << "currentStep: " << currentStep << "\n\n";
			return true;
		}
		return false;
	}

	//Взять фигуру
	Piece takePiece(Vector2f mousePos) {
		for (size_t i = 0; i < NUMBER_OF_CELLS; i++) {
			for (size_t j = 0; j < NUMBER_OF_CELLS; j++) {
				if (boardArray[i][j].field.getGlobalBounds().contains(mousePos)) {
					Piece piece = boardArray[i][j].piece;
					boardArray[i][j].piece = Piece();
					isMove = true;

					//Очищаем историю
					if (currentStep < history.size()) {
						//std::cout << "Очищаем историю\n";
						history.erase(history.begin() + currentStep, history.end());
					}

					/*
					std::cout << std::endl;
					for (size_t i = 0; i < history.size(); i++) {
						std::cout << i << ". prePos: (" << history[i].previousPos.x << ", " << history[i].previousPos.y << ")\t";
						std::cout << ". curPos: (" << history[i].currentPos.x << ", " << history[i].currentPos.y << ")\n";
					}
					std::cout << "history size: " << history.size() << std::endl;
					std::cout << "currentStep: " << currentStep << std::endl;*/

					Step newStep;
					newStep.previousPos = Vector2i(i, j);
					history.push_back(newStep);
					/*std::cout << "Cнята с доски (последний шаг неполный)" << std::endl;

					std::cout << std::endl;
					for (size_t i = 0; i < history.size(); i++) {
						std::cout << i << ". prePos: (" << history[i].previousPos.x << ", " << history[i].previousPos.y << ")\t";
						std::cout << ". curPos: (" << history[i].currentPos.x << ", " << history[i].currentPos.y << ")\n";
					}
					std::cout << "history size: " << history.size() << std::endl;
					std::cout << "currentStep: " << currentStep << std::endl;*/

					return piece; //фигура снята с доски
				}
			}
		}
		return Piece();
	}

	//Очистить доску
	void clear() {
		for (size_t i = 0; i < NUMBER_OF_CELLS; i++) {
			for (size_t j = 0; j < NUMBER_OF_CELLS; j++) {
				boardArray[i][j].piece = Piece();
			}
		}
		history = {};
		currentStep = 0;
	}

	//Задать начальную позицию
	void setup(Templates whiteTemplates, Templates blackTemplates) {
		clear();
		if (isStandartPosition) {
			boardArray[0][0].piece = blackTemplates.getRolePiece("Rook");
			boardArray[1][0].piece = blackTemplates.getRolePiece("Knight");
			boardArray[2][0].piece = blackTemplates.getRolePiece("Bishop");
			boardArray[3][0].piece = blackTemplates.getRolePiece("Queen");
			boardArray[4][0].piece = blackTemplates.getRolePiece("King");
			boardArray[5][0].piece = blackTemplates.getRolePiece("Bishop");
			boardArray[6][0].piece = blackTemplates.getRolePiece("Knight");
			boardArray[7][0].piece = blackTemplates.getRolePiece("Rook");

			boardArray[0][7].piece = whiteTemplates.getRolePiece("Rook");
			boardArray[1][7].piece = whiteTemplates.getRolePiece("Knight");
			boardArray[2][7].piece = whiteTemplates.getRolePiece("Bishop");
			boardArray[3][7].piece = whiteTemplates.getRolePiece("Queen");
			boardArray[4][7].piece = whiteTemplates.getRolePiece("King");
			boardArray[5][7].piece = whiteTemplates.getRolePiece("Bishop");
			boardArray[6][7].piece = whiteTemplates.getRolePiece("Knight");
			boardArray[7][7].piece = whiteTemplates.getRolePiece("Rook");
		}
		else {
			boardArray[0][0].piece = whiteTemplates.getRolePiece("Rook");
			boardArray[1][0].piece = whiteTemplates.getRolePiece("Knight");
			boardArray[2][0].piece = whiteTemplates.getRolePiece("Bishop");
			boardArray[3][0].piece = whiteTemplates.getRolePiece("King");
			boardArray[4][0].piece = whiteTemplates.getRolePiece("Queen");
			boardArray[5][0].piece = whiteTemplates.getRolePiece("Bishop");
			boardArray[6][0].piece = whiteTemplates.getRolePiece("Knight");
			boardArray[7][0].piece = whiteTemplates.getRolePiece("Rook");

			boardArray[0][7].piece = blackTemplates.getRolePiece("Rook");
			boardArray[1][7].piece = blackTemplates.getRolePiece("Knight");
			boardArray[2][7].piece = blackTemplates.getRolePiece("Bishop");
			boardArray[3][7].piece = blackTemplates.getRolePiece("King");
			boardArray[4][7].piece = blackTemplates.getRolePiece("Queen");
			boardArray[5][7].piece = blackTemplates.getRolePiece("Bishop");
			boardArray[6][7].piece = blackTemplates.getRolePiece("Knight");
			boardArray[7][7].piece = blackTemplates.getRolePiece("Rook");
		}

		for (size_t i = 0; i < NUMBER_OF_CELLS; i++) {
			for (size_t j = 0; j < NUMBER_OF_CELLS; j++) {
				if (j == 2 || j == 3 || j == 4 || j == 5) continue;
				if (isStandartPosition) {
					if (j == 1) boardArray[i][j].piece = blackTemplates.getRolePiece("Pawn");
					if (j == 6) boardArray[i][j].piece = whiteTemplates.getRolePiece("Pawn");
				}
				else {
					if (j == 1) boardArray[i][j].piece = whiteTemplates.getRolePiece("Pawn");
					if (j == 6) boardArray[i][j].piece = blackTemplates.getRolePiece("Pawn");
				}
				centerPiece(i, j);
			}
		}
	}

	//Вернуть предыщий ход
	bool undoStep() {
		if (history.size() > 0 && currentStep > 0) {
			std::cout << "Шаг назад\n";
			currentStep--;
			auto prePos = history[currentStep].previousPos;
			auto curPos = history[currentStep].currentPos;

			//Если поставили или переставили
			if (curPos != Vector2i(-1, -1)) {
				if (prePos != Vector2i(-1, -1)) {
					boardArray[prePos.x][prePos.y].piece = boardArray[curPos.x][curPos.y].piece;
					centerPiece(prePos.x, prePos.y);
				}
				boardArray[curPos.x][curPos.y].piece = history[currentStep].previousPieceInCurrentPos;
				centerPiece(curPos.x, curPos.y);
				if (isSound) sound.play();
			}
			//Если выкинули фигуру
			else if (prePos != Vector2i(-1, -1)) {
				boardArray[prePos.x][prePos.y].piece = history[currentStep].newPiece;
				centerPiece(prePos.x, prePos.y);
			}

			for (size_t i = 0; i < history.size(); i++) {
				std::cout << i << ". (" << history[i].previousPos.x << ", " << history[i].previousPos.y << ") ->";
				std::cout << " (" << history[i].currentPos.x << ", " << history[i].currentPos.y << ")\n";
				std::cout << "newPiece: " << history[i].newPiece.getRole() << "\t";
				std::cout << "prePiece: " << history[i].previousPieceInCurrentPos.getRole() << "\n";
			}
			std::cout << "history size: " << history.size() << std::endl;
			std::cout << "currentStep: " << currentStep << "\n\n";

			return canUndo();
		}
		else {
			//std::cout << "У вас нет истории.\n";
			return false;
		}
		return false;
	}

	//Повторить ход
	bool redoStep() {
		if (history.size() > 0 && currentStep < history.size()) {
			std::cout << "Шаг вперёд\n";
			auto prePos = history[currentStep].previousPos;
			auto curPos = history[currentStep].currentPos;

			//Если взяли фигуру из шаблонов
			if (prePos == Vector2i(-1, -1) && curPos != Vector2i(-1, -1)) {
				boardArray[curPos.x][curPos.y].piece = history[currentStep].newPiece;
				centerPiece(curPos.x, curPos.y);
				if (isSound) sound.play();
			}
			//Если выкинули фигуру
			else if (curPos == Vector2i(-1, -1) && prePos != Vector2i(-1, -1)) {
				boardArray[prePos.x][prePos.y].piece = Piece();
			}
			//Если переставили фигуру
			else if (curPos != Vector2i(-1, -1) && prePos != Vector2i(-1, -1)) {
				boardArray[curPos.x][curPos.y].piece = boardArray[prePos.x][prePos.y].piece;
				centerPiece(curPos.x, curPos.y);
				boardArray[prePos.x][prePos.y].piece = Piece();
				if (isSound) sound.play();
			}

			currentStep++;

			for (size_t i = 0; i < history.size(); i++) {
				std::cout << i << ". (" << history[i].previousPos.x << ", " << history[i].previousPos.y << ") ->";
				std::cout << " (" << history[i].currentPos.x << ", " << history[i].currentPos.y << ")\n";
				std::cout << "newPiece: " << history[i].newPiece.getRole() << "\t";
				std::cout << "prePiece: " << history[i].previousPieceInCurrentPos.getRole() << "\n";
			}
			std::cout << "history size: " << history.size() << std::endl;
			std::cout << "currentStep: " << currentStep << "\n\n";

			return canRedo();
		}
		else {
			//std::cout << "У вас нет истории.\n";
			return false;
		}
	}

	bool canUndo() {
		if (currentStep == 0) {
			return false; //больше нельзя сделать шаг назад
		}
		else {
			return true; //можно сделать шаг назад
		}
	}

	bool canRedo() {
		if (currentStep < history.size()) {
			return true; //можно сделать шаг вперёд
		}
		else {
			return false; //нельзя сделать шаг вперёд
		}
	}

	//Повернуть доску
	void replacePosition() {
		isStandartPosition = !isStandartPosition;
		Piece** boardCopy{ new Piece * [NUMBER_OF_CELLS] {} };

		//Заполним перевёрнутый "снимок" доски
		for (size_t i = 0; i < NUMBER_OF_CELLS; i++) {
			boardCopy[NUMBER_OF_CELLS - i - 1] = new Piece[NUMBER_OF_CELLS];
			for (size_t j = 0; j < NUMBER_OF_CELLS; j++) {
				boardCopy[NUMBER_OF_CELLS - i - 1][NUMBER_OF_CELLS - j - 1] = boardArray[i][j].piece;
			}
		}

		//Присваиваем доске снимок и меняем метки
		for (size_t i = 0; i < NUMBER_OF_CELLS; i++) {
			//Меняем метки
			if (isStandartPosition) {
				numbersColumn[i].setString(std::to_string(NUMBER_OF_CELLS - i));
				letterRow[i].setString((char)((int)'A' + i));
				numbersColumn2[i].setString(std::to_string(NUMBER_OF_CELLS - i));
				letterRow2[i].setString((char)((int)'A' + i));
			}
			else {
				numbersColumn[i].setString(std::to_string(i + 1));
				letterRow[i].setString((char)((int)'H' - i));
				numbersColumn2[i].setString(std::to_string(i + 1));
				letterRow2[i].setString((char)((int)'H' - i));
			}

			//Присваиваем доске перевёрнутый снимок
			for (size_t j = 0; j < NUMBER_OF_CELLS; j++) {
				boardArray[i][j].piece = boardCopy[i][j];
				centerPiece(i, j);
			}
		}

		//Разворачиваем позиции в истории
		for (size_t i = 0; i < history.size(); i++) {
			if (history[i].previousPos != Vector2i(-1, -1)) {
				history[i].previousPos = Vector2i(NUMBER_OF_CELLS - 1 - history[i].previousPos.x, NUMBER_OF_CELLS - 1 - history[i].previousPos.y);
			}
			if (history[i].currentPos != Vector2i(-1, -1)) {
				history[i].currentPos = Vector2i(NUMBER_OF_CELLS - 1 - history[i].currentPos.x, NUMBER_OF_CELLS - 1 - history[i].currentPos.y);
			}
		}
	}

	//Получить шахматную позицию для сохранения
	json getChessPosition() {
		json data;

		//Сохраняем положение доски
		data["is_standart_position_board"] = isStandartPosition;

		//Сохраняем фигуры
		for (size_t i = 0; i < NUMBER_OF_CELLS; i++) {
			for (size_t j = 0; j < NUMBER_OF_CELLS; j++) {
				data["pieces"][i][j]["color"] = boardArray[i][j].piece.getColor();
				data["pieces"][i][j]["role"] = boardArray[i][j].piece.getRole();
			}
		}

		//Сохранение истории
		data["history"]["array"] = json::array();
		json step;
		for (Step& entry : history) {
			step = json::object();
			step["previousPos"]["x"] = entry.previousPos.x;
			step["previousPos"]["y"] = entry.previousPos.y;
			step["currentPos"]["x"] = entry.currentPos.x;
			step["currentPos"]["y"] = entry.currentPos.y;
			step["previousPieceInCurrentPos"]["color"] = entry.previousPieceInCurrentPos.getColor();
			step["previousPieceInCurrentPos"]["role"] = entry.previousPieceInCurrentPos.getRole();
			step["newPiece"]["color"] = entry.newPiece.getColor();
			step["newPiece"]["role"] = entry.newPiece.getRole();
			data["history"]["array"].push_back(step);
		}
		data["history"]["currentStep"] = currentStep;
		return data;
	}

	//Задать шахматную позицию
	bool setChessPosition(json data, Templates whiteTemplates, Templates blackTemplates) {
		if (!data["is_standart_position_board"].empty() && !data["pieces"].empty()) {

			//Задаём позицию доски
			bool replacePos = false;
			if (data["is_standart_position_board"] != isStandartPosition) {
				replacePos = true;
			}

			//Расставляем фигуры
			for (size_t i = 0; i < NUMBER_OF_CELLS; i++) {
				for (size_t j = 0; j < NUMBER_OF_CELLS; j++) {
					if (!data["pieces"][i].empty() && !data["pieces"][i][j].empty() && !data["pieces"][i][j]["color"].empty()) {
						Templates source = (data["pieces"][i][j]["color"].get<std::string>() == "White" ? whiteTemplates : blackTemplates);
						if (replacePos) {
							boardArray[NUMBER_OF_CELLS - 1 - i][NUMBER_OF_CELLS - 1 - j].piece = source.getRolePiece(data["pieces"][i][j]["role"].get<std::string>());
							centerPiece(NUMBER_OF_CELLS - 1 - i, NUMBER_OF_CELLS - 1 - j);
						}
						else {
							boardArray[i][j].piece = source.getRolePiece(data["pieces"][i][j]["role"].get<std::string>());
							centerPiece(i, j);
						}
					}
				}
			}

			//Загружаем историю
			history = {};
			if (!data["history"].empty() && !data["history"]["currentStep"].empty() && !data["history"]["array"].empty()) {
				currentStep = data["history"]["currentStep"];
				for (size_t i = 0; i < data["history"]["array"].size(); i++) {
					Step step;
					step.previousPos = Vector2i(data["history"]["array"][i]["previousPos"]["x"], data["history"]["array"][i]["previousPos"]["y"]);
					step.currentPos = Vector2i(data["history"]["array"][i]["currentPos"]["x"], data["history"]["array"][i]["currentPos"]["y"]);
					Templates source1 = (data["history"]["array"][i]["previousPieceInCurrentPos"]["color"].get<std::string>() == "White" ? whiteTemplates : blackTemplates);
					Templates source2 = (data["history"]["array"][i]["newPiece"]["color"].get<std::string>() == "White" ? whiteTemplates : blackTemplates);
					step.previousPieceInCurrentPos = source1.getRolePiece(data["history"]["array"][i]["previousPieceInCurrentPos"]["role"].get<std::string>());
					step.newPiece = source2.getRolePiece(data["history"]["array"][i]["newPiece"]["role"].get<std::string>());
					history.push_back(step);
				}
			}
			else {
				currentStep = 0;
			}
			return true;
		}
		return false;
	}

	//Включить или выключить звук
	void switchSound() {
		isSound = !isSound;
	}

	//Выбрать режим звука
	void setSoundMode(bool active) {
		isSound = active;
	}
};