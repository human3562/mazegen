#pragma once
#include "gameScene.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include <stack>

struct Cell {
	int i, j;
	bool walls[4] = { true, true, true, true };
	bool visited = false;
	Cell(int i, int j) : i(i), j(j) {};
};

class MazegenScene : public GameScene
{
public:
	void Start(sf::RenderWindow* window) override;
	void Update(sf::RenderWindow* window, float fElapsedTime) override;
	void EventHandle(sf::RenderWindow* window, sf::Event* event) override;
	e_gameState switchSceneEvent() override;

private:
	std::vector<Cell> cells;
	std::stack<Cell*> stack;

	sf::Color wallColor = sf::Color(160, 160, 160);
	float wallColor_t[3] = { 160 / 255.f, 160 / 255.f, 160 / 255.f };
	sf::Color bgColor = sf::Color(100, 100, 100);
	float bgColor_t[3] = {100/255.f, 100/255.f, 100/255.f };
	sf::Color c_visited = sf::Color(0, 65, 110, 88);
	float c_visited_t[4] = {0, 65/255.f, 110/255.f, 88/255.f };
	sf::Color c_current = sf::Color(37, 0, 255, 105);
	float c_current_t[4] = {37/255.f, 0, 1, 105/255.f };

	Cell* current = nullptr;
	int index(int i, int j);
	void showCell(sf::RenderWindow* window, Cell* cell);
	void reset(sf::RenderWindow* window);
	int checkCellNeighbors(Cell* cell);
	int cols, rows;
	int w = 20;
	int dimensions = 10;
	bool animate = true;
	bool show_visited = false;
	bool done = false;
};

