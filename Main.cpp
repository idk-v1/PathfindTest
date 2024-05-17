#include "C:/SFML/SFML-Setup.hpp"
#include <SFML/Graphics.hpp>

template <typename T>
using Vec = std::vector<T>;
using PosF = sf::Vector2f;
using PosI = sf::Vector2i;

sf::Color
playerColor(0xFFFFFFFF), 
playerPathColor(0xFFFF00FF), playerPathOldColor(0xBFBF00FF),
targetColor(0x7F0000FF), targetTargetColor(0xFF7F7FFF),
wallColor(0x3F3F3FFF), floorColor(0x0F0F0FFF);


float pythag(float a, float b);

float pythagPos(PosI pos);

int random(int min, int max);

int dirToX(int i);

int dirToY(int i);

PosI dirToPos(int i);


void setVertTile(sf::VertexArray& vertArr, PosI pos, PosI size,
	sf::Color color);

void moveTargets(Vec<PosI>& target, PosI& pos, sf::VertexArray& vertArr,
	Vec<Vec<char>>& grid, PosI size, Vec<char>& posList);

void fillRandom(Vec<Vec<char>>& grid, sf::VertexArray& vertArr, PosI size,
	float scale, Vec<PosI>& target, PosI& pos);

void fillWalls(Vec<Vec<char>>& grid, sf::VertexArray& vertArr, PosI size);

void init(Vec<Vec<char>>& grid, sf::VertexArray& vertArr, PosI size, PosI& pos,
	Vec<PosI>& target, int scale);

bool move(Vec<Vec<char>>& grid, sf::VertexArray& vertArr, Vec<char>& posList,
	PosI size, PosI& pos, PosI& target);


int main()
{
	Vec<Vec<char>> grid;
	PosI size = { 100, 60 };

	Vec<PosI> target;
	sf::Vector2i pos;
	std::vector<char> posList;

	int scale = 10;
	sf::RenderWindow win(sf::VideoMode(scale * size.x, scale * size.y), "");
	sf::VertexArray vertArr(sf::Quads);

	init(grid, vertArr, size, pos, target, scale);

	win.setFramerateLimit(60);

	while (win.isOpen())
	{
		sf::Event e;
		while (win.pollEvent(e))
			if (!e.type)
				win.close();

		if (target.size())
		{
			if (!move(grid, vertArr, posList, size, pos, target[0]))
				fillRandom(grid, vertArr, size, scale, target, pos);
			else
				moveTargets(target, pos, vertArr, grid, size, posList);
		}
		else
		{
			posList.clear();
			fillRandom(grid, vertArr, size, scale, target, pos);
		}

		win.setTitle("Pathfind | Remaining: " + 
			std::to_string(target.size()) + " | Depth: " + 
			std::to_string(posList.size()));
		win.clear();
		win.draw(&vertArr[0], vertArr.getVertexCount(), sf::Quads);
		win.display();
	}

	return 0;
}


float pythag(float a, float b) 
{ 
	return sqrtf(a * a + b * b); 
}

float pythagPos(PosI pos)
{
	return pythag(pos.x, pos.y);
}

int random(int min, int max)
{
	return (rand() % (max - min)) + min;
}

int dirToX(int i)
{
	return (i % 2) * (i - 1 ? -1 : 1);
}

int dirToY(int i)
{
	return !(i % 2) * (i ? -1 : 1);
}

PosI dirToPos(int i)
{
	return PosI(dirToX(i), dirToY(i));
}

void setVertTile(sf::VertexArray& vertArr, PosI pos, PosI size, 
	sf::Color color)
{
	for (int i = 0; i < 4; i++)
		vertArr[4 * (pos.x + pos.y * size.x) + i].color = color;
}

void moveTargets(Vec<PosI>& target, PosI& pos, sf::VertexArray& vertArr, 
	Vec<Vec<char>>& grid, PosI size, Vec<char>& posList)
{
	int delIndex = -1;

	for (int ii = 0; ii < target.size(); ii++)
	{
		if (pos == target[ii])
			delIndex = ii;
		else
		{
			int i = rand() % 4;
			if (!(grid[target[ii].x + dirToX(i)][target[ii].y + dirToY(i)]))
			{
				setVertTile(vertArr, target[ii], size, floorColor);
				target[ii] += dirToPos(i);
				setVertTile(vertArr, target[ii], size, ii ? 
					targetColor : targetTargetColor);
			}
		}
	}

	if (delIndex != -1)
	{
		if (!delIndex)
		{
			posList.clear();
			grid[pos.x][pos.y] |= 0x80;
			for (int x = 1; x < size.x - 1; x++)
				for (int y = 1; y < size.y - 1; y++)
					if (grid[x][y] & 0x80)
					{
						grid[x][y] &= ~0x80;
						setVertTile(vertArr, PosI(x, y), size, floorColor);
					}
		}
		target.erase(target.begin() + delIndex);
	}
}

void fillRandom(Vec<Vec<char>>& grid, sf::VertexArray& vertArr, PosI size, 
	float scale, Vec<PosI>& target, PosI& pos)
{
	for (int x = 1; x < size.x - 1; x++)
		for (int y = 1; y < size.y - 1; y++)
		{
			grid[x][y] = !(rand() % 5);
			setVertTile(vertArr, PosI(x, y), size, grid[x][y] ?
				wallColor : floorColor);
		}

	pos = { rand() % (size.x - 2) + 1, rand() % (size.y - 2) + 1 };
	target.resize(50);
	for (int i = 0; i < target.size(); i++)
	{
		target[i] = PosI(random(1, size.x - 2), random(1, size.y - 2));
		grid[target[i].x][target[i].y] &= ~0x01;
		setVertTile(vertArr, target[i], size, i ? 
			targetColor : targetTargetColor);
	}
}

void fillWalls(Vec<Vec<char>>& grid, sf::VertexArray& vertArr, PosI size)
{
	for (int x = 0; x < size.x; x++)
	{
		grid[x][0] = 0x01;
		grid[x][size.y - 1] = 0x01;
		setVertTile(vertArr, PosI(x, 0), size, wallColor);
		setVertTile(vertArr, PosI(x, size.y - 1), size, wallColor);
	}
	for (int y = 0; y < size.y; y++)
	{
		grid[0][y] = 0x01;
		grid[size.x - 1][y] = 0x01;
		setVertTile(vertArr, PosI(0, y), size, wallColor);
		setVertTile(vertArr, PosI(size.x - 1, y), size, wallColor);
	}
}

void init(Vec<Vec<char>>& grid, sf::VertexArray& vertArr, PosI size, PosI& pos, 
	Vec<PosI>& target, int scale)
{
	srand(time(0));

	vertArr.resize(size.x * size.y * 4);
	for (int x = 0; x < size.x; x++)
		for (int y = 0; y < size.y; y++)
			for (int i = 0; i < 4; i++)
				vertArr[4 * (x + y * size.x) + i].position = 
				PosF((x + i % 3) * scale, (y + i / 2) * scale);

	grid.resize(size.x);
	for (auto& col : grid)
		col.resize(size.y, 0);
	
	fillWalls(grid, vertArr, size);

	fillRandom(grid, vertArr, size, scale, target, pos);
}

bool move(Vec<Vec<char>>& grid, sf::VertexArray& vertArr, Vec<char>& posList, 
	PosI size, PosI& pos, PosI& target)
{
	int closest = -1;
	float dist = INFINITY, cd;
	for (int i = 0; i < 4; i++)
	{
		if (!(grid[pos.x + dirToX(i)][pos.y + dirToY(i)] & 0x81))
		{
			cd = pythagPos(pos - target + dirToPos(i));
			if (cd < dist)
			{
				dist = cd;
				closest = i;
			}
		}
	}

	setVertTile(vertArr, pos, size, playerPathColor);
	grid[pos.x][pos.y] |= 0x80;
	if (closest == -1)
	{
		if (!posList.size())
			return false;
		grid[pos.x][pos.y] &= ~0x20;
		setVertTile(vertArr, pos, size, playerPathOldColor);
		pos -= dirToPos(posList.back());
		posList.pop_back();
	}
	else
	{
		posList.push_back(closest);
		pos += dirToPos(closest);
	}

	grid[pos.x][pos.y] |= 0xC0;
	setVertTile(vertArr, pos, size, playerColor);
	return true;
}