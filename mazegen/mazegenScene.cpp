#include "mazegenScene.h"

void MazegenScene::Start(sf::RenderWindow* window){
	isStarted = true;
	reset(window);
}

void MazegenScene::Update(sf::RenderWindow* window, float fElapsedTime){
	window->clear(bgColor);
	ImGui::SFML::Update(*window, sf::Time(sf::seconds(fElapsedTime)));
	

	for (int i = 0; i < cells.size(); i++) {
		showCell(window, &cells[i]);
	}
	if (animate) {
		if (!done) {
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
	}

	ImGui::SetNextWindowSize(ImVec2(300, 600));
	ImGui::SetNextWindowPos(ImVec2(600, 0));
	bool p = true;
	ImGui::Begin("Settings", &p, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	std::string fps = "FPS: " + std::to_string(1.0f / fElapsedTime);
	ImGui::Text(fps.c_str());
	if (ImGui::SliderInt("gridSize", &dimensions, 2, 200)) {
		reset(window);
	}
	ImGui::Checkbox("Animate", &animate);
	if (ImGui::Button("Generate")) {
		reset(window);
		done = false;
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
	done = true;
	w = window->getSize().y / dimensions;
	//done = true;
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

	if(right >= 0)
	if (!cells[right].visited) 
		neighbors.push_back(&cells[right]);

	if(bottom >= 0)
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

	if (cell->visited && show_visited) {
		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f(w, w));
		rect.setFillColor(c_visited);
		rect.setPosition(x, y);
		window->draw(rect);
	}

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
