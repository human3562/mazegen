#pragma once
#include "gameScene.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include <stack>
#include <set>

struct Cell {
	int i, j;
	bool walls[4] = { true, true, true, true };
	bool visited = false;
	int f = 0; int g = 0; int h = 0;
	Cell* previous = nullptr;
	Cell(int i, int j) : i(i), j(j) {};
};
struct cell_compare {
	//using is_transparent = void;
	bool operator() (Cell const *lhs, Cell const *rhs) const {
		//if (lhs->f == rhs->f) return lhs < rhs;
		return lhs->f < rhs->f;
	}
};

class MazegenScene : public GameScene
{
public:
	void Start(sf::RenderWindow* window) override;
	void Update(sf::RenderWindow* window, float fElapsedTime) override;
	void EventHandle(sf::RenderWindow* window, sf::Event* event) override;
	e_gameState switchSceneEvent() override;
	~MazegenScene() { delete img_visitedCells; delete tx_visitedCells; }

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

	sf::Image* img_visitedCells;	//why...
	sf::Texture* tx_visitedCells;	   //
	sf::Sprite spr_visitedCells;	   //

	Cell* current = nullptr;
	int index(int i, int j);
	void showCell(sf::RenderWindow* window, Cell* cell);
	void reset(sf::RenderWindow* window);
	int checkCellNeighbors(Cell* cell);
	std::vector<Cell*> getCellNeighbors(Cell* cell);
	int cols, rows;
	int w = 20;
	int dimensions = 10;
	bool animate = true;
	bool show_visited = false;
	bool done = false;

	//A*
	bool solved = false;
	bool shouldSolve = false;
	std::multiset<Cell*, cell_compare> open_set;
	std::multiset<Cell*> closed_set;
};

