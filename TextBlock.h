#pragma once
#include <string>
#include <regex>
#include "Button.h"

typedef unsigned int uint;

//Класс блока текста
class TextBlock : public Drawable {
	uint lineBreak = 30;    //ширина блока текста (в символах)
    uint countLines = 1;    //количество строк

    virtual void draw(RenderTarget& target, RenderStates states) const {
        target.draw(text);
    }
public:
	Text text;              //сам текст

    TextBlock() {
        text.setFont(fontUsual);
        text.setFillColor(Color::White);
        text.setCharacterSize(20);
    }

    //Функция для удаления переносов
    static std::wstring removeHyphenation(std::wstring originalString) {
        return std::regex_replace(originalString, std::wregex(L"\n"), L" ");
    }

    //Функция для переноса текста
    //возвращает строку с блоком текста и количество строк
    static std::pair <std::wstring, uint> lineBreaking(std::wstring originalString, uint lineBreak) {
        originalString = removeHyphenation(originalString);
        uint cntStrings = 1;
        int k = 0, j = 0;
        std::wstring newString = L"";
        while (k <= originalString.size()) {
            if (originalString[k] == L' ') {
                j = k;
            }
            if (k > lineBreak) {
                if (originalString[k] != ' ' && j != 0) {
                    k = j;
                }
                if (j == 0) {
                    originalString.insert(k, L" ");
                }
                originalString[k] = '\n';
                newString = newString + originalString.substr(0, k + 1);
                originalString.replace(0, k + 1, L"");
                k = -1;
                j = 0;
                cntStrings++;
            }
            if (k == originalString.size()) {
                newString = newString + originalString.substr(0, k);
                cntStrings++;
            }
            k++;
        }
        return std::make_pair(newString, cntStrings);
    }

    //Назначить текст
    void setString(std::wstring string) {
        auto newString = lineBreaking(string, lineBreak);
        text.setString(newString.first);
        countLines = newString.second;
    }

    //Выбрать ширину блока текста (в символах)
    void setLineBreak(uint lineBreak) {
        this->lineBreak = lineBreak;
        setString(std::wstring(text.getString()));
    }

    //Получить ширину блока текста (в символах)
    uint getLineBreak() {
        return lineBreak;
    }

    //Получить количество строк в тексте
    uint getCountLines() {
        return countLines;
    }
};