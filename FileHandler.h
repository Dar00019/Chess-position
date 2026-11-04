#pragma once
#define NOMINMAX
#include <SFML/Graphics.hpp>
#include <fstream>
#include <Windows.h>
#include "json.hpp"
using json = nlohmann::json;

class FileHandler {
	std::wstring fileDialog(bool openFile) {
		OPENFILENAME ofn;				//структура для хранения параметров диалогового окна
		WCHAR filePath[MAX_PATH] = L"";	//объявляем пустой массив для хранения файла
		ZeroMemory(&ofn, sizeof(ofn));	//обнуляем структуру
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFilter = L"JSON Files (*.json)\0*.json";
		ofn.lpstrFile = filePath;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = openFile ? OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST : OFN_OVERWRITEPROMPT;
		if (GetOpenFileName(&ofn)) {
			return std::wstring(ofn.lpstrFile);
		}
		return L"";
	}

	void saveFile(const std::wstring& filePath, json save) {
		std::ofstream file(filePath.c_str());
		if (file.is_open()) {
			file.clear();
			file << save.dump(2);
			file.close();
			currentFilePath = filePath;
		}
	}

public:
	std::wstring currentFilePath; //переменная для хранения пути к текущему файлу

	json loadFile(const std::wstring& filePath) {
		std::ifstream file(filePath.c_str());
		if (file.is_open()) {
			currentFilePath = filePath;
			json data = json::parse(file);
			file.close();
			if (!data.empty() && !data["chess_position"].empty()) {
				return data["chess_position"];
			}
			else {
				//std::cout << "Data not found\n";
			}
		}
		if (file.bad()) {
			//std::cout << "Fatal error: badbit is set.\n";
		}
		else if (file.eof()) {
			//std::cout << "EOF.\n";
		}
		else if (file.fail()) {
			//std::cout << "File fail.\n";
		}
		else {
			//std::cout << "Unknown error.\n";
		}
		return json();
	}

	json loadFile() {
		WCHAR currentDir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, currentDir);
		std::wstring filePath = fileDialog(true);
		SetCurrentDirectory(currentDir);
		if (!filePath.empty()) {
			return loadFile(filePath);
		}
	}

	void saveAsFile(json save) {
		WCHAR currentDir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, currentDir);
		std::wstring filePath = fileDialog(false);
		SetCurrentDirectory(currentDir);
		if (!filePath.empty()) {
			filePath += L".json";
			json data;
			data["chess_position"] = save;
			saveFile(filePath, data);
		}
	}

	void saveFile(json save) {
		if (!currentFilePath.empty()) {
			json data;
			data["chess_position"] = save;
			saveFile(currentFilePath, data);
		}
		else {
			saveAsFile(save);
		}
	}
};