#pragma once
#include"tilemap.h"
#include<iostream>

TileMaps::TileMaps(char* TilesIn, int heightIn, int widthIn)
	:	Tiles(new Tmpl8::Surface(TilesIn))
{ // initializing the map buffer to be a black screen with the given width and height
	height = heightIn;
	width = widthIn;
	Map = new char* [heightIn];
	for (int i = 0; i < heightIn; i++)
	{
		Map[i] = new char[widthIn];
		for (int j = 0; j < widthIn; j += 3)
		{
			Map[i][j] = 'p';
			Map[i][j + 1] = 'f';
			Map[i][j + 2] = ' ';
		}
	}
}

TileMaps::TileMaps(char* TilesIn)
	: Tiles(new Tmpl8::Surface(TilesIn))
{// initializing the map buffer to be a black screen with the width and height of the screen
	XoffSet = 0;
	YoffSet = 0;
	height = ScreenHeight / TileLength;
	width = ScreenWidth / TileLength;
	Map = new char* [height];
	for (int i = 0; i < height; i++)
	{
		Map[i] = new char[width];
		for (int j = 0; j < width; j += 3)
		{
			Map[i][j] = 'p';
			Map[i][j + 1] = 'f';
			Map[i][j + 2] = ' ';
		}
	}
}

void TileMaps::setTile(char** mapAdd, int sizeI, int sizeJ, int startX, int startY,char sign)
{ // changing the tiles on the map buffer and adding the collider to the class object object vectors depending on the given sign
	// this function adds a 2d object
	startX *= 3;
	for (int i = startY; i < sizeI + startY; i++)
	{
		for (int j = startX; j < sizeJ + startX; j++)
		{
			Map[i][j] = mapAdd[i - startY][j - startX];
		}
	}
	setColliders((float)(startX / 3 * TileLength), (float)(startY * TileLength), (float)((sizeJ / 3) * TileLength), (float)(sizeI * TileLength), sign);
}

void TileMaps::setTile(char** mapAdd, int sizeI, int sizeJ, int startX, int startY)
{// changing the tiles on the map buffer 
	// this function adds a 2d object
	
	startX *= 3;
	for (int i = startY; i < sizeI + startY; i++)
	{
		for (int j = startX; j < sizeJ + startX; j++)
		{
			Map[i][j] = mapAdd[i - startY][j - startX];
		}
	}
}

void TileMaps::setTile(char* mapAdd, int sizeI, int startX, int startY, char sign)
{// changing the tiles on the map buffer and adding the collider to the class object object vectors depending on the given sign
	// this function adds a 2d object with a given height of 1
	startX *= 3;
	for (int j = startX; j < sizeI + startX; j+= 3)
	{
		Map[startY][j] = mapAdd[j - startX];
		Map[startY][j + 1] = mapAdd[j + 1 - startX];
		Map[startY][j + 2] = mapAdd[j + 2 - startX];
	}
	setColliders((float)(startX / 3 * TileLength), (float)(startY * TileLength), (float)(sizeI * TileLength), (float)TileLength, sign);
}
template <typename T>
static int find(std::vector<T> vec, T to_find)
{
	int counter = 0;
	for (const T& a : vec)
	{
		if (to_find == a)
		{
			return counter;
		}
		counter++;
	}
	return -1;
}
void TileMaps::setColliders(float strtX, float strtY, float length, float height, char sign)
{ // adds the collider vec4 to the appropriate type vector depending on the given sign
	switch (sign)
	{
	case ' ':
		/*for (auto& a : std::vector<std::vector<Tmpl8::vec4>>{colliders,jumpers, slowers, deaths, win})
			if (find(a, Tmpl8::vec4(strtX * 32.0f, strtY * 32.0f, length, height)) != -1)
			{
				a.erase(a.begin() + find(a, Tmpl8::vec4(strtX * 32.0f, strtY * 32.0f, length, height)));
				printf("f");
			}*/
		if (find(colliders, Tmpl8::vec4(strtX, strtY , length , height)) != -1)
			colliders.erase(colliders.begin() + find(colliders, Tmpl8::vec4(strtX, strtY, length, height)));
		if (find(slowers, Tmpl8::vec4(strtX, strtY, length, height)) != -1)
			slowers.erase(slowers.begin() + find(slowers, Tmpl8::vec4(strtX, strtY, length, height)));
		break;
	case 'x':
		if (find(colliders, Tmpl8::vec4(strtX, strtY, length, height)) == -1)
			colliders.push_back(Tmpl8::vec4(strtX, strtY, length, height));
		break;
	case 'j':
		if (find(jumpers, Tmpl8::vec4(strtX, strtY, length, height)) == -1)
			jumpers.push_back(Tmpl8::vec4(strtX, strtY, length, height));
		break;
	case 'd':
		if (find(deaths, Tmpl8::vec4(strtX, strtY, length, height)) == -1)
			deaths.push_back(Tmpl8::vec4(strtX, strtY, length, height));
		break;
	case 's':
		if (find(slowers, Tmpl8::vec4(strtX, strtY, length, height)) == -1)
			slowers.push_back(Tmpl8::vec4(strtX, strtY, length, height));
		break;
	case 'w':
		if (find(win, Tmpl8::vec4(strtX, strtY, length, height)) == -1)
			win.push_back(Tmpl8::vec4(strtX, strtY, length, height));
		break;
	default:
		break;
	}
}
void TileMaps::printMap()
{ // prints the map buffer to the debug window
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j += 3)
		{
			std::cout << Map[i][j] << Map[i][j + 1] << Map[i][j + 2];
		}
		std::cout << "\n";
	}
}

void TileMaps::DrawTile(int tx, int ty, Tmpl8::Surface* screen, int x, int y)
{ // function to drawing a tile taken from the c++ beginner lessons https://www.3dgep.com/cpp-fast-track-12-classes/#more-9350
	Tmpl8::Pixel* src = Tiles->GetBuffer() + 1 + tx * 33 + (1 + ty * 33) * 595;
	Tmpl8::Pixel* dst = screen->GetBuffer() + x + y * ScreenWidth;
	for (int i = 0; i < TileLength; i++, src += 595, dst += ScreenWidth)
		for (int j = 0; j < TileLength; j++) {
			dst[j] = src[j];
		}
}

void TileMaps::DrawTile(int tx, int ty, Tmpl8::Surface* screen, int x, int y, float precY, int startY, float precX, int startX)
{ // a modefied function of DrawTile that can control the starting and end point of tile on print
	Tmpl8::Pixel* src = Tiles->GetBuffer() + 1 + tx * 33 + (1 + ty * 33) * 595;
	Tmpl8::Pixel* dst = screen->GetBuffer() + x + y * ScreenWidth;
	for (int i = 0; i < TileLength * precY; i++, src += 595, dst += ScreenWidth)
		for (int j = 0; j < TileLength * precX; j++)
			if(x + j < ScreenWidth)
				dst[j] = src[(j + startX) + startY * 595];
}
void TileMaps::mapScroll(Tmpl8::Surface* screen)
{ // function that prints the map on the screen depending on the offset of the map

	/*
	* the function uses a class variable that shows how much the map was moved in each axis
	* the funcion prints all the tiles that can fit in the screen width and height
	* for each tile it changes the starting point it the tile and its ending point
	* as well as it position on the screen.
	* if the offset isnt a whole number then the function will take the fraction
	* and print an extra tile with, so the first and last tiles make together the a whole tile
	*/

	for (int i = 0; i < ScreenHeight/ TileLength + int(fmodf(2 - fmodf(YoffSet, 1), 2)); i++)
	{ // run through the amount of tiles that fit on the screen's height, if the first one is cut short then it will run an extra tile
		for (int j = 0; j < ScreenWidth/ TileLength + int(fmodf(2 - fmodf(XoffSet, 1), 2)); j++)
		{// run through the amount of tiles that fit on the screen's width, if the first one is cut short then it will run an extra tile
			int tx = Map[int(i + YoffSet)][int(j + XoffSet) * 3] - 'a';
			int ty = Map[int(i + YoffSet)][int(j + XoffSet) * 3 + 1] - 'a';
			float precX = 1, precY = 1;
			int startX = 0, startY = 0;
			int posX = (j * TileLength) - int(fmodf(XoffSet, 1) * TileLength), posY = (i * TileLength) - int(fmodf(YoffSet, 1) * TileLength);
			if (j == 0)
				startX = int(fmodf(XoffSet, 1) * TileLength),posX = 0,precX = 1.0f - fmodf(XoffSet,1);
			if (j == (ScreenWidth / TileLength + int(fmodf(2 - fmodf(XoffSet, 1), 2))) - 1)
				precX = (fmodf(XoffSet,1) == 0.0f) ? 1.0f : fmodf(XoffSet, 1);
			if (i == 0)
				startY = int(fmodf(YoffSet, 1) * TileLength), posY = 0, precY = 1.0f - fmodf(YoffSet, 1);
			if (i == (ScreenHeight / TileLength + int(fmodf(2 - fmodf(YoffSet, 1), 2))) - 1)
				precY = (fmodf(YoffSet, 1) == 0.0f) ? 1.0f : fmodf(YoffSet, 1);

			DrawTile(tx, ty, screen, posX, posY,precY,startY,precX,startX); // drawing the tile
		}
	}
}
