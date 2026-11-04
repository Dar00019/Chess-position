#pragma once
#include "Button.h"

//Класс кнопок
class Buttons : public Drawable {
	std::vector <Button*> buttons; //вектор кнопок

	//Нарисовать кнопки
	virtual void draw(RenderTarget& target, RenderStates states) const {
		for (Button* button : buttons) {
			target.draw(*button);
		}
	}
public:
	//Конструктор по умолчанию
	Buttons() {}

	//Добавить кнопку
	void pushBack(Button* button) {
		if (buttons.size() > 0) {
			button->setPosition((buttons.size() - 1) * button->getSize().width, buttons[0]->getPosition().y);
		}
		buttons.push_back(button);
	}

	//Добавить набор кнопок
	void pushBack(std::vector<Button*> buttons) {
		for (Button*& button : buttons) {
			pushBack(button);
		}
	}

	//Взаимодействие с курсором
	void hoverInteraction(Vector2f mousePos) {
		for (Button*& button : buttons) {
			button->hoverInteraction(mousePos);
		}
	}

	//Действия кнопок (возвращает указатель на кнопку, которая совершает действие)
	std::pair<Button*, bool> actInteraction(Vector2f mousePos) {
		for (Button*& button : buttons) {
			auto resultAct = button->actInteraction(mousePos);
			if (resultAct.first) {
				return std::make_pair(button, resultAct.second);
			}
		}
		return std::make_pair(nullptr, false);
	}

	//Выбор позиции
	void setPosition(float x, float y) {
		for (size_t i = 0; i < buttons.size(); i++) {
			buttons[i]->setPosition(x, y);
			if (i != 0) {
				buttons[i]->move(i * buttons[i]->getSize().width, 0);
			}
		}
	}

	//Получить позицию
	Vector2f getPosition() {
		if (buttons.size() > 0) {
			return buttons[0]->getPosition();
		}
		return Vector2f(0, 0);
	}

	//Получить размеры одной кнопки
	FloatRect buttonSize() {
		if (buttons.size() > 0) {
			return buttons[0]->getSize();
		}
		return FloatRect(0, 0, 0, 0);
	}

	//Выровнять кнопки относительно доски
	void centerRelativeToTheBoard(Vector2f boardPos, Vector2f boardSize, bool isUpper = true) {
		FloatRect size = buttonSize();
		setPosition(boardPos.x + (boardSize.x - size.width * buttons.size()) / 2, isUpper? boardPos.y - size.height - 10 : boardPos.y + boardSize.y + 10);
	}
};