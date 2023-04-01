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
	TileMaps tilemap("assets/nc2tiles.png", mapHeight * 2, mapWidth * 6);
	
	void Game::Init()
	{
		char** obs;
		obs = new char* [1];
		char** obsV;
		obsV = new char* [tilemap.getHeight() - 2];
		for(int i = 0; i < 1; i++)
			obs[i] = new char[tilemap.getWidth()];
		for (int i = 0; i < tilemap.getHeight() - 2; i++)
			obsV[i] = new char[3];

		//bottom side
		for (int j = 0; j < tilemap.getWidth(); j += 3)
		{
			obs[0][j] = 'n', obs[0][j + 1] = 'e', obs[0][j + 2] = 'x';
		}
		tilemap.setTile(obs, 1, tilemap.getWidth(), 0, 31,'x');

		//top side
		for (int j = 0; j < tilemap.getWidth(); j += 3)
		{
			obs[0][j] = 'a', obs[0][j + 1] = 'e', obs[0][j + 2] = 'x';
		}
		tilemap.setTile(obs, 1, tilemap.getWidth(), 0, 0, 'x');

		//right side
		for (int j = 0; j < tilemap.getHeight() - 2; j++)
		{
			obsV[j][0] = 'j', obsV[j][1] = 'd', obsV[j][2] = 'x';
		}
		tilemap.setTile(obsV, tilemap.getHeight() - 2,3, ((int)mapWidth) -1, 1, 'x');

		//left side
		for (int j = 0; j < tilemap.getHeight() - 2; j++ )
		{
			obsV[j][0] = 'l', obsV[j][1] = 'f', obsV[j][2] = 'x';
		}
		tilemap.setTile(obsV, tilemap.getHeight() - 2, 3, 0, 1, 'x');

		char ob[4] = "nex";
		//tilemap.setTile(ob,3, 0, 7, 'x');
		tilemap.setTile("idx", 9, 4, 31, 'j');
		//tilemap.printMap();
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

	Ball player(68, 74, 18);
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