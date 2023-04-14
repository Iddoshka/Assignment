#include "surface.h"
#include "template.h"
#include<vector>

#pragma once

class TileMaps
{
	float XoffSet; // X axis offset var
	float YoffSet; // Y axis offset var
	int width; // width of the map
	int height; // height of the map
	char** Map; // map buffer
	std::vector<Tmpl8::vec4> colliders; // vector containing the colliders of the map
	std::vector<Tmpl8::vec4> jumpers; // vector containing the jump tiles of the map
	std::vector<Tmpl8::vec4> slowers; // vector containing the slow tiles of the map
	std::vector<Tmpl8::vec4> deaths; // vector containing the death tiles of the map
	std::vector<Tmpl8::vec4> win; // vector containing the winning tiles of the map
	Tmpl8::Surface* Tiles; // a surface class object with the images containing all the tiles

	

public:
	TileMaps(char* TilesIn);
	TileMaps(char* TilesIn, int heightIn, int widthtIn);
	void mapScroll(Tmpl8::Surface* screen); // map printing function 
	char** getMap() { return Map; } // returns the map buffer
	std::vector<Tmpl8::vec4> getColliders() { return colliders; } // returns the collider vector
	std::vector<Tmpl8::vec4> getJumpers() { return jumpers; } // returns the jump tiles vector
	std::vector<Tmpl8::vec4> getSlowers() { return slowers; } // returns the slow tiles vector
	std::vector<Tmpl8::vec4> getDeaths() { return deaths; } // returns the death tiles vector
	std::vector<Tmpl8::vec4> getWin() { return win; } // returns the winning tiles vector
	int getWidth() { return width; } // returns the width of the map
	float getXoffSet() { return XoffSet; } // returns the X axis offset of the map
	void setXoffSet(float xoff) { // sets the X axis offset of the map but clamps it at the edge of the map
		XoffSet = xoff;
		XoffSet = Tmpl8::Clamp(XoffSet, 0.0f, width / 3.0f - 25.0f);
	}
	float getYoffSet() { return YoffSet; }// returns the Y axis offset of the map
	void setYoffSet(float yoff) {// sets the Y axis offset of the map but clamps it at the edge of the map
		YoffSet = yoff;
		YoffSet = Tmpl8::Clamp(YoffSet, 0.0f, height - 16.0f);
	}
	int getHeight() { return height; }// returns the height of the map
	void setTile(char** mapAdd, int sizeI, int sizeJ, int startX, int startY, char sign); // map editing function
	void setTile(char* mapAdd, int sizeI, int startX, int startY, char sign); // map editing function
	void setTile(char** mapAdd, int sizeI, int sizeJ, int startX, int startY); // map editing function
	void setMap(char** newMap) { Map = newMap; } // setting the map buffer
	void setColliders(int strtX, int strtY, int length, int height, char sign); // editing the map objects function
	void printMap(); // printing the char buffer to the debug window function
	void DrawTile(int tx, int ty, Tmpl8::Surface* screen, int x, int y);
	void DrawTile(int tx, int ty, Tmpl8::Surface* screen, int x, int y, float precY, int startY, float precX, int startX);
};