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
namespace Tmpl8
{

	std::ifstream map_file; 
	std::ifstream obj_file;

	TileMaps tilemap("assets/nc2tiles.png", mapHeight , mapWidth * 3);
	struct object {
		Tmpl8::vec4 obj;
		char sign;
	};

	std::vector<object> game_map_objects;


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
		std::string line;
		obs = new char* [tilemap.getHeight()];
		for(int i = 0; i < tilemap.getHeight(); i++)
			obs[i] = new char[tilemap.getWidth() + 1];
		map_file.open("map.txt");
		if (map_file.fail())
			printf("didnt work");
		for (int i = 1; i < tilemap.getHeight() + 1; i++)
		{
			std::getline(map_file, line);
			strcpy(obs[i - 1], line.c_str());
		}
		map_file.close();
		obj_file.open("objects.txt");
		std::getline(obj_file, line);
		for (int i = 1; i < 4; i++)
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
		tilemap.setMap(obs);
		//bottom side
		/*for (int j = 0; j < tilemap.getWidth(); j += 3)
		{
			obs[0][j] = 'n', obs[0][j + 1] = 'e', obs[0][j + 2] = 'x';
		}
		tilemap.setTile(obs, 1, tilemap.getWidth(), 0, 15,'x');

		//top side
		char** first_obj;
		first_obj = new char* [5];
		for (int i = 0; i < 5; i++)
		{
			first_obj[i] = new char[43];
		}
		for(int i = 0; i< 5; i++)
			if (i != 4)
				first_obj[i] = { "maxmaxmaxmaxmaxmaxmaxmaxmaxmaxmaxmaxmaxrax" };
			else
				first_obj[i] = { "aexaexaexaexaexaexaexaexaexaexaexaexaexrbx" };
		tilemap.setTile(first_obj, 5, 42, 0, 0, 'x');

		//right side
		for (int j = 0; j < tilemap.getHeight() - 2; j++)
		{
			obsV[j][0] = 'j', obsV[j][1] = 'd', obsV[j][2] = 'x';
		}
		tilemap.setTile(obsV, tilemap.getHeight() - 2,3, ((int)mapWidth) -1, 1, 'x');

		//left side
		for (int j = 0; j < tilemap.getHeight() - 2; j++)
		{
			obsV[j][0] = 'l', obsV[j][1] = 'f', obsV[j][2] = 'x';
		}
		tilemap.setTile(obsV, tilemap.getHeight() - 2, 3, 0, 1, 'x');

		char ob[4] = "nex";
		//tilemap.setTile(ob,3, 0, 7, 'x');
		tilemap.setTile("idx", 3, 4, 15, 'j');
		//tilemap.printMap();*/
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

	Ball player(19, 280, 18);
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