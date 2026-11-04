#pragma once
#include "Board.h"
#include "FileHandler.h"
#include "Settings.h"

class Engine {
	RenderWindow window;							//окно
	const int DIMENSION_SCREEN_X = 1200;			//ширина экрана
	const int DIMENSION_SCREEN_Y = 800;				//высота экрана
	
	const String PROGRAM_NAME = L"Шахматная позиция";			//основное название программы
	const String DEFAULT_NAME_FILE = L" (Неизвестный файл)";	//название открытого файла
	
	bool windowMode = false;	//свёрнутое или развёрнутое окно

	RECT workArea;				//рабочая область
	float resizeCoefficient;	//насколько увеличиваются элементы окна при изменении его размеров

	FileHandler fileHandler;	// для работы с проводником

	//Структура касаний
	struct PointerState {
		bool active = false;	// есть ли текущее нажатие/касание
		int id = -1;			// для touch: индекс пальца; для мыши: -1
		Vector2f pos;			// позиция в координатах мира (mapPixelToCoords)
	};

	//Экраны
	enum class Screen {
		Settings,				// настройки
		Game					// игра
	};

	Screen currentScreen = Screen::Game;	//начинаем в игре

	Board board;				//шахматная доска
	Templates whiteTemplates;	//шаблоны белых фигур
	Templates blackTemplates;	//шаблоны чёрных фигур
	Image icon;					//иконка приложения

	Settings settings;
	Setting sound;				//кнопка для включения и выключения звука
	Setting autoScaleWindow;	//автоматическое расширение элементов под размер экрана в развёрнутом режиме

	Buttons buttons;			//основные кнопки
	Button settingsButton;		//кнопка настроек
	Button saveAs;				//кнопка "сохранить как"
	Button save;				//кнопка "сохранить"
	Button load;				//кнопка "загрузить из файла"
	Button clearBoard;			//кнопка для очистки доски
	Button startingPosition;	//кнопка для установления стандартной позиции
	Button replace;				//кнопка для разаорота доски

	Buttons actionButtons;		//кнопки управления историей
	Button undo;				//кнопка "вернуть"
	Button redo;				//кнопка "повторить"

	Button back;				//кнопка для возвращения из настроек

	PointerState pointer;		//указатель (мышь или палец для сенсорного экрана)
	bool isMove = false;		//отвечает за перетаскивание
	Vector2f dragOffset;		//корректировка нажатия
	Piece pieceMove;			//движущаяся фигура

	TextBlock hint;				//текст-подсказка
	json saveSettings;			//сохранение насторек

public:

	// --- ИНИЦИАЛИЗАЦИЯ ---
	Engine() {
		//Установка языка
		setlocale(LC_ALL, "Russian");

		//Настройка окна
		window.create(VideoMode(DIMENSION_SCREEN_X, DIMENSION_SCREEN_Y), PROGRAM_NAME + DEFAULT_NAME_FILE);
		window.setFramerateLimit(60);   //количество кадров в секунду

		//Установка иконки приложения
		if (icon.loadFromFile("png\\Chess\\White\\k.png")) {
			window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
		}

		//Получаем параметры рабочей области (с учётом панели задач)
		SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

		//Шрифт
		fontUsual.loadFromFile("fonts\\Monospace.ttf");

		//Центрируем доску
		board.center(DIMENSION_SCREEN_X, DIMENSION_SCREEN_Y);

		//Настраиваем подсказку
		hint.setLineBreak(100);
		hint.setString(L"Нажмите ESC для выхода из полноэкранного режима");
		hint.text.setStyle(Text::Style::Italic);
		hint.text.setFillColor(Color(200, 200, 200));
		hint.text.setPosition(15, 15);

		//Создаём шаблоны фигур
		whiteTemplates.create("White");
		blackTemplates.create("Black");

		Vector2f boardPos = board.getPosition();
		Vector2f boardSize = board.getSize();

		whiteTemplates.centerRelativeToTheBoard(boardPos, boardSize);
		blackTemplates.centerRelativeToTheBoard(boardPos, boardSize);


		//Открываем файл с настройками
		std::ifstream s("save.json");
		if (s.is_open()) {
			saveSettings = json::parse(s);
			s.close();
		}

		//Настройки
		sound.textBlock.setString(L"Звук");
		autoScaleWindow.textBlock.setString(L"Автоматически увеличивать элементы окна в развёрнутом режиме");

		if (!saveSettings.empty()) {
			if (!saveSettings["sound"].empty()) {
				sound.setMode(saveSettings["sound"]);
				board.setSoundMode(saveSettings["sound"]);
			}

			if (!saveSettings["autoscale"].empty()) {
				autoScaleWindow.setMode(saveSettings["autoscale"]);
			}
		}

		settings.push_back({ {&sound}, {&autoScaleWindow} });
		settings.setPosition(150, 100);


		//Задаём картинки для кнопок
		back.create("png\\50\\back.png");
		settingsButton.create("png\\50\\settings.png");
		saveAs.create("png\\50\\saveAs.png");
		save.create("png\\50\\save.png");
		load.create("png\\50\\load.png");
		clearBoard.create("png\\50\\clear.png");
		startingPosition.create("png\\50\\pawn.png");
		undo.create("png\\50\\undo.png");
		redo.create("png\\50\\redo.png");
		replace.create("png\\50\\replace.png");

		//Называем кнопки, чтобы не путать их
		sound.checkbox.operationName = "Sound";
		autoScaleWindow.checkbox.operationName = "Auto scale";
		back.operationName = "Back";
		settingsButton.operationName = "Settings";
		saveAs.operationName = "Save as";
		save.operationName = "Save";
		load.operationName = "Load";
		clearBoard.operationName = "Clear board";
		startingPosition.operationName = "Starting position";
		replace.operationName = "Replace";
		undo.operationName = "Undo";
		redo.operationName = "Redo";

		//Блокируем кнопки "назад", "вперёд" и "сохранить"
		undo.block();
		redo.block();
		save.block();


		// --- ПРИВЯЗКА ФУНКЦИЙ К КНОПКАМ ---

		//Возвращение в игру
		back.actEvent = [&]() {
			currentScreen = Screen::Game;
			return true;
			};

		//Открыть настройки
		settingsButton.actEvent = [&]() {
			currentScreen = Screen::Settings;
			return true;
			};

		//Сохранить как
		saveAs.actEvent = [&]() {
			fileHandler.saveAsFile(board.getChessPosition());
			window.setTitle(PROGRAM_NAME + " (" + fileHandler.currentFilePath + ")");
			save.block();
			return true;
			};

		//Сохранить
		save.actEvent = [&]() {
			fileHandler.saveFile(board.getChessPosition());
			window.setTitle(PROGRAM_NAME + " (" + fileHandler.currentFilePath + ")");
			save.block();
			return true;
			};

		//Загрузить
		load.actEvent = [&]() {
			json fileLoad = fileHandler.loadFile();
			if (!fileLoad.empty()) {
				if (board.setChessPosition(fileLoad, whiteTemplates, blackTemplates)) {
					save.block();
					window.setTitle(PROGRAM_NAME + " (" + fileHandler.currentFilePath + ")");
					if (board.canUndo()) {
						undo.unblock();
					}
					else {
						undo.block();
					}
					if (board.canRedo()) {
						redo.unblock();
					}
					else {
						redo.block();
					}
					return true;
				}
			}
			return false;
			};

		//Очистить доску
		clearBoard.actEvent = [&]() {
			board.clear();
			undo.block();
			redo.block();
			return true;
			};

		//Задать начальную позицию
		startingPosition.actEvent = [&]() {
			board.setup(whiteTemplates, blackTemplates);
			undo.block();
			redo.block();
			return true;
			};

		//Повернуть доску
		replace.actEvent = [&]() {
			board.replacePosition();
			return true;
			};

		//Шаг назад
		undo.actEvent = [&]() {
			return board.undoStep();
			};

		//Шаг вперёд
		redo.actEvent = [&]() {
			return board.redoStep();
			};

		//Включить и выключить звук
		sound.checkbox.actEvent = [&]() {
			board.switchSound();
			sound.switchSetting();
			return true;
			};

		//Автоматическое увеличение элементов окна
		autoScaleWindow.checkbox.actEvent = [&]() {
			autoScaleWindow.switchSetting();
			return true;
			};


		// --- НАСТРОЙКА ПАНЕЛЕЙ КНОПОК ---

		//Добавляем все кнопки в верхнюю панель
		buttons.pushBack({ { &settingsButton }, { &saveAs }, { &save }, { &load }, { &clearBoard }, { &startingPosition }, { &replace } });
		buttons.centerRelativeToTheBoard(boardPos, boardSize);

		//Добавляем кнопки истории в нижнюю панель
		actionButtons.pushBack({ { &undo }, { &redo } });
		actionButtons.centerRelativeToTheBoard(boardPos, boardSize, false);

		//Кнопка возвращения в игру из настроек
		back.setPosition(50, 100);

		//Коэффициент масштабирования
		float boardSizeInResize = 0.9 * workArea.bottom - (10 + buttons.buttonSize().height) * 2;
		resizeCoefficient = boardSizeInResize / board.getSize().y;
	}


	// --- ЗАПУСК ОСНОВНОГО ПРОЦЕССА ---
	void start() {

		// --- ОСНОВНОЙ ЦИКЛ SFML ---
		while (window.isOpen()) {

			//Забираем координаты курсора
			Vector2i pixelPos = Mouse::getPosition(window);
			//Переводим их в игровые (уходим от координат окна)
			pointer.pos = window.mapPixelToCoords(pixelPos);

			Event event;

			// --- СОБЫТИЯ ---
			while (window.pollEvent(event)) {

				//Закрытие окна
				if (event.type == Event::Closed) {
					std::ofstream s("save.json");
					saveSettings["sound"] = sound.getActive();
					saveSettings["autoscale"] = autoScaleWindow.getActive();
					s << saveSettings.dump(2);
					s.close();
					window.close();
				}

				//Изменение размеров окна
				if (event.type == Event::Resized) {
					resizeWindow();
				}


				// --- ОБРАБОТКА КЛАВИШ КЛАВИАТУРЫ ---
				if (event.type == Event::KeyPressed) {

					// нажатие на Escape сворачивает окно
					if (event.key.code == Keyboard::Escape) {
						if (windowMode) resizeWindow();
					}
				}


				// --- ОБРАБОТКА КАСАНИЯ (для сенсорного экрана) ---
				//если касание совершено
				if (event.type == Event::TouchBegan) {
					//Захватываем только первый палец
					pointer.active = true;
					pointer.id = event.touch.finger;

					//Забираем координаты нажатия
					Vector2i pixelPos(event.touch.x, event.touch.y);
					//Переводим их в "игровые"
					pointer.pos = window.mapPixelToCoords(pixelPos);

					//Если окно могло потерять фокус (на Windows), запрашиваем фокус
					window.requestFocus();

					//Выполняем действие при касании
					executeOnTouchOrClick();
				}

				//если палец перестал касаться
				if (event.type == Event::TouchEnded) {
					if (pointer.active && pointer.id == event.touch.finger) {
						pointer.active = false;
						pointer.id = -1;
						
						//Выполняем действие при отпускании
						executeOnMouseOrTouchRelease();
					}
				}


				// --- ОБРАБОТКА МЫШИ (для десктопа) ---
				//если нажата клавиша мыши
				if (event.type == Event::MouseButtonPressed) {

					//Игнорируем мышь, если сейчас активен touch, чтобы не обрабатывать
					//синтетичекие mouse-события
					if (!pointer.active) {
						pointer.active = true;
						pointer.id = -1; //мышь
						
						//Забираем координаты курсора
						Vector2i pixelPos = Mouse::getPosition(window);
						//Переводим их в "игровые"
						pointer.pos = window.mapPixelToCoords(pixelPos);

						if (event.key.code == Mouse::Left) {
							executeOnTouchOrClick();
						}
					}
				}

				//если отпустили клавишу мыши
				if (event.type == Event::MouseButtonReleased) {
					if (pointer.active && pointer.id == -1) {
						if (event.key.code == Mouse::Left) {
							pointer.active = false;
							executeOnMouseOrTouchRelease();
						}
					}
				}
			}

			// --- ОБРАБОТКА НАВЕДЕНИЯ ---
			// Если в игре
			if (currentScreen == Screen::Game) {
				//если можем двигать
				if (isMove) {
					pieceMove.setPosition(pointer.pos - dragOffset);
					board.hoverInteraction(pointer.pos);
				}
				else {
					board.speciallyHoverInteraction(pointer.pos);
					buttons.hoverInteraction(pointer.pos);
					actionButtons.hoverInteraction(pointer.pos);
					whiteTemplates.hoverInteraction(pointer.pos);
					blackTemplates.hoverInteraction(pointer.pos);
				}
			}
			//Если в настройках
			else if (currentScreen == Screen::Settings) {
				back.hoverInteraction(pointer.pos);
				settings.hoverInteraction(pointer.pos);
			}


			// --- ОТРИСОВКА ---
			window.clear(Color(50, 10, 0));
			if (currentScreen == Screen::Game) {
				window.draw(board);
				window.draw(whiteTemplates);
				window.draw(blackTemplates);
				window.draw(buttons);
				window.draw(actionButtons);
				window.draw(pieceMove);
			}
			else if (currentScreen == Screen::Settings) {
				window.draw(back);
				window.draw(settings);
			}
			if (windowMode) {
				window.draw(hint);
			}
			window.display();
		}
	}


private:
	// --- ПРИВАТНЫЕ ФУНКЦИИ ---

	//Изменение размеров окна
	void resizeWindow() {
		windowMode = !windowMode;
		
		if (windowMode) {
			window.create(VideoMode(workArea.right, workArea.bottom), "", Style::None);
			window.setPosition({ workArea.left, workArea.top });
			if (autoScaleWindow.getActive()) {
				board.resize(resizeCoefficient);
				whiteTemplates.resize(resizeCoefficient);
				blackTemplates.resize(resizeCoefficient);
			}
			board.center(workArea.right, workArea.bottom);
		}
		else {
			String name = PROGRAM_NAME + (fileHandler.currentFilePath != L"" ? " (" + fileHandler.currentFilePath + ")" : DEFAULT_NAME_FILE);
			window.create(VideoMode(DIMENSION_SCREEN_X, DIMENSION_SCREEN_Y), name);
			window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
			board.defaultSize();
			whiteTemplates.defaultSize();
			blackTemplates.defaultSize();
			board.center(DIMENSION_SCREEN_X, DIMENSION_SCREEN_Y);
		}

		Vector2f boardPos = board.getPosition();
		Vector2f boardSize = board.getSize();
		whiteTemplates.centerRelativeToTheBoard(boardPos, boardSize);
		blackTemplates.centerRelativeToTheBoard(boardPos, boardSize);
		buttons.centerRelativeToTheBoard(boardPos, boardSize);
		actionButtons.centerRelativeToTheBoard(boardPos, boardSize, false);
	}

	//Действие, выполняемое при щелчке / касании
	void executeOnTouchOrClick() {
		//Если в игре
		if (currentScreen == Screen::Game) {
			Piece piece = whiteTemplates.actInteraction(pointer.pos);
			if (!piece.isSetup()) piece = blackTemplates.actInteraction(pointer.pos);
			if (!piece.isSetup()) piece = board.takePiece(pointer.pos);
			if (piece.isSetup()) {
				//делаем разность между позицией курсора и фигурой для корректировки нажатия
				dragOffset = pointer.pos - piece.getPosition();
				pieceMove = piece;	//запоминаем фигуру
				isMove = true;		//можем двигать фигуру
			}
			else {
				auto button = buttons.actInteraction(pointer.pos);
				if (button.first != nullptr) {
					if (button.first->operationName == "Save") {
						if (button.second) {
							save.block();
						}
					}
				}

				auto actButton = actionButtons.actInteraction(pointer.pos);
				if (actButton.first != nullptr) {
					if (actButton.first->operationName == "Undo" || actButton.first->operationName == "Redo") {
						//Открываем или закрываем кнопку в зависимости от результата
						if (actButton.second) {
							actButton.first->unblock();
						}
						else {
							actButton.first->block();
						}

						//Открываем противоположную кнопку
						if (actButton.first->operationName == "Undo") {
							redo.unblock();
						}
						else {
							undo.unblock();
						}
						save.unblock();
					}
				}
			}
		}
		//Если в настройках
		else if (currentScreen == Screen::Settings) {
			back.actInteraction(pointer.pos);
			settings.actInteraction(pointer.pos);
		}
	}


	//Действие, выполняемое при отпускании мыши / касания
	void executeOnMouseOrTouchRelease() {
		if (currentScreen == Screen::Game) {
			if (isMove) {
				isMove = false;			//не можем двигать фигуру
				bool place = board.placePiece(pointer.pos, pieceMove);	//кладём фигуру на доску
				if (place) {
					undo.unblock();		//разблокируем кнопочку "назад"
					redo.block();		//блокируем кнопку "вперёд"
					save.unblock();		//можно сохранить
				}
				pieceMove = Piece();	//обнуляем фигуру
			}
		}
	}
};