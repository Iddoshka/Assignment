#pragma once
#include "game.h"
#include "surface.h"
#include<windows.h>
#include <cstdio> //printf
#include<iostream>
#include<fstream>
#include<string>
#include<istream>
#include"gun.h"


namespace Tmpl8
{
	const std::string diff_line = "easyhard";
	std::string line;
	constexpr float mapHeight = ScreenHeight / TileLength;
	constexpr float mapWidth = ScreenWidth / TileLength;
	constexpr float player_start_X = 19; // starting coordinates of the player
	constexpr float player_start_Y = 320;
	double best_time;
	uint32_t startTime;
	const std::vector<char*> map_files = { "world/map1.1.txt","world/map1.2.txt" }; // the two map txt files
	const std::vector<char*> object_files = { "world/objects1.1.txt","world/objects1.2.txt", "world/objects1.3.txt"}; // the two object txt files 
	const std::vector<char*> gun_files = { "world/guns_easy.txt","world/guns_hard.txt" }; // the two object txt files 

	TileMaps tilemap("assets/nc2tiles.png", (int)mapHeight * 3, (int)mapWidth * 3 * (int)map_files.size()); // the tilemap class object
	std::vector<Gun*> guns[2];
	Ball player(player_start_X, player_start_Y, 18); // ball class object

	static void openGuns(const std::vector<char*>& a_file)
	{
		std::ifstream gun_file;
		for(int i = 0; i < 2; i++)
		{
			gun_file.open(a_file[i]);
			while (!gun_file.eof())
			{
				float a = 0, b = 0;
				unsigned int c = 0, d = 0;
				std::getline(gun_file, line);
				sscanf(line.c_str(), "%f / %f / %i / %i", &a, &b, &c, &d);
				if(line != "")
					guns[i].push_back(new Gun(vec2(a, b), c, d));
			}
			gun_file.close();
		}
	}

	// the flow between states of the state machine
	void Game::switchState()
	{
		switch (state_machine)
		{
		case menu:
			state_machine = play;
			startTime = (uint32_t)SDL_GetPerformanceCounter();
			if (!difficulty) tilemap.setTile("pf ",1,22,21,' ');
			else
			{
				tilemap.setTile("lbx", 1, 22, 21, 'x');
				tilemap.setColliders(22.0f * TileLength, 21.0f * TileLength, TileLength, TileLength, 's');
			}
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

	void Game::setState(states stateIn) 
	{ 
		state_machine = stateIn;
		reset(player);
	}

	// an object stuct that holds its starting coordinates its length and width and what type of tiles
	struct obstacles {
		vec4 obj;
		char sign;
	};

	std::vector<obstacles> game_map_objects; // will contain all the objects of the game

	// function opens each map file and stores it in a 2d buffer that will transfer to the tilemap class object
	void openMap(std::vector<char*> maps)
	{
		std::ifstream map_file; // used to open txt files containing the map buffer		
		std::vector<std::string> string_map;
		char** obs;
		obs = new char* [tilemap.getHeight()];
		for (int i = 0; i < tilemap.getHeight(); i++)
			obs[i] = new char[tilemap.getWidth() + 1];
		string_map.resize(tilemap.getHeight());
		for (auto a : maps)
		{
			map_file.open(a);
			if (map_file.fail())
			{
				exit(1);
			}
			for (int i = 0; i < tilemap.getHeight(); i++)
			{
				std::getline(map_file, line);
				string_map[i].append(line);
			}
			map_file.close();
		}
		for (int i = 0; i < tilemap.getHeight(); i++)
			strcpy(obs[i], string_map[i].c_str());
		tilemap.setMap(obs); // setting the map buffer
	}

	// reseting the player to the start position upon death or reset button pressed
	void Game::reset(Ball& player)
	{
		for (int i = 0; i < (difficulty + 1); i++)
			for(Gun* a : guns[i]) (*a).reset();
		player.setPX(player_start_X);
		player.setPY(player_start_Y);
		player.setX(player_start_X);
		player.setY(player_start_Y);
		player.SetAngle(0);
		tilemap.setXoffSet(0);
		tilemap.setYoffSet(0);
	}

	// function will set all the objects into the tilemap class object
	void setMapObjects(std::vector<obstacles> map_objects)
	{
		for (const auto& a : map_objects)
		{
			tilemap.setColliders(a.obj.x, a.obj.y, a.obj.z, a.obj.w, a.sign);
		}
	}

	// function opens each object file and stores it in its corrosponding tilemap class object vector
	void openObjects(std::vector<char*> objects)
	{

		std::ifstream obj_file; // used to open txt files containing the objects indicators
		int count_files = 0;
		for (auto a : object_files)
		{
			obj_file.open(a);
			if (obj_file.fail())
				exit(1);
			std::getline(obj_file, line);
			while (!obj_file.eof())
			{
				float a, b, c, d;
				char sign;
				std::getline(obj_file, line);
				obstacles collider;
				sscanf(line.c_str(), "%f / %f / %f / %f / %c", &a, &b, &c, &d, &sign);
				collider.obj = vec4((a + count_files * mapWidth) * TileLength, b * TileLength, c * TileLength, d * TileLength);
				collider.sign = sign;
				game_map_objects.push_back(collider);
			}
			obj_file.close();
			count_files = (count_files + 1) % 2;
		}
		setMapObjects(game_map_objects);
	}

	void Game::Init()
	{
		openGuns(gun_files);
		openMap(map_files);
		openObjects(object_files);
	}

	// -----------------------------------------------------------
	// Close down application
	// -----------------------------------------------------------
	void Game::Shutdown()
	{
		exit(1);
	}

	void Game::menuLine()
	{
		/*
		* if the best time isn't changed to the players' best time then it won't print
		* if it is initialized then it will print the best time and ask if you can do better
		*/
		screen->Print(const_cast<char*>(diff_line.substr(difficulty * 4, 4).c_str()), 5, 5, RedMask , 2);
		std::ifstream time_file("world/best_time.txt");
		time_file >> best_time;
		if (best_time != 19022002)
		{
			line = { "YOUR BEST TIME WAS: " + std::to_string(best_time) + "!" };
			char* win_line = new char[line.size() + 1];
			strcpy(win_line, line.c_str());
			screen->Centre(win_line, 245, GreenMask, 2);
			screen->Centre("DO YOU FEEL YOU CAN DO BETTER?", 262, GreenMask, 2);
		}
	}

	void Game::winningLine()
	{
		/*
		* if winning conditions are met then write a messege
		* if the elapsed time is faster then best time then
		* it will rewrite the best time in the txt file and print the best time
		*/
		uint32_t currTime = (uint32_t)SDL_GetPerformanceCounter();
		double elapsedTime = static_cast<double>(
			(currTime - startTime) / static_cast<double>(SDL_GetPerformanceFrequency())
			); // taken from https://seanballais.com/blog/computing-elapsed-time-in-sdl2/
		line = { "YOUR TIME WAS: " + std::to_string(elapsedTime) + "!" };
		char* win_line = new char[line.size() + 1];
		strcpy(win_line, line.c_str());
		screen->Centre(win_line, 245, GreenMask, 2);
		std::fstream time_file("world/best_time.txt");
		time_file >> best_time;
		time_file.close();
		if (best_time > elapsedTime)
		{
			time_file.open("world/best_time.txt");
			time_file << elapsedTime;
			time_file.close();
		}
		time_file.open("world/master_time.txt");
		time_file >> best_time;
		if (best_time > elapsedTime)
			screen->Centre("FINALLY A WORTHY OPPONENT! YOU BEAT THE MASTER!", 262, GreenMask , 2);
	}

	// -----------------------------------------------------------
	// Main application tick function
	// -----------------------------------------------------------
	char* display_time = new char[8];
	void Game::Tick(float deltaTime)
	{	
		uint32_t currTime;
		switch (state_machine)
		{
		case menu:	
			screen->Clear(0);
			menuLine();
			screen->Centre("PRESS ENTER TO START!", 210, GreenMask , 4);
			if (GetAsyncKeyState(VK_RETURN)) // when enter is pressed then switch to play state
			{
				switchState();
			}
			break;
		case play:
			if (player.isdead())
			{ // on death condition after 2 scondes of a printed messege reset player
				Sleep(2000);
				player.revive();
			}
			else if (player.victory())
			{ // on win condition after 4 seconds of a printed messege shutdown
				Sleep(4000);
				state_machine = menu;
				player.lose();
				reset(player);
				break;
			}
			if (GetAsyncKeyState(0X52)) // if 'R' key is pressed then reset ball
			{
				startTime = (uint32_t)SDL_GetPerformanceCounter();
				reset(player);
			}
			// clear the graphics window
			screen->Clear(0);
			currTime = (uint32_t)SDL_GetPerformanceCounter(); // displaying the current elapsed time
			line = std::to_string(ceil(static_cast<double>(
				(currTime - startTime) / static_cast<double>(SDL_GetPerformanceFrequency())) * 100.0) / 100.0);
			std::strncpy(display_time, line.c_str(),7);
			display_time[7] = '\0';
			
			tilemap.mapScroll(screen); // map printing function
			for(int i =0; i < difficulty + 1; i++) 
				for (Gun* a : guns[i]) (*a).render(screen, tilemap, player, startTime);
			player.printBall(screen); // ball printing function
			player.Drive(tilemap); // ball control function
			player.verlet(tilemap); // ball movement resolution function
			player.mapReact(screen, tilemap); // map interaction function
			screen->Print(display_time, 5,5, RedMask,2);
			
			if (player.isdead() || player.victory()) switchState(); // if death or win conditions are met switch to stop state
			break;
		case stop:
			if (player.isdead())
			{ // death messege
				screen->Centre("YOU DIED!", 256, RedMask, 3);
				reset(player);
			}
			if (player.victory())
			{// winning messege
				screen->Clear(0);
				screen->Centre("YOU'VE WON!", 210, GreenMask, 4);
				winningLine();
			}
			switchState();
		default:
			break;
		}
		Sleep(25);
	}
};