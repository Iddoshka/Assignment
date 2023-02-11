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
		char obs[(int)mapHeight][(int)mapWidth * 3];
		for (int i = 0; i < (int)mapHeight - 1; i++)
		{
			for (int j = 0; j < mapWidth * 3; j+= 3)
			{
				obs[i][j] = 'm', obs[i][j + 1] = 'a', obs[i][j + 2] = ' ';
			}
		}
		for (int j = 0; j < (int)mapWidth * 3; j += 3)
		{
			obs[(int)mapHeight - 1][j] = 'n', obs[(int)mapHeight - 1][j + 1] = 'e', obs[(int)mapHeight - 1][j + 2] = 'x';
		}
		for(int i = 0; i < mapHeight; i++) {
			tilemap.setTile(obs[i], mapWidth * 3,0, i);
		}
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
		for (int i = 0; i < map.getHeight(); i++)
		{
			for (int j = 0; j < map.getWidth() / 3; j++)
			{
				int tx = map.getMap()[i][j * 3] - 'a', ty = map.getMap()[i][j * 3 + 1] - 'a';
				map.DrawTile(tx, ty, screen, j * 32, i * 32);
			}
		}
	}

	Ball player(ScreenWidth / 2.0f, 0, 18);
	// -----------------------------------------------------------
	// Main application tick function
	// -----------------------------------------------------------
	void Game::Tick(float deltaTime)
	{
		// clear the graphics window
		screen->Clear(0);
		printScreen(tilemap);
		player.printBall(screen);
		player.verlet();
		player.addY(gravity);
		_sleep(25);
	}
};