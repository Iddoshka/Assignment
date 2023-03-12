#include "game.h"
#include "surface.h"
#include<windows.h>
#include <cstdio> //printf
#include"player.h"
#include<iostream>
constexpr float gravity = 0.5f;
constexpr float mapHeight = ScreenHeight / 32.0f;
constexpr float mapWidth = ScreenWidth / 32.0f;
namespace Tmpl8
{
	TileMaps tilemap("assets/nc2tiles.png", mapHeight, mapWidth * 3);
	
	void Game::Init()
	{
		char** obs;
		obs = new char* [1];
		for(int i = 0; i < 1; i++)
			obs[i] = new char[tilemap.getWidth()];
		for (int j = 0; j < tilemap.getWidth(); j += 3)
		{
			obs[0][j] = 'n', obs[0][j + 1] = 'e', obs[0][j + 2] = 'x';
		}
		tilemap.setTile(obs, 1, tilemap.getWidth(), 0, (int)mapHeight-1,'x');
		char ob[4] = "nex";
		tilemap.setTile(ob,3, 0, 7, 'x');
		tilemap.setTile("aex", 3, 5, 9, 'x');
		tilemap.printMap();
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

	Ball player(34, 37, 18);
	// -----------------------------------------------------------
	// Main application tick function
	// -----------------------------------------------------------
	void Game::Tick(float deltaTime)
	{
		// clear the graphics window
		screen->Clear(0);
		printScreen(tilemap);
		//tilemap.mapScroll(screen, player.getCrd().y / 32.0f, player.getCrd().x / 32.0f);
		player.printBall(screen);
		player.verlet(tilemap);
		player.mapReact(screen,tilemap);
		_sleep(25);
	}
};