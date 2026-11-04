#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <iostream>
using namespace sf;

Font fontUsual;	//шрифт

//Класс кнопки
class Button : public Drawable {
	Texture texture;		//текстура кнопки
	Sprite sprite;			//спрайт кнопки
	bool isBlock = false;	//заблокирована ли кнопка

	//Нарисовать кнопку
	virtual void draw(RenderTarget& target, RenderStates states) const {
		target.draw(sprite);
	}

	//наведение курсора на кнопку
	void hover() {
		sprite.setColor(Color(200, 200, 200));
	}

	//сведение курсора с кнопки
	void unhover() {
		sprite.setColor(Color(255, 255, 255));
	}

public:
	//Пустой конструктор
	Button() {}

	//Конструктор с созданием
	Button(std::string path) {
		create(path);
	}

	//Действие
	std::function<bool()> actEvent = []() { return false; };

	std::string operationName; //что делает кнопка

	//Создать кнопку
	void create(std::string path) {
		if (texture.loadFromFile(path)) {
			sprite.setTexture(texture);
		}
	}

	//Задать позицию кнопке
	void setPosition(float x, float y) {
		sprite.setPosition(x, y);
	}

	//Подвинуть кнопку
	void move(float x, float y) {
		sprite.move(x, y);
	}

	//Получить позицию кнопки
	Vector2f getPosition() {
		return sprite.getPosition();
	}

	//Получить размеры кнопки
	FloatRect getSize() {
		return sprite.getGlobalBounds();
	}

	//Наведение курсора на кнопку
	bool hoverInteraction(Vector2f mousePos) {
		if (!isBlock) {
			if (sprite.getGlobalBounds().contains(mousePos)) {
				hover();
				return true;
			}
			unhover();
		}
		return false;
	}

	//Выполнить действие: возвращает два значения
	//1. Выполнено ли действие вообще
	//2. Результат действия
	std::pair<bool, bool> actInteraction(Vector2f mousePos) {
		if (!isBlock) {
			if (sprite.getGlobalBounds().contains(mousePos)) {
				return std::make_pair(true, actEvent());
			}
		}
		return std::make_pair(false, false);
	}

	//Заблокировать кнопку
	void block() {
		isBlock = true;
		sprite.setColor(Color(120, 120, 120));
	}

	//Разблокировать кнопку
	void unblock() {
		isBlock = false;
		unhover();
	}
};