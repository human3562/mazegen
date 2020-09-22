#include <iostream>
#include <chrono>

#include "imgui.h"
#include "imgui-SFML.h"

#include "SFML/Graphics.hpp"

#include "mazegenScene.h"

int main() {
	srand(unsigned(time(0)));
	sf::ContextSettings settings;
	settings.antialiasingLevel = 0;
	
	sf::RenderWindow window(sf::VideoMode(900, 600), "mazegen", sf::Style::Close | sf::Style::Titlebar, settings);
	//window.setFramerateLimit(5);
	float fElapsedTime = 0.1f;

	ImGui::SFML::Init(window, false);

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Clear();
	io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("font2.ttf", 16.f);
	ImGui::SFML::UpdateFontTexture();

	e_gameState currentGameState = MAZEGENERATOR;
	GameScene* scenes[1];
	MazegenScene s_mazegen;
	
	scenes[MAZEGENERATOR] = &s_mazegen;

	while(window.isOpen()){
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			if (scenes[currentGameState]->isStarted) scenes[currentGameState]->EventHandle(&window, &event);
			if (event.type == sf::Event::Resized) {
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
			}
		}

		if (!scenes[currentGameState]->isStarted)
			scenes[currentGameState]->Start(&window);
		else {
			if (scenes[currentGameState]->switchScene)
				currentGameState = scenes[currentGameState]->switchSceneEvent();
			else
				scenes[currentGameState]->Update(&window, fElapsedTime);
		}

		window.display();
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		fElapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e+9;
	}
	ImGui::SFML::Shutdown();

	return 0;
}