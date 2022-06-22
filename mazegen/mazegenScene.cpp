#include "mazegenScene.h"


void MazegenScene::Start(sf::RenderWindow* window){
	isStarted = true;
	reset(window);
	done = false;
}

void MazegenScene::Update(sf::RenderWindow* window, float fElapsedTime){
	window->clear(bgColor);
	
	ImGui::SFML::Update(*window, sf::Time(sf::seconds(fElapsedTime)));
	
	tx_visitedCells->update(*img_visitedCells);
	if (show_visited) {
		window->draw(spr_visitedCells);
	}

	for (int i = 0; i < cells.size(); i++) {
		showCell(window, &cells[i]);
	}

	if (animate) {
		if (!done) {
			current->visited = true;
			img_visitedCells->setPixel(current->i, current->j, c_visited);
			int neighborID = checkCellNeighbors(current);
			if (neighborID >= 0) {
				Cell* next = &cells[neighborID];
				next->visited = true;
				stack.push(current);
				current = next;
			}
			else {
				if (!stack.empty()) {
					current = stack.top(); stack.pop();
				}
				else {
					done = true;
					//std::cout << "done!" << std::endl;
				}
			}
		}
	}
	else {
		while (!done) {
			current->visited = true;
			int neighborID = checkCellNeighbors(current);
			if (neighborID >= 0) {
				Cell* next = &cells[neighborID];
				next->visited = true;
				stack.push(current);
				current = next;
			}
			else {
				if (!stack.empty()) {
					current = stack.top(); stack.pop();
				}
				else {
					done = true;
					std::cout << "done!" << std::endl;
				}
			}
		}
		for (int i = 0; i < cells.size(); i++) {
			Cell* c = &cells[i];
			img_visitedCells->setPixel(c->i, c->j, c_visited);
		}
	}

	//A*
	if (animate) {
		if (shouldSolve) {
			if (open_set.size() > 0 && !solved) {
				Cell* current_astar = *open_set.rbegin();
				if (current_astar->i == dimensions - 1 && current_astar->j == dimensions - 1) {
					solved = true;
					std::cout << "solved??" << std::endl;
				}

				open_set.erase(current_astar);
				closed_set.insert(current_astar);
				img_visitedCells->setPixel(current_astar->i, current_astar->j, sf::Color(100,100,100,100));

				std::vector<Cell*> neighbors = getCellNeighbors(current_astar);
				for (int i = 0; i < neighbors.size(); i++) {
					Cell* neighbor = neighbors[i];
					if (closed_set.find(neighbor) != closed_set.end()) continue;

					img_visitedCells->setPixel(neighbor->i, neighbor->j, sf::Color::Green);

					int newG = current_astar->g + 1;
					if (open_set.find(neighbor) != open_set.end()) {
						if (newG < neighbor->g) neighbor->g = newG;
					}
					else {
						neighbor->g = newG;
						open_set.insert(neighbor);
					}
					Cell* endCell = &cells[cells.size() - 1];
					neighbor->h = std::abs(neighbor->i - endCell->i) + std::abs(neighbor->j - endCell->j);
					neighbor->f = neighbor->g + neighbor->h;
					neighbor->previous = current_astar;
				}
				Cell* drawCell = current_astar;
				while (drawCell->previous != nullptr) {
					sf::Vertex line[] = {
						sf::Vertex(sf::Vector2f(drawCell->i * w + w / 2.0, drawCell->j * w + w / 2.0), sf::Color::Red),
						sf::Vertex(sf::Vector2f(drawCell->previous->i * w + w / 2.0, drawCell->previous->j * w + w / 2.0), sf::Color::Red)
					};
					window->draw(line, 2, sf::Lines);
					drawCell = drawCell->previous;
				}
			}
		}
	}
	else {
		if (shouldSolve) {
			while (open_set.size() > 0 && !solved) {
				Cell* current_astar = *open_set.rbegin();
				if (current_astar->i == dimensions - 1 && current_astar->j == dimensions - 1) {
					solved = true;
					break;
				}

				open_set.erase(current_astar);
				closed_set.insert(current_astar);

				std::vector<Cell*> neighbors = getCellNeighbors(current_astar);
				for (int i = 0; i < neighbors.size(); i++) {
					Cell* neighbor = neighbors[i];
					if (closed_set.find(neighbor) != closed_set.end()) continue;

					int newG = current_astar->g + 1;
					if (open_set.find(neighbor) != open_set.end()) {
						if (newG < neighbor->g) neighbor->g = newG;
					}
					else {
						neighbor->g = newG;
						open_set.insert(neighbor);
					}
					Cell* endCell = &cells[cells.size() - 1];
					neighbor->h = std::abs(neighbor->i - endCell->i) + std::abs(neighbor->j - endCell->j);
					neighbor->f = neighbor->g + neighbor->h;
					neighbor->previous = current_astar;
				}
			}
		}
	}

	if (solved) {
		Cell* drawCell = &cells[cells.size()-1];
		while (drawCell->previous != nullptr) {
			sf::Vertex line[] = {
				sf::Vertex(sf::Vector2f(drawCell->i * w + w / 2.0, drawCell->j * w + w / 2.0), sf::Color::Red),
				sf::Vertex(sf::Vector2f(drawCell->previous->i * w + w / 2.0, drawCell->previous->j * w + w / 2.0), sf::Color::Red)
			};
			window->draw(line, 2, sf::Lines);
			drawCell = drawCell->previous;
		}
	}

	ImGui::SetNextWindowSize(ImVec2(300, 600));
	ImGui::SetNextWindowPos(ImVec2(600, 0));
	bool p = true;
	ImGui::Begin("Settings", &p, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	std::string fps = "FPS: " + std::to_string(1.0f / fElapsedTime);
	ImGui::Text(fps.c_str());
	if (ImGui::SliderInt("gridSize", &dimensions, 2, 200)) {
		//reset(window);
	}
	ImGui::Checkbox("Animate", &animate);
	if (ImGui::Button("Generate")) {
		reset(window);
	}
	if (ImGui::Button("Solve")) {
		if (done) {
			std::cout << "should solve..." << std::endl;
			shouldSolve = true;
		}
	}
	ImGui::Text("");
	ImGui::Text("Colors:");
	if (ImGui::ColorEdit3("wall", wallColor_t)) {
		wallColor.r = static_cast<sf::Uint8>(wallColor_t[0] * 255.f);
		wallColor.g = static_cast<sf::Uint8>(wallColor_t[1] * 255.f);
		wallColor.b = static_cast<sf::Uint8>(wallColor_t[2] * 255.f);
	}
	if (ImGui::ColorEdit3("bg", bgColor_t)) {
		bgColor.r = static_cast<sf::Uint8>(bgColor_t[0] * 255.f);
		bgColor.g = static_cast<sf::Uint8>(bgColor_t[1] * 255.f);
		bgColor.b = static_cast<sf::Uint8>(bgColor_t[2] * 255.f);
	}
	if (ImGui::ColorEdit3("visited", c_visited_t)) {
		c_visited.r = static_cast<sf::Uint8>(c_visited_t[0] * 255.f);
		c_visited.g = static_cast<sf::Uint8>(c_visited_t[1] * 255.f);
		c_visited.b = static_cast<sf::Uint8>(c_visited_t[2] * 255.f);
		//c_visited.a = static_cast<sf::Uint8>(c_visited_t[3] * 255.f);
	}
	if (ImGui::ColorEdit4("current", c_current_t)) {
		c_current.r = static_cast<sf::Uint8>(c_current_t[0] * 255.f);
		c_current.g = static_cast<sf::Uint8>(c_current_t[1] * 255.f);
		c_current.b = static_cast<sf::Uint8>(c_current_t[2] * 255.f);
		c_current.a = static_cast<sf::Uint8>(c_current_t[3] * 255.f);
	}
	ImGui::Checkbox("fill visited cells", &show_visited);

	ImGui::End();

	ImGui::SFML::Render(*window);
}

void MazegenScene::EventHandle(sf::RenderWindow* window, sf::Event* event){
	ImGui::SFML::ProcessEvent(*event);
}

e_gameState MazegenScene::switchSceneEvent(){
	return e_gameState();
}

int MazegenScene::index(int i, int j) {
	if (i < 0 || j < 0 || i > cols - 1 || j > rows - 1)
		return -1;
	return i + j * cols;
}

void MazegenScene::reset(sf::RenderWindow* window) {
	//window->clear(bgColor);

	open_set.clear();
	closed_set.clear();

	delete img_visitedCells;
	img_visitedCells = new sf::Image();
	img_visitedCells->create(dimensions, dimensions, bgColor);

	delete tx_visitedCells;
	tx_visitedCells = new sf::Texture();
	tx_visitedCells->create(dimensions, dimensions);
	//tx_visitedCells->setSmooth(true);
	spr_visitedCells.setTexture(*tx_visitedCells, true);

	w = window->getSize().y / dimensions;
	spr_visitedCells.setScale(sf::Vector2f(w,w));

	done = false;
	solved = false;
	shouldSolve = false;
	stack = std::stack<Cell*>();
	cells.clear();
	cols = dimensions; rows = dimensions;

	for (int j = 0; j < rows; j++) {
		for (int i = 0; i < cols; i++) {
			Cell c = { i, j };
			cells.push_back(c);
		}
	}
	current = &cells[0];
	open_set.insert(&cells[0]);
}

std::vector<Cell*> MazegenScene::getCellNeighbors(Cell* cell) {
	std::vector<Cell*> neighbors;

	int top = index(cell->i, cell->j - 1);
	int right = index(cell->i + 1, cell->j);
	int bottom = index(cell->i, cell->j + 1);
	int left = index(cell->i - 1, cell->j);

	if (top >= 0)
		if (!cells[top].walls[2])
			neighbors.push_back(&cells[top]);

	if (right >= 0 && right < cells.size())
		if (!cells[right].walls[3])
			neighbors.push_back(&cells[right]);

	if (bottom >= 0 && bottom < cells.size())
		if (!cells[bottom].walls[0])
			neighbors.push_back(&cells[bottom]);

	if (left >= 0)
		if (!cells[left].walls[1])
			neighbors.push_back(&cells[left]);

	return neighbors;
}

int MazegenScene::checkCellNeighbors(Cell* cell) {
	std::vector<Cell*> neighbors;

	int top =    index(cell->i, cell->j - 1);
	int right =  index(cell->i + 1, cell->j);
	int bottom = index(cell->i, cell->j + 1);
	int left =   index(cell->i - 1, cell->j);

	if(top >= 0)
	if (!cells[top].visited) 
		neighbors.push_back(&cells[top]);

	if(right >= 0 && right < cells.size())
	if (!cells[right].visited) 
		neighbors.push_back(&cells[right]);

	if(bottom >= 0 && bottom < cells.size())
	if (!cells[bottom].visited) 
		neighbors.push_back(&cells[bottom]);

	if(left >= 0)
	if (!cells[left].visited)
		neighbors.push_back(&cells[left]);

	if (!neighbors.empty()) {
		int r = floor(rand() % neighbors.size());

		if (cell->i - neighbors[r]->i == -1) {
			cell->walls[1] = false;
			neighbors[r]->walls[3] = false;
		}
		if (cell->i - neighbors[r]->i == 1) {
			cell->walls[3] = false;
			neighbors[r]->walls[1] = false;
		}

		if (cell->j - neighbors[r]->j == -1) {
			cell->walls[2] = false;
			neighbors[r]->walls[0] = false;
		}
		if (cell->j - neighbors[r]->j == 1) {
			cell->walls[0] = false;
			neighbors[r]->walls[2] = false;
		}

		return index(neighbors[r]->i, neighbors[r]->j);
	}
	else {
		return -1;
	}
}

void MazegenScene::showCell(sf::RenderWindow* window, Cell* cell) {
	if (cell->walls[0] && cell->walls[1] && cell->walls[2] && cell->walls[3]) return;
	int x = cell->i * w;
	int y = cell->j * w;

	//if (cell->visited && show_visited) {
	//	sf::RectangleShape rect;
	//	rect.setSize(sf::Vector2f(w, w));
	//	rect.setFillColor(c_visited);
	//	rect.setPosition(x, y);
	//	window->draw(rect);
	//}

	if (cell == current) {
		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f(w, w));
		rect.setFillColor(c_current);
		rect.setPosition(x, y);
		window->draw(rect);
	}

	//TOP
	/*if (cell->walls[0]) {
		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f(x, y), wallColor),
			sf::Vertex(sf::Vector2f(x + w, y), wallColor)
		};
		window->draw(line, 2, sf::Lines);
	}*/

	//RIGHT
	if (cell->walls[1]) {
		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f(x + w, y), wallColor),
			sf::Vertex(sf::Vector2f(x + w, y + w), wallColor)
		};
		window->draw(line, 2, sf::Lines);
	}

	//BOTTOM
	if (cell->walls[2]) {
		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f(x, y + w), wallColor),
			sf::Vertex(sf::Vector2f(x + w, y + w), wallColor)
		};
		window->draw(line, 2, sf::Lines);
	}

	////LEFT
	//if (cell->walls[3]) {
	//	sf::Vertex line[] = {
	//		sf::Vertex(sf::Vector2f(x, y), wallColor),
	//		sf::Vertex(sf::Vector2f(x, y + w), wallColor)
	//	};
	//	window->draw(line, 2, sf::Lines);
	//}
}
