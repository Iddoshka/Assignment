#include "game.h"
#include "surface.h"
#include<windows.h>
#include <cstdio> //printf
#include<iostream>
#include<fstream>
#include<string>
#include<istream>

constexpr float gravity = 0.5f;
constexpr float mapHeight = ScreenHeight / 32.0f;
constexpr float mapWidth = ScreenWidth / 32.0f;
constexpr float player_start_X = 19;
constexpr float player_start_Y = 306;
uint32_t startTime = SDL_GetPerformanceCounter();
namespace Tmpl8
{
	void Game::switch_state()
	{
		switch (state_machine)
		{
		case menu:
			state_machine = play;
			break;
		case play:
			state_machine = stop;
			break;
		case stop:
			state_machine = play;
			break;
		default:
			break;
		}
	}

	std::ifstream map_file; 
	std::ifstream obj_file;
	std::ifstream stat_file;
	std::vector<char*> maps = { "map1.1.txt","map1.2.txt" };
	std::vector<char*> objects_files = { "objects1.1.txt","objects1.2.txt" };

	TileMaps tilemap("assets/nc2tiles.png", mapHeight , mapWidth * 3 * maps.size());

	struct object {
		Tmpl8::vec4 obj;
		char sign;
	};

	std::vector<object> game_map_objects;
	
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

	void Game::reset(Ball &player)
	{
		player.setPX(player_start_X);
		player.setPY(player_start_Y);
		player.setX(player_start_X);
		player.setY(player_start_Y);
		tilemap.setXoffSet(0);
		tilemap.setYoffSet(0);
	}

	void setMapObjects(std::vector<object> map_objects)
	{
		for (auto a : map_objects)
		{
			tilemap.setColliders(a.obj.x, a.obj.y, a.obj.z, a.obj.w, a.sign);
		}
	}

	void openObjects(std::vector<char*> objects)
	{
		int count_files = 0;
		std::string line;
		for (auto a : objects_files)
		{
			obj_file.open(a);
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
				collider.obj = Tmpl8::vec4((a + count_files * mapWidth) * 32, b * 32, c * 32, d * 32);
				collider.sign = sign;
				game_map_objects.push_back(collider);
			}
			obj_file.close();
			count_files += 1;
		}
		setMapObjects(game_map_objects);
	}
	void Game::Init()
	{
		openMap(maps);
		openObjects(objects_files);
	}
	
	// -----------------------------------------------------------
	// Close down application
	// -----------------------------------------------------------
	void Game::Shutdown()
	{
		exit(1);
	}

	void Game::winingLine()
	{
		uint32_t currTime = SDL_GetPerformanceCounter();
		double elapsedTime = static_cast<double>(
			(currTime - startTime) / static_cast<double>(SDL_GetPerformanceFrequency())
			);
		std::string line = { "YOUR TIME WAS: " + std::to_string(elapsedTime) + "!" };
		char* win_line = new char[line.size() + 1];
		strcpy(win_line, line.c_str());
		screen->Centre(win_line, 260, Tmpl8::GreenMask);
		std::fstream time_file("best_time.txt");
		double best_time;
		time_file >> best_time;
		if (best_time > elapsedTime)
			time_file << elapsedTime;
		time_file.close();
		time_file.open("master_time.txt");
		time_file >> best_time;
		if (best_time < elapsedTime)
			screen->Centre("YOU BEAT THE MASTER!", 268, Tmpl8::GreenMask);
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
	double angle = 0;
	// -----------------------------------------------------------
	// Main application tick function
	// -----------------------------------------------------------
	void Game::Tick(float deltaTime)
	{	
		switch (state_machine)
		{
		case menu:
			screen->Centre("PRESS ENTER TO START!", 256, Tmpl8::GreenMask);
			if (GetAsyncKeyState(VK_RETURN))
			{
				switch_state();
			}
			break;
		case play:
			if (player.isdead())
			{
				_sleep(2000);
				player.revive();
			}
			else if (player.victory())
			{
				_sleep(4000);
				Shutdown();
			}
			// clear the graphics window
			screen->Clear(0);
			tilemap.mapScroll(screen);
			player.printBall(screen);
			player.Drive(tilemap);
			player.verlet(tilemap);
			player.mapReact(screen, tilemap);
			if (player.isdead() || player.victory()) switch_state();
			break;
		case stop:
			if (player.isdead())
			{
				screen->Centre("YOU DIED!", 256, Tmpl8::RedMask);
				reset(player);
			}
			if (player.victory())
			{
				screen->Clear(0);
				screen->Centre("YOU'VE WON!", 252, Tmpl8::GreenMask);
				winingLine();
			}
			switch_state();
		default:
			break;
		}
		_sleep(25);
	}
};