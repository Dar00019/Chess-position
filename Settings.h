#pragma once
#include "TextBlock.h"

typedef unsigned int uint;

class Setting : public Drawable {
	bool on = true;

	virtual void draw(RenderTarget& target, RenderStates states) const {
		target.draw(textBlock);
		target.draw(checkbox);
	}

public:
	TextBlock textBlock;
	Button checkbox;

    Setting() {
		checkbox.create("png\\50\\marker_checkbox.png");
    }

	void setMode(bool active) {
		on = active;
		if (on) {
			checkbox.create("png\\50\\marker_checkbox.png");
		}
		else {
			checkbox.create("png\\50\\empty_checkbox.png");
		}
	}

	void switchSetting() {
		setMode(!on);
	}

	bool getActive() {
		return on;
	}

	void alignCheckbox(float maxWidth) {
		Vector2f textParam = textBlock.text.getPosition();
		checkbox.setPosition(textParam.x + maxWidth + 50, textParam.y - 10);
	}

	void setPosition(float x, float y, float maxWidth) {
		textBlock.text.setPosition(x, y);
		alignCheckbox(maxWidth);
	}
};


class Settings : public Drawable {
	std::vector<Setting*> settings;
	uint lineBreak = 30;
	float offsetY = 50;
	uint characterSize = 20;
	float maxWidth = 0;

	virtual void draw(RenderTarget& target, RenderStates states) const {
		for (Setting* setting : settings) {
			target.draw(*setting);
		}
	}
public:

	Settings() {}

	void push_back(Setting* setting) {
		settings.push_back(setting);
		setting->textBlock.text.setCharacterSize(characterSize);
		setting->textBlock.setLineBreak(lineBreak);
		if (maxWidth < setting->textBlock.text.getGlobalBounds().width) {
			maxWidth = setting->textBlock.text.getGlobalBounds().width;
		}
	}

	void push_back(std::vector<Setting*> settings) {
		for (Setting*& setting : settings) {
			push_back(setting);
		}
		for (Setting*& setting : settings) {
			setting->alignCheckbox(maxWidth);
		}
	}

	void setPosition(float x, float y) {
		for (size_t i = 0; i < settings.size(); i++) {
			if (i == 0) {
				settings[i]->setPosition(x, y, maxWidth);
			}
			else {
				Text preText = settings[i - 1]->textBlock.text;
				settings[i]->setPosition(x, preText.getPosition().y + preText.getGlobalBounds().height + offsetY, maxWidth);
			}
		}
	}

	void hoverInteraction(Vector2f mousePos) {
		for (size_t i = 0; i < settings.size(); i++) {
			settings[i]->checkbox.hoverInteraction(mousePos);
		}
	}

	std::pair<Setting*, bool> actInteraction(Vector2f mousePos) {
		for (Setting*& setting : settings) {
			auto resultAct = setting->checkbox.actInteraction(mousePos);
			if (resultAct.first) {
				return std::make_pair(setting, resultAct.second);
			}
		}
		return std::make_pair(nullptr, false);
	}
};