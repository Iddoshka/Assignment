#include "game.h"
#include "surface.h"
#include<windows.h>
#include <cstdio> //printf
#include"player.h"
#include<iostream>
#include<fstream>
#include<string>
#include<istream>

constexpr float gravity = 0.5f;
constexpr float mapHeight = ScreenHeight / 32.0f;
constexpr float mapWidth = ScreenWidth / 32.0f;
constexpr int player_start_X = 300;
constexpr int player_start_Y = 300;
namespace Tmpl8
{

	std::ifstream map_file; 
	std::ifstream obj_file;

	TileMaps tilemap("assets/nc2tiles.png", mapHeight , mapWidth * 6);
	struct object {
		Tmpl8::vec4 obj;
		char sign;
	};

	std::vector<object> game_map_objects;
	std::vector<char*> maps = { "testmap.txt","testmap2.txt" };
	void openMap(std::vector<char*> maps)
	{
		char** obs;
		obs = new char* [tilemap.getHeight()];
		for (int i = 0; i < tilemap.getHeight(); i++)
			obs[i] = new char[tilemap.getWidth() + 1];
		std::string line;
		std::vector<std::string> string_map;
		string_map.resize(tilemap.getHeight());
		for (auto a : maps)
		{
			map_file.open(a);
			if (map_file.fail())
				printf("didnt work");
			for (int i = 0; i < tilemap.getHeight(); i++)
			{
				std::getline(map_file, line);
				string_map[i].append(line);
			}
			map_file.close();
		}
		for (int i = 0; i < tilemap.getHeight(); i++)
			strcpy(obs[i], string_map[i].c_str());
		tilemap.setMap(obs);
	}

	void setMapObjects(std::vector<object> map_objects)
	{
		for (auto a : map_objects)
		{
			tilemap.setColliders(a.obj.x, a.obj.y, a.obj.z, a.obj.w, a.sign);
		}
	}

	void Game::Init()
	{
		char** obs;
		obs = new char* [tilemap.getHeight()];
		for (int i = 0; i < tilemap.getHeight(); i++)
			obs[i] = new char[tilemap.getWidth() + 1];
		std::string line;
		openMap(maps);
		
		tilemap.printMap();
		obj_file.open("testobjects.txt");
		if (obj_file.fail())
			printf("didnt work");
		std::getline(obj_file, line);
		while (!obj_file.eof())
		{
			float a, b, c, d;
			char sign;
			std::getline(obj_file, line);
			object collider;
			sscanf(line.c_str(), "%f / %f / %f / %f / %c", &a, &b, &c, &d, &sign);
			collider.obj = Tmpl8::vec4(a * 32, b * 32, c * 32, d * 32);
			collider.sign = sign;
			game_map_objects.push_back(collider);
		}
		setMapObjects(game_map_objects);
		obj_file.close();
		
	}
	
	// -----------------------------------------------------------
	// Close down application
	// -----------------------------------------------------------
	void Game::Shutdown()
	{
	}

	void Game::printScreen(TileMaps map)
	{
		for (int i = 0; i < mapHeight; i++)
		{
			for (int j = 0; j < mapWidth; j++)
			{
				int tx = map.getMap()[i][j * 3] - 'a', ty = map.getMap()[i][j * 3 + 1] - 'a';
				map.DrawTile(tx, ty, screen, j * 32, i * 32);
			}
		}
	}

	Ball player(player_start_X, player_start_Y, 18);
	// -----------------------------------------------------------
	// Main application tick function
	// -----------------------------------------------------------
	void Game::Tick(float deltaTime)
	{
		// clear the graphics window
		screen->Clear(0);
		//printScreen(tilemap);
		tilemap.mapScroll(screen);
		player.printBall(screen);
		player.Drive(tilemap);
		player.verlet(tilemap);
		player.mapReact(screen,tilemap);
		_sleep(25);
	}
};