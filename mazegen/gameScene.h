#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>

enum e_gameState {
	MAZEGENERATOR
};

class GameScene {
public:
	GameScene() {};
	~GameScene() {};

public:
	virtual void Start(sf::RenderWindow* window) {}
	virtual void Update(sf::RenderWindow* window, float fElapsedTime) {}
	virtual void EventHandle(sf::RenderWindow* window, sf::Event* event) {}
	virtual e_gameState switchSceneEvent() { return MAZEGENERATOR; }
	bool isStarted = false;
	bool switchScene = false;
};
